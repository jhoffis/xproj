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
SRCS = $(shell find $(CURDIR)/src -name '*.c')
NXDK_SDL = y
SHADER_OBJS = src/ps.inl src/vs.inl src/ps2.inl src/vs2.inl
DEBUG = y
NXDK_CFLAGS += -O0

include $(NXDK_DIR)/Makefile

.PHONY: win ch run

win:
	./tools/xdvdfs.exe pack out/ xproj.iso
	./tools/xemu.exe -dvd_path "xproj.iso" -s

ch: # clean here
	rm -f src/*.obj
	rm -f src/*.c.d

run:
	./tools/xdvdfs pack out/ xproj.iso
	../xemu/dist/xemu -dvd_path "xproj.iso" -s

endif

