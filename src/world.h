#pragma once
#include "cube.h"

#define CHUNK_SIZE 16
// it can render up to 1024 faces per draw call
#define FACE_POOL_SIZE 8*1024

#define FACE_TYPE_COBBLESTONE 0
#define FACE_TYPE_GRASS_TOP 1
#define FACE_TYPE_GRASS_SIDE 2
#define FACE_TYPE_DIRT 3


#define FACE_DIRECTION_DOWN 0  // -y
#define FACE_DIRECTION_UP 1    // +y
#define FACE_DIRECTION_SOUTH 2 // -z
#define FACE_DIRECTION_NORTH 3 // +z
#define FACE_DIRECTION_WEST 4  // -x
#define FACE_DIRECTION_EAST 5  // +x
#define FACE_DIRECTION_TOTAL 6
#define FACE_MASK_INFO_DIRECTION 0x07 // lower 3 bits

/*
 * Keep only the closest ones of these active, whereas the ones further way
 * just load in the mesh to render and move it back to the disk so that you
 * don't use unnecessary amounts of ram on this.
 */
typedef struct {
    i32 x, y, z;
    cube_entity cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; // XYZ
} chunk_data;

/*
 * Render all the faces that you can see AND go through PER type so that you minimize texture loading.
 * And combine all those of the same type into one draw call.
 */
typedef struct {
    f32 vertices[4][3];
    f32 tex_coords[4][2];
    u16 indices[6];
} face; // Kan ha 326 tusen faces i 30MB, så disse burde lagres! Burde ha at man laster inn de nermeste 9 chunks i disse.

typedef struct {
    i8 a0, a1, b0, b1, c;
} corners_i8;

/*
 * We only need two corners with two dimensions, and we know the direction so that we can assume whether it's x and y or x and z etc.
 * List the same face_type grouped together instead of storing it with each face.
 */
typedef struct {
    corners_i8 corners; // TODO possibly make into f16's with offsets of chunk location. And also, we only need 2 (opposing corners!) vertices because we know it's a flat plane!
    u8 info; // first 3 bits are direction
} face_stored; // Use to store basic info of the face, so that you can recreate it into a full face.

extern chunk_data *loaded_chunks;
extern face_stored *faces_pool;
extern u32 num_faces_pooled;
extern u32 num_chunks_pooled;
extern u32 *chunk_offsets;

void init_world(void);
void generate_chunk(i32 chunk_x, i32 chunk_y, i32 chunk_z);
