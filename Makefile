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

# Debug (default) vs Release build configuration
ifeq ($(filter release,$(MAKECMDGOALS)),)
SRC_OUT_DIR = $(CURDIR)/src_out
NXDK_LIB_OUT_DIR = $(CURDIR)/nxdk_out
DEBUG = y
NXDK_CFLAGS += -O0
NXDK_CFLAGS += -DDBG=1
else
SRC_OUT_DIR = $(CURDIR)/src_out_release
NXDK_LIB_OUT_DIR = $(CURDIR)/nxdk_out_release
DEBUG = n
NXDK_CFLAGS += -Oz
NXDK_CFLAGS += -fno-strict-aliasing
endif

SRCS = $(sort $(shell find $(SRC_DIR) -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.s' \)))

# ---------------------------------------------------------------------------
# Shaders
# Shader sources are under: src/shaders/**
# This Makefile guarantees the shader_data_gen.inc generator runs *every* time
# you invoke `make` or `make release`, and that it completes before any C/C++
# compilation starts (even with `make -j`).
# ---------------------------------------------------------------------------

SHADER_DIR      := $(SRC_DIR)/shaders
SHADER_INL_DIR  := $(SRC_OUT_DIR)/shaders
SHADER_DATA_GEN := $(SRC_OUT_DIR)/shader_data_gen.inc

# Python generator (override on command line if needed, e.g. SHADER_GEN_PY=tools/gen_shader_data_gen.py)
SHADER_GEN_PY ?= $(CURDIR)/gen_shader_data_gen.py
SHADER_GEN_ARGS := --shader-dir $(SHADER_DIR) --out $(SHADER_DATA_GEN) --inl-prefix shaders

SHADER_SRCS = $(sort $(shell find $(SHADER_DIR) -type f \( -name '*.vs.cg' -o -name '*.ps.cg' \)))
SHADER_OBJS = $(patsubst $(SHADER_DIR)/%.vs.cg,$(SHADER_INL_DIR)/%.vs.inl,$(filter %.vs.cg,$(SHADER_SRCS)))
SHADER_OBJS += $(patsubst $(SHADER_DIR)/%.ps.cg,$(SHADER_INL_DIR)/%.ps.inl,$(filter %.ps.cg,$(SHADER_SRCS)))

# If you have a dedicated shader "driver" object that includes shader_data_gen.inc, keep it here.
SHADER_DRIVER_OBJ = $(SRC_OUT_DIR)/shader.obj

NXDK_SDL = y
NXDK_CFLAGS += -I$(SRC_OUT_DIR)
NXDK_CFLAGS += -I$(CURDIR)

include $(NXDK_DIR)/Makefile

.PHONY: FORCE shaders shader_data_gen
FORCE:

# Always run the python script, but only rewrite the .inc when contents change.
shader_data_gen: FORCE
	@mkdir -p '$(dir $(SHADER_DATA_GEN))'
	@python3 '$(SHADER_GEN_PY)' $(SHADER_GEN_ARGS)

# Convenience alias
shaders: shader_data_gen

# Also make the generated file part of the graph for anything that wants it.
$(SHADER_DATA_GEN): shader_data_gen ;

# If your shader driver depends on the generated include, keep that dependency.
$(SHADER_DRIVER_OBJ): $(SHADER_DATA_GEN)

# --- HARD ORDERING: make sure shader generation finishes before any compilation ---
# NXDK's Makefile typically defines OBJS and TARGET after the include above.
# We hook both (and also the main outputs) to avoid any parallel compilation starting early.
ifneq ($(strip $(OBJS)),)
$(OBJS): | shader_data_gen
endif
ifneq ($(strip $(TARGET)),)
$(TARGET): | shader_data_gen
endif
# Many nxdk projects build out/default.xbe; ensure it also waits.
$(OUTPUT_DIR)/default.xbe: | shader_data_gen

.PHONY: release
release: all

.PHONY: compdb
compdb:
	bear -- $(MAKE) ch all

.PHONY: win ch run kd-serve run-kd

# ---------------------------------------------------------------------------
# Xemu kernel debugging (KD) over serial TCP
#
# Usage (two terminals is recommended):
#   1) make kd-serve
#   2) make run-kd
#
# Notes:
# - `pykdclient` is installable via PyPI: pip install pykdclient
# - Xemu is started with the LPC47M157 device and its serial port redirected
#   to the KD server.
# ---------------------------------------------------------------------------
KD_HOST ?= 127.0.0.1
KD_PORT ?= 9091
KDCLIENT ?= pykdclient
XEMU_BIN ?= ../xemu/dist/xemu

win:
	./tools/xdvdfs.exe pack out/ xproj.iso
	./tools/xemu.exe -dvd_path "xproj.iso" -s

ch: # clean here
	rm -rf $(CURDIR)/src_out
	rm -rf $(CURDIR)/src_out_release
	rm -rf $(CURDIR)/nxdk_out
	rm -rf $(CURDIR)/nxdk_out_release

run:
	./tools/xdvdfs pack out/ xproj.iso
	../xemu/dist/xemu -dvd_path "xproj.iso" -s

kd-serve:
	$(KDCLIENT) --serve --port $(KD_PORT) --host $(KD_HOST)

run-kd:
	./tools/xdvdfs pack out/ xproj.iso
	$(XEMU_BIN) -device lpc47m157 -serial tcp:$(KD_HOST):$(KD_PORT) -dvd_path "xproj.iso"

ftp:
	curl -T $(OUTPUT_DIR)/default.xbe ftp://192.168.0.207/F/xproj/ --user xbox:xbox

ftp-all:
	find $(OUTPUT_DIR) -type f -exec curl -T {} ftp://192.168.0.207/F/xproj/ --user xbox:xbox \;

endif
