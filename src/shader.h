#pragma once
#include "nums.h"
#include <pbkit/pbkit.h>

extern u8 g_render_method;

#define SHADER_VARIANT_TABLE(X) \
    X(SHADER_BASIC, basic) \
    X(SHADER_TERRAIN, terrain)

typedef enum shader_variant {
#define SHADER_VARIANT_ENUM_ITEM(enum_name, name) enum_name,
    SHADER_VARIANT_TABLE(SHADER_VARIANT_ENUM_ITEM)
#undef SHADER_VARIANT_ENUM_ITEM
    SHADER_COUNT
} shader_variant;

void init_shader(shader_variant which);
void set_attrib_pointer(u32 index, u32 format, u32 size, u32 stride, const void* data);
void draw_arrays(u32 mode, s32 start, s32 count);
void draw_indexed(u32 num_cube_indices, u32 *cube_indices);

