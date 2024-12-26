#include "world.h"
#include "random.h"
#include <stdlib.h>
#include <string.h>

// it can render up to 1024 faces per draw call
#define FACE_POOL_SIZE 512
/*
 * Single zeroed pool of chunks.
 * Greedy meshing.
 * Calculate and store faces of the chunk.
 * Store those meshes further away on disk.
 * Load the faces you need to the gpu.
 */

chunk_data test_chunk = {0};
face_stored *faces_pool;
u32 num_faces_pooled = 0;

static face_stored find_single_face(
        int start_x, 
        int start_y, 
        int start_z, 
        u8 face_direction, 
        u8 covered[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE][FACE_DIRECTION_TOTAL]) {

    face_stored res = {0};
    bool first = true;

    if (face_direction <= FACE_DIRECTION_UP) {
        int max_z = CHUNK_SIZE; // the other size of the x tile
        for (int z = start_z + 1; z < CHUNK_SIZE; z++) {
            // check if covered or see through.
            if (test_chunk.cubes[start_x][start_y][z].type == BLOCK_TYPE_AIR
                    || (first && covered[start_x][start_y][z][face_direction])) {
                max_z = z;
                break;
            }
            first = false;
        }

        int max_x = CHUNK_SIZE; // andre siden av x tile
        for (int x = start_x + 1; x < CHUNK_SIZE; x++) {
            // check if from this x it crashes anywhere towards the max z.
            first = true;
            for (int z = start_z; z < max_z; z++) {
                if (test_chunk.cubes[x][start_y][z].type == BLOCK_TYPE_AIR
                        || (first && covered[x][start_y][z][face_direction])
                        ) {
                    max_x = x;
                    goto ExitLoopUp; 
                }
                first = false;
            }
        }
ExitLoopUp:
        res.info = face_direction;
        res.corners.a0 = start_x;
        res.corners.b0 = start_z;
        res.corners.a1 = max_x;
        res.corners.b1 = max_z;
        res.corners.c = face_direction == FACE_DIRECTION_DOWN ? start_y - 1 : start_y;

        // make sure that these are now covered as they are used in this face.
        for (int x = start_x; x < max_x; x++) {
            for (int z = start_z; z < max_z; z++) {
                covered[x][start_y][z][face_direction] = true;
            }
        } 
        return res;
    } 

    if (face_direction <= FACE_DIRECTION_NORTH) {
        int max_x = CHUNK_SIZE; // the other size of the x tile
        for (int x = start_x + 1; x < CHUNK_SIZE; x++) {
            // check if covered or see through.
            if (test_chunk.cubes[x][start_y][start_z].type == BLOCK_TYPE_AIR
                    || (first && covered[x][start_y][start_z][face_direction])) {
                max_x = x;
                break;
            }
            first = false;
        }

        int max_y = CHUNK_SIZE; // andre siden av x tile
        for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
            // check if from this x it crashes anywhere towards the max z.
            first = true;

            for (int x = start_x; x < max_x; x++) {
                if (test_chunk.cubes[x][y][start_z].type == BLOCK_TYPE_AIR
                        || (first && covered[x][y][start_z][face_direction])) {
                    max_y = y;
                    goto ExitLoopNorth; 
                }         
                first = false;
            }
        }
ExitLoopNorth:
        res.info = face_direction;
        res.corners.a0 = start_x;
        res.corners.b0 = start_y - 1;
        res.corners.a1 = max_x;
        res.corners.b1 = max_y - 1;
        res.corners.c = face_direction == FACE_DIRECTION_SOUTH ? start_z : start_z + 1;

        // make sure that these are now covered as they are used in this face.
        for (int x = start_x; x < max_x; x++) {
            for (int y = start_y; y < max_y; y++) {
                covered[x][y][start_z][face_direction] = true;
            }
        } 
        return res;
    }

    // face_direction <= FACE_DIRECTION_EAST
    // a=y, b=z
    int max_z = CHUNK_SIZE; // the other size of the x tile
    for (int z = start_z + 1; z < CHUNK_SIZE; z++) {
        // check if covered or see through.
        if (test_chunk.cubes[start_x][start_y][z].type == BLOCK_TYPE_AIR
                || (first && covered[start_x][start_y][z][face_direction])) {
            max_z = z;
            break;
        }
            first = false;
    }

    int max_y = CHUNK_SIZE; // andre siden av x tile
    for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
        // check if from this x it crashes anywhere towards the max z.
        first = true;
        for (int z = start_z; z < max_z; z++) {
            if (test_chunk.cubes[start_x][y][z].type == BLOCK_TYPE_AIR
                    || (first && covered[start_x][y][z][face_direction])) {
                max_y = y;
                goto ExitLoopEast; 
            }
            first = false;
        }
    }
ExitLoopEast: 

    res.info = face_direction;
    res.corners.a0 = start_y - 1;
    res.corners.b0 = start_z;
    res.corners.a1 = max_y - 1;
    res.corners.b1 = max_z;
    res.corners.c = face_direction == FACE_DIRECTION_EAST ? start_x + 1 : start_x;

    // make sure that these are now covered as they are used in this face.
    for (int z = start_z; z < max_z; z++) {
        for (int y = start_y; y < max_y; y++) {
            covered[start_x][y][z][face_direction] = true;
        }
    } 

    return res;
}

static face_stored *find_faces_of_chunk(
        u32 max_faces,
        u32 *out_faces_found,
        u8 covered[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE][FACE_DIRECTION_TOTAL]) {
    // First find all possible upwards facing faces.
    face_stored faces[max_faces]; // array for reading memory more easy...
    u32 num_faces_found = 0;
    int start_x, start_y, start_z;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (covered[x][y][z][direction] == false) {
                        // we got one!
                        face_stored found_one = find_single_face(x, y, z, direction, covered);
                        faces[num_faces_found] = found_one;
                        num_faces_found++;
                        if (num_faces_found == max_faces) {
                            *out_faces_found = num_faces_found;
                            face_stored *out_res = malloc(sizeof(face_stored) * max_faces);
                            memcpy(out_res, faces, sizeof(face_stored) * max_faces);
                            if (out_res == NULL) return NULL;
                            return out_res;
                        }
                    }
                }
            }
        }
    }
    *out_faces_found = num_faces_found;
    face_stored *out_res = malloc(sizeof(face_stored) * num_faces_found);
    memcpy(out_res, faces, sizeof(face_stored) * num_faces_found);
    if (out_res == NULL) return NULL;
    return out_res;
}

void init_world(void) {
    faces_pool = malloc(FACE_POOL_SIZE * sizeof(face_stored));
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
#define CHUNK_TEST 0
void generate_chunk(i32 chunk_x, i32 chunk_y) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
#if CHUNK_TEST == 0
            u64 y_ran = lehmer32_seeded(1032487 + 100*x*z);
            y_ran = y_ran % 3; 
            y_ran += 1;
            if (x > 1) y_ran += 1;
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (y < y_ran) {
                    test_chunk.cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                } else {
                    test_chunk.cubes[x][y][z].type = BLOCK_TYPE_AIR;
                }
            }
#elif CHUNK_TEST == 1
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if ((y < 2 && x % 2 != 0 && z % 2 != 0) || (y < 1)) {
                    test_chunk.cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                } else {
                    test_chunk.cubes[x][y][z].type = BLOCK_TYPE_AIR;
                }
            }
#endif
        }
    }
    u8 covered[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE][FACE_DIRECTION_TOTAL];

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (test_chunk.cubes[x][y][z].type == BLOCK_TYPE_AIR) {
                        covered[x][y][z][direction] = true;
                        continue;
                    }
                    switch (direction) {
                        case FACE_DIRECTION_DOWN:
                            if (y == 0) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x][y - 1][z].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                        case FACE_DIRECTION_UP:
                            if (y + 1 == CHUNK_SIZE) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x][y + 1][z].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                        case FACE_DIRECTION_SOUTH:
                            if (z == 0) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x][y][z - 1].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                        case FACE_DIRECTION_NORTH:
                            if (z + 1 == CHUNK_SIZE) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x][y][z + 1].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                        case FACE_DIRECTION_WEST:
                            if (x == 0) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x - 1][y][z].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                        case FACE_DIRECTION_EAST:
                            if (x + 1 == CHUNK_SIZE) {
                                covered[x][y][z][direction] = false;
                            } else if (test_chunk.cubes[x + 1][y][z].type == BLOCK_TYPE_AIR) {
                                covered[x][y][z][direction] = false;
                            } else {
                                covered[x][y][z][direction] = true;
                            }
                            break;
                    }
                }
            }
        }
    }

    u32 num_found;
    face_stored *faces = find_faces_of_chunk(FACE_POOL_SIZE, &num_found, covered);
    memcpy(&faces_pool[num_faces_pooled], faces, sizeof(face_stored) * num_found);
    num_faces_pooled += num_found;

    free(faces);
}
