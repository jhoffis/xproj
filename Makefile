XBE_TITLE = nxdk\ sample\ -\ hello
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/src/main.c $(CURDIR)/src/shader.c $(CURDIR)/src/math3d.c $(CURDIR)/src/file_util.c
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
	xemu -dvd_path "nxdk sample - hello.iso" -s


