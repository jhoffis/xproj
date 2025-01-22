#include "world.h"
#include "allocator.h"
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
u32 *chunk_offsets;
u32 num_chunks_pooled = 0;

f32_v3 *chunk_vertices;
f32_v2 *chunk_tex_coords;
u32    *chunk_indices;
u32 *offset_vertices;
u32 *offset_indices;
u32 *num_faces_type;

face_stored *faces_pool;
u32 num_faces_pooled = 0;


void init_world(void) {
    faces_pool = xmalloc(FACE_POOL_SIZE * sizeof(face_stored));
    loaded_chunks = xcalloc(CHUNK_AMOUNT, sizeof(chunk_data));
    chunk_offsets = xcalloc(CHUNK_AMOUNT, sizeof(u32));

    chunk_vertices   = xMmAllocateContiguousMemoryEx(FACE_POOL_SIZE * 4 * sizeof(f32_v3), 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    chunk_tex_coords = xMmAllocateContiguousMemoryEx(FACE_POOL_SIZE * 4 * sizeof(f32_v2), 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    // chunk_indices    = MmAllocateContiguousMemoryEx(FACE_POOL_SIZE * 6 * sizeof(u16), 0, MAX_MEM_64, 0, PAGE_READWRITE);
    chunk_indices = x_aligned_malloc(FACE_POOL_SIZE * 6 * sizeof(u16), 16);
    
    offset_vertices = xmalloc(sizeof(u32) * (FACE_TYPE_AMOUNT - 1));
    offset_indices = xmalloc(sizeof(u32) * (FACE_TYPE_AMOUNT - 1));

    num_faces_type = xcalloc(FACE_TYPE_AMOUNT, sizeof(u32));
}

void destroy_world(void) {
    xfree(num_faces_type);
    xfree(offset_indices);
    xfree(offset_vertices);
    xfree(loaded_chunks);
    xfree(faces_pool);
    xfree(chunk_offsets);
    xMmFreeContiguousMemory(chunk_vertices);
    xMmFreeContiguousMemory(chunk_tex_coords);
    x_aligned_free(chunk_indices);
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
        SET_FACE_STORED(res, block_type, FACE_STORED_INFO_TYPE);
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
        SET_FACE_STORED(res, block_type, FACE_STORED_INFO_TYPE);
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
    SET_FACE_STORED(res, block_type, FACE_STORED_INFO_TYPE);
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
        u32 *out_faces_found,
        u8 *covered) {
    // First find all possible upwards facing faces.
    if (num_faces_pooled == FACE_POOL_SIZE) {
        return;
    }
    u32 num_faces_found = 0;
    int start_x, start_y, start_z;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int direction = 0; direction < FACE_DIRECTION_TOTAL; direction++) {
                    if (COVERED(covered, x, y, z, direction) == COVERED_FALSE) {
                        // we got one!
                        face_stored found_one = find_single_face(chunk, x, y, z, direction, covered);
                        faces_pool[num_faces_pooled] = found_one;
                        num_faces_pooled++;
                        num_faces_found++;
                        if (num_faces_pooled == FACE_POOL_SIZE) {
                            *out_faces_found = num_faces_found;
                            return;
                        }
                    }
                }
            }
        }
    }
    *out_faces_found = num_faces_found;
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
                if (chunk_y == 0) {
                    if (y < 5 && x % 2 != 0 && z % 2 != 0) {                    
                        if (y == 4)
                            chunk->cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                        else 
                            chunk->cubes[x][y][z].type = BLOCK_TYPE_DIRT;
                    } else if (y < 3 && x+z != 2) {
                        if (y == 2)
                            chunk->cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                        else 
                            chunk->cubes[x][y][z].type = BLOCK_TYPE_DIRT;
                        // } else if (x % 2 == 0) {
                        // chunk->cubes[x][y][z].type = BLOCK_TYPE_COBBLESTONE;
                } else {
                    chunk->cubes[x][y][z].type = BLOCK_TYPE_AIR;
                }
                // chunk->cubes[x][y][z].type = BLOCK_TYPE_GRASS;
                } else {
                    if (x == 0 && z == 0) {
                        chunk->cubes[x][y][z].type = BLOCK_TYPE_COBBLESTONE;
                    } else {
                        chunk->cubes[x][y][z].type = BLOCK_TYPE_AIR;
                    }
                }
            }
        }
    }

    size_t covered_size = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * FACE_DIRECTION_TOTAL * sizeof(u8);
    u8 *covered = xmalloc(covered_size);
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
    find_faces_of_chunk(chunk, &num_found, covered);
    xfree(covered);

    chunk_offsets[num_chunks_pooled] = num_found;
    num_chunks_pooled++;
}

static void fill_face_indices(u16 indices[], u8 type) {
    switch (type) { 
        case 1:
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 3;
            indices[3] = 1;
            indices[4] = 2;
            indices[5] = 3;
            break;
        case 2:
            indices[0] = 0;
            indices[1] = 3;
            indices[2] = 1;
            indices[3] = 3;
            indices[4] = 2;
            indices[5] = 1;
            break;
    }
}

static u8 determine_face_indices_type(u8 direction) {
    switch (direction) { 
        case FACE_DIRECTION_DOWN:
        case FACE_DIRECTION_SOUTH:
        case FACE_DIRECTION_EAST:
            return 1;
        case FACE_DIRECTION_UP:
        case FACE_DIRECTION_NORTH:
        case FACE_DIRECTION_WEST:
            return 2;
    }
    return 0;
}

static void convert_face_vertices(face *out, f32 chunk_offset[3], face_stored face) {
    // out->info = GET_FACE_STORED(face, FACE_STORED_INFO); 
    int a0 = GET_FACE_STORED(face, FACE_STORED_A0) * (int)BLOCK_SIZE;
    int a1 = (1 + GET_FACE_STORED(face, FACE_STORED_A1)) * (int)BLOCK_SIZE;
    int b0 = GET_FACE_STORED(face, FACE_STORED_B0) * (int)BLOCK_SIZE;
    int b1 = (1 + GET_FACE_STORED(face, FACE_STORED_B1)) * (int)BLOCK_SIZE;
    int c  = GET_FACE_STORED(face, FACE_STORED_C)  * (int)BLOCK_SIZE;

    int tex_a = (1 + GET_FACE_STORED(face, FACE_STORED_A1)) - GET_FACE_STORED(face, FACE_STORED_A0);
    int tex_b = (1 + GET_FACE_STORED(face, FACE_STORED_B1)) - GET_FACE_STORED(face, FACE_STORED_B0);

    int direction = GET_FACE_STORED(face, FACE_STORED_INFO_DIRECTION);

    out->indices_type = determine_face_indices_type(direction);
    
    if (direction <= FACE_DIRECTION_UP) {
        if (direction == FACE_DIRECTION_UP) {
            c += (int)BLOCK_SIZE;
        }

        out->vertices[0].x = chunk_offset[0] + a0;
        out->vertices[0].y = chunk_offset[1] + c;
        out->vertices[0].z = chunk_offset[2] + b1;
        out->tex_coords[0].x = 0;
        out->tex_coords[0].y = tex_b;

        out->vertices[1].x = chunk_offset[0] + a1;
        out->vertices[1].y = chunk_offset[1] + c;
        out->vertices[1].z = chunk_offset[2] + b1;
        out->tex_coords[1].x = tex_a;
        out->tex_coords[1].y = tex_b;

        out->vertices[2].x = chunk_offset[0] + a1;
        out->vertices[2].y = chunk_offset[1] + c;
        out->vertices[2].z = chunk_offset[2] + b0;
        out->tex_coords[2].x = tex_a;
        out->tex_coords[2].y = 0;
                          
        out->vertices[3].x = chunk_offset[0] + a0;
        out->vertices[3].y = chunk_offset[1] + c;
        out->vertices[3].z = chunk_offset[2] + b0;
        out->tex_coords[3].x = 0;
        out->tex_coords[3].y = 0;
        return;
    } 

    if (direction <= FACE_DIRECTION_NORTH) {
        if (direction == FACE_DIRECTION_NORTH) {
            c += (int)BLOCK_SIZE;
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

        out->tex_coords[0].x = tex_a;
        out->tex_coords[0].y = tex_b;
        out->tex_coords[1].x = tex_a;
        out->tex_coords[1].y = 0;
        out->tex_coords[2].x = 0;
        out->tex_coords[2].y = 0;
        out->tex_coords[3].x = 0;
        out->tex_coords[3].y = tex_b;
        return;
    } 

    if (direction == FACE_DIRECTION_EAST) {
        c += (int)BLOCK_SIZE;
    }

    out->vertices[0].x = chunk_offset[0] + c;
    out->vertices[0].y = chunk_offset[1] + a0;
    out->vertices[0].z = chunk_offset[2] + b1;
                      
    out->vertices[1].x = chunk_offset[0] + c;
    out->vertices[1].y = chunk_offset[1] + a1;
    out->vertices[1].z = chunk_offset[2] + b1;
                      
    out->vertices[2].x = chunk_offset[0] + c;
    out->vertices[2].y = chunk_offset[1] + a1;
    out->vertices[2].z = chunk_offset[2] + b0;
                      
    out->vertices[3].x = chunk_offset[0] + c;
    out->vertices[3].y = chunk_offset[1] + a0;
    out->vertices[3].z = chunk_offset[2] + b0;

        out->tex_coords[0].x = 0;
        out->tex_coords[0].y = tex_a;
        out->tex_coords[1].x = 0;
        out->tex_coords[1].y = 0;
        out->tex_coords[2].x = tex_b;
        out->tex_coords[2].y = 0;
        out->tex_coords[3].x = tex_b;
        out->tex_coords[3].y = tex_a;
}

/*
 * TODO skal laste inn chunks basert på hvor spilleren er.
 *      Kalkuler også chunk-endepunkter på mellom chunks og sambind flater.
 *      Endepunkter skal ikke bare blindt lage faces.
 *      Gjør også de lenger unna gradvis mer og mer lod.
 *
 *
 *      Sort types within every chunk, and then seperate the chunks into hashmap.
 */

void load_chunks(void) {
    int current_chunk_x = (int)(floorf(v_cam_loc.x / (CHUNK_SIZE * BLOCK_SIZE)));
    int current_chunk_y = (int)(floorf(v_cam_loc.y / (CHUNK_SIZE * BLOCK_SIZE)));
    int current_chunk_z = (int)(floorf(v_cam_loc.z / (CHUNK_SIZE * BLOCK_SIZE)));

    const int view_dist = CHUNK_VIEW_DISTANCE / 2;

    // Load chunks in the view distance
    for (int x = current_chunk_x - view_dist; x < current_chunk_x + view_dist; x++) {
        for (int y = current_chunk_y - view_dist; y < current_chunk_y + view_dist; y++) {
            for (int z = current_chunk_z - view_dist; z < current_chunk_z + view_dist; z++) {
                generate_chunk(x, y, z);
            }
        }
    }

    // Allocate dynamic storage for face types
    u16 *types_sizes = xcalloc(CHUNK_AMOUNT * FACE_TYPE_AMOUNT, sizeof(u16));
    u16 *types_capacities = xcalloc(CHUNK_AMOUNT * FACE_TYPE_AMOUNT, sizeof(u16));
    face **types_faces = xcalloc(CHUNK_AMOUNT * FACE_TYPE_AMOUNT, sizeof(face*));

    const f32 BLOCK_CHUNK_SCALE = BLOCK_SIZE * CHUNK_SIZE;

    f32 pos_offset[3] = {(f32) (loaded_chunks[0].x * BLOCK_CHUNK_SCALE), 
                         (f32) (loaded_chunks[0].y * BLOCK_CHUNK_SCALE), 
                         (f32) (loaded_chunks[0].z * BLOCK_CHUNK_SCALE)};
    int chunk_i = 0, chunk_i_cmp = 0;
    for (int i = 0; i < num_faces_pooled; i++) {
        if (chunk_i_cmp >= chunk_offsets[chunk_i]) {
            chunk_i++;
            chunk_i_cmp = 0;

            pos_offset[0] = loaded_chunks[chunk_i].x * BLOCK_CHUNK_SCALE;
            pos_offset[1] = loaded_chunks[chunk_i].y * BLOCK_CHUNK_SCALE;
            pos_offset[2] = loaded_chunks[chunk_i].z * BLOCK_CHUNK_SCALE;
        }
        chunk_i_cmp++;

        face_stored fs = faces_pool[i];
        face f = {0};
        convert_face_vertices(&f, pos_offset, fs);

        u32 face_type = convert_block_to_face_type(
            GET_FACE_STORED(fs, FACE_STORED_INFO_TYPE),
            GET_FACE_STORED(fs, FACE_STORED_INFO_DIRECTION)
            );

        const int chunk_face_idx = chunk_i * FACE_TYPE_AMOUNT + face_type;

        // Check and grow capacity if needed
        if (types_sizes[chunk_face_idx] >= types_capacities[chunk_face_idx]) {
            int new_capacity = (types_capacities[chunk_face_idx] == 0) ? 16 : types_capacities[chunk_face_idx] * 2;

            void *temp_ptr = xrealloc(types_faces[chunk_face_idx], new_capacity * sizeof(face));
            if (!temp_ptr) {
                perror("Failed to reallocate types_faces");
                xfree(types_sizes);
                xfree(types_capacities);
                xfree(types_faces);
                return;
            }
            types_faces[chunk_face_idx] = temp_ptr;
            types_capacities[chunk_face_idx] = new_capacity;
        }

        // Add face to the array
        types_faces[chunk_face_idx][types_sizes[chunk_face_idx]++] = f;
    }

    // Prepare vertices and indices
    u32 vertex_i = 0, indices = 0;
    for (int ftype = 0; ftype < FACE_TYPE_AMOUNT; ftype++) {
        u32 this_vertex_i = 0;
        u32 temp_faces = 0;
        for (int c = 0; c < CHUNK_AMOUNT; c++) {
            const int chunk_face_idx = c * FACE_TYPE_AMOUNT + ftype;
            const u32 type_size = types_sizes[chunk_face_idx];
            if (type_size == 0) continue;

            num_faces_type[ftype] += type_size; // Reset or move `num_faces_type` on reload

            for (int fi = 0; fi < type_size; fi++) {
                u16 index_nums[4];
                face f = types_faces[chunk_face_idx][fi];
                temp_faces++;

                if (this_vertex_i >= MAX_VERTICES) {
                    // Indices are 16 bit on the GPU.
                    // this tells the renderer that we have multiple batches of U16_MAX!
                    this_vertex_i = 0;
                    // indices = (indices + 3) & ~3; // Align to next multiple of 4 because of how shader.c works
                }
                
                for (int v = 0; v < 4; v++) {
                    chunk_vertices[vertex_i] = f.vertices[v];
                    chunk_tex_coords[vertex_i] = f.tex_coords[v];
                    vertex_i++;

                    index_nums[v] = this_vertex_i;
                    this_vertex_i++;
                }

                for (int index = 0; index < 6; index += 2) {
                    u16 findices[6];
                    fill_face_indices(findices, f.indices_type);
                    chunk_indices[indices] = ((u32)index_nums[findices[index]]) |
                                             ((u32)index_nums[findices[index + 1]] << 16);
                    indices++;
                }
            }
        }

        if (ftype < FACE_TYPE_AMOUNT - 1) {
            offset_vertices[ftype] = vertex_i;
            indices = (indices + 3) & ~3; // Align to next multiple of 4 because of how shader.c works
            offset_indices[ftype] = indices;
        }
    }

    // Free dynamic memory
    for (int i = 0; i < CHUNK_AMOUNT * FACE_TYPE_AMOUNT; i++) {
        xfree(types_faces[i]);
    }
    xfree(types_faces);
    xfree(types_sizes);
    xfree(types_capacities);
}
