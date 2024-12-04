#pragma once
#include "nums.h"

typedef struct {
    i32 w;
    i32 h;
    i32 comp;
    u8 *image;
} ImageData;


ImageData load_image(const char *name);
