boot.img: boot.asm *.inc
	nasm -f bin -l boot.lst $(INC_PATH) -o boot.img boot.asm

clean:
	rm -rf *.lst *.img *.o
