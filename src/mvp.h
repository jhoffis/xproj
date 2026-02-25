#pragma once
#include <pbkit/pbkit.h>
#include "math3d.h"
#include "nums.h"

extern u16 screen_width, screen_height;
extern f32  m_viewport[4][4];
extern f32_m4x4 m_model, m_view, m_proj, m_mvp;
extern f32_v4 v_cam_loc;
extern f32_v4 v_cam_rot;
