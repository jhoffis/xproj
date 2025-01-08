#include "world.h"
#include "random.h"
#include "mvp.h"
#include <stdlib.h>
#include <string.h>

// Helper macro to index into the 1D covered array
#define COVERED(covered_ptr, x, y, z, dir) ((covered_ptr)[((x) * CHUNK_SIZE * CHUNK_SIZE * FACE_DIRECTION_TOTAL) + \
                                                ((z) * CHUNK_SIZE * FACE_DIRECTION_TOTAL) +              \
                                                ((y) * FACE_DIRECTION_TOTAL) + (dir)])
#define COVERED_FALSE 0
#define COVERED_TRUE 1
#define COVERED_CHECKED 2

/*
 * Single zeroed pool of chunks.
 * Greedy meshing.
 * Calculate and store faces of the chunk.
 * Store those meshes further away on disk.
 * Load the faces you need to the gpu.
 */

chunk_data *loaded_chunks;
face_stored *faces_pool;
face *faces_calculated_pool;
u32 num_faces_pooled = 0;
u32 num_chunks_pooled = 0;
u32 *chunk_offsets;


void init_world(void) {
    faces_pool = malloc(FACE_POOL_SIZE * sizeof(face_stored));
    loaded_chunks = calloc(25, sizeof(chunk_data));
    chunk_offsets = calloc(25, sizeof(u32));

    faces_calculated_pool = calloc(10000, sizeof(face));
}

void destroy_world(void) {
    free(loaded_chunks);
    free(faces_pool);
    free(faces_calculated_pool);
    free(chunk_offsets);
}

static face_stored find_single_face(
    chunk_data *chunk,
    int start_x,
    int start_y,
    int start_z,
    u8 face_direction,
    u8 *covered) 
{
    face_stored res = {0};
    u16 block_type = chunk->cubes[start_x][start_y][start_z].type & BLOCK_TYPE_MASK;

    if (face_direction <= FACE_DIRECTION_UP) {
        int max_z = CHUNK_SIZE - 1; 
        for (int z = start_z + 1; z < CHUNK_SIZE; z++) {
            if (chunk->cubes[start_x][start_y][z].type != block_type ||
                COVERED(covered, start_x, start_y, z, face_direction) == COVERED_CHECKED) {
                max_z = z - 1;
                break;
            }
        }

        int max_x = CHUNK_SIZE - 1; 
        for (int x = start_x + 1; x < CHUNK_SIZE; x++) {
            bool found_wrong_type = false;
            int found_opens = 0;
            for (int z = start_z; z <= max_z; z++) {
                if (chunk->cubes[x][start_y][z].type != block_type) {
                    found_wrong_type = true;
                    break;
                }
                if (COVERED(covered, x, start_y, z, face_direction) == COVERED_FALSE) {
                    found_opens++;
                }
            }
            if (found_wrong_type || found_opens == 0) {
                max_x = x - 1;
                break;
            }
        }

        SET_FACE_STORED(res, face_direction, FACE_STORED_INFO_DIRECTION);
        SET_FACE_STORED(res, start_x, FACE_STORED_A0);
        SET_FACE_STORED(res, start_z, FACE_STORED_B0);
        SET_FACE_STORED(res, max_x, FACE_STORED_A1);
        SET_FACE_STORED(res, max_z, FACE_STORED_B1);
        SET_FACE_STORED(res, start_y, FACE_STORED_C);

        for (int x = start_x; x <= max_x; x++) {
            for (int z = start_z; z <= max_z; z++) {
                COVERED(covered, x, start_y, z, face_direction) = COVERED_CHECKED;
            }
        }
        return res;
    } 

    if (face_direction <= FACE_DIRECTION_NORTH) {
        int max_x = CHUNK_SIZE - 1; 
        for (int x = start_x; x < CHUNK_SIZE; x++) {
            if (chunk->cubes[x][start_y][start_z].type != block_type ||
                COVERED(covered, x, start_y, start_z, face_direction)) {
                max_x = x - 1;
                break;
            }
        }

        int max_y = CHUNK_SIZE - 1; 
        for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
            for (int x = start_x; x <= max_x; x++) {
                if (chunk->cubes[x][y][start_z].type != block_type ||
                    COVERED(covered, x, y, start_z, face_direction)) {
                    max_y = y - 1;
                    goto BreakNorthLoop;
                }
            }
        }
BreakNorthLoop:
        SET_FACE_STORED(res, face_direction, FACE_STORED_INFO_DIRECTION);
        SET_FACE_STORED(res, start_x, FACE_STORED_A0);
        SET_FACE_STORED(res, start_y, FACE_STORED_B0);
        SET_FACE_STORED(res, max_x, FACE_STORED_A1);
        SET_FACE_STORED(res, max_y, FACE_STORED_B1);
        SET_FACE_STORED(res, start_z, FACE_STORED_C);

        for (int x = start_x; x <= max_x; x++) {
            for (int y = start_y; y <= max_y; y++) {
                COVERED(covered, x, y, start_z, face_direction) = COVERED_CHECKED;
            }
        }
        return res;
    }

    // FACE_DIRECTION_EAST
    int max_z = CHUNK_SIZE - 1; 
    for (int z = start_z; z < CHUNK_SIZE; z++) {
        if (chunk->cubes[start_x][start_y][z].type != block_type ||
            COVERED(covered, start_x, start_y, z, face_direction)) {
            max_z = z - 1;
            break;
        }
    }

    int max_y = CHUNK_SIZE - 1; 
    for (int y = start_y + 1; y < CHUNK_SIZE; y++) {
        for (int z = start_z; z <= max_z; z++) {
            if (chunk->cubes[start_x][y][z].type != block_type ||
                COVERED(covered, start_x, y, z, face_direction)) {
                max_y = y - 1;
                goto BreakEastLoop;
            }
        }
    }
BreakEastLoop:
    SET_FACE_STORED(res, face_direction, FACE_STORED_INFO_DIRECTION);
    SET_FACE_STORED(res, start_y, FACE_STORED_A0);
    SET_FACE_STORED(res, start_z, FACE_STORED_B0);
    SET_FACE_STORED(res, max_y, FACE_STORED_A1);
    SET_FACE_STORED(res, max_z, FACE_STORED_B1);
    SET_FACE_STORED(res, start_x, FACE_STORED_C);

    for (int z = start_z; z <= max_z; z++) {
        for (int y = start_y; y <= max_y; y++) {
            COVERED(covered, start_x, y, z, face_direction) = COVERED_CHECKED;
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
                    if (COVERED(covered, x, y, z, direction) == COVERED_FALSE) {
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
                if ((y < 5 && x % 2 != 0 && z % 2 != 0) || (y < 3) && x+z != 2) {
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
    memset(covered, COVERED_TRUE, covered_size); // Initialize all to true

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (chunk->cubes[x][y][z].type == BLOCK_TYPE_AIR) {
                        continue;
                    }
                    switch (direction) {
                        case FACE_DIRECTION_DOWN:
                            if (y == 0 || chunk->cubes[x][y - 1][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
                            }
                            break;
                        case FACE_DIRECTION_UP:
                            if (y + 1 == CHUNK_SIZE || chunk->cubes[x][y + 1][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
                            }
                            break;
                        case FACE_DIRECTION_SOUTH:
                            if (z == 0 || chunk->cubes[x][y][z - 1].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
                            }
                            break;
                        case FACE_DIRECTION_NORTH:
                            if (z + 1 == CHUNK_SIZE || chunk->cubes[x][y][z + 1].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
                            }
                            break;
                        case FACE_DIRECTION_WEST:
                            if (x == 0 || chunk->cubes[x - 1][y][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
                            }
                            break;
                        case FACE_DIRECTION_EAST:
                            if (x + 1 == CHUNK_SIZE || chunk->cubes[x + 1][y][z].type == BLOCK_TYPE_AIR) {
                                COVERED(covered, x, y, z, direction) = COVERED_FALSE;
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

static void convert_face_vertices(face *out, f32 chunk_offset[3], face_stored face) {
    out->info = GET_FACE_STORED(face, FACE_STORED_INFO); 
    int a0 = GET_FACE_STORED(face, FACE_STORED_A0) * (int)cube_size;
    int a1 = (1 + GET_FACE_STORED(face, FACE_STORED_A1)) * (int)cube_size;
    int b0 = GET_FACE_STORED(face, FACE_STORED_B0) * (int)cube_size;
    int b1 = (1 + GET_FACE_STORED(face, FACE_STORED_B1)) * (int)cube_size;
    int c  = GET_FACE_STORED(face, FACE_STORED_C)  * (int)cube_size;

    int tex_a = (1 + GET_FACE_STORED(face, FACE_STORED_A1)) - GET_FACE_STORED(face, FACE_STORED_A0);
    int tex_b = (1 + GET_FACE_STORED(face, FACE_STORED_B1)) - GET_FACE_STORED(face, FACE_STORED_B0);

    int direction = GET_FACE_STORED(face, FACE_STORED_INFO_DIRECTION);
    if (direction <= FACE_DIRECTION_UP) {
        if (direction == FACE_DIRECTION_UP) {
            c += (int)cube_size;
        }

        out->vertices[0].x = chunk_offset[0] + a0;
        out->vertices[0].y = chunk_offset[1] + c;
        out->vertices[0].z = chunk_offset[2] + b1;
        out->tex_coords[0][0] = 0;
        out->tex_coords[0][1] = tex_b;

        out->vertices[1].x = chunk_offset[0] + a1;
        out->vertices[1].y = chunk_offset[1] + c;
        out->vertices[1].z = chunk_offset[2] + b1;
        out->tex_coords[1][0] = tex_a;
        out->tex_coords[1][1] = tex_b;

        out->vertices[2].x = chunk_offset[0] + a1;
        out->vertices[2].y = chunk_offset[1] + c;
        out->vertices[2].z = chunk_offset[2] + b0;
        out->tex_coords[2][0] = tex_a;
        out->tex_coords[2][1] = 0;
                          
        out->vertices[3].x = chunk_offset[0] + a0;
        out->vertices[3].y = chunk_offset[1] + c;
        out->vertices[3].z = chunk_offset[2] + b0;
        out->tex_coords[3][0] = 0;
        out->tex_coords[3][1] = 0;
        return;
    } 

    if (direction <= FACE_DIRECTION_NORTH) {
        if (direction == FACE_DIRECTION_NORTH) {
            c += (int)cube_size;
        }

        out->vertices[0].x = chunk_offset[0] + a1;
        out->vertices[0].y = chunk_offset[1] + b0;
        out->vertices[0].z = chunk_offset[2] + c;
                          
        out->vertices[1].x = chunk_offset[0] + a1;
        out->vertices[1].y = chunk_offset[1] + b1;
        out->vertices[1].z = chunk_offset[2] + c;
                          
        out->vertices[2].x = chunk_offset[0] + a0;
        out->vertices[2].y = chunk_offset[1] + b1;
        out->vertices[2].z = chunk_offset[2] + c;
                          
        out->vertices[3].x = chunk_offset[0] + a0;
        out->vertices[3].y = chunk_offset[1] + b0;
        out->vertices[3].z = chunk_offset[2] + c;

        out->tex_coords[0][0] = 0;
        out->tex_coords[0][1] = tex_a;
        out->tex_coords[1][0] = tex_b;
        out->tex_coords[1][1] = tex_a;
        out->tex_coords[2][0] = tex_b;
        out->tex_coords[2][1] = 0;
        out->tex_coords[3][0] = 0;
        out->tex_coords[3][1] = 0;
        return;
    } 

    if (direction == FACE_DIRECTION_EAST) {
        c += (int)cube_size;
    }

    out->vertices[0].x = chunk_offset[0] + c;
    out->vertices[0].y = chunk_offset[1] + a0;
    out->vertices[0].z = chunk_offset[2] + b1;
    out->tex_coords[0][0] = 0;
    out->tex_coords[0][1] = tex_b;
                      
    out->vertices[1].x = chunk_offset[0] + c;
    out->vertices[1].y = chunk_offset[1] + a1;
    out->vertices[1].z = chunk_offset[2] + b1;
    out->tex_coords[1][0] = tex_a;
    out->tex_coords[1][1] = tex_b;
                      
    out->vertices[2].x = chunk_offset[0] + c;
    out->vertices[2].y = chunk_offset[1] + a1;
    out->vertices[2].z = chunk_offset[2] + b0;
    out->tex_coords[2][0] = tex_a;
    out->tex_coords[2][1] = 0;
                      
    out->vertices[3].x = chunk_offset[0] + c;
    out->vertices[3].y = chunk_offset[1] + a0;
    out->vertices[3].z = chunk_offset[2] + b0;
    out->tex_coords[3][0] = 0;
    out->tex_coords[3][1] = 0;
}

/*
 * TODO skal laste inn chunks basert på hvor spilleren er.
 *      Kalkuler også chunk-endepunkter på mellom chunks og sambind flater.
 *      Endepunkter skal ikke bare blindt lage faces.
 *      Gjør også de lenger unna gradvis mer og mer lod.
 */
void load_chunks(void) {

    // load chunks around player, if not loaded in, check disk, if not there then generate and store on disk.
    int current_chunk_x = (int)(floorf(v_cam_loc.x / (CHUNK_SIZE * BLOCK_SIZE)));
    int current_chunk_y = (int)(floorf(v_cam_loc.y / (CHUNK_SIZE * BLOCK_SIZE)));
    int current_chunk_z = (int)(floorf(v_cam_loc.z / (CHUNK_SIZE * BLOCK_SIZE)));

    for (int x = current_chunk_x - CHUNK_VIEW_DISTANCE; x < current_chunk_x + CHUNK_VIEW_DISTANCE; x++) {
        for (int z = current_chunk_z - CHUNK_VIEW_DISTANCE; z < current_chunk_z + CHUNK_VIEW_DISTANCE; z++) {

            generate_chunk(x, 0, z);
        }
    }

    int chunk_i = 0, chunk_i_cmp = 0;
    for (int i = 0; i < num_faces_pooled; i++) {
        if (chunk_i_cmp >= chunk_offsets[chunk_i]) {
            chunk_i++;
            chunk_i_cmp = 0;
        }
        chunk_i_cmp++;
        f32 pos_offset[3] = {(f32) (loaded_chunks[chunk_i].x * cube_size * CHUNK_SIZE), 
                             (f32) (loaded_chunks[chunk_i].y * cube_size * CHUNK_SIZE), 
                             (f32) (loaded_chunks[chunk_i].z * cube_size * CHUNK_SIZE)};

        face_stored fs = faces_pool[i];
        face f = {0};
        convert_face_vertices(&f, pos_offset, fs);
        memcpy(&faces_calculated_pool[i], &f, sizeof(face));
   }
}
