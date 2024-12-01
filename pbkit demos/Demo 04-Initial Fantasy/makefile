#
# update this variable to wherever you installed the OpenXDK libraries
#
PREFIX = /usr/local/openxdk

CC = gcc
CCAS = gcc
CPP  = cpp
CXBE = $(PREFIX)/bin/cxbe

SDLFLAGS = -DENABLE_XBOX -DDISABLE_CDROM 
CC_FLAGS = -c -g -std=gnu99 -ffreestanding -nostdlib -fno-builtin -fno-exceptions -mno-cygwin -march=i386 $(SDLFLAGS)
CCAS_FLAGS = -g -O2
INCLUDE  = -I$(PREFIX)/i386-pc-xbox/include -I$(PREFIX)/include -I$(PREFIX)/include/SDL 

CLINK = -nostdlib
ALIGN = -Wl,--file-alignment,0x20 -Wl,--section-alignment,0x20 
SHARED = -shared
ENTRYPOINT = -Wl,--entry,_WinMainCRTStartup 
STRIP = -Wl,--strip-all
LD_FLAGS = $(CLINK) $(ALIGN) $(SHARED) $(ENTRYPOINT) $(STRIP)
LD_DIRS = -L$(PREFIX)/i386-pc-xbox/lib -L$(PREFIX)/lib 
LD_LIBS  = $(LD_DIRS) -lSDL -lm -lopenxdk -lhal -lc -lusb -lc -lxboxkrnl -lc -lhal -lxboxkrnl -lhal -lopenxdk -lc

#LD_LIBS = -lSDL_ttf -lfreetype2 -lSDL_image -lSDL -ljpeg -lpng -lz -lxml2 -lm -lhal -lusb -lopenxdk -lhal -lc -lxboxkrnl -lstdc++ -lgcc

OBJS=main.o pbKit.o graphics.o math3d.o mesh.o

all: default.exe

.c.o:
	$(CC) -c $< $(CC_FLAGS) $(INCLUDE)


.s.o:
	$(CCAS) -c $< $(CCAS_FLAGS)

default.exe: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LD_LIBS) $(LD_FLAGS)
	$(CXBE) -TITLE:"$@" -DUMPINFO:"default.cxbe" -OUT:"default.xbe" $@ > /dev/null

send: all
	make
	./ftp2xbox.bat

clean: 
	rm -f *.o *.exe *.dll *.xbe *.cxbe
