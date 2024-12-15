#pragma once
#include "png_loader.h"
#include "nums.h"

#include <string.h>
#include "random.h"
#include "shader.h"
#include "mvp.h"
#include "cube.h"

/*
 * TODO combine two cubes into one buffer, and one index array.
 */

// static ImageData img;
// static void *textureAddr;

// FIXME does not work when running non-statically or directly in main.c
inline static void render_cube(f32 x, f32 y, f32 rotX, f32 rotY) {
    
    u64 abc = lehmer32_seeded(1032487 + 100*x + y);
    abc = abc % 200; 

    v_obj_pos[0] = x;
    v_obj_pos[1] = abc;
    v_obj_pos[2] = y;

    /* Tilt and rotate the object a bit */
    v_obj_rot[0] = rotX;
    v_obj_rot[1] = rotY;

    /* Create local->world matrix given our updated object */
    matrix_unit(m_model);
    matrix_rotate(m_model, m_model, v_obj_rot);
    matrix_scale(m_model, m_model, v_obj_scale);
    matrix_translate(m_model, m_model, v_obj_pos);

    u32 *p = pb_begin();
    /* Set shader constants cursor at C0 */
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

    /* Send the transformation matrix */
    // pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
    // memcpy(p, m_viewport, 16*4); p+=16;

    /* Send the model matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_model, 16*4); p+=16;

    /* Send the view matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_view, 16*4); p+=16;

    /* Send the projection matrix */
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
    memcpy(p, m_proj, 16*4); p+=16;

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

    /*
     * Setup vertex attributes
     */

    u16 cube_indices[] = {
        //Top
        2, 6, 7,
        2, 7, 3,

        //Bottom
        0, 5, 4,
        0, 1, 5,

        //Left
        0, 6, 2,
        0, 4, 6,

        //Right
        1, 3, 7,
        1, 7, 5,

        //Front
        0, 2, 3,
        3, 1, 0,

        //Back
        4, 7, 6,
        4, 5, 7,
        // Top
        10, 14, 15,
        10, 15, 11,

        // Bottom
        8, 13, 12,
        8, 9, 13,

        // Left
        8, 14, 10,
        8, 12, 14,

        // Right
        9, 11, 15,
        9, 15, 13,

        // Front
        8, 10, 11,
        11, 9, 8,

        // Back
        12, 15, 14,
        12, 13, 15
    };
    float cube_vertices[][4] = {
     {-cube_size, -cube_size,  cube_size, 10},
     { cube_size, -cube_size,  cube_size, 10},
     {-cube_size,  cube_size,  cube_size, 10},
     { cube_size,  cube_size,  cube_size, 10},
     {-cube_size, -cube_size, -cube_size, 10},
     { cube_size, -cube_size, -cube_size, 10},
     {-cube_size,  cube_size, -cube_size, 10},
     { cube_size,  cube_size, -cube_size, 10},

     {100 + -cube_size, -cube_size,  cube_size, 10},
     {100 +  cube_size, -cube_size,  cube_size, 10},
     {100 + -cube_size,  cube_size,  cube_size, 10},
     {100 +  cube_size,  cube_size,  cube_size, 10},
     {100 + -cube_size, -cube_size, -cube_size, 10},
     {100 +  cube_size, -cube_size, -cube_size, 10},
     {100 + -cube_size,  cube_size, -cube_size, 10},
     {100 +  cube_size,  cube_size, -cube_size, 10},    
    };
    u32 *abctest = MmAllocateContiguousMemoryEx(sizeof(cube_vertices), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(abctest, cube_vertices, sizeof(cube_vertices));
    /* Set vertex position attribute */
    set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            4, sizeof(float) * 4, &abctest[0]);

    /* Set vertex diffuse color attribute */
    set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(Vertex), &alloc_vertices_cube[3]);

    /* Set texture coordinate attribute */
    set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            2, sizeof(Vertex), &alloc_vertices_cube[6]);

    /* Begin drawing triangles */
    draw_indexed(2*num_cube_indices, cube_indices);
    
    MmFreeContiguousMemory(abctest);
}

inline static void render_terrain(image_data img) {
    init_shader(1);
    {
        /*
         * Setup texture stages
         */

        int channels = 2;
        DWORD format = ((channels & NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) |
               (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR * NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE)) |
               (((2 << 4) & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY)) |
               ((NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8 << 8) & NV097_SET_TEXTURE_FORMAT_COLOR) |
               ((1 << 16) & NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS); 
        DWORD filter = ((4 << 24) & NV097_SET_TEXTURE_FILTER_MAG) | 
                       ((7 << 16) & NV097_SET_TEXTURE_FILTER_MIN) | 
				       ((4 << 12) & NV097_SET_SURFACE_FORMAT_ANTI_ALIASING); // 0x04074000
		DWORD control_enable = NV097_SET_TEXTURE_CONTROL0_ENABLE | NV097_SET_TEXTURE_CONTROL0_MAX_LOD_CLAMP;

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
        u32 *p = pb_begin();
        // Retain the lower 26 bits of the address
        u32 a = NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA;
        p = pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0), img.addr26bits, format); //set stage 0 texture address & format
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),img.pitch<<16); //set stage 0 texture pitch (pitch<<16)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(img.w<<16)|img.h); //set stage 0 texture width & height ((witdh<<16)|height)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),  0x00030303); //set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
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

        f32 dist = 50;
        // for (int y = 0; y < 10; y++) {
        //     for (int x = 0; x < 10; x++) {
                render_cube(0, 0, 0, 0); //  obj_rotationX/1000.0f * M_PI * -0.25f, obj_rotationY/1000.0f * M_PI * -0.25f);
        //     }
        // }
    }
}

void init_terrain();
