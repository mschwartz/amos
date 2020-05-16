boot.img: boot.asm *.inc
	nasm -f bin -l boot.lst -o boot.img $(SERIAL) boot.asm

clean:
	rm -rf *.lst *.img *.o
