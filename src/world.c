#include "world.h"
#include "random.h"
#include <stdlib.h>
#include <string.h>

// Helper macro to index into the 1D covered array
#define COVERED(covered_ptr, x, y, z, dir) ((covered_ptr)[((x) * CHUNK_SIZE * CHUNK_SIZE * FACE_DIRECTION_TOTAL) + \
                                                ((z) * CHUNK_SIZE * FACE_DIRECTION_TOTAL) +              \
                                                ((y) * FACE_DIRECTION_TOTAL) + (dir)])

/*
 * Single zeroed pool of chunks.
 * Greedy meshing.
 * Calculate and store faces of the chunk.
 * Store those meshes further away on disk.
 * Load the faces you need to the gpu.
 */

chunk_data *loaded_chunks;
face_stored *faces_pool;
u32 num_faces_pooled = 0;
u32 num_chunks_pooled = 0;
u32 *chunk_offsets;

static face_stored find_single_face(
    chunk_data *chunk,
    int start_x,
    int start_y,
    int start_z,
    u8 face_direction,
    u8 *covered) 
{
    face_stored res = {0};
    bool first = true;

    if (face_direction <= FACE_DIRECTION_UP) {
        int max_z = CHUNK_SIZE; 
        for (int z = start_z + 1; z < CHUNK_SIZE; z++) {
            if (chunk->cubes[start_x][start_y][z].type == BLOCK_TYPE_AIR ||
                (first && COVERED(covered, start_x, start_y, z, face_direction))) {
                max_z = z;
                break;
            }
            first = false;
        }

        int max_x = CHUNK_SIZE; 
        for (int x = start_x + 1; x < CHUNK_SIZE; x++) {
            first = true;
            for (int z = start_z; z < max_z; z++) {
                if (chunk->cubes[x][start_y][z].type == BLOCK_TYPE_AIR ||
                    (first && COVERED(covered, x, start_y, z, face_direction))) {
                    max_x = x;
                    goto BreakUpLoop;
                }
                first = false;
            }
        }
BreakUpLoop:
        res.info = face_direction;
        res.corners.a0 = start_x;
        res.corners.b0 = start_z;
        res.corners.a1 = max_x;
        res.corners.b1 = max_z;
        res.corners.c = (face_direction == FACE_DIRECTION_DOWN) ? start_y - 1 : start_y;

        for (int x = start_x; x < max_x; x++) {
            for (int z = start_z; z < max_z; z++) {
                COVERED(covered, x, start_y, z, face_direction) = true;
            }
        }
        return res;
    } 

    if (face_direction <= FACE_DIRECTION_NORTH) {
        int max_x = CHUNK_SIZE; 
        for (int x = start_x + 1; x < CHUNK_SIZE; x++) {
            if (chunk->cubes[x][start_y][start_z].type == BLOCK_TYPE_AIR ||
                (first && COVERED(covered, x, start_y, start_z, face_direction))) {
                max_x = x;
                break;
            }
            first = false;
        }

        int max_y = CHUNK_SIZE; 
        for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
            first = true;
            for (int x = start_x; x < max_x; x++) {
                if (chunk->cubes[x][y][start_z].type == BLOCK_TYPE_AIR ||
                    (first && COVERED(covered, x, y, start_z, face_direction))) {
                    max_y = y;
                    goto BreakNorthLoop;
                }
                first = false;
            }
        }
BreakNorthLoop:
        res.info = face_direction;
        res.corners.a0 = start_x;
        res.corners.b0 = start_y - 1;
        res.corners.a1 = max_x;
        res.corners.b1 = max_y - 1;
        res.corners.c = (face_direction == FACE_DIRECTION_SOUTH) ? start_z : start_z + 1;

        for (int x = start_x; x < max_x; x++) {
            for (int y = start_y; y < max_y; y++) {
                COVERED(covered, x, y, start_z, face_direction) = true;
            }
        }
        return res;
    }

    // FACE_DIRECTION_EAST
    int max_z = CHUNK_SIZE; 
    for (int z = start_z + 1; z < CHUNK_SIZE; z++) {
        if (chunk->cubes[start_x][start_y][z].type == BLOCK_TYPE_AIR ||
            (first && COVERED(covered, start_x, start_y, z, face_direction))) {
            max_z = z;
            break;
        }
        first = false;
    }

    int max_y = CHUNK_SIZE; 
    for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
        first = true;
        for (int z = start_z; z < max_z; z++) {
            if (chunk->cubes[start_x][y][z].type == BLOCK_TYPE_AIR ||
                (first && COVERED(covered, start_x, y, z, face_direction))) {
                max_y = y;
                goto BreakEastLoop;
            }
            first = false;
        }
    }
BreakEastLoop:
    res.info = face_direction;
    res.corners.a0 = start_y - 1;
    res.corners.b0 = start_z;
    res.corners.a1 = max_y - 1;
    res.corners.b1 = max_z;
    res.corners.c = (face_direction == FACE_DIRECTION_EAST) ? start_x + 1 : start_x;

    for (int z = start_z; z < max_z; z++) {
        for (int y = start_y; y < max_y; y++) {
            COVERED(covered, start_x, y, z, face_direction) = true;
        }
    }
    return res;
}

static void find_faces_of_chunk(
        chunk_data *chunk,
        u32 max_faces,
        u32 *out_faces_found,
        u8 *covered) {
    // First find all possible upwards facing faces.
    face_stored faces[max_faces]; // array for reading memory more easy...
    u32 num_faces_found = 0;
    int start_x, start_y, start_z;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (COVERED(covered, x, y, z, direction) == false) {
                        // we got one!
                        face_stored found_one = find_single_face(chunk, x, y, z, direction, covered);
                        faces[num_faces_found] = found_one;
                        num_faces_found++;
                        if (num_faces_found == max_faces) {
                            *out_faces_found = num_faces_found;
                            memcpy(&faces_pool[num_faces_pooled], faces, sizeof(face_stored) * num_faces_found);
                            num_faces_pooled += num_faces_found;
                            return;
                        }
                    }
                }
            }
        }
    }
    *out_faces_found = num_faces_found;
    memcpy(&faces_pool[num_faces_pooled], faces, sizeof(face_stored) * num_faces_found);
    num_faces_pooled += num_faces_found;
}

void init_world(void) {
    faces_pool = malloc(FACE_POOL_SIZE * sizeof(face_stored));
    loaded_chunks = calloc(4, sizeof(chunk_data));
    chunk_offsets = calloc(4, sizeof(u32));

    // FIXME if faces_pool == null
}

/*
 * FIXME When you create a chunk, map all the tiles to be faced for each direction 
 * as long as that direction is open!
 * For now we can just store the faced values on the blocks themselves, but that is
 * ultimately not necessary as you would only need to check this when creating a new
 * chunk and when altering a existing chunk's face. Therefore, in the future, make it
 * only a temporary list of values for all blocks in the chunk.
 * Actually, you don't need to know if it's covered in any way as long as the faces
 * are made!
 */
// #define CHUNK_TEST 0
void generate_chunk(i32 chunk_x, i32 chunk_y, i32 chunk_z) {
    chunk_data *chunk = &loaded_chunks[num_chunks_pooled];
    chunk->x = chunk_x;
    chunk->y = chunk_y;
    chunk->z = chunk_z;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if ((y < 5 && x % 2 != 0 && z % 2 != 0) || (y < 3)) {
                    chunk->cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                } else {
                    chunk->cubes[x][y][z].type = BLOCK_TYPE_AIR;
                }
            }
        }
    }

    size_t covered_size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * FACE_DIRECTION_TOTAL * sizeof(u8);
    u8 *covered = (u8 *)malloc(covered_size);
    if (!covered) {
        exit(EXIT_FAILURE);
    }
    memset(covered, true, covered_size); // Initialize all to true

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (chunk->cubes[x][y][z].type == BLOCK_TYPE_AIR) {
                        // COVERED(covered, x, y, z, direction) = true;
                        continue;
                    }
                    switch (direction) {
                        case FACE_DIRECTION_DOWN:
                            if (y == 0 || chunk->cubes[x][y - 1][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                        case FACE_DIRECTION_UP:
                            if (y + 1 == CHUNK_SIZE || chunk->cubes[x][y + 1][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                        case FACE_DIRECTION_SOUTH:
                            if (z == 0 || chunk->cubes[x][y][z - 1].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                        case FACE_DIRECTION_NORTH:
                            if (z + 1 == CHUNK_SIZE || chunk->cubes[x][y][z + 1].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                        case FACE_DIRECTION_WEST:
                            if (x == 0 || chunk->cubes[x - 1][y][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                        case FACE_DIRECTION_EAST:
                            if (x + 1 == CHUNK_SIZE || chunk->cubes[x + 1][y][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = false;
                            }
                            break;
                    }
                }
            }
        }
    }

    u32 num_found;
    find_faces_of_chunk(chunk, FACE_POOL_SIZE, &num_found, covered);
    free(covered);

    chunk_offsets[num_chunks_pooled] = num_found;
    num_chunks_pooled++;
}
