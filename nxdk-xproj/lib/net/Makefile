# Include LWIP sources
LWIPDIR = $(NXDK_DIR)/lib/net/lwip/src
include $(LWIPDIR)/Filelists.mk
NETIFFILES := $(filter-out $(LWIPDIR)/netif/slipif.c,$(NETIFFILES))
LWIPSRCS := $(COREFILES) \
            $(CORE4FILES) \
            $(CORE6FILES) \
            $(APIFILES) \
            $(NETIFFILES)

# Include driver sources
DRIVERSRCS := $(NXDK_DIR)/lib/net/nvnetdrv/nvnetdrv.c \
              $(NXDK_DIR)/lib/net/nvnetdrv/nvnetdrv_lwip.c \
              $(NXDK_DIR)/lib/net/nforceif/src/sys_arch.c

NET_SRCS := \
	$(LWIPSRCS) \
	$(DRIVERSRCS)

NET_OBJS = $(call to_lib_obj_path,$(NET_SRCS))
LIBNXDK_NET = $(call to_lib_out_path,$(NXDK_DIR)/lib/libnxdk_net.lib)

NXDK_CFLAGS += -I$(LWIPDIR)/include
NXDK_CFLAGS += -I$(NXDK_DIR)/lib/net/nforceif/include
NXDK_CFLAGS += -I$(NXDK_DIR)/lib/net/nvnetdrv
NXDK_CXXFLAGS += -I$(LWIPDIR)/include
NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/net/nforceif/include
NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/net/nvnetdrv

$(LIBNXDK_NET): $(NET_OBJS)

main.exe: $(LIBNXDK_NET)

CLEANRULES += clean-net

.PHONY: clean-net
clean-net:
	$(VE)rm -f $(NET_OBJS) $(LIBNXDK_NET)
