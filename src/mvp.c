#include "mvp.h"

u16 screen_width = 1280, screen_height = 720;
f32 m_viewport[4][4];
f32_m4x4 m_model, m_view, m_proj, m_mvp;
f32_v4 v_cam_loc = {0, 0, 0, 1};
f32_v4 v_cam_rot = {0, 0, 0, 1};
