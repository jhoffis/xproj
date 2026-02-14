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

void init_terrain();


/*
 * https://bruop.github.io/improved_frustum_culling/ 
 */
static bool is_point_in_frustum(f32_v3 point, f32_m4x4 viewproj) {

    f32_v4 cs;
    
    // Direct calculation avoiding loops and temporary array
    cs.w =  1.0f / (point.x * viewproj[3] + point.y * viewproj[7] + point.z * viewproj[11] + viewproj[15]);
    cs.x =         (point.x * viewproj[0] + point.y * viewproj[4] + point.z * viewproj[8]  + viewproj[12]) *  cs.w;
    if (!(cs.x >= 0 && cs.x <= screen_width)) return false;
    cs.y =         (point.x * viewproj[1] + point.y * viewproj[5] + point.z * viewproj[9]  + viewproj[13]) *  cs.w;
    if (!(cs.y >= 0 && cs.y <= screen_height)) return false;
    cs.z =         (point.x * viewproj[2] + point.y * viewproj[6] + point.z * viewproj[10] + viewproj[14]) * -cs.w;
    // if (!(cs.z >= -1 && cs.z <= 1)) return false;
    return true;

    // Check if point is inside the frustum in NDC space
    // pb_print("cp x%d y%d z%d w%d IN %d\n", (i32) cs[0], 
    //                                        (i32) cs[1], 
    //                                        (i32) cs[2], 
    //                                        (i32) cs[3], 
    //                                        (i32) inside_view_frustum);
    // return cs.x >= 0   && 
    //        cs.x <= 640 &&
    //        cs.y >= 0   &&
    //        cs.y <= 480 &&
    //        cs.z >= 0   &&
    //        cs.z <= 65536;
}

static bool is_face_in_frustum(face f, f32_m4x4 viewproj) {
    // TODO check if right point is to the left and if left point is to the right>?
    //      and if bottom is above top
    if (is_point_in_frustum(f.vertices[0], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[1], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[2], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[3], viewproj)) return true;

    return false;
}

// Draw one indexed batch.
// - face_count: number of quads (faces)
// - vertex_offset: offset in f32_v3/f32_v2 arrays (in vertices)
// - index_offset_u32: offset in chunk_indices (in u32 words; 3 words per face)
// Push per-frame constants/state once before drawing terrain batches.
static inline void terrain_prepare_draw_state(void) {
    matrix_unit(m_model);
    u32 *p = pb_begin();
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_model, 16 * 4);
    p += 16;

    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_view, 16 * 4);
    p += 16;

    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_proj, 16 * 4);
    p += 16;

    // Clear all attributes (we only enable what we bind for terrain)
    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 16);
    for (u8 i = 0; i < 16; i++) {
        *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    }
    pb_end(p);
}

static void render_cube(u32 face_count, u32 vertex_offset, u32 index_offset_u32) {
    (void)num_faces_pooled;
    // face_stored temp_faces[num];
    // memcpy(temp_faces, faces_pool, num * sizeof(face_stored));
    // u16 *cube_indices = malloc(6 * num * sizeof(u16)); // TODO maybe add these at the end? Isn't this just saying which to render first? well yeah because we know that every 4 verticies is a face. Instead just store direction and distance from camera.

    // f32 *cube_vertices = MmAllocateContiguousMemoryEx(4*num*3 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // f32 *tex_coors = MmAllocateContiguousMemoryEx(4*num*2 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);

    // timer_stamp_print("setup vertex", &win_clock_start);
    // MATRIX vm;
    // matrix_multiply(vm, m_view, m_model);
    // (Per-face culling/frustum tests removed from the hot draw path; do it at chunk/mesh build time.)

    // timer_stamp_print("after setup vertex", &win_clock_start);

    // for (int i = 0; i < num; i++) {
    //     
    //     u8 direction = faces_calculated_pool[i].info;
    //
    //     // if (remove_directions[direction]) continue;
    //
    //     f32_v3 view_dir = vec3_normalize(vec3_subtract((f32 *) &v_cam_loc, (f32 *) &faces_calculated_pool[i].vertices[0]));
    //     f32 dot_prod = vec3_dot_product(face_normals[direction], view_dir);
    //     // if (dot_prod < 0) continue;
    //     // pb_print("viewdir x%d, y%d, z%d dot %d i%d dir%d\n", (i32) (100*view_dir.x), (i32) (100*view_dir.y), (i32) (100*view_dir.z), (i32) (100*dot_prod), i, direction);
    //
    //     fill_face_indices(&cube_indices[n*6], 0, n*4, direction);
    //     // if (!is_face_in_frustum(f, mvp)) continue; // TODO perhaps first frustum cull whole chunks as this calculation is very heavy.
    //
    //     // if (n == 0) {
    //     //     timer_stamp_print("fill face", &win_clock_start);
    //     // }
    //     // 3 000 000 ns ish
    //     // memcpy(&cube_indices[n*6], f.indices, sizeof(u16) * 6);
    //     memcpy(&cube_vertices[n*4*3], faces_calculated_pool[i].vertices, sizeof(f32) * 4 * 3);
    //     memcpy(&tex_coors[n*4*2], faces_calculated_pool[i].tex_coords, sizeof(f32) * 4 * 2); // TODO texture coordinates can be baked into the shader!
    //
    //     // if (n == 0) {
    //     //     timer_stamp_print("indices and memcpys", &win_clock_start);
    //     // }
    //     n++;
    // }
    // n = 16000;
    // memcpy(&cube_vertices[0], chunk_vertices, n * sizeof(f32) * 4 * 3);
    // timer_stamp_print("copy test", &win_clock_start);
    // pb_print("rendered faces: %d, vi %d\n", n, vertex_i);

    // u32 real_size_of_verts = sizeof(f32) * 4 * n * 3;
    // u32 *allocated_verts = MmAllocateContiguousMemoryEx(4*num*3 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // memcpy(allocated_verts, cube_vertices, real_size_of_verts);
    // u32 real_size_of_texs = sizeof(f32) * 4 * n * 2;
    // u32 *allocated_texs = MmAllocateContiguousMemoryEx(real_size_of_texs, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // memcpy(allocated_texs, chunk_tex_coords, real_size_of_texs);
    /* Set vertex position attribute */
    set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(float) * 3, &chunk_vertices[vertex_offset]);
    

    /* Set vertex diffuse color attribute (shared static buffer) */
    set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(f32_v4), g_terrain_vertex_colors);

    /* Set texture coordinate attribute */
    set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            2, sizeof(float) * 2, &chunk_tex_coords[vertex_offset]);

    // debugPrint("spot2\n");
    // timer_stamp_print("calculate vertices", &win_clock_start);

    /* Begin drawing triangles */
    draw_indexed(face_count * 3u, &chunk_indices[index_offset_u32]);
    // draw_arrays(g_render_method, 0, n);
    // MmFreeContiguousMemory(cube_vertices);
    // MmFreeContiguousMemory(allocated_texs);
    // free(cube_vertices);
    // free(tex_coors);
    // g_terrain_vertex_colors is reused; no free here.

    // timer_stamp_print("after drawn", &win_clock_start);
}

inline static void render_terrain() {
    init_shader(SHADER_TERRAIN);
    // Safety: allow render_terrain() even if init_terrain() wasn't called.
    if (!g_terrain_vertex_colors) {
        init_terrain();
    }
    terrain_prepare_draw_state();
    for (int i = 0; i < FACE_TYPE_AMOUNT; i++) {
        image_data *img;
        u8 u, v;
        int channels;
        DWORD format, filter, control_enable;
        u32 *p;

        if (num_faces_type[i] == 0) continue;
        pb_print("faces: %d, %d\n", num_faces_type[i], i);
 
        img = get_cube_texture(i);

        u = fast_log2(img->w);
        v = fast_log2(img->h);

        /*
         * Setup texture stages
         */
        channels = 2;
        format = ((channels & NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) |
                       (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR * NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE)) | // 0x0000000F
                       (((2 << 4) & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY)) | // 0x000000F0
                       ((NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8 << 8) & NV097_SET_TEXTURE_FORMAT_COLOR) | // 0x0000FF00
                       ((1 << 16) & NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS) | // 0x000F0000
                       ((u << 20) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U) | // I manually did 512 (which is the width of my texture) and log2 (so up to 16^2)
                       ((v << 24) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U               0x00F00000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V               0x0F000000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P               0xF0000000
        filter = ((4 << 24) & NV097_SET_TEXTURE_FILTER_MAG) | 
                       ((7 << 16) & NV097_SET_TEXTURE_FILTER_MIN) | 
				       ((4 << 12) & NV097_SET_SURFACE_FORMAT_ANTI_ALIASING); // 0x04074000
        filter = 0x01014000; // nearest neighbor
		control_enable = NV097_SET_TEXTURE_CONTROL0_ENABLE | NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP;

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
        p = pb_begin();
        // Retain the lower 26 bits of the address
        p = pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0), img->addr26bits, format); //set stage 0 texture address & format
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),img->pitch<<16); //set stage 0 texture pitch (pitch<<16)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(img->w<<16)|img->h); //set stage 0 texture width & height ((witdh<<16)|height)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),  0x00010101); //set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0), control_enable); //set stage 0 texture enable flags
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0), filter); //set stage 0 texture filters (AA!)
        pb_end(p);

        /* Disable other texture stages */
        p = pb_begin();
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1), 0x0003ffc0);//set stage 1 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2), 0x0003ffc0);//set stage 2 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3), 0x0003ffc0);//set stage 3 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),   0x00030303);//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),   0x00030303);//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),   0x00030303);//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1), 0x02022000);//set stage 1 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2), 0x02022000);//set stage 2 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3), 0x02022000);//set stage 3 texture filters (no AA, stage not even used)
        pb_end(p);


        // Use the batching info generated by world.c (correct for 16-bit index limits)
        for (u32 b = 0; b < (u32)num_face_batches[i]; b++) {
            const face_batch *fb = &face_batches[i][b];
            const u32 faces_in_batch = fb->vertex_count / 4u;
            if (faces_in_batch == 0) continue;
            render_cube(faces_in_batch, fb->first_vertex, fb->first_index_u32);
        }
        // break;
    }
}
