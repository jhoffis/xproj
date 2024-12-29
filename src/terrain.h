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
    switch (GET_FACE_STORED(face, FACE_STORED_INFO_DIRECTION)) { 
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


static void mul_left_vec4_matrix(f32 vec[4], f32 mat[16]) {    
    // Cache the input vector since we'll overwrite it
    const f32 x = vec[0], y = vec[1], z = vec[2], w = vec[3];
    
    // Direct calculation avoiding loops and temporary array
    vec[0] = x * mat[0] + y * mat[4] + z * mat[8]  + w * mat[12];
    vec[1] = x * mat[1] + y * mat[5] + z * mat[9]  + w * mat[13];
    vec[2] = x * mat[2] + y * mat[6] + z * mat[10] + w * mat[14];
    vec[3] = x * mat[3] + y * mat[7] + z * mat[11] + w * mat[15];
}

static void mul_right_vec4_matrix(f32 vec[4], f32 mat[16]) {    
    // Cache the input vector since we'll overwrite it
    const f32 x = vec[0], y = vec[1], z = vec[2], w = vec[3];

    vec[0] = x * mat[0] + y * mat[1] + z * mat[2]  + w * mat[3];
    vec[1] = x * mat[4] + y * mat[5] + z * mat[6]  + w * mat[7];
    vec[2] = x * mat[8] + y * mat[9] + z * mat[10] + w * mat[11];
    vec[3] = x * mat[12] + y * mat[13] + z * mat[14] + w * mat[15];
}
typedef struct {
    float x, y, z, w;
} Vec4;
static Vec4 transform_vector(MATRIX m, Vec4 v) {
    Vec4 result;
    result.x = m[0]*v.x + m[4]*v.y + m[8]*v.z + m[12]*v.w;
    result.y = m[1]*v.x + m[5]*v.y + m[9]*v.z + m[13]*v.w;
    result.z = m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w;
    result.w = m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w;
    return result;
}
/// Check if a point is inside the normalized device coordinates (NDC) frustum
static inline bool is_point_in_frustum_ndc(f32 clip_space[4]) {
    return clip_space[0] >= 0 && 
           clip_space[0] <= 640 &&
           clip_space[1] >= 0 &&
           clip_space[1] <= 480 &&
           clip_space[2] >= 0 &&
           clip_space[2] <= 65536;
}
/*
   https://bruop.github.io/improved_frustum_culling/ 
*/

static bool is_point_in_frustum(f32 point[3], MATRIX mat) {

    f32 cs[4];
    
    // Direct calculation avoiding loops and temporary array
    cs[3] =  1.0f / (point[0] * mat[3] + point[1] * mat[7] + point[2] * mat[11] + mat[15]);
    cs[0] = (point[0] * mat[0] + point[1] * mat[4] + point[2] * mat[8]  + mat[12]) * cs[3];
    cs[1] = (point[0] * mat[1] + point[1] * mat[5] + point[2] * mat[9]  + mat[13]) * cs[3];
    cs[2] = (point[0] * mat[2] + point[1] * mat[6] + point[2] * mat[10] + mat[14]) * -cs[3];

    // Check if point is inside the frustum in NDC space
    bool inside_view_frustum =  is_point_in_frustum_ndc(cs);
    // pb_print("cp x%d y%d z%d w%d IN %d\n", (i32) cs[0], 
    //                                        (i32) cs[1], 
    //                                        (i32) cs[2], 
    //                                        (i32) cs[3], 
    //                                        (i32) inside_view_frustum);
    return inside_view_frustum;
}

static bool is_face_in_frustum(face f, MATRIX viewproj) {
    if (is_point_in_frustum(f.vertices[0], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[1], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[2], viewproj)) return true;
    if (is_point_in_frustum(f.vertices[3], viewproj)) return true;

    return false;
}

static void my_matrix_multiply(MATRIX result, MATRIX a, MATRIX b) {
    // Zero out the result matrix
    for (int i = 0; i < 16; i++) {
        result[i] = 0.0f;
    }
    
    // For each column in the result
    for (int j = 0; j < 4; j++) {
        // For each row in the result
        for (int i = 0; i < 4; i++) {
            // Calculate dot product of row from a and column from b
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                // In column-major:
                // row i, col k of a = a[k*4 + i]
                // row k, col j of b = b[j*4 + k]
                sum += a[k*4 + i] * b[j*4 + k];
            }
            // Store in column-major: row i, col j = result[j*4 + i]
            result[j*4 + i] = sum;
        }
    }
}

// Define a 3D vector
typedef struct {
    float x, y, z;
} Vector3;

// Define a 3x3 rotation matrix
typedef struct {
    float m[3][3];
} Matrix3;

// Normalize a 3D vector
static Vector3 normalize(Vector3 v) {
    float magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vector3){v.x / magnitude, v.y / magnitude, v.z / magnitude};
}

// Convert Euler angles (yaw, pitch, roll in radians) to a 3x3 rotation matrix
static Matrix3 euler_to_rotation_matrix(float yaw, float pitch, float roll) {
    float cy = cos(yaw), sy = sin(yaw);
    float cp = cos(pitch), sp = sin(pitch);
    float cr = cos(roll), sr = sin(roll);

    Matrix3 rotation_matrix = {
        .m = {
            {cy * cr + sy * sp * sr, cr * sy * sp - cy * sr, cp * sy},
            {cp * sr,               cp * cr,               -sp     },
            {cy * sp * sr - cr * sy, cy * cr * sp + sr * sy, cy * cp}
        }
    };
    return rotation_matrix;
}

// Multiply a 3x3 matrix by a 3D vector
static Vector3 multiply_matrix_vector(Matrix3 matrix, Vector3 vector) {
    return (Vector3){
        matrix.m[0][0] * vector.x + matrix.m[0][1] * vector.y + matrix.m[0][2] * vector.z,
        matrix.m[1][0] * vector.x + matrix.m[1][1] * vector.y + matrix.m[1][2] * vector.z,
        matrix.m[2][0] * vector.x + matrix.m[2][1] * vector.y + matrix.m[2][2] * vector.z
    };
}

// Compute the dot product of two vectors
static float dot_product(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Compare the direction of two vectors
static int is_same_direction(Vector3 a, Vector3 b, float threshold) {
    // Normalize both vectors
    a = normalize(a);
    b = normalize(b);

    // Compute the dot product
    float dot = dot_product(a, b);

    // Check if the angle between them is within the threshold
    return dot >= threshold;  // Cosine similarity threshold (e.g., 0.9 for ~25°)
}

static f32 distance_vec3(f32 *a, f32 *b) {
    f32 x = b[0] - a[0];
    f32 y = b[1] - a[1];
    f32 z = b[2] - a[2];
    return sqrtf(x*x + y*y + z*z);
}

static Vector3 subtract(f32 *a, f32 *b) {
    return (Vector3){a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

static void render_cube(f32 x, f32 y, f32 rotX, f32 rotY) {
    
    v_obj_pos[0] = x;
    v_obj_pos[1] = 0;
    v_obj_pos[2] = y;

    /* Tilt and rotate the object a bit */
    v_obj_rot[0] = rotX;
    v_obj_rot[1] = rotY;

    Vector3 camera_position = {v_cam_loc[0], v_cam_loc[1], v_cam_loc[2]};

    // Forward vector in view space
    Vector3 forward_vector_view = {0.0f, 0.0f, -1.0f};

    // Compute the rotation matrix
    Matrix3 rotation_matrix = euler_to_rotation_matrix(v_cam_rot[1], v_cam_rot[0], v_cam_rot[2]);

    // Transform the forward vector to world space
    Vector3 camera_normal_world = multiply_matrix_vector(rotation_matrix, forward_vector_view);

    // Normalize the resulting vector
    camera_normal_world = normalize(camera_normal_world);
    // pb_print("normal x%d y%d z%d \n", (i32) (1000 * camera_normal_world.x), 
    //                                   (i32) (1000 * camera_normal_world.y), 
    //                                   (i32) (1000 * camera_normal_world.z));

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
    int num = num_faces_pooled < FACE_POOL_SIZE ? num_faces_pooled : FACE_POOL_SIZE;
    // face_stored temp_faces[num];
    // memcpy(temp_faces, faces_pool, num * sizeof(face_stored));
    u16 *cube_indices = malloc(6 * num * sizeof(u16)); // TODO maybe add these at the end? Isn't this just saying which to render first? well yeah because we know that every 4 verticies is a face. Instead just store direction and distance from camera.

    f32 *cube_vertices = MmAllocateContiguousMemoryEx(4*num*3 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    f32 *tex_coors = MmAllocateContiguousMemoryEx(4*num*2 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);

    // MATRIX vm;
    // matrix_multiply(vm, m_view, m_model);
    MATRIX mvp;
    matrix_multiply(mvp, m_view, m_proj);

    Vector3 face_normals[FACE_DIRECTION_TOTAL];
    bool remove_directions[FACE_DIRECTION_TOTAL];
    for (int d = 0; d < FACE_DIRECTION_TOTAL; d++) { // TODO check if the object is more than 1 distance away and has the same direction because then it should not be visible.
                                  // basically, if object has same direction and is ahead by 1 then remove.
        Vector3 face_normal = {0};
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
        remove_directions[d] = is_same_direction(camera_normal_world, face_normal, 0.7f);
        face_normals[d] = face_normal;
    }

    int n = 0;
    for (int i = 0; i < num; i++) {
        
        face f = faces_calculated_pool[i];
        face_stored fs = faces_pool[i];
        u8 direction = GET_FACE_STORED(fs, FACE_STORED_INFO_DIRECTION);

        if (remove_directions[direction]) continue;

        // face f = {0};
        // fill_face_vertices(f.vertices, f.tex_coords, 0, pos_offset, fs);

        Vector3 view_dir = normalize(subtract(v_cam_loc, f.vertices[0]));
        f32 dot_prod = dot_product(face_normals[direction], view_dir);
        if (dot_prod < 0) continue;
        // pb_print("viewdir x%d, y%d, z%d dot %d i%d dir%d\n", (i32) (100*view_dir.x), (i32) (100*view_dir.y), (i32) (100*view_dir.z), (i32) (100*dot_prod), i, direction);

        fill_face_indices(f.indices, 0, n*4, fs);
        // if (!is_face_in_frustum(f, mvp)) continue; // TODO perhaps first frustum cull whole chunks as this calculation is very heavy.

        // 3 000 000 ns ish
        memcpy(&cube_indices[n*6], f.indices, sizeof(u16) * 6);
        memcpy(&cube_vertices[n*4*3], f.vertices, sizeof(f32) * 4 * 3);
        memcpy(&tex_coors[n*4*2], f.tex_coords, sizeof(f32) * 4 * 2); // TODO texture coordinates can be baked into the shader!

        n++;
    }
    pb_print("rendered faces: %d\n", n);

    // u32 real_size_of_verts = sizeof(f32) * 4 * n * 3;
    // u32 *allocated_verts = MmAllocateContiguousMemoryEx(4*num*3 * sizeof(f32), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // memcpy(allocated_verts, cube_vertices, real_size_of_verts);
    // u32 real_size_of_texs = sizeof(f32) * 4 * n * 2;
    // u32 *allocated_texs = MmAllocateContiguousMemoryEx(real_size_of_texs, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // memcpy(allocated_texs, tex_coors, real_size_of_texs);
    /* Set vertex position attribute */
    set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(float) * 3, cube_vertices);

    /* Set vertex diffuse color attribute */
    set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            3, sizeof(Vertex), &alloc_vertices_cube[3]);

    /* Set texture coordinate attribute */
    set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            2, sizeof(float) * 2, tex_coors);

    /* Begin drawing triangles */
    draw_indexed(n*6, cube_indices);
    MmFreeContiguousMemory(cube_vertices);
    MmFreeContiguousMemory(tex_coors);
    // free(cube_vertices);
    // free(tex_coors);
    free(cube_indices);
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
