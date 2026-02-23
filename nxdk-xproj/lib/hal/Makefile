HAL_SRCS := \
	$(NXDK_DIR)/lib/hal/audio.c \
	$(NXDK_DIR)/lib/hal/debug.c \
	$(NXDK_DIR)/lib/hal/fileio.c \
	$(NXDK_DIR)/lib/hal/led.c \
	$(NXDK_DIR)/lib/hal/video.c \
	$(NXDK_DIR)/lib/hal/xbox.c

HAL_OBJS = $(call to_lib_obj_path,$(HAL_SRCS))
LIBNXDK_HAL = $(call to_lib_out_path,$(NXDK_DIR)/lib/libnxdk_hal.lib)

$(LIBNXDK_HAL): $(HAL_OBJS)

main.exe: $(LIBNXDK_HAL)

CLEANRULES += clean-hal

.PHONY: clean-hal
clean-hal:
	$(VE)rm -f $(HAL_OBJS) $(LIBNXDK_HAL)
