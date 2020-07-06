# INSPIRATION

LIB=		libinspiration.a

DISPLAY=	Display.o \
		Display/DisplayTask.o \
		Display/Cursor.o \
		Display/MousePointerTask.o

IDCMP=		idcmp/IdcmpTask.o \
		idcmp/IdcmpSubscribers.o

OBJ= 		$(DISPLAY) \
		$(IDCMP) \
		DirtyRect.o \
		BScreen.o \
		Desktop.o \
		BWindow.o \
		BConsoleWindow.o \
		TestTask.o \
		InspirationBase.o

.cpp.o:
		@echo $*.cpp
		$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
		@echo ""

.asm.o:
		@echo ""
		nasm -f elf64 -o $*.o $*.asm
		@echo ""

$(LIB):		$(OBJ)
		$(AR) rcs $(LIB) $(OBJ)
		$(RANLIB) $(LIB)
