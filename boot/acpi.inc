;;process acpi
global process_acpi
process_acpi:
	xor  rax, rax
	mov [sys_info + SYSINFO.acpi_numcpu], rax
	mov [sys_info + SYSINFO.acpi_numioapic], rax
	mov [sys_info + SYSINFO.acpi_hpet], rax
	;search for RDPS
	mov esi, 0xe00000
	mov rbx, 'RSD PTR '
.find:
	lodsq
	cmp rax, rbx
	je .found
	cmp esi, 0xffff
	jne .find
	; not found!
	BOCHS
	hlt
.found:
	call .checksum
	jne .find
	lodsb 			;checksum
	lodsd 			;oem 4 bytes
	lodsw			;oem 2 bytes
	lodsb 			;revision
	cmp al, 0
	je .v1
	jmp .v2
	
.checksum:
	push rsi
	sub rsi, 8
	mov rcx, 20
	xor rdx, rdx
.checksum_loop:
	lodsb
	add dl, al
	loop .checksum_loop
	pop rsi
	cmp dl, 0
	ret

.v1:
	;process RSDT
	xor rax, rax
	lodsd
	mov rsi, rax
	lodsd
	cmp eax, 'RSDT'
	jne rsdt_error
	sub rsi, 4
	mov [sys_info + SYSINFO.acpi_xrsdt], rsi
	add rsi, 4
	xor rax, rax
	lodsd 			;length
	add rsi, 28 		;-> entry
	sub rax, 36		;compute table size
	shr rax, 2		;-> dword
	mov rdx, rax
	xor rcx, rcx
.v1_next:
	lodsd
	push rax
	add rcx, 1
	cmp rcx, rdx
	je .common
	jmp .v1_next
	
.v2:
	lodsd 			;RSDT address
	lodsd			;length
	lodsq 			;XSDT address
	mov rsi, rax
	cmp eax, 'XSDT'
	jne rsdt_error
	sub rsi, 4
	mov [sys_info + SYSINFO.acpi_xrsdt], rsi
	add rsi, 4

	xor rax, rax
	lodsd			;length
	add rsi, 28
	sub rax, 36
	shr rax, 3
	mov rdx, rax
	xor rcx, rcx
.v2_next:
	lodsq
	push rax
	add rcx, 1
	cmp rcx, rdx
	jne .v2_next

.common:
	xor rcx, rcx

.next_table:
	pop rsi
	lodsd
	add ecx, 1
	mov ebx, 'APIC'
	cmp rax, rbx
	je .apic
	mov ebx, 'HPET'
	je .hpet
	cmp rcx, rdx
	jne .next_table
	ret

.apic:
	call parse_apic_table
	jmp .next_table
.hpet:
	call parse_hpet_table
	jmp .next_table
	
rsdt_error:
	jmp $
	
	
parse_apic_table:
	push rcx
	push rdx
	xor rax, rax
	lodsd			;MADT length
	mov rcx, rax
	xor rbx, rbx		;counter
	lodsb			;revision
	lodsb			;checksum
	lodsd			;OEM first 4
	lodsw			;OEM next 2
	lodsq			;OEM table id
	lodsd			;OEM revision
	lodsd 			;creator ID
	lodsd			;creator revision
	xor rax, rax
	lodsd			;local APIC
	mov [sys_info + SYSINFO.acpi_lapic], rax
	lodsd			;flags
	add rbx, 44
.parse:
	cmp rbx, rcx
	jge .done
	lodsb			;type
	cmp al, 0 		;processor local apic
	je .apic
	cmp al, 1
	je .ioapic
	cmp al, 2
	je .interrupt		;interrupt source
	; cmp al, 3
	; je .nmi
	; cmp al, 4
	; je .apic_nmi
	; cmp al, 5
	; je .address
	cmp al, 9
	je .x2apic
	; cmp al, 10
	; je .x2apic_nmi

	xor rax, rax
	lodsb			;skip next byte
	add rbx, rax
	add rsi, rax
	sub rsi, 2
	jmp .parse

.apic:
	xor rax, rax
	xor rdx, rdx
	lodsb
	add rbx, rax
	lodsb			;processor id
	lodsb			;apic id
	xchg rax, rdx
	lodsd			;flags
	bt eax, 0
	jnc .parse		;skip if disabled
	inc qword [sys_info + SYSINFO.acpi_numcpu]

	xchg rax, rdx
	mov rdi, [cpu_ptr]
	stosb			;store found apic id
	mov [cpu_ptr], rdi

	jmp .parse
	
.ioapic:
	xor rax, rax
	lodsb			;length
	add rbx, rax
	lodsb			;id
	lodsb			;reserved
	xor rax, rax
	lodsd			;address of ioapic
	mov rdi, [ioapic_ptr]
	stosq
	mov [ioapic_ptr], rdi
	lodsd
	mov rdi, [ioapic_vecs]
	stosq
	mov [ioapic_vecs], rdi
	inc qword [sys_info + SYSINFO.acpi_numioapic]
	jmp .parse
	

.interrupt:
	xor rax, rax
	lodsb
	add rbx, rax
	lodsb			;bus
	lodsb			;source
	lodsd			;global system interrupt
	lodsw			;flags
	jmp .parse
	
.x2apic:
	xor rax, rax
	xor rdx, rdx
	lodsb			;length
	add rbx, rax
	lodsw			;reserved
	lodsd			;x2apic id
	xchg rax, rdx
	lodsd			;flags
	bt rax, 0
	jne .skip
	xchg rax, rdx
	; TODO save ids
	mov rdi, [x2apic_ptr]
	stosq
	mov [x2apic_ptr], rdi
.skip:
	lodsd			; acpi processor uid
	jmp .parse

.done:
	pop rdx
	pop rcx
	ret
	
cpu_ptr: dq sys_info + SYSINFO.acpi_apicids	
ioapic_ptr: dq sys_info + SYSINFO.acpi_ioapicaddrs
ioapic_vecs: dq sys_info + SYSINFO.acpi_ioapicvecs
x2apic_ptr: dq sys_info + SYSINFO.acpi_x2apics
	
parse_hpet_table:
	lodsd			;length
	lodsb			;revision
	lodsb			;checksum
	lodsd			;oem 4 bytes
	lodsw			;oem 2 bytes
	lodsq			;oem table id
	lodsd			;oem revision
	lodsd			;creator id
	lodsd			;creator revision
	lodsd			;event timrer block id
	lodsd			;base address settings
	lodsq			;base address value
	mov [sys_info + SYSINFO.acpi_hpet], rax
	lodsb			;hpet number
	lodsw			;main counter minimum
	lodsw			;page protection and oem attribute
	ret
	
	
