WINMM_SRCS := \
	$(NXDK_DIR)/lib/winapi/winmm/timeapi.c

WINMM_OBJS = $(call to_lib_obj_path,$(WINMM_SRCS))
WINMM_LIB = $(call to_lib_out_path,$(NXDK_DIR)/lib/winmm.lib)

NXDK_CFLAGS += -I$(NXDK_DIR)/lib/winapi/winmm
NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/winapi/winmm

$(WINMM_LIB): $(WINMM_OBJS)

main.exe: $(WINMM_LIB)

CLEANRULES += clean-winmm

.PHONY: clean-winmm
clean-winmm:
	$(VE)rm -f $(WINMM_OBJS) $(WINMM_LIB)
