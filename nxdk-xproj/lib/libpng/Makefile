LIBPNG_DIR = $(NXDK_DIR)/lib/libpng/libpng

LIBPNG_SRCS = $(LIBPNG_DIR)/png.c \
              $(LIBPNG_DIR)/pngerror.c \
              $(LIBPNG_DIR)/pngget.c \
              $(LIBPNG_DIR)/pngmem.c \
              $(LIBPNG_DIR)/pngpread.c \
              $(LIBPNG_DIR)/pngread.c \
              $(LIBPNG_DIR)/pngrio.c \
              $(LIBPNG_DIR)/pngrtran.c \
              $(LIBPNG_DIR)/pngrutil.c \
              $(LIBPNG_DIR)/pngset.c \
              $(LIBPNG_DIR)/pngtrans.c \
              $(LIBPNG_DIR)/pngwio.c \
              $(LIBPNG_DIR)/pngwrite.c \
              $(LIBPNG_DIR)/pngwtran.c \
              $(LIBPNG_DIR)/pngwutil.c

LIBPNG_OBJS = $(call to_lib_obj_path,$(LIBPNG_SRCS))
LIBPNG_LIB = $(call to_lib_out_path,$(NXDK_DIR)/lib/libpng.lib)

NXDK_CFLAGS += -I$(LIBPNG_DIR) -I$(NXDK_DIR)/lib/libpng

$(LIBPNG_LIB): $(LIBPNG_OBJS)

main.exe: $(LIBPNG_LIB)

CLEANRULES += clean-libpng

.PHONY: clean-libpng
clean-libpng:
	$(VE)rm -f $(LIBPNG_OBJS) $(LIBPNG_LIB)
