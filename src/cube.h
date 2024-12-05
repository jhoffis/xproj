#ifndef TEMP_CUBEE
#define TEMP_CUBEE

#include "nums.h"

#pragma pack(1)
typedef struct Vertex {
    float pos[3];
    float color[3];
    float texcoord[2];
} Vertex;
#pragma pack()

static const f32 cube_size = 25.f;

static struct Vertex cube_vertices[] = {
 {{-cube_size, -cube_size,  cube_size}, { 1,  0,  0}, {0, 0}}, // bl
 {{ cube_size, -cube_size,  cube_size}, { 0,  1,  0}, {1, 0}}, // br
 {{-cube_size,  cube_size,  cube_size}, { 0,  0,  1}, {0, 1}}, // tl
 {{ cube_size,  cube_size,  cube_size}, { 1,  0,  1}, {1, 1}}, // tr
 {{-cube_size, -cube_size, -cube_size}, { 1,  1,  0}, {0, 0}},
 {{ cube_size, -cube_size, -cube_size}, { 0,  1,  1}, {1, 0}},
 {{-cube_size,  cube_size, -cube_size}, { 1,  1,  1}, {0, 1}},
 {{ cube_size,  cube_size, -cube_size}, { 0.2,.5,.8}, {1, 1}},
};

static u16 cube_indices[] = {
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
        4, 5, 7
};

static u32 packed_cube_indices[] = {
    // Top
    (2 << 16) | 6, (7 << 16) | 2,
    (7 << 16) | 3,

    // Bottom
    (0 << 16) | 5, (4 << 16) | 0,
    (1 << 16) | 5,

    // Left
    (0 << 16) | 6, (2 << 16) | 0,
    (4 << 16) | 6,

    // Right
    (1 << 16) | 3, (7 << 16) | 1,
    (7 << 16) | 5,

    // Front
    (0 << 16) | 2, (3 << 16) | 0,
    (3 << 16) | 1,

    // Back
    (4 << 16) | 7, (6 << 16) | 4,
    (5 << 16) | 7,
};

static u32 num_cube_indices = 18;

#endif // TEMP_CUBE
