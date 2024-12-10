#pragma once
#include "nums.h"

typedef struct {
    u16 w; // TODO make unsigned
    u16 h;
    u16 pitch;
    i32 comp;
    u8 *image; // TODO maybe make this the MmAllocateContiguousMemoryEx part only and delete this array after fully loaded textureaddress?
    // u16 w; // TODO make unsigned
    // u16 h;
    // u16 pitch;
    // void *data;
    u32 addr26bits;
} image_data;

image_data load_image(const char *name);
