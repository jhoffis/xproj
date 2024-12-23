#pragma once
#include "png_loader.h"
#include "nums.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"
#include "shader.h"
#include "mvp.h"
#include "cube.h"
#include "world.h"

/*
 * TODO combine two cubes into one buffer, and one index array.
 */

// static ImageData img;
// static void *textureAddr;

// static void fill_array_cube_indices(u16 *indices, u16 num_cubes) {
//     for (int i = 0; i < num_cubes; i++) {
//         int n = 36*i;
//         int cubeN = 8*i;

//         // Top
//         indices[n]     = cubeN + 2;
//         indices[n + 1] = cubeN + 6;
//         indices[n + 2] = cubeN + 7;
//         indices[n + 3] = cubeN + 2;
//         indices[n + 4] = cubeN + 7;
//         indices[n + 5] = cubeN + 3;

//         // Bottom
//         indices[n + 6]  = cubeN + 0;
//         indices[n + 7]  = cubeN + 5;
//         indices[n + 8]  = cubeN + 4;
//         indices[n + 9]  = cubeN + 0;
//         indices[n + 10] = cubeN + 1;
//         indices[n + 11] = cubeN + 5;

//         // Left
//         indices[n + 12] = cubeN + 0;
//         indices[n + 13] = cubeN + 6;
//         indices[n + 14] = cubeN + 2;
//         indices[n + 15] = cubeN + 0;
//         indices[n + 16] = cubeN + 4;
//         indices[n + 17] = cubeN + 6;

//         // Right
//         indices[n + 18] = cubeN + 1;
//         indices[n + 19] = cubeN + 3;
//         indices[n + 20] = cubeN + 7;
//         indices[n + 21] = cubeN + 1;
//         indices[n + 22] = cubeN + 7;
//         indices[n + 23] = cubeN + 5;

//         // Front
//         indices[n + 24] = cubeN + 0;
//         indices[n + 25] = cubeN + 2;
//         indices[n + 26] = cubeN + 3;
//         indices[n + 27] = cubeN + 3;
//         indices[n + 28] = cubeN + 1;
//         indices[n + 29] = cubeN + 0;

//         // Back
//         indices[n + 30] = cubeN + 4;
//         indices[n + 31] = cubeN + 7;
//         indices[n + 32] = cubeN + 6;
//         indices[n + 33] = cubeN + 4;
//         indices[n + 34] = cubeN + 5;
//         indices[n + 35] = cubeN + 7;
//     }
// }

// static void fill_array_singular_cube_vertices(u32 offset, float cubes[][5], float x, float y, float z) {
//     offset *= 8;
//     x *= 2*cube_size;
//     y *= 2*cube_size;
//     z *= 2*cube_size;
//     cubes[offset][0] = x + -cube_size;
//     cubes[offset][1] = y + -cube_size;
//     cubes[offset][2] = z +  cube_size;
//     cubes[offset][3] = 0; // Texture
//     cubes[offset][4] = 0;
    
//     cubes[offset + 1][0] = x +  cube_size;
//     cubes[offset + 1][1] = y + -cube_size;
//     cubes[offset + 1][2] = z +  cube_size;
//     cubes[offset + 1][3] = cube_tex_w;
//     cubes[offset + 1][4] = 0;
    
//     cubes[offset + 2][0] = x + -cube_size;
//     cubes[offset + 2][1] = y +  cube_size;
//     cubes[offset + 2][2] = z +  cube_size;
//     cubes[offset + 2][3] = 0;
//     cubes[offset + 2][4] = cube_tex_h;
    
//     cubes[offset + 3][0] = x +  cube_size;
//     cubes[offset + 3][1] = y +  cube_size;
//     cubes[offset + 3][2] = z +  cube_size;
//     cubes[offset + 3][3] = cube_tex_w;
//     cubes[offset + 3][4] = cube_tex_h;

//     cubes[offset + 4][0] = x + -cube_size;
//     cubes[offset + 4][1] = y + -cube_size;
//     cubes[offset + 4][2] = z + -cube_size;
//     cubes[offset + 4][3] = cube_tex_w;
//     cubes[offset + 4][4] = cube_tex_h;

//     cubes[offset + 5][0] = x +  cube_size;
//     cubes[offset + 5][1] = y + -cube_size;
//     cubes[offset + 5][2] = z + -cube_size;
//     cubes[offset + 5][3] = 0;
//     cubes[offset + 5][4] = cube_tex_h;

//     cubes[offset + 6][0] = x + -cube_size;
//     cubes[offset + 6][1] = y +  cube_size;
//     cubes[offset + 6][2] = z + -cube_size;
//     cubes[offset + 6][3] = cube_tex_w;
//     cubes[offset + 6][4] = 0;

//     cubes[offset + 7][0] = x +  cube_size;
//     cubes[offset + 7][1] = y +  cube_size;
//     cubes[offset + 7][2] = z + -cube_size;
//     cubes[offset + 7][3] = 0;
//     cubes[offset + 7][4] = 0;
// }


// /*
//  * TODO replace this because in the future we want to reuse vertices 
//  * and here we expect 4 new verticies for each face.
//  */
// static void fill_array_face_indices(u16 *indices, u16 num_faces) {
//     for (int i = 0; i < num_faces; i++) {
//         int n = 6*i;
//         int faceN = 4*i; // 4 vertices per face

//         // Top (FIXME add all directions... Probably by sending a face struct array and providing the info etc.)
//         indices[n + 0] = faceN + 1;
//         indices[n + 1] = faceN + 3;
//         indices[n + 2] = faceN + 2;
//         indices[n + 3] = faceN + 0;
//         indices[n + 4] = faceN + 3;
//         indices[n + 5] = faceN + 1;
//     }
// }

// static void fill_array_singular_face_vertices(u32 offset, float cubes[][5], float x, float y, float z) {
//     offset *= 4;
//     x *= 2*cube_size;
//     y *= 2*cube_size;
//     z *= 2*cube_size;

//     cubes[offset + 0][0] = x + -cube_size;
//     cubes[offset + 0][1] = y +  cube_size;
//     cubes[offset + 0][2] = z +  cube_size;
//     cubes[offset + 0][3] = 0;
//     cubes[offset + 0][4] = cube_tex_h;
    
//     cubes[offset + 1][0] = x +  cube_size;
//     cubes[offset + 1][1] = y +  cube_size;
//     cubes[offset + 1][2] = z +  cube_size;
//     cubes[offset + 1][3] = cube_tex_w;
//     cubes[offset + 1][4] = cube_tex_h;

//     cubes[offset + 2][0] = x + -cube_size;
//     cubes[offset + 2][1] = y +  cube_size;
//     cubes[offset + 2][2] = z + -cube_size;
//     cubes[offset + 2][3] = cube_tex_w;
//     cubes[offset + 2][4] = 0;

//     cubes[offset + 3][0] = x +  cube_size;
//     cubes[offset + 3][1] = y +  cube_size;
//     cubes[offset + 3][2] = z + -cube_size;
//     cubes[offset + 3][3] = 0;
//     cubes[offset + 3][4] = 0;
// }

// /*
//  * Quite inaccurate for now.
//  */
// static face find_full_face(int start_x, int start_y, int start_z, u8 face_direction) {
//     const int max_len = 16;
//     face res = {0};
//     bool found = false;
//     if (face_direction <= FACE_DIRECTION_UP) {
//         int max_z = start_z + 1; // the other size of the x tile
//         for (int z = start_z + 1; z < max_len; z++) {
//             // TODO perhaps check if covered or smt.
//             if (test_chunk.cubes[start_x][start_y][z].type != BLOCK_TYPE_GRASS) {
//                 max_z = z;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_z = max_len;
//         }
//         found = false;
//         int max_x = start_x + 1; // andre siden av x tile
//         for (int x = start_x + 1; x < max_len; x++) {
//             // TODO check if from this x it crashes anywhere towards the max z.
//             if (test_chunk.cubes[x][start_y][start_z].type != BLOCK_TYPE_GRASS) {
//                 max_x = x;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_x = max_len;
//         }
//         switch (face_direction) {
//             case FACE_DIRECTION_DOWN:
//                 start_y -= 1; // move to the bottom.
//                 res.indices[0] = 1;
//                 res.indices[1] = 2;
//                 res.indices[2] = 3;
//                 res.indices[3] = 0;
//                 res.indices[4] = 1;
//                 res.indices[5] = 3;
//                 break;
//             case FACE_DIRECTION_UP:
//                 res.indices[0] = 1;
//                 res.indices[1] = 3;
//                 res.indices[2] = 2;
//                 res.indices[3] = 0;
//                 res.indices[4] = 3;
//                 res.indices[5] = 1;
//                 break;
//         }

//         int x0 = start_x * 2*cube_size;
//         int x1 = max_x * 2*cube_size;
//         int y = start_y * 2*cube_size;
//         int z0 = start_z * 2*cube_size;
//         int z1 = max_z * 2*cube_size;

//         res.vertices[0][0] = x0;
//         res.vertices[0][1] = y;
//         res.vertices[0][2] = z1;
//         res.vertices[0][3] = 0;
//         res.vertices[0][4] = (max_z - start_z);

//         res.vertices[1][0] = x1;
//         res.vertices[1][1] = y;
//         res.vertices[1][2] = z1;
//         res.vertices[1][3] = (max_x - start_x);
//         res.vertices[1][4] = (max_z - start_z);

//         res.vertices[2][0] = x1;
//         res.vertices[2][1] = y;
//         res.vertices[2][2] = z0;
//         res.vertices[2][3] = (max_x - start_x);
//         res.vertices[2][4] = 0;

//         res.vertices[3][0] = x0; // TODO flytt denne til nullte vertex
//         res.vertices[3][1] = y;
//         res.vertices[3][2] = z0;
//         res.vertices[3][3] = 0;
//         res.vertices[3][4] = 0;
//     } else if (face_direction <= FACE_DIRECTION_NORTH) {        // Sides
//         int max_x = start_x + 1; // the other size of the x tile
//         for (int x = start_x + 1; x < max_len; x++) {
//             // TODO perhaps check if covered or smt.
//             if (test_chunk.cubes[x][start_y][start_z].type != BLOCK_TYPE_GRASS) {
//                 max_x = x;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_x = max_len;
//         }
//         found = false;
//         int max_y = start_y + 1; // andre siden av x tile
//         for (int y = start_y + 1; y < max_len; y++) {
//             // TODO check if from this x it crashes anywhere towards the max z.
//             if (test_chunk.cubes[start_x][y][start_z].type != BLOCK_TYPE_GRASS) {
//                 max_y = y;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_y = max_len;
//         }

//         switch (face_direction) {
//             case FACE_DIRECTION_NORTH:
//                 res.indices[0] = 0;
//                 res.indices[1] = 2;
//                 res.indices[2] = 3;
//                 res.indices[3] = 0;
//                 res.indices[4] = 1;
//                 res.indices[5] = 2;
//                 break;
//             case FACE_DIRECTION_SOUTH:
//                 start_z += 1;
//                 res.indices[0] = 0;
//                 res.indices[1] = 3;
//                 res.indices[2] = 2;
//                 res.indices[3] = 0;
//                 res.indices[4] = 2;
//                 res.indices[5] = 1;
//                 break;
//         }
//         int x0 = start_x * 2*cube_size;
//         int x1 = max_x * 2*cube_size;
//         int y0 = (start_y - 1) * 2*cube_size;
//         int y1 = (max_y - 1) * 2*cube_size;
//         int z = start_z * 2*cube_size;

//         res.vertices[0][0] = x1;
//         res.vertices[0][1] = y0;
//         res.vertices[0][2] = z;
//         res.vertices[0][3] = 0;
//         res.vertices[0][4] = (max_x - start_x);

//         res.vertices[1][0] = x1;
//         res.vertices[1][1] = y1;
//         res.vertices[1][2] = z;
//         res.vertices[1][3] = (max_y - start_y);
//         res.vertices[1][4] = (max_x - start_x);

//         res.vertices[2][0] = x0;
//         res.vertices[2][1] = y1;
//         res.vertices[2][2] = z;
//         res.vertices[2][3] = (max_y - start_y);
//         res.vertices[2][4] = 0;

//         res.vertices[3][0] = x0;
//         res.vertices[3][1] = y0;
//         res.vertices[3][2] = z;
//         res.vertices[3][3] = 0;
//         res.vertices[3][4] = 0;

//     } else {
//         // Sides
//         int max_z = start_z + 1; // the other size of the x tile
//         for (int z = start_z + 1; z < max_len; z++) {
//             // TODO perhaps check if covered or smt.
//             if (test_chunk.cubes[start_x][start_y][z].type != BLOCK_TYPE_GRASS) {
//                 max_z = z;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_z = max_len;
//         }
//         found = false;
//         int max_y = start_y + 1; // andre siden av x tile
//         for (int y = start_y + 1; y < max_len; y++) {
//             // TODO check if from this x it crashes anywhere towards the max z.
//             if (test_chunk.cubes[start_x][y][start_z].type != BLOCK_TYPE_GRASS) {
//                 max_y = y;
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             max_y = max_len;
//         }

//         switch (face_direction) {
//             case FACE_DIRECTION_WEST:
//                 res.indices[0] = 0;
//                 res.indices[1] = 3;
//                 res.indices[2] = 2;
//                 res.indices[3] = 0;
//                 res.indices[4] = 2;
//                 res.indices[5] = 1;
//                 break;
//             case FACE_DIRECTION_EAST:
//                 start_x += 1;
//                 res.indices[0] = 0;
//                 res.indices[1] = 2;
//                 res.indices[2] = 3;
//                 res.indices[3] = 0;
//                 res.indices[4] = 1;
//                 res.indices[5] = 2;
//                 break;
//         }
//         int x = start_x * 2*cube_size;
//         int y0 = (start_y - 1) * 2*cube_size;
//         int y1 = (max_y - 1) * 2*cube_size;
//         int z0 = start_z * 2*cube_size;
//         int z1 = max_z * 2*cube_size;

//         res.vertices[0][0] = x;
//         res.vertices[0][1] = y0;
//         res.vertices[0][2] = z1;
//         res.vertices[0][3] = 0;
//         res.vertices[0][4] = (max_z - start_z);

//         res.vertices[1][0] = x;
//         res.vertices[1][1] = y1;
//         res.vertices[1][2] = z1;
//         res.vertices[1][3] = (max_y - start_y);
//         res.vertices[1][4] = (max_z - start_z);

//         res.vertices[2][0] = x;
//         res.vertices[2][1] = y1;
//         res.vertices[2][2] = z0;
//         res.vertices[2][3] = (max_y - start_y);
//         res.vertices[2][4] = 0;

//         res.vertices[3][0] = x;
//         res.vertices[3][1] = y0;
//         res.vertices[3][2] = z0;
//         res.vertices[3][3] = 0;
//         res.vertices[3][4] = 0;
//     }
//     return res;
// }

static void fill_face_indices(u16 indices[], u32 index_offset, u32 vertex_offset, face_stored face) {
    switch (face.info & FACE_MASK_INFO_DIRECTION) { 
        case FACE_DIRECTION_DOWN:
            indices[index_offset + 0] = vertex_offset + 1;
            indices[index_offset + 1] = vertex_offset + 2;
            indices[index_offset + 2] = vertex_offset + 3;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 1;
            indices[index_offset + 5] = vertex_offset + 3;
            break;
        case FACE_DIRECTION_UP:
            indices[index_offset + 0] = vertex_offset + 1;
            indices[index_offset + 1] = vertex_offset + 3;
            indices[index_offset + 2] = vertex_offset + 2;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 3;
            indices[index_offset + 5] = vertex_offset + 1;
            break;
        case FACE_DIRECTION_NORTH:
            indices[index_offset + 0] = vertex_offset + 0;
            indices[index_offset + 1] = vertex_offset + 3;
            indices[index_offset + 2] = vertex_offset + 2;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 2;
            indices[index_offset + 5] = vertex_offset + 1;
            break;
        case FACE_DIRECTION_SOUTH:
            indices[index_offset + 0] = vertex_offset + 0;
            indices[index_offset + 1] = vertex_offset + 2;
            indices[index_offset + 2] = vertex_offset + 3;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 1;
            indices[index_offset + 5] = vertex_offset + 2;
            break;
        case FACE_DIRECTION_WEST:
            indices[index_offset + 0] = vertex_offset + 0;
            indices[index_offset + 1] = vertex_offset + 3;
            indices[index_offset + 2] = vertex_offset + 2;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 2;
            indices[index_offset + 5] = vertex_offset + 1;
            break;
        case FACE_DIRECTION_EAST:
            indices[index_offset + 0] = vertex_offset + 0;
            indices[index_offset + 1] = vertex_offset + 2;
            indices[index_offset + 2] = vertex_offset + 3;
            indices[index_offset + 3] = vertex_offset + 0;
            indices[index_offset + 4] = vertex_offset + 1;
            indices[index_offset + 5] = vertex_offset + 2;
            break;
    }
}

static void fill_face_vertices(f32 vertices[][3], f32 tex_coors[][2], u32 offset, face_stored face) {
    int a0 = face.corners.a0 * 2*cube_size;
    int a1 = face.corners.a1 * 2*cube_size;
    int b0 = face.corners.b0 * 2*cube_size;
    int b1 = face.corners.b1 * 2*cube_size;
    int c = face.corners.c * 2*cube_size;

    int tex_a = face.corners.a1 - face.corners.a0;
    int tex_b = face.corners.b1 - face.corners.b0;

    if ((face.info & FACE_MASK_INFO_DIRECTION) <= FACE_DIRECTION_UP) {

        vertices[offset + 0][0] = a0;
        vertices[offset + 0][1] = c;
        vertices[offset + 0][2] = b1;
        tex_coors[offset + 0][0] = 0;
        tex_coors[offset + 0][1] = tex_b;
                          
        vertices[offset + 1][0] = a1;
        vertices[offset + 1][1] = c;
        vertices[offset + 1][2] = b1;
        tex_coors[offset + 1][0] = tex_a;
        tex_coors[offset + 1][1] = tex_b;
                          
        vertices[offset + 2][0] = a1;
        vertices[offset + 2][1] = c;
        vertices[offset + 2][2] = b0;
        tex_coors[offset + 2][0] = tex_a;
        tex_coors[offset + 2][1] = 0;
                          
        vertices[offset + 3][0] = a0; // TODO flytt denne til nullte vertex
        vertices[offset + 3][1] = c;
        vertices[offset + 3][2] = b0;
        tex_coors[offset + 3][0] = 0;
        tex_coors[offset + 3][1] = 0;
        return;
    } 

    if ((face.info & FACE_MASK_INFO_DIRECTION) <= FACE_DIRECTION_NORTH) {

        vertices[offset + 0][0] = a1;
        vertices[offset + 0][1] = b0;
        vertices[offset + 0][2] = c;
        tex_coors[offset + 0][0] = 0;
        tex_coors[offset + 0][1] = tex_a;
                          
        vertices[offset + 1][0] = a1;
        vertices[offset + 1][1] = b1;
        vertices[offset + 1][2] = c;
        tex_coors[offset + 1][0] = tex_b;
        tex_coors[offset + 1][1] = tex_a;
                          
        vertices[offset + 2][0] = a0;
        vertices[offset + 2][1] = b1;
        vertices[offset + 2][2] = c;
        tex_coors[offset + 2][0] = tex_b;
        tex_coors[offset + 2][1] = 0;
                          
        vertices[offset + 3][0] = a0;
        vertices[offset + 3][1] = b0;
        vertices[offset + 3][2] = c;
        tex_coors[offset + 3][0] = 0;
        tex_coors[offset + 3][1] = 0;
        return;
    } 

    // int x = start_x * 2*cube_size;
    // int y0 = (start_y - 1) * 2*cube_size;
    // int y1 = (max_y - 1) * 2*cube_size;
    // int z0 = start_z * 2*cube_size;
    // int z1 = max_z * 2*cube_size;

    vertices[offset + 0][0] = c;
    vertices[offset + 0][1] = a0;
    vertices[offset + 0][2] = b1;
    tex_coors[offset + 0][0] = 0;
    tex_coors[offset + 0][1] = tex_b;
                      
    vertices[offset + 1][0] = c;
    vertices[offset + 1][1] = a1;
    vertices[offset + 1][2] = b1;
    tex_coors[offset + 1][0] = tex_a;
    tex_coors[offset + 1][1] = tex_b;
                      
    vertices[offset + 2][0] = c;
    vertices[offset + 2][1] = a1;
    vertices[offset + 2][2] = b0;
    tex_coors[offset + 2][0] = tex_a;
    tex_coors[offset + 2][1] = 0;
                      
    vertices[offset + 3][0] = c;
    vertices[offset + 3][1] = a0;
    vertices[offset + 3][2] = b0;
    tex_coors[offset + 3][0] = 0;
    tex_coors[offset + 3][1] = 0;
}

static bool is_face_in_frustum(face f, MATRIX view) {


    return true;
}

// FIXME does not work when running non-statically or directly in main.c
inline static void render_cube(f32 x, f32 y, f32 rotX, f32 rotY) {
    
    v_obj_pos[0] = x;
    v_obj_pos[1] = 0;
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

    /*
     * Setup vertex attributes
     */
    int num = num_faces_pooled;
    face_stored temp_faces[num];
    memcpy(temp_faces, faces_pool, num * sizeof(face_stored));
    u16 cube_indices[6 * num];

    f32 cube_vertices[4*num][3];
    f32 tex_coors[4*num][2];
    face render_faces[num];

    u8 removeDirection = -1; // TODO change into 6 different normals instead!
    if (v_cam_rot[1] < .6 || v_cam_rot[1] > 5.6) {
        removeDirection = FACE_DIRECTION_SOUTH;
    } else if (v_cam_rot[1] < 3.35 && v_cam_rot[1] > 2.15) {
        removeDirection = FACE_DIRECTION_NORTH;
    } else if (v_cam_rot[1] < 5.31 && v_cam_rot[1] > 4.11) {
        removeDirection = FACE_DIRECTION_EAST;
    } else if (v_cam_rot[1] < 2.57 && v_cam_rot[1] > 0.97) {
        removeDirection = FACE_DIRECTION_WEST;
    }

    int n = 0;
    for (int i = 0; i < num; i++) {
        face_stored fs = temp_faces[i];
        u8 direction = fs.info & FACE_MASK_INFO_DIRECTION;
        if (direction == removeDirection) continue;
        face f = {0};
        fill_face_indices(f.indices, 0, n*4, fs);
        fill_face_vertices(f.vertices, f.tex_coords, 0, fs);
        if (!is_face_in_frustum(f, m_view)) continue;

        memcpy(&cube_indices[n*6], f.indices, sizeof(u16) * 6);
        memcpy(cube_vertices[n*4], f.vertices, sizeof(f32) * 4 * 3);
        memcpy(tex_coors[n*4], f.tex_coords, sizeof(f32) * 4 * 2);


        n++;
        

                // if (test_chunk.cubes[i][Y][Z].type == BLOCK_TYPE_GRASS) {
                //     test_face[0] = find_full_face(X, Y, Z, FACE_DIRECTION_UP);    
                //     test_face[1] = find_full_face(X, Y, Z, FACE_DIRECTION_DOWN);    
                //     test_face[2] = find_full_face(X, Y, Z, FACE_DIRECTION_WEST);    
                //     test_face[3] = find_full_face(X, Y, Z, FACE_DIRECTION_NORTH);    
                //     test_face[4] = find_full_face(X+15, Y, Z, FACE_DIRECTION_EAST);    
                //     test_face[5] = find_full_face(X, Y, Z+15, FACE_DIRECTION_SOUTH);    
                //     // test_face[3] = find_full_face(X, Y, Z, FACE_DIRECTION_EAST);    
                //     memcpy(cube_vertices, test_face[0].vertices, sizeof(float) * 4 * 5);
                //     memcpy(&cube_vertices[4], test_face[1].vertices, sizeof(float) * 4 * 5);
                //     memcpy(&cube_vertices[8], test_face[2].vertices, sizeof(float) * 4 * 5);
                //     memcpy(&cube_vertices[12], test_face[3].vertices, sizeof(float) * 4 * 5);
                //     memcpy(&cube_vertices[16], test_face[4].vertices, sizeof(float) * 4 * 5);
                //     memcpy(&cube_vertices[20], test_face[5].vertices, sizeof(float) * 4 * 5);
                //     memcpy(cube_indices, test_face[0].indices, sizeof(float) * 6);
                //     for (int i = 0; i < 6; i++)
                //         cube_indices[i + 6] = 4 + test_face[1].indices[i];
                //     for (int i = 0; i < 6; i++)
                //         cube_indices[i + 12] = 8 + test_face[2].indices[i];
                //     for (int i = 0; i < 6; i++)
                //         cube_indices[i + 18] = 12 + test_face[3].indices[i];
                //     for (int i = 0; i < 6; i++)
                //         cube_indices[i + 24] = 16 + test_face[4].indices[i];
                //     for (int i = 0; i < 6; i++)
                //         cube_indices[i + 30] = 20 + test_face[5].indices[i];
                //     // fill_array_singular_face_vertices(actual_num, cube_vertices, X, Y, Z);
    }
    u32 real_size_of_verts = sizeof(f32) * 4 * n * 3;
    u32 *allocated_verts = MmAllocateContiguousMemoryEx(real_size_of_verts, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(allocated_verts, cube_vertices, real_size_of_verts);
    u32 real_size_of_texs = sizeof(f32) * 4 * n * 2;
    u32 *allocated_texs = MmAllocateContiguousMemoryEx(real_size_of_verts, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(allocated_texs, tex_coors, real_size_of_texs);
    /* Set vertex position attribute */
    set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(float) * 3, allocated_verts);

    /* Set vertex diffuse color attribute */
    set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(Vertex), &alloc_vertices_cube[3]);

    /* Set texture coordinate attribute */
    set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            2, sizeof(float) * 2, allocated_texs);

    /* Begin drawing triangles */
    draw_indexed(n*6, cube_indices);
    MmFreeContiguousMemory(allocated_verts);
    MmFreeContiguousMemory(allocated_texs);
}

inline static void render_terrain(image_data img) {
    init_shader(1);
    {
        /*
         * Setup texture stages
         */

        int channels = 2;
        DWORD format = ((channels & NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) |
                       (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR * NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE)) | // 0x0000000F
                       (((2 << 4) & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY)) | // 0x000000F0
                       ((NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8 << 8) & NV097_SET_TEXTURE_FORMAT_COLOR) | // 0x0000FF00
                       ((1 << 16) & NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS) | // 0x000F0000
                       ((9 << 20) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U) | // I manually did 512 (which is the width of my texture) and log2 (so up to 16^2)
                       ((9 << 24) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U               0x00F00000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V               0x0F000000
// #       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P               0xF0000000
        DWORD filter = ((4 << 24) & NV097_SET_TEXTURE_FILTER_MAG) | 
                       ((7 << 16) & NV097_SET_TEXTURE_FILTER_MIN) | 
				       ((4 << 12) & NV097_SET_SURFACE_FORMAT_ANTI_ALIASING); // 0x04074000
		DWORD control_enable = NV097_SET_TEXTURE_CONTROL0_ENABLE | NV097_SET_TEXTURE_CONTROL0_MAX_LOD_CLAMP;

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
        u32 *p = pb_begin();
        // Retain the lower 26 bits of the address
        p = pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0), img.addr26bits, format); //set stage 0 texture address & format
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),img.pitch<<16); //set stage 0 texture pitch (pitch<<16)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(img.w<<16)|img.h); //set stage 0 texture width & height ((witdh<<16)|height)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),  0x00000101); //set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
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
        // for (int y = 0; y < 20; y++) {
        //     for (int x = 0; x < 10; x++) {
                render_cube(0, 0, 0, 0); //  obj_rotationX/1000.0f * M_PI * -0.25f, obj_rotationY/1000.0f * M_PI * -0.25f);
        //     }
        // }
    }
}

void init_terrain();
