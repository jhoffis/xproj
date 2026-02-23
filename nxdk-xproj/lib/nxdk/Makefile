NXDK_SRCS := \
	$(NXDK_DIR)/lib/nxdk/configsector.c \
	$(NXDK_DIR)/lib/nxdk/format.c \
	$(NXDK_DIR)/lib/nxdk/mount.c \
	$(NXDK_DIR)/lib/nxdk/net.c \
	$(NXDK_DIR)/lib/nxdk/path.c \
	$(NXDK_DIR)/lib/nxdk/xbe.c

NXDK_OBJS = $(call to_lib_obj_path,$(NXDK_SRCS))
LIBNXDK = $(call to_lib_out_path,$(NXDK_DIR)/lib/libnxdk.lib)

$(LIBNXDK): $(NXDK_OBJS)

main.exe: $(LIBNXDK)

CLEANRULES += clean-nxdk

.PHONY: clean-nxdk
clean-nxdk:
	$(VE)rm -f $(NXDK_OBJS) $(LIBNXDK)

NXDK_AUTOMOUNT_D_SRCS := \
	$(NXDK_DIR)/lib/nxdk/automount_d.c

NXDK_AUTOMOUNT_D_OBJS = $(call to_lib_obj_path,$(NXDK_AUTOMOUNT_D_SRCS))
LIBNXDK_AUTOMOUNT_D = $(call to_lib_out_path,$(NXDK_DIR)/lib/libnxdk_automount_d.lib)

$(LIBNXDK_AUTOMOUNT_D): $(NXDK_AUTOMOUNT_D_OBJS)

ifneq ($(NXDK_DISABLE_AUTOMOUNT_D),y)
main.exe: $(LIBNXDK_AUTOMOUNT_D)
NXDK_LDFLAGS += -include:_automount_d_drive
endif

CLEANRULES += clean-nxdk-automount-d

.PHONY: clean-nxdk-automount-d
clean-nxdk-automount-d:
	$(VE)rm -f $(NXDK_AUTOMOUNT_D_OBJS) $(LIBNXDK_AUTOMOUNT_D)
