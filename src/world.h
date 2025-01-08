#pragma once
#include "cube.h"



#define CHUNK_SIZE 16
#define CHUNK_VIEW_DISTANCE 2
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
    f32_v3 vertices[4];
    f32 tex_coords[4][2];
    u16 indices[6];
    u16 info;
} face; // Kan ha 326 tusen faces i 30MB, så disse burde lagres! Burde ha at man laster inn de nermeste 9 chunks i disse.

typedef struct {
    i8 a0, a1, b0, b1, c; // TODO siden vi vet at en chunk er 16x16x16 eller 32x32x32 
                          // så kan jeg bruke enten 4 eller 5 bits for hver av disse og så ha resten i info elns.
                          // Da kan jeg gå ned til 32 bits i stedet for 48 og ha 4 ekstra bits til info.
} corners_i8;
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
#define FACE_STORED_INFO_DIRECTION  0x00000007
#define SET_FACE_STORED(value, input, mask) (value) = ((value) & ~(mask)) | (((input & 0xF) << __builtin_ctz(mask)))
#define GET_FACE_STORED(value, mask) ((value & mask) >> __builtin_ctz(mask))

extern chunk_data *loaded_chunks;
extern face_stored *faces_pool;
extern face *faces_calculated_pool;
extern u32 num_faces_pooled;
extern u32 num_chunks_pooled;
extern u32 *chunk_offsets;

void init_world(void);
void destroy_world(void);
void generate_chunk(i32 chunk_x, i32 chunk_y, i32 chunk_z);
void load_chunks(void);
