#pragma once
#include "nums.h"

typedef struct {
    i32 w; // TODO make unsigned
    i32 h;
    u32 pitch;
    i32 comp;
    u8 *image;
    u32 length;
} ImageData;


ImageData load_image(const char *name);
