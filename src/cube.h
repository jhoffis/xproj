#ifndef TEMP_CUBEE
#define TEMP_CUBEE

#include "nums.h"

#pragma pack(1)
typedef struct Vertex {
    float pos[3];
    float color[3];
} Vertex;
#pragma pack()

static struct Vertex cube_vertices[] = {
 {{-10.1, -10.1f,  10.1}, { 1,  0,  0}, }, // bl
 {{ 10.1, -10.1f,  10.1}, { 0,  1,  0}, }, // br
 {{-10.1,  10.1f,  10.1}, { 0,  0,  1}, }, // tl
 {{ 10.1,  10.1f,  10.1}, { 1,  0,  1}, }, // tr
 {{-10.1, -10.1f, -10.1}, { 1,  1,  0}, },
 {{ 10.1, -10.1f, -10.1}, { 0,  1,  1}, },
 {{-10.1,  10.1f, -10.1}, { 1,  1,  1}, },
 {{ 10.1,  10.1f, -10.1}, { 0.2,  0.5,  0.8}, },
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
