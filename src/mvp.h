#pragma once
#include <pbkit/pbkit.h>
#include "math3d.h"
#include "nums.h"

static f32  m_viewport[4][4];

static MATRIX m_model, m_view, m_proj, m_mvp;
static VECTOR v_cam_loc     = {  0,   0, 165,  1 };
static VECTOR v_cam_rot     = {  0,   0,   0,  1 };


static VECTOR v_obj_rot     = {  0,   0,   0,  1 };
static VECTOR v_obj_scale   = {  1,   1,   1,  1 };
static VECTOR v_obj_pos     = {  0,   0,   0,  1 };
