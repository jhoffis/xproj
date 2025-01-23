#pragma once
#include <pbkit/pbkit.h>
#include "nums.h"

static u16 screen_width = 1280, screen_height = 720;
static f32  m_viewport[4][4];
static f32_m4x4 m_model, m_view, m_proj, m_mvp;
static f32_v4 v_cam_loc   = {0, 0, 0, 1};
static f32_v4 v_cam_rot   = {0, 0, 0, 1};
extern i32 current_chunk_x;
extern i32 current_chunk_y;
extern i32 current_chunk_z;
