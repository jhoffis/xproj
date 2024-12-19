XBE_TITLE = default
OUTPUT_DIR = out
SRCS = $(CURDIR)/src/main.c $(CURDIR)/src/shader.c $(CURDIR)/src/math3d.c $(CURDIR)/src/png_loader.c $(CURDIR)/src/terrain.c $(CURDIR)/src/audio.c $(CURDIR)/src/wav_loader.c $(CURDIR)/src/file_util.c $(CURDIR)/src/str_util.c $(CURDIR)/src/world.c $(CURDIR)/src/main.c
NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
SHADER_OBJS = src/ps.inl src/vs.inl src/ps2.inl src/vs2.inl
DEBUG = y
NXDK_CFLAGS +=-O0 

include $(NXDK_DIR)/Makefile

run:
	./tools/xdvdfs pack out/ xproj.iso
	../xemu/dist/xemu -dvd_path "xproj.iso" -s


