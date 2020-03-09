boot.img: boot.asm *.inc
	nasm -f bin -l boot.lst -o boot.img boot.asm

clean:
	rm -rf *.lst *.img *.o
