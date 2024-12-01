XBE_TITLE = nxdk\ sample\ -\ hello
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c $(CURDIR)/shader.c
NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
DEBUG = y

# all:
# 	mkdir -p out
# 	cp src/*.c out
# 	cp src/*.h out
# 	cp src/*.inl out

include $(NXDK_DIR)/Makefile

TARGET += $(OUTPUT_DIR)/testimg.jpg
$(GEN_XISO): $(OUTPUT_DIR)/testimg.jpg
$(OUTPUT_DIR)/testimg.jpg: $(CURDIR)/testimg.jpg $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'

run:
	xemu -dvd_path "nxdk sample - hello.iso" -s


