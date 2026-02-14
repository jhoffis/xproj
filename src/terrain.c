#include "terrain.h"

// Shared vertex color stream (see terrain.h)
f32_v4 *g_terrain_vertex_colors = NULL;

void init_terrain() {
    // img = load_image("grass");
    // // u8 imggg[] = {
    // //              0, 255,   0,   0xff, 
    // //              0, 255, 110,   0xff,
    // //              0, 0,   255,   0xff,
    // //              0, 100,     5, 0xff};
    // //              // 255,   0,   0, 0xff, 
    // //              // 255, 110,   0, 0xff,
    // //              // 0,   255,   0, 0xff,
    // //              // 100,     5, 0, 0xff};
    // // img.image = imggg;
    // // img.w = 2;
    // // img.h = 2;
    // // img.pitch = img.w * 4;
    // textureAddr = MmAllocateContiguousMemoryEx(img.pitch * img.h, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // memcpy(textureAddr, img.image, img.pitch * img.h); // TODO use img.length (whatever that is...)
    //
    // Allocate a reusable per-vertex color stream (one batch = MAX_VERTICES verts).
    // This replaces the old per-draw malloc/free in render_cube().
    if (!g_terrain_vertex_colors) {
        g_terrain_vertex_colors = xmalloc(MAX_VERTICES * sizeof(f32_v4));
        for (u32 i = 0; i < MAX_VERTICES; i++) {
            g_terrain_vertex_colors[i] = (f32_v4){ 1.0f, 1.0f, 1.0f, 1.0f };
        }
    }
}
