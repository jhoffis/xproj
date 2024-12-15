#pragma once
#include "cube.h"

/*
 * Keep only the closest ones of these active, whereas the ones further way
 * just load in the mesh to render and move it back to the disk so that you
 * don't use unnecessary amounts of ram on this.
 */
typedef struct {
    cube_entity cubes[16][16][16]; // XYZ
} chunk_data;


typedef struct {
    f32 v[4][3];
    u16 tex_coor[4][2];
    u16 indices[6];
} face;

extern chunk_data test_chunk;

void generate_chunk(i32 x, i32 y);
