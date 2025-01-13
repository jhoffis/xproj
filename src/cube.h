#pragma once

#include "nums.h"
#include "png_loader.h"

#define BLOCK_SIZE 50
#define BLOCK_TEXTURE_SIZE 2
#define BLOCK_TYPE_MASK 0x00000FFF
// empty:
#define BLOCK_TYPE_AIR -1
// multiples:
#define BLOCK_TYPE_GRASS 0
// singles:
#define BLOCK_TYPE_DIRT 1

typedef struct {
    u32 type; // 12 first bits represent the block type? 20 bits rest are for more information. Like for instance, flowing or smt..
} cube_entity;


void init_cubes(void);
void destroy_cubes(void);
image_data *get_cube_texture(u32 type, u8 direction);

