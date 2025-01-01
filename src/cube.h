#pragma once

#include "nums.h"

#define BLOCK_TYPE_AIR 0 // TODO don't have this
#define BLOCK_TYPE_GRASS 1
#define BLOCK_TYPE_MASK 0x00000FFF
#define BLOCK_SIZE 50
#define BLOCK_TEXTURE_SIZE 2

typedef struct {
    u32 type; // 12 first bits represent the block type? 20 bits rest are for more information. Like for instance, flowing or smt..
} cube_entity;

static const f32 cube_size = 50.f;
static const f32 cube_tex_w = 2, cube_tex_h = 2;
