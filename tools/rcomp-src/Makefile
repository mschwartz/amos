OBJ=	ResourceFile.o SpriteSheet.o RawFile.o BMPFile.o TileMap.o TMXFile.o rcomp.o

TMX=    tmx/TMXMap.o tmx/TMXTileSet.o tmx/TMXLayer.o

LIB=    lib/utils.o lib/RawBitmap.o

CE=     Panic.o ../../kernel/Exec/Memory.o ../../kernel/Exec/BBase.o ../../kernel/Exec/BList.o ../../kernel/Exec/BSymbolTable.o

INC=	-I. -I../../ -I../../kernel -I../../kernel/Exec

.cpp.o:
	g++ -c -O3 -DTOOL $(INC) -o $*.o $*.cpp

all: rcomp

rcomp:	$(LIB) $(OBJ) $(TMX) $(CE)
	g++ -o rcomp $(OBJ) $(LIB) $(TMX) $(CE)
	cp rcomp ..

clean:
	@rm -f *.o rcomp
