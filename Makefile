XBE_TITLE = default
OUTPUT_DIR = out
SRCS = $(CURDIR)/src/main.c $(CURDIR)/src/shader.c $(CURDIR)/src/math3d.c $(CURDIR)/src/file_util.c $(CURDIR)/src/terrain.c $(CURDIR)/src/audio.c
NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
SHADER_OBJS = src/ps.inl src/vs.inl src/ps2.inl src/vs2.inl
DEBUG = y

# all:
# 	mkdir -p out
# 	cp src/*.c out
# 	cp src/*.h out
# 	cp src/*.inl out

include $(NXDK_DIR)/Makefile

TARGET += $(OUTPUT_DIR)/testimg.png
$(GEN_XISO): $(OUTPUT_DIR)/testimg.png
$(OUTPUT_DIR)/testimg.png: $(CURDIR)/testimg.png $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'

run:
	./tools/xdvdfs pack out/ xproj.iso
	xemu -dvd_path "xproj.iso" -s


