XBE_TITLE = default
OUTPUT_DIR = out
SRCS = $(shell find $(CURDIR)/src -name '*.c')
NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
SHADER_OBJS = src/ps.inl src/vs.inl src/ps2.inl src/vs2.inl
DEBUG = y
NXDK_CFLAGS += -Og 

include $(NXDK_DIR)/Makefile

win:
	./tools/xdvdfs.exe pack out/ xproj.iso
	./tools/xemu.exe -dvd_path "xproj.iso" -s

ch: # clean here
	rm src/*.obj
	rm src/*.c.d

run:
	./tools/xdvdfs pack out/ xproj.iso
	../xemu/dist/xemu -dvd_path "xproj.iso" -s


