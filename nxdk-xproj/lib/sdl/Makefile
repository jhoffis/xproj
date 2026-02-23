SDL_TTF_DIR = $(NXDK_DIR)/lib/sdl/SDL_ttf
SDL_TTF_SRCS += $(SDL_TTF_DIR)/SDL_ttf.c
SDL_TTF_OBJS = $(call to_lib_obj_path,$(SDL_TTF_SRCS))
LIBSDL_TTF = $(call to_lib_out_path,$(NXDK_DIR)/lib/libSDL_ttf.lib)

$(LIBSDL_TTF): $(SDL_TTF_OBJS)

FREETYPE_DIR = $(NXDK_DIR)/lib/sdl/SDL_ttf/external/freetype-2.4.12
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftbase.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftglyph.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftbitmap.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftstroke.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftinit.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/base/ftsystem.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/truetype/truetype.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/raster/raster.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/smooth/smooth.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/autofit/autofit.c
FREETYPE_SRCS += $(FREETYPE_DIR)/src/sfnt/sfnt.c
FREETYPE_OBJS = $(call to_lib_obj_path,$(FREETYPE_SRCS))
LIBFREETYPE = $(call to_lib_out_path,$(NXDK_DIR)/lib/libfreetype.lib)

$(LIBFREETYPE): $(FREETYPE_OBJS)

NXDK_CFLAGS += -I$(NXDK_DIR)/lib/sdl \
               -I$(SDL_TTF_DIR) \
               -I$(FREETYPE_DIR)/include \
               -DXBOX

NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/sdl \
                 -I$(SDL_TTF_DIR) \
                 -I$(FREETYPE_DIR)/include \
                 -DXBOX

main.exe: $(LIBSDL_TTF) $(LIBFREETYPE)

CLEANRULES += clean-sdl_ttf

.PHONY: clean-sdl_ttf
clean-sdl_ttf:
	$(VE)rm -f $(SDL_TTF_OBJS) \
	           $(FREETYPE_OBJS) \
	           $(LIBSDL_TTF) \
	           $(LIBFREETYPE)

LIBSDLIMAGE_DIR = $(NXDK_DIR)/lib/sdl/SDL2_image

LIBSDLIMAGE_SRCS = $(LIBSDLIMAGE_DIR)/IMG.c \
                   $(LIBSDLIMAGE_DIR)/IMG_png.c \
                   $(LIBSDLIMAGE_DIR)/IMG_bmp.c \
                   $(LIBSDLIMAGE_DIR)/IMG_gif.c \
                   $(LIBSDLIMAGE_DIR)/IMG_jpg.c \
                   $(LIBSDLIMAGE_DIR)/IMG_lbm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_pcx.c \
                   $(LIBSDLIMAGE_DIR)/IMG_pnm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_svg.c \
                   $(LIBSDLIMAGE_DIR)/IMG_tga.c \
                   $(LIBSDLIMAGE_DIR)/IMG_tif.c \
                   $(LIBSDLIMAGE_DIR)/IMG_webp.c \
                   $(LIBSDLIMAGE_DIR)/IMG_WIC.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xcf.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xpm.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xv.c \
                   $(LIBSDLIMAGE_DIR)/IMG_xxx.c

LIBSDLIMAGE_OBJS = $(call to_lib_obj_path,$(LIBSDLIMAGE_SRCS))
LIBSDL2_IMAGE = $(call to_lib_out_path,$(NXDK_DIR)/lib/libSDL2_image.lib)
NXDK_CFLAGS += -I$(NXDK_DIR)/lib/sdl/SDL2_image -DLOAD_BMP -DLOAD_GIF -DLOAD_JPG -DLOAD_LBM -DLOAD_PCX -DLOAD_PNG -DLOAD_PNM -DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV -DLOAD_XXX
NXDK_CXXFLAGS += -I$(NXDK_DIR)/lib/sdl/SDL2_image -DLOAD_BMP -DLOAD_GIF -DLOAD_JPG -DLOAD_LBM -DLOAD_PCX -DLOAD_PNG -DLOAD_PNM -DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV -DLOAD_XXX

$(LIBSDL2_IMAGE): $(LIBSDLIMAGE_OBJS)

main.exe: $(LIBSDL2_IMAGE)

CLEANRULES += clean-libsdlimage

.PHONY: clean-libsdlimage
clean-libsdlimage:
	$(VE)rm -f $(LIBSDLIMAGE_OBJS) $(LIBSDL2_IMAGE)
