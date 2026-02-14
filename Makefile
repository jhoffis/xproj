# --- bootstrap: re-run make inside activated env, exactly once ---
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
NXDK_DIR ?= $(MAKEFILE_DIR)nxdk-xproj

ifeq ($(NXDK_ENV_ACTIVE),)

.PHONY: __reexec
__reexec:
	@bash -lc 'cd "$(MAKEFILE_DIR)" && source "$(MAKEFILE_DIR)/activate.sh" && exec "$(MAKE)" NXDK_ENV_ACTIVE=1 $(MAKECMDGOALS)'

# If user asked for specific goals (e.g. "make run"), make those goals just trigger __reexec
ifneq ($(MAKECMDGOALS),)
$(MAKECMDGOALS): __reexec
	@:
else
# No goals given ("make"): default to reexec, and let inner make pick its default goal
.DEFAULT_GOAL := __reexec
endif

else
# --- real build (only runs in activated env) ---

XBE_TITLE = xproj
OUTPUT_DIR = out
SRC_DIR = $(CURDIR)/src
SRC_OUT_DIR = $(CURDIR)/src_out
SRCS = $(sort $(shell find $(SRC_DIR) -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.s' \)))
SHADER_SRCS = $(sort $(shell find $(SRC_DIR) -type f \( -name '*.vs.cg' -o -name '*.ps.cg' \)))
SHADER_OBJS = $(patsubst $(SRC_DIR)/%.vs.cg,$(SRC_OUT_DIR)/%.inl,$(filter %.vs.cg,$(SHADER_SRCS)))
SHADER_OBJS += $(patsubst $(SRC_DIR)/%.ps.cg,$(SRC_OUT_DIR)/%.inl,$(filter %.ps.cg,$(SHADER_SRCS)))
SHADER_DRIVER_OBJ = $(SRC_OUT_DIR)/shader.obj
NXDK_SDL = y
DEBUG = y
NXDK_CFLAGS += -O0
NXDK_CFLAGS += -I$(SRC_OUT_DIR)
NXDK_CFLAGS += -I$(CURDIR)

include $(NXDK_DIR)/Makefile

$(SHADER_DRIVER_OBJ): $(SHADER_OBJS)

.PHONY: win ch run

win:
	./tools/xdvdfs.exe pack out/ xproj.iso
	./tools/xemu.exe -dvd_path "xproj.iso" -s

ch: # clean here
	rm -rf $(SRC_OUT_DIR)
	rm -f $(SRC_DIR)/*.obj
	rm -f $(SRC_DIR)/*.c.d
	rm -f $(SRC_DIR)/*.cpp.d
	rm -f $(SRC_DIR)/*.inl

run:
	./tools/xdvdfs pack out/ xproj.iso
	../xemu/dist/xemu -dvd_path "xproj.iso" -s

endif

