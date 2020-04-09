boot.img: boot.asm *.inc
	echo nasm -f bin -l boot.lst $(KGFX) -o boot.img boot.asm
	nasm -f bin -l boot.lst $(KGFX) -o boot.img boot.asm

clean:
	rm -rf *.lst *.img *.o
