OBJ=	build-img.o

.cpp.o:
	gcc -c -O2 -g -o $*.o $*.cpp

build-img:	$(OBJ)
	gcc -o build-img $(OBJ) -lstdc++

clean:
	rm build-img *.o

install: build-img
	cp build-img /usr/local/bin
