#pragma once

#include "nums.h"

#define BLOCK_TYPE_AIR 0 // TODO don't have this
#define BLOCK_TYPE_GRASS 1
#define BLOCK_TYPE_MASK 0x00000FFF

typedef struct {
    u32 type; // 12 first bits represent the block type? 20 bits rest are for more information. Like for instance, flowing or smt..
} cube_entity;


#pragma pack(1)
typedef struct Vertex {
    float pos[3];
    float color[3];
    float texcoord[2];
} Vertex;
#pragma pack()

static const f32 cube_size = 50.f;
static const f32 cube_tex_w = 2, cube_tex_h = 2;

static struct Vertex cube_vertices[] = {
 {{-cube_size, -cube_size,  cube_size}, { 1,  0,  0}, {0,   0}}, // bl
 {{ cube_size, -cube_size,  cube_size}, { 0,  1,  0}, {cube_tex_w, 0}}, // br
 {{-cube_size,  cube_size,  cube_size}, { 0,  0,  1}, {0,   cube_tex_h}}, // tl
 {{ cube_size,  cube_size,  cube_size}, { 1,  0,  1}, {cube_tex_w, cube_tex_h}}, // tr
 {{-cube_size, -cube_size, -cube_size}, { 1,  1,  0}, {cube_tex_w, cube_tex_h}},
 {{ cube_size, -cube_size, -cube_size}, { 0,  1,  1}, {0, cube_tex_h}},
 {{-cube_size,  cube_size, -cube_size}, { 1,  1,  1}, {cube_tex_w, 0}},
 {{ cube_size,  cube_size, -cube_size}, { 0.2,.5,.8}, {0, 0}},
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

static u32 num_cube_indices = 18; // FIXME actually 36 but there's a mishmash of salad code between u8 u16 and u32 numbers!!

static u32 *alloc_vertices_cube;
static u32  num_vertices;
