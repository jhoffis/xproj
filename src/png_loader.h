#pragma once
#include "nums.h"

typedef struct {
    u16 w;
    u16 h;
    u16 pitch;
    i32 comp;
    u8 *image;
    u32 addr26bits;
} image_data;

image_data load_image(const char *name);
