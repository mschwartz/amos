OBJ=	ResourceFile.o SpriteSheet.o RawFile.o BMPFile.o TileMap.o TMXFile.o rcomp.o Panic.o 

TMX=    tmx/TMXMap.o tmx/TMXTileSet.o tmx/TMXLayer.o

LIB=    lib/utils.o lib/RawBitmap.o

CE=     ../../kernel/Exec/Memory.o \
	../../kernel/Exec/BBase.o \
	../../kernel/Types/BList.o \
	../../kernel/Types/BSymbolTable.o

INC=	-I. -I../../ -I../../kernel -I../../kernel/Exec

.cpp.o:
	$(GPP) -c -O3 $(INC) -o $*.o $*.cpp

all: rcomp

rcomp:	$(LIB) $(OBJ) $(TMX) $(CE)
	$(GPP) -o rcomp $(OBJ) $(LIB) $(TMX) $(CE)
	cp rcomp ..
	rm -f $(CE)

clean:
	@rm -f *.o rcomp
