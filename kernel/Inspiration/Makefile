# INSPIRATION

LIB=		libinspiration.a

WINDOWMANAGER=	Display.o \
		WindowManager/DisplayTask.o \
		WindowManager/KeyboardTask.o \
		WindowManager/MousePointerTask.o

IDCMP=		idcmp/IdcmpTask.o \
		idcmp/IdcmpSubscribers.o

OBJ= 		$(WINDOWMANAGER) \
		DirtyRect.o \
		Cursor.o \
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
