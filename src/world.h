#pragma once
#include "cube.h"

#define MAX_VERTICES 65536
#define MAX_FACES 16384 // MAX_VERTICES / 4
#define INDEX_ALIGNMENT 49152 // MAX_VERTICES - MAX_FACES
#define MAX_RENDERED_FACES 40000 // 36864 // appearently

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define CHUNK_VIEW_DISTANCE 12
#define CHUNK_AMOUNT (CHUNK_VIEW_DISTANCE * CHUNK_VIEW_DISTANCE)
_Static_assert(CHUNK_VIEW_DISTANCE % 2 == 0, "View distance needs to be a whole number!");
// it can render up to 1024 faces per draw call
#define FACE_POOL_SIZE 4*32*1024
#define FACE_TEXTURE_ATLAS_WIDTH 4
#define FACE_TEXTURE_ATLAS_HEIGHT 2


#define FACE_TYPE_GRASS_TOP 0
#define FACE_TYPE_GRASS_SIDE 1
#define FACE_TYPE_DIRT 2
#define FACE_TYPE_COBBLESTONE 3
#define FACE_TYPE_AMOUNT 4


#define FACE_DIRECTION_DOWN 0  // -y
#define FACE_DIRECTION_UP 1    // +y
#define FACE_DIRECTION_SOUTH 2 // -z
#define FACE_DIRECTION_NORTH 3 // +z
#define FACE_DIRECTION_WEST 4  // -x
#define FACE_DIRECTION_EAST 5  // +x
#define FACE_DIRECTION_TOTAL 6

/*
 * Keep only the closest ones of these active, whereas the ones further way
 * just load in the mesh to render and move it back to the disk so that you
 * don't use unnecessary amounts of ram on this.
 */
typedef struct {
    s32 x, z;
    cube_entity cubes[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
} chunk_data;

/*
 * Render all the faces that you can see AND go through PER type so that you minimize texture loading.
 * And combine all those of the same type into one draw call.
 */
typedef struct {
    f32_v3 vertices[4];
    f32_v2 tex_coords[4];
    u8 indices_type;
} face; // Kan ha 326 tusen faces i 30MB, så disse burde lagres! Burde ha at man laster inn de nermeste 9 chunks i disse.

/*
 * We only need two corners with two dimensions, and we know the direction so that we can assume whether it's x and y or x and z etc.
 * List the same face_type grouped together instead of storing it with each face.
 */
// Stored (greedy) face description.
//
// Coordinates are inclusive ranges in a 2D slice:
//  - UP/DOWN:     A=x (0..15),             B=z (0..15),             C=y (0..CHUNK_HEIGHT-1)
//  - NORTH/SOUTH: A=x (0..15),             B=y (0..CHUNK_HEIGHT-1), C=z (0..15)
//  - EAST/WEST:   A=y (0..CHUNK_HEIGHT-1), B=z (0..15),             C=x (0..15)
//
// NOTE: uses u8, so CHUNK_HEIGHT must be <= 255.
typedef struct {
    u8 a0, a1;
    u8 b0, b1;
    u8 c;
    u8 dir;
    u8 type;
    u8 _pad;
} face_stored;
_Static_assert(CHUNK_HEIGHT <= 255, "face_stored uses u8; CHUNK_HEIGHT must be <= 255");

// Rendering batches (16-bit index limit)
#define MAX_FACES_PER_BATCH (MAX_VERTICES / 4u)
#define MAX_BATCHES_PER_TYPE ((FACE_POOL_SIZE + MAX_FACES_PER_BATCH - 1u) / MAX_FACES_PER_BATCH)

typedef struct {
    // Offsets into the global arrays
    u32 first_vertex;
    u32 vertex_count;
    // Index buffer stores packed pairs of u16 in u32 words.
    u32 first_index_u32;
    u32 index_count_u32;
} face_batch;

extern chunk_data loaded_chunks[CHUNK_AMOUNT];
extern u32 chunk_offsets[CHUNK_AMOUNT];
extern u32 num_chunks_pooled;

extern f32_v3 *chunk_vertices;
extern f32_v2 *chunk_tex_coords;
extern u32    *chunk_indices;
extern u32 offset_vertices[FACE_TYPE_AMOUNT - 1];
extern u32 offset_indices[FACE_TYPE_AMOUNT - 1];
extern u32 num_faces_type[FACE_TYPE_AMOUNT];

extern face_batch face_batches[FACE_TYPE_AMOUNT][MAX_BATCHES_PER_TYPE];
extern u8 num_face_batches[FACE_TYPE_AMOUNT];

extern face_stored faces_pool[FACE_POOL_SIZE];
extern u32 num_faces_pooled;


void init_world(void);
void destroy_world(void);
void load_chunks(void);
