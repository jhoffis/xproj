#pragma once
#include "cube.h"

#define MAX_VERTICES 65536
#define MAX_FACES 16384 // MAX_VERTICES / 4
#define INDEX_ALIGNMENT 49152 // MAX_VERTICES - MAX_FACES
#define MAX_RENDERED_FACES 40000 // 36864 // appearently

#define CHUNK_SIZE 16
#define CHUNK_VIEW_DISTANCE 6
#define CHUNK_AMOUNT CHUNK_VIEW_DISTANCE*CHUNK_VIEW_DISTANCE*CHUNK_VIEW_DISTANCE
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
    s32 x, y, z;
    cube_entity cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; // XYZ
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
typedef u32 face_stored; // Use to store basic info of the face, so that you can recreate it into a full face.
#define FACE_STORED_A0    0xF0000000
#define FACE_STORED_A1    0x0F000000
#define FACE_STORED_B0    0x00F00000
#define FACE_STORED_B1    0x000F0000
#define FACE_STORED_C     0x0000F000
#define FACE_STORED_INFO  0x00000FFF 
#define FACE_STORED_INFO_EXPANSION  0x00000800 
#define FACE_STORED_INFO_TYPE       0x000007F8 // 256 types
#define FACE_STORED_INFO_DIRECTION  0x00000007
// Bitfield helpers (works for any mask width, not just 4-bit fields)
#define SET_FACE_STORED(value, input, mask) do { \
    const u32 _shift = (u32)__builtin_ctz((u32)(mask)); \
    const u32 _field = ((u32)(mask)) >> _shift; \
    (value) = ((u32)(value) & ~((u32)(mask))) | ((((u32)(input)) & _field) << _shift); \
} while (0)

#define GET_FACE_STORED(value, mask) (((u32)(value) & (u32)(mask)) >> (u32)__builtin_ctz((u32)(mask)))

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
void generate_chunk(s32 chunk_x, s32 chunk_y, s32 chunk_z);
void load_chunks(void);
