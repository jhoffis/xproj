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
 {{-.1, -.1,  0.05}, { 1,  0,  0}, }, // bl
 {{ .1, -.1,  0.05}, { 0,  1,  0}, }, // br
 {{-.1,  .1,  0.05}, { 0,  0,  1}, }, // tl
 {{ .1,  .1,  0.05}, { 1,  0,  1}, }, // tr
 {{-.1, -.1, -0.05}, { 1,  1,  0}, },
 {{ .1, -.1, -0.05}, { 0,  1,  1}, },
 {{-.1,  .1, -0.05}, { 1,  1,  1}, },
 {{ .1,  .1, -0.05}, { 0.2,  0.5,  0.8}, },
};

static u16 cube_indices[] = {
//Top
        // 2, 6, 7,
        // 2, 3, 7,
        //
        // //Bottom
        // 0, 4, 5,
        // 0, 1, 5,
        //
        // //Left
        // 0, 2, 6,
        // 0, 4, 6,
        //
        // //Right
        // 1, 3, 7,
        // 1, 5, 7,

        //Front
        0, 2, 3,
        0, 3, 1,

        //Back
        // 4, 6, 7,
        // 4, 5, 7
};

static u32 num_cube_indices = 6;

#endif // TEMP_CUBE
