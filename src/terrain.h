#pragma once
#include "allocator.h"
#include "pbkit/pbkit.h"
#include "png_loader.h"
#include "nums.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "profileapi.h"
#include "random.h"
#include "shader.h"
#include "mvp.h"
#include "cube.h"
#include "world.h"
#include "timer_util.h"

// One-time allocated vertex color stream reused across all terrain draw calls.
// (Avoids per-batch malloc/free in the hot render loop.)
extern f32_v4 *g_terrain_vertex_colors;

void init_terrain(void);
void cleanup_terrain(void);


void render_terrain();
