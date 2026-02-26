#pragma once

#include "nums.h"
#include "png_loader.h"

typedef enum {
    anchor_tl,
    anchor_bl,
} anchor_e;

void init_ui(void);
void cleanup_ui(void);
void ui_sprite(const image_data *img,
               const f32 x,
               const f32 y,
               const f32 w,
               const f32 h,
               const f32 scale,
               const anchor_e anchor);
