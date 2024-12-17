#pragma once
#include "cube.h"

#define FACE_TYPE_COBBLESTONE 0
#define FACE_TYPE_GRASS_TOP 1
#define FACE_TYPE_GRASS_SIDE 2
#define FACE_TYPE_DIRT 3

#define FACE_DIRECTION_DOWN 0
#define FACE_DIRECTION_UP 1
#define FACE_DIRECTION_SOUTH 2
#define FACE_DIRECTION_NORTH 3
#define FACE_DIRECTION_WEST 4
#define FACE_DIRECTION_EAST 5

/*
 * Keep only the closest ones of these active, whereas the ones further way
 * just load in the mesh to render and move it back to the disk so that you
 * don't use unnecessary amounts of ram on this.
 */
typedef struct {
    cube_entity cubes[16][16][16]; // XYZ
} chunk_data;

/*
 * Render all the faces that you can see AND go through PER type so that you minimize texture loading.
 * And combine all those of the same type into one draw call.
 */
typedef struct {
    f32 vertices[4][5];
    u16 indices[6];
} face;

typedef struct {
    i16 corners[2][3]; // TODO possibly make into f16's with offsets of chunk location. And also, we only need 2 (opposing corners!) vertices because we know it's a flat plane!
    u16 info; // first 10 bits are face_type, next 2 bits are direction and last idk
} face_stored; // Use to store basic info of the face, so that you can recreate it into a full face.

extern chunk_data test_chunk;

void generate_chunk(i32 x, i32 y);
