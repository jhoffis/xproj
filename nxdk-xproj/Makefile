ifeq ($(NXDK_DIR),)
NXDK_DIR = $(shell pwd)
endif

ifeq ($(XBE_TITLE),)
XBE_TITLE = nxdk_app
endif

ifeq ($(OUTPUT_DIR),)
OUTPUT_DIR = bin
endif

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

LD  = nxdk-link
LIB = nxdk-lib
AS  = nxdk-as
CC  = nxdk-cc
CXX = nxdk-cxx

ifeq ($(UNAME_S),Linux)
ifneq ($(UNAME_M),x86_64)
CGC          = $(NXDK_DIR)/tools/cg/linux/cgc.i386
else
CGC          = $(NXDK_DIR)/tools/cg/linux/cgc
endif #UNAME_M != x86_64
endif
ifeq ($(UNAME_S),Darwin)
CGC          = $(NXDK_DIR)/tools/cg/mac/cgc
endif
ifneq (,$(findstring MSYS_NT,$(UNAME_S)))
$(error Please use a MinGW64 shell)
endif
ifneq (,$(findstring MINGW,$(UNAME_S)))
CGC          = $(NXDK_DIR)/tools/cg/win/cgc
endif

TARGET       = $(OUTPUT_DIR)/default.xbe
CXBE         = $(NXDK_DIR)/tools/cxbe/cxbe
VP20COMPILER = $(NXDK_DIR)/tools/vp20compiler/vp20compiler
FP20COMPILER = $(NXDK_DIR)/tools/fp20compiler/fp20compiler
EXTRACT_XISO = $(NXDK_DIR)/tools/extract-xiso/build/extract-xiso
TOOLS        = cxbe vp20compiler fp20compiler extract-xiso

ifeq ($(DEBUG),y)
NXDK_ASFLAGS += -g -gdwarf-4
NXDK_CFLAGS += -g -gdwarf-4
NXDK_CXXFLAGS += -g -gdwarf-4
NXDK_LDFLAGS += -debug
endif

ifeq ($(LTO),y)
NXDK_ASFLAGS += -flto
NXDK_CFLAGS += -flto
NXDK_CXXFLAGS += -flto
endif

ifneq ($(GEN_XISO),)
TARGET += $(GEN_XISO)
endif

ifneq ($(NXDK_ONLY),)
NXDK_CXX = y
NXDK_SDL = y
TARGET = main.exe
endif

all: $(TARGET)

NXDK_OUT_OF_TREE := $(if $(and $(SRC_DIR),$(SRC_OUT_DIR)),y,)
SRC_DIR_CANONICAL := $(patsubst %/,%,$(SRC_DIR))
SRC_OUT_DIR_CANONICAL := $(patsubst %/,%,$(SRC_OUT_DIR))
to_out_path = $(if $(NXDK_OUT_OF_TREE),$(patsubst $(SRC_DIR_CANONICAL)/%,$(SRC_OUT_DIR_CANONICAL)/%,$(1)),$(1))

NXDK_LIB_OUT_OF_TREE := $(if $(NXDK_LIB_OUT_DIR),y,)
NXDK_DIR_CANONICAL := $(patsubst %/,%,$(NXDK_DIR))
NXDK_LIB_OUT_DIR_CANONICAL := $(patsubst %/,%,$(NXDK_LIB_OUT_DIR))
to_lib_out_path = $(if $(NXDK_LIB_OUT_OF_TREE),$(patsubst $(NXDK_DIR_CANONICAL)/%,$(NXDK_LIB_OUT_DIR_CANONICAL)/%,$(1)),$(1))
to_lib_obj_path = $(addsuffix .obj,$(basename $(call to_lib_out_path,$(1))))

include $(NXDK_DIR)/lib/Makefile

C_SRCS := $(filter %.c, $(SRCS))
CPP_SRCS := $(filter %.cpp, $(SRCS))
S_SRCS := $(filter %.s, $(SRCS))

C_OBJS := $(addsuffix .obj, $(basename $(call to_out_path,$(C_SRCS))))
CPP_OBJS := $(addsuffix .obj, $(basename $(call to_out_path,$(CPP_SRCS))))
S_OBJS := $(addsuffix .obj, $(basename $(call to_out_path,$(S_SRCS))))

OBJS = $(C_OBJS) $(CPP_OBJS) $(S_OBJS)

ifneq ($(NXDK_CXX),)
include $(NXDK_DIR)/lib/libcxx/Makefile.nxdk
endif

include $(NXDK_DIR)/lib/net/Makefile

ifneq ($(NXDK_SDL),)
include $(NXDK_DIR)/lib/sdl/SDL2/Makefile.xbox
include $(NXDK_DIR)/lib/sdl/Makefile
endif

V = 0
VE_0 := @
VE_1 :=
VE = $(VE_$(V))

ifeq ($(V),1)
QUIET=
else
QUIET=>/dev/null
endif

DEPS := $(patsubst %.obj,%.c.d,$(C_OBJS))
DEPS += $(patsubst %.obj,%.cpp.d,$(CPP_OBJS))

$(OUTPUT_DIR)/default.xbe: main.exe $(OUTPUT_DIR) $(CXBE)
	@echo "[ CXBE     ] $@"
	$(VE)$(CXBE) -OUT:$@ -TITLE:$(XBE_TITLE) $< $(QUIET)

$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR);

ifneq ($(GEN_XISO),)
$(GEN_XISO): $(OUTPUT_DIR)/default.xbe $(EXTRACT_XISO)
	@echo "[ XISO     ] $@"
	$(VE) $(EXTRACT_XISO) -c $(OUTPUT_DIR) $(XISO_FLAGS) "$@" $(QUIET)
endif

$(SRCS): $(SHADER_OBJS)

ifneq ($(NXDK_ONLY),)
.PHONY: main.exe
main.exe: $(OBJS)
else
main.exe: $(OBJS) $(NXDK_DIR)/lib/xboxkrnl/libxboxkrnl.lib
	@echo "[ LD       ] $@"
	$(VE) $(LD) $(NXDK_LDFLAGS) $(LDFLAGS) -out:'$@' $^
endif

%.lib:
	@echo "[ LIB      ] $@"
	$(VE) $(LIB) -out:'$@' $^

%.obj: %.cpp
	@echo "[ CXX      ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CXX) $(NXDK_CXXFLAGS) $(CXXFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.cpp.d,$@)' -c -o '$@' '$<'

%.obj: %.c
	@echo "[ CC       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CC) $(NXDK_CFLAGS) $(CFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.c.d,$@)' -c -o '$@' '$<'

%.obj: %.s
	@echo "[ AS       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(AS) $(NXDK_ASFLAGS) $(ASFLAGS) -c -o '$@' '$<'

ifeq ($(NXDK_OUT_OF_TREE),y)
$(SRC_OUT_DIR_CANONICAL)/%.obj: $(SRC_DIR_CANONICAL)/%.cpp
	@echo "[ CXX      ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CXX) $(NXDK_CXXFLAGS) $(CXXFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.cpp.d,$@)' -c -o '$@' '$<'

$(SRC_OUT_DIR_CANONICAL)/%.obj: $(SRC_DIR_CANONICAL)/%.c
	@echo "[ CC       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CC) $(NXDK_CFLAGS) $(CFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.c.d,$@)' -c -o '$@' '$<'

$(SRC_OUT_DIR_CANONICAL)/%.obj: $(SRC_DIR_CANONICAL)/%.s
	@echo "[ AS       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(AS) $(NXDK_ASFLAGS) $(ASFLAGS) -c -o '$@' '$<'
endif

ifeq ($(NXDK_LIB_OUT_OF_TREE),y)
$(NXDK_LIB_OUT_DIR_CANONICAL)/%.obj: $(NXDK_DIR_CANONICAL)/%.cpp
	@echo "[ CXX      ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CXX) $(NXDK_CXXFLAGS) $(CXXFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.cpp.d,$@)' -c -o '$@' '$<'

$(NXDK_LIB_OUT_DIR_CANONICAL)/%.obj: $(NXDK_DIR_CANONICAL)/%.c
	@echo "[ CC       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CC) $(NXDK_CFLAGS) $(CFLAGS) -MD -MP -MT '$@' -MF '$(patsubst %.obj,%.c.d,$@)' -c -o '$@' '$<'

$(NXDK_LIB_OUT_DIR_CANONICAL)/%.obj: $(NXDK_DIR_CANONICAL)/%.s
	@echo "[ AS       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(AS) $(NXDK_ASFLAGS) $(ASFLAGS) -c -o '$@' '$<'
endif

%.vs.inl: %.vs.cg $(VP20COMPILER)
	@echo "[ CG       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CGC) -profile vp20 -o $@.$$$$ $< $(QUIET) && \
	$(VP20COMPILER) $@.$$$$ > $@ && \
	rm -rf $@.$$$$

%.ps.inl: %.ps.cg $(FP20COMPILER)
	@echo "[ CG       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CGC) -profile fp20 -o $@.$$$$ $< $(QUIET) && \
	$(FP20COMPILER) $@.$$$$ > $@ && \
	rm -rf $@.$$$$

ifeq ($(NXDK_OUT_OF_TREE),y)
$(SRC_OUT_DIR_CANONICAL)/%.vs.inl: $(SRC_DIR_CANONICAL)/%.vs.cg $(VP20COMPILER)
	@echo "[ CG       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CGC) -profile vp20 -o $@.$$$$ $< $(QUIET) && \
	$(VP20COMPILER) $@.$$$$ > $@ && \
	rm -rf $@.$$$$

$(SRC_OUT_DIR_CANONICAL)/%.ps.inl: $(SRC_DIR_CANONICAL)/%.ps.cg $(FP20COMPILER)
	@echo "[ CG       ] $@"
	$(VE)mkdir -p '$(dir $@)'
	$(VE) $(CGC) -profile fp20 -o $@.$$$$ $< $(QUIET) && \
	$(FP20COMPILER) $@.$$$$ > $@ && \
	rm -rf $@.$$$$
endif

tools: $(TOOLS)
.PHONY: tools $(TOOLS)

cxbe: $(CXBE)
$(CXBE):
	@echo "[ BUILD    ] $@"
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/cxbe $(QUIET)

vp20compiler: $(VP20COMPILER)
$(VP20COMPILER):
	@echo "[ BUILD    ] $@"
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/vp20compiler $(QUIET)

fp20compiler: $(FP20COMPILER)
$(FP20COMPILER):
	@echo "[ BUILD    ] $@"
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/fp20compiler $(QUIET)

extract-xiso: $(EXTRACT_XISO)
$(EXTRACT_XISO):
	@echo "[ BUILD    ] $@"
	$(VE)(mkdir $(NXDK_DIR)/tools/extract-xiso/build; \
	cd $(NXDK_DIR)/tools/extract-xiso/build && \
	cmake -G "Unix Makefiles" .. $(QUIET) && \
	$(MAKE) $(QUIET))

.PHONY: clean
clean: $(CLEANRULES)
	$(VE)rm -f $(TARGET) \
	           main.exe main.exe.manifest main.lib \
	           $(OBJS) $(SHADER_OBJS) $(DEPS) \
	           $(GEN_XISO)

.PHONY: distclean
distclean: clean
	$(VE)rm -rf $(NXDK_DIR)/tools/extract-xiso/build
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/fp20compiler distclean $(QUIET)
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/vp20compiler distclean $(QUIET)
	$(VE)$(MAKE) -C $(NXDK_DIR)/tools/cxbe clean $(QUIET)
	$(VE)bash -c "if [ -d $(OUTPUT_DIR) ]; then rmdir $(OUTPUT_DIR); fi"

-include $(DEPS)
