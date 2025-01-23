#pragma once
#include "allocator.h"
#include "math3d.h"
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

static void render_cube(u32 n, u32 vertex_offset, u32 index_offset) {
    // LARGE_INTEGER win_clock_frequency, win_clock_start, win_clock_end;
    // QueryPerformanceFrequency(&win_clock_frequency); // Get the frequency of the counter
    // QueryPerformanceCounter(&win_clock_start);      // Record start time
    f32_v4 v_obj_pos   = {0,0,0,1}; 
    f32_v4 v_obj_rot   = {0,0,0,1}; 
    f32_v4 v_obj_scale = {1,1,1,1}; 
    v_obj_pos.x = 0;
    v_obj_pos.y = 0;
    v_obj_pos.z = 0;

    /* Tilt and rotate the object a bit */
    v_obj_rot.x = 0;
    v_obj_rot.y = 0;

    f32_v3 camera_position = {v_cam_loc.x, v_cam_loc.y, v_cam_loc.z};

    // Forward vector in view space
    f32_v3 forward_vector_view = {0.0f, 0.0f, -1.0f};

    // Compute the rotation matrix
    f32_m3x3 rotation_matrix;
    mat3x3_euler_to_rotation_matrix(rotation_matrix, v_cam_rot.x, v_cam_rot.y, v_cam_rot.z);

    // Transform the forward vector to world space
    f32_v3 camera_normal_world = vec3_multiply_mat3x3(rotation_matrix, forward_vector_view);

    // Normalize the resulting vector
    camera_normal_world = vec3_normalize(camera_normal_world);
    // pb_print("normal x%d y%d z%d \n", (i32) (1000 * camera_normal_world.x), 
    //                                   (i32) (1000 * camera_normal_world.y), 
    //                                   (i32) (1000 * camera_normal_world.z));

    /* Create local->world matrix given our updated object */
    matrix_unit(m_model);
    // matrix_rotate(m_model, m_model, v_obj_rot);
    // matrix_scale(m_model, m_model, v_obj_scale);
    // matrix_translate(m_model, m_model, v_obj_pos);

    u32 *p = pb_begin();
    /* Set shader constants cursor at C0 */
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

    /* Send the transformation matrix */
    // pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
    // memcpy(p, m_viewport, 16*4); p+=16;

    /* Send the model matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_model, 16*4); 
    p += 16;

    /* Send the view matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_view, 16*4); 
    p += 16;

    /* Send the projection matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_proj, 16*4); 
    p += 16;

    /* Send camera position */
    // pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
    // memcpy(p, v_cam_loc, 4*4); p+=4;

    // float constants_0[2] = {0, 1,};
    // pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 8);
    // memcpy(p, constants_0, 8); p+=8;

    /* Clear all attributes */
    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT,16);
    for(u8 i = 0; i < 16; i++) {
        *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    }
    pb_end(p);

    // debugPrint("spot1\n");
    // timer_stamp_print("setup pb", &win_clock_start);
    /*
     * Setup vertex attributes
     */
    int num = num_faces_pooled < FACE_POOL_SIZE ? num_faces_pooled : FACE_POOL_SIZE;
    // face_stored temp_faces[num];
    // memcpy(temp_faces, faces_pool, num * sizeof(face_stored));
    // u16 *cube_indices = malloc(6 * num * sizeof(u16)); // TODO maybe add these at the end? Isn't this just saying which to render first? well yeah because we know that every 4 verticies is a face. Instead just store direction and distance from camera.

    // f32 *cube_vertices = MmAllocateContiguousMemoryEx(4*num*3 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // f32 *tex_coors = MmAllocateContiguousMemoryEx(4*num*2 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);

    // timer_stamp_print("setup vertex", &win_clock_start);
    // MATRIX vm;
    // matrix_multiply(vm, m_view, m_model);
    f32_m4x4 mvp;
    matrix_multiply(mvp, m_view, m_proj);

    f32_v3 face_normals[FACE_DIRECTION_TOTAL];
    bool remove_directions[FACE_DIRECTION_TOTAL];
    for (int d = 0; d < FACE_DIRECTION_TOTAL; d++) { // TODO check if the object is more than 1 distance away and has the same direction because then it should not be visible.
                                  // basically, if object has same direction and is ahead by 1 then remove.
        f32_v3 face_normal = {0};
        switch (d) {
            case FACE_DIRECTION_UP:
                face_normal.y = 1;
                break;
            case FACE_DIRECTION_DOWN:
                face_normal.y = -1;
                break;
            case FACE_DIRECTION_EAST:
                face_normal.x = 1;
                break;
            case FACE_DIRECTION_WEST:
                face_normal.x = -1;
                break;
            case FACE_DIRECTION_NORTH:
                face_normal.z = 1;
                break;
            case FACE_DIRECTION_SOUTH:
                face_normal.z = -1;
                break;
        }
        remove_directions[d] = vec3_is_same_direction(camera_normal_world, face_normal, 0.7f);
        face_normals[d] = face_normal;
    }

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
    

    f32_v4 *colors = xmalloc(n * sizeof(f32_v4));
    memset(colors, 3, n * sizeof(f32_v4));
    for (int a = 0; a < n; a++) {
        colors[a].x = 0.5;
        colors[a].y = 0.5;
    }

    /* Set vertex diffuse color attribute */
    set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(f32_v4), colors);

    /* Set texture coordinate attribute */
    set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            2, sizeof(float) * 2, &chunk_tex_coords[vertex_offset]);

    // debugPrint("spot2\n");
    // timer_stamp_print("calculate vertices", &win_clock_start);

    /* Begin drawing triangles */
    draw_indexed(n*3, &chunk_indices[index_offset]);
    // draw_arrays(g_render_method, 0, n);
    // MmFreeContiguousMemory(cube_vertices);
    // MmFreeContiguousMemory(allocated_texs);
    // free(cube_vertices);
    // free(tex_coors);
    xfree(colors);

    // timer_stamp_print("after drawn", &win_clock_start);
}

inline static void render_terrain() {
    init_shader(1);
    for (int i = 0; i < FACE_TYPE_AMOUNT; i++) {

        if (num_faces_type[i] == 0) continue;
        pb_print("faces: %d, %d\n", num_faces_type[i], i);
 
        image_data *img = get_cube_texture(i);

        u8 u = fast_log2(img->w);
        u8 v = fast_log2(img->h);

        /*
         * Setup texture stages
         */
        int channels = 2;
        DWORD format = ((channels & NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) |
                       (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR * NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE)) | // 0x0000000F
                       (((2 << 4) & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY)) | // 0x000000F0
                       ((NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8 << 8) & NV097_SET_TEXTURE_FORMAT_COLOR) | // 0x0000FF00
                       ((1 << 16) & NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS) | // 0x000F0000
                       ((u << 20) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U) | // I manually did 512 (which is the width of my texture) and log2 (so up to 16^2)
                       ((v << 24) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U               0x00F00000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V               0x0F000000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P               0xF0000000
        DWORD filter = ((4 << 24) & NV097_SET_TEXTURE_FILTER_MAG) | 
                       ((7 << 16) & NV097_SET_TEXTURE_FILTER_MIN) | 
				       ((4 << 12) & NV097_SET_SURFACE_FORMAT_ANTI_ALIASING); // 0x04074000
        filter = 0x01014000; // nearest neighbor
		DWORD control_enable = NV097_SET_TEXTURE_CONTROL0_ENABLE | NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP;

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
        u32 *p = pb_begin();
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


        u32 num_batch;
        i32 num = num_faces_type[i];
        u32 offset_v = 0;
        u32 offset_i = 0;
        
        if (i != 0) {
            offset_v = offset_vertices[i-1];
            offset_i = offset_indices[i-1];
        }

        const u32 max_faces = 16384;
        u32 times = 0;
        do {
            if (num > max_faces) {
                num_batch = max_faces;
            } else  {
                num_batch = num + 8*3;
                // if (num_batch + times > MAX_RENDERED_FACES) num_batch = MAX_RENDERED_FACES - times;
            }

            render_cube(num_batch, offset_v, offset_i);

            num -= max_faces;
            offset_v += MAX_VERTICES;
            offset_i += 49152; // Align to next multiple of 4 because of how shader.c works
        } while (num > 0);
        // break;
    }
}

void init_terrain();
