PBKIT_SRCS := \
	$(NXDK_DIR)/lib/pbkit/pbkit.c \
	$(NXDK_DIR)/lib/pbkit/pbkit_gamma.c \
	$(NXDK_DIR)/lib/pbkit/pbkit_dma.c \
	$(NXDK_DIR)/lib/pbkit/pbkit_draw.c \
	$(NXDK_DIR)/lib/pbkit/pbkit_print.c \
	$(NXDK_DIR)/lib/pbkit/pbkit_pushbuffer.c

PBKIT_OBJS = $(call to_lib_obj_path,$(PBKIT_SRCS))
PBKIT_LIB = $(call to_lib_out_path,$(NXDK_DIR)/lib/libpbkit.lib)

$(PBKIT_LIB): $(PBKIT_OBJS)

main.exe: $(PBKIT_LIB)

CLEANRULES += clean-pbkit

.PHONY: clean-pbkit
clean-pbkit:
	$(VE)rm -f $(PBKIT_OBJS) $(PBKIT_LIB)
