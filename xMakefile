bare.img: 	boot/boot.img kernel/kernel.img tools/build-img
	./tools/build-img boot/boot.img kernel/kernel.img

boot/boot.img: boot/*.asm boot/*.inc
	+cd boot && make

kernel/kernel.img:
	+cd kernel && make

tools/build-img:
	+cd tools && make

clean:
	+cd tools && make clean
	+cd boot && make clean
	+cd kernel && make clean

run: 	bare.img
	qemu-system-x86_64 bare.img
