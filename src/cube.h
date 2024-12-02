#include "nums.h"

#pragma pack(1)
typedef struct Vertex {
    float pos[3];
    float normal[3];
    float texcoord[2];
} Vertex;
#pragma pack()

struct Vertex cube_vertices[] = {
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
 {{}, {}, {},},
};

u16 cube_indices[] = {

};

u32 num_cube_indices = 0;
