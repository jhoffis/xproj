#include "world.h"
#include "allocator.h"
#include "mvp.h"
#include <string.h>

/*
 * Single zeroed pool of chunks.
 * Greedy meshing.
 * Calculate and store faces of the chunk.
 * Store those meshes further away on disk.
 * Load the faces you need to the gpu.
 */

face_stored faces_pool[FACE_POOL_SIZE];
u32 num_faces_pooled = 0;

chunk_data loaded_chunks[CHUNK_AMOUNT];
u32 chunk_offsets[CHUNK_AMOUNT];
u32 num_chunks_pooled = 0;

f32_v3 *chunk_vertices;
f32_v2 *chunk_tex_coords;
u32    *chunk_indices;
u32 offset_vertices[FACE_TYPE_AMOUNT - 1];
u32 offset_indices[FACE_TYPE_AMOUNT - 1];
u32 num_faces_type[FACE_TYPE_AMOUNT];

face_batch face_batches[FACE_TYPE_AMOUNT][MAX_BATCHES_PER_TYPE];
u8 num_face_batches[FACE_TYPE_AMOUNT];


void init_world(void) {
    chunk_vertices   = xMmAllocateContiguousMemoryEx(FACE_POOL_SIZE * 4 * sizeof(f32_v3), 0, PAGE_READWRITE);
    chunk_tex_coords = xMmAllocateContiguousMemoryEx(FACE_POOL_SIZE * 4 * sizeof(f32_v2), 0, PAGE_READWRITE);
    // indices are written sequentially by CPU and read by GPU; keep 16-byte alignment.
    // We store 6x u16 per face, packed as 3x u32 words (2 indices per word).
    chunk_indices = x_aligned_malloc((FACE_POOL_SIZE * 6 * sizeof(u16)) + 64, 16);
}

void destroy_world(void) {
    xMmFreeContiguousMemory(chunk_vertices);
    xMmFreeContiguousMemory(chunk_tex_coords);
    x_aligned_free(chunk_indices);
}

// --- Greedy meshing (classic 2D mask per slice) ---------------------------------

static inline u8 cube_type_at(const chunk_data *chunk, int x, int y, int z) {
    return (u8)(chunk->cubes[x][y][z].type & BLOCK_TYPE_MASK);
}

static inline int is_air_type(u8 t) {
    return t == (u8)BLOCK_TYPE_AIR;
}

// NOTE: the greedy mask uses 0 as "empty". To allow FACE_TYPE values that are 0
// (e.g. grass top), we store (face_type + 1) in the mask and subtract here.
static inline void push_face(u8 mask_face_type, u8 dir, u8 a0, u8 b0, u8 a1, u8 b1, u8 c, u32 *faces_found) {
    if (num_faces_pooled >= FACE_POOL_SIZE) return;
    const u8 face_type = (u8)(mask_face_type - 1u);
    face_stored fs;
    fs.a0 = a0;
    fs.a1 = a1;
    fs.b0 = b0;
    fs.b1 = b1;
    fs.c = c;
    fs.dir = dir;
    fs.type = face_type;
    fs._pad = 0;
    faces_pool[num_faces_pooled++] = fs;
    (*faces_found)++;
}

// Greedy rectangle extraction on a WxH mask (u8 face_type, 0 = empty)
static void greedy_from_mask(
    u8 *mask,
    int width,
    int height,
    u8 dir,
    u8 c,
    u32 *faces_found)
{
    for (int b = 0; b < height; b++) {
        int row = b * width;
        for (int a = 0; a < width; ) {
            const u8 t = mask[row + a];
            if (!t) { a++; continue; }

            int rect_w = 1;
            while ((a + rect_w) < width && mask[row + a + rect_w] == t) rect_w++;

            int rect_h = 1;
            for (; (b + rect_h) < height; rect_h++) {
                const int row2 = (b + rect_h) * width;
                int k = 0;
                for (; k < rect_w; k++) {
                    if (mask[row2 + a + k] != t) {
                        break;
                    }
                }
                if (k != rect_w) {
                    break;
                }
            }

            // emit and clear
            push_face(t, dir, (u8)a, (u8)b, (u8)(a + rect_w - 1), (u8)(b + rect_h - 1), c, faces_found);

            for (int y = 0; y < rect_h; y++) {
                const int r = (b + y) * width;
                for (int x = 0; x < rect_w; x++) {
                    mask[r + a + x] = 0;
                }
            }

            a += rect_w;
        }
    }
}

static void mesh_chunk_greedy(chunk_data *chunk, u32 *out_faces_found) {
    // IMPORTANT: this only checks within-chunk neighbors.
    // TODO: when chunks are adjacent, suppress faces on borders by checking neighbor chunks.

    u32 faces_found = 0;
    // Max 2D mask used by greedy: max(CHUNK_SIZE*CHUNK_HEIGHT, CHUNK_HEIGHT*CHUNK_SIZE)
    u8 mask[CHUNK_SIZE * CHUNK_HEIGHT];

    // Y planes: UP/DOWN (A=x, B=z, C=y)
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        // UP
        memset(mask, 0, CHUNK_SIZE * CHUNK_SIZE);
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (y + 1 == CHUNK_HEIGHT) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x, y + 1, z);
                if (is_air_type(nb)) {
                    mask[z * CHUNK_SIZE + x] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_UP) + 1u;
                }
            }
        }
        // greedy expects A and B in [0..15]. Here A=x, B=z.
        // Our push_face stores A=a, B=b, C=c. For Y planes that is correct.
        greedy_from_mask(mask, CHUNK_SIZE, CHUNK_SIZE, (u8)FACE_DIRECTION_UP, (u8)y, &faces_found);

        // DOWN
        memset(mask, 0, CHUNK_SIZE * CHUNK_SIZE);
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (y == 0) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x, y - 1, z);
                if (is_air_type(nb)) {
                    mask[z * CHUNK_SIZE + x] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_DOWN) + 1u;
                }
            }
        }
        greedy_from_mask(mask, CHUNK_SIZE, CHUNK_SIZE, (u8)FACE_DIRECTION_DOWN, (u8)y, &faces_found);
    }

    // Z planes: NORTH/SOUTH (A=x, B=y, C=z)
    for (int z = 0; z < CHUNK_SIZE; z++) {
        // NORTH (+z)
        memset(mask, 0, CHUNK_SIZE * CHUNK_HEIGHT);
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (z + 1 == CHUNK_SIZE) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x, y, z + 1);
                if (is_air_type(nb)) {
                    mask[y * CHUNK_SIZE + x] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_NORTH) + 1u;
                }
            }
        }
        greedy_from_mask(mask, CHUNK_SIZE, CHUNK_HEIGHT, (u8)FACE_DIRECTION_NORTH, (u8)z, &faces_found);

        // SOUTH (-z)
        memset(mask, 0, CHUNK_SIZE * CHUNK_HEIGHT);
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (z == 0) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x, y, z - 1);
                if (is_air_type(nb)) {
                    mask[y * CHUNK_SIZE + x] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_SOUTH) + 1u;
                }
            }
        }
        greedy_from_mask(mask, CHUNK_SIZE, CHUNK_HEIGHT, (u8)FACE_DIRECTION_SOUTH, (u8)z, &faces_found);
    }

    // X planes: EAST/WEST (A=y, B=z, C=x)
    for (int x = 0; x < CHUNK_SIZE; x++) {
        // EAST (+x)
        memset(mask, 0, CHUNK_HEIGHT * CHUNK_SIZE);
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (x + 1 == CHUNK_SIZE) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x + 1, y, z);
                if (is_air_type(nb)) {
                    // mask is indexed as B=z rows, A=y columns
                    mask[z * CHUNK_HEIGHT + y] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_EAST) + 1u;
                }
            }
        }
        // greedy emits (A=a, B=b). For X planes we want A=y, B=z.
        greedy_from_mask(mask, CHUNK_HEIGHT, CHUNK_SIZE, (u8)FACE_DIRECTION_EAST, (u8)x, &faces_found);

        // WEST (-x)
        memset(mask, 0, CHUNK_HEIGHT * CHUNK_SIZE);
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                const u8 bt = cube_type_at(chunk, x, y, z);
                if (is_air_type(bt)) continue;
                const u8 nb = (x == 0) ? BLOCK_TYPE_AIR : cube_type_at(chunk, x - 1, y, z);
                if (is_air_type(nb)) {
                    mask[z * CHUNK_HEIGHT + y] = (u8)convert_block_to_face_type(bt, (u8)FACE_DIRECTION_WEST) + 1u;
                }
            }
        }
        greedy_from_mask(mask, CHUNK_HEIGHT, CHUNK_SIZE, (u8)FACE_DIRECTION_WEST, (u8)x, &faces_found);
    }

    *out_faces_found = faces_found;
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
static void generate_chunk(s32 chunk_x, s32 chunk_z) {
    if (num_chunks_pooled >= CHUNK_AMOUNT) return;
    chunk_data *chunk = &loaded_chunks[num_chunks_pooled];
    chunk->x = chunk_x;
    chunk->z = chunk_z;


    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y < 16) {
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

    u32 num_found;
    mesh_chunk_greedy(chunk, &num_found);

    chunk_offsets[num_chunks_pooled] = num_found;
    num_chunks_pooled++;
}

static inline u8 dir_winding(u8 direction) {
    // matches previous determine_face_indices_type()
    return (direction == FACE_DIRECTION_DOWN || direction == FACE_DIRECTION_SOUTH || direction == FACE_DIRECTION_EAST) ? 1u : 2u;
}

static inline void write_indices_packed(u32 *dst_u32, u16 base, u8 winding) {
    if (winding == 1u) {
        // 0,1,3, 1,2,3  => (0,1) (3,1) (2,3)
        dst_u32[0] = (u32)(base + 0) | ((u32)(base + 1) << 16);
        dst_u32[1] = (u32)(base + 3) | ((u32)(base + 1) << 16);
        dst_u32[2] = (u32)(base + 2) | ((u32)(base + 3) << 16);
    } else {
        // 0,3,1, 3,2,1  => (0,3) (1,3) (2,1)
        dst_u32[0] = (u32)(base + 0) | ((u32)(base + 3) << 16);
        dst_u32[1] = (u32)(base + 1) | ((u32)(base + 3) << 16);
        dst_u32[2] = (u32)(base + 2) | ((u32)(base + 1) << 16);
    }
}

static inline void write_face_to_buffers(
    u32 dst_v,
    u32 dst_i_u32,
    const f32 chunk_offset[2],
    face_stored fs,
    u16 base_vertex_in_batch)
{
    const u8 a0i = fs.a0;
    const u8 a1i = fs.a1;
    const u8 b0i = fs.b0;
    const u8 b1i = fs.b1;
    const u8 ci  = fs.c;
    const u8 dir = fs.dir;

    const f32 bs = (f32)BLOCK_SIZE;

    const f32 a0 = (f32)a0i * bs;
    const f32 a1 = (f32)(a1i + 1u) * bs;
    const f32 b0 = (f32)b0i * bs;
    const f32 b1 = (f32)(b1i + 1u) * bs;
    f32 c = (f32)ci * bs;

    const f32 tex_a = (f32)(a1i + 1u - a0i);
    const f32 tex_b = (f32)(b1i + 1u - b0i);

    // packed indices (3 u32 words per face)
    write_indices_packed(&chunk_indices[dst_i_u32], base_vertex_in_batch, dir_winding(dir));

    // vertices + uvs (4 each)
    if (dir <= FACE_DIRECTION_UP) {
        // A=x, B=z, C=y
        if (dir == FACE_DIRECTION_UP) c += bs;

        chunk_vertices[dst_v + 0] = (f32_v3){ chunk_offset[0] + a0, c, chunk_offset[1] + b1 };
        chunk_vertices[dst_v + 1] = (f32_v3){ chunk_offset[0] + a1, c, chunk_offset[1] + b1 };
        chunk_vertices[dst_v + 2] = (f32_v3){ chunk_offset[0] + a1, c, chunk_offset[1] + b0 };
        chunk_vertices[dst_v + 3] = (f32_v3){ chunk_offset[0] + a0, c, chunk_offset[1] + b0 };

        chunk_tex_coords[dst_v + 0] = (f32_v2){ 0.0f,  tex_b };
        chunk_tex_coords[dst_v + 1] = (f32_v2){ tex_a, tex_b };
        chunk_tex_coords[dst_v + 2] = (f32_v2){ tex_a, 0.0f };
        chunk_tex_coords[dst_v + 3] = (f32_v2){ 0.0f,  0.0f };
        return;
    }

    if (dir <= FACE_DIRECTION_NORTH) {
        // A=x, B=y, C=z
        if (dir == FACE_DIRECTION_NORTH) c += bs;

        chunk_vertices[dst_v + 0] = (f32_v3){ chunk_offset[0] + a1, b0, chunk_offset[1] + c };
        chunk_vertices[dst_v + 1] = (f32_v3){ chunk_offset[0] + a1, b1, chunk_offset[1] + c };
        chunk_vertices[dst_v + 2] = (f32_v3){ chunk_offset[0] + a0, b1, chunk_offset[1] + c };
        chunk_vertices[dst_v + 3] = (f32_v3){ chunk_offset[0] + a0, b0, chunk_offset[1] + c };

        chunk_tex_coords[dst_v + 0] = (f32_v2){ tex_a, tex_b };
        chunk_tex_coords[dst_v + 1] = (f32_v2){ tex_a, 0.0f };
        chunk_tex_coords[dst_v + 2] = (f32_v2){ 0.0f,  0.0f };
        chunk_tex_coords[dst_v + 3] = (f32_v2){ 0.0f,  tex_b };
        return;
    }

    // WEST/EAST: A=y, B=z, C=x
    if (dir == FACE_DIRECTION_EAST) c += bs;

    chunk_vertices[dst_v + 0] = (f32_v3){ chunk_offset[0] + c, a0, chunk_offset[1] + b1 };
    chunk_vertices[dst_v + 1] = (f32_v3){ chunk_offset[0] + c, a1, chunk_offset[1] + b1 };
    chunk_vertices[dst_v + 2] = (f32_v3){ chunk_offset[0] + c, a1, chunk_offset[1] + b0 };
    chunk_vertices[dst_v + 3] = (f32_v3){ chunk_offset[0] + c, a0, chunk_offset[1] + b0 };

    chunk_tex_coords[dst_v + 0] = (f32_v2){ 0.0f,  tex_a };
    chunk_tex_coords[dst_v + 1] = (f32_v2){ 0.0f,  0.0f };
    chunk_tex_coords[dst_v + 2] = (f32_v2){ tex_b, 0.0f };
    chunk_tex_coords[dst_v + 3] = (f32_v2){ tex_b, tex_a };
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
    int current_chunk_z = (int)(floorf(v_cam_loc.z / (CHUNK_SIZE * BLOCK_SIZE)));

    const int view_dist = CHUNK_VIEW_DISTANCE / 2;

    // Reset pools (no heap allocations per load)
    num_chunks_pooled = 0;
    num_faces_pooled = 0;
    memset(num_faces_type, 0, sizeof(num_faces_type));
    memset(offset_vertices, 0, sizeof(offset_vertices));
    memset(offset_indices, 0, sizeof(offset_indices));
    memset(num_face_batches, 0, sizeof(num_face_batches));

    // Load/generate chunks in the view distance
    for (int x = current_chunk_x - view_dist; x < current_chunk_x + view_dist; x++) {
        for (int z = current_chunk_z - view_dist; z < current_chunk_z + view_dist; z++) {
            generate_chunk(x, z);
        }
    }

    // --- Build render buffers (no per-chunk malloc/realloc) ----------------------

    // 1) Count faces per chunk per face-type
    u16 chunk_type_counts[CHUNK_AMOUNT * FACE_TYPE_AMOUNT];
    memset(chunk_type_counts, 0, sizeof(chunk_type_counts));

    u32 face_cursor = 0;
    for (u32 c = 0; c < num_chunks_pooled; c++) {
        const u32 n = chunk_offsets[c];
        for (u32 i = 0; i < n; i++) {
            const face_stored fs = faces_pool[face_cursor++];
            const u8 ftype = fs.type;
            chunk_type_counts[c * FACE_TYPE_AMOUNT + ftype]++;
            num_faces_type[ftype]++;
        }
    }

    // 2) Per-type chunk prefix (face index within that type stream)
    u32 chunk_type_base[CHUNK_AMOUNT * FACE_TYPE_AMOUNT];
    for (u32 t = 0; t < FACE_TYPE_AMOUNT; t++) {
        u32 prefix = 0;
        for (u32 c = 0; c < num_chunks_pooled; c++) {
            chunk_type_base[c * FACE_TYPE_AMOUNT + t] = prefix;
            prefix += (u32)chunk_type_counts[c * FACE_TYPE_AMOUNT + t];
        }
    }

    // 3) Global type starts + offsets (match previous layout: all faces of type0, then type1, ...)
    u32 type_vertex_start[FACE_TYPE_AMOUNT];
    u32 type_index_start[FACE_TYPE_AMOUNT];
    u32 v_cursor = 0;
    u32 i_cursor_u32 = 0;
    for (u32 t = 0; t < FACE_TYPE_AMOUNT; t++) {
        type_vertex_start[t] = v_cursor;
        type_index_start[t] = i_cursor_u32;

        // batches for this type
        const u32 faces = num_faces_type[t];
        const u32 batches = (faces + MAX_FACES_PER_BATCH - 1u) / MAX_FACES_PER_BATCH;
        num_face_batches[t] = (u8)batches;
        for (u32 b = 0; b < batches; b++) {
            const u32 faces_in_batch = (faces - b * MAX_FACES_PER_BATCH) > MAX_FACES_PER_BATCH
                ? MAX_FACES_PER_BATCH
                : (faces - b * MAX_FACES_PER_BATCH);
            face_batches[t][b].first_vertex = type_vertex_start[t] + b * MAX_VERTICES;
            face_batches[t][b].vertex_count = faces_in_batch * 4u;
            face_batches[t][b].first_index_u32 = type_index_start[t] + b * (MAX_FACES_PER_BATCH * 3u);
            face_batches[t][b].index_count_u32 = faces_in_batch * 3u;
        }

        v_cursor += faces * 4u;
        i_cursor_u32 += faces * 3u;

        if (t < (u32)FACE_TYPE_AMOUNT - 1u) {
            offset_vertices[t] = v_cursor;
            i_cursor_u32 = (i_cursor_u32 + 3u) & ~3u; // align to 4 u32 words
            offset_indices[t] = i_cursor_u32;
        }
    }

    // 4) Fill buffers (single linear pass over faces_pool)
    u16 chunk_type_written[CHUNK_AMOUNT * FACE_TYPE_AMOUNT];
    memset(chunk_type_written, 0, sizeof(chunk_type_written));

    const f32 BLOCK_CHUNK_SCALE = (f32)BLOCK_SIZE * (f32)CHUNK_SIZE;
    face_cursor = 0;
    for (u32 c = 0; c < num_chunks_pooled; c++) {
        const chunk_data *ch = &loaded_chunks[c];
        const f32 pos_offset[2] = {
            (f32)ch->x * BLOCK_CHUNK_SCALE,
            (f32)ch->z * BLOCK_CHUNK_SCALE,
        };

        const u32 n = chunk_offsets[c];
        for (u32 i = 0; i < n; i++) {
            const face_stored fs = faces_pool[face_cursor++];
            const u8 t = fs.type;

            const u32 base_in_type = chunk_type_base[c * FACE_TYPE_AMOUNT + t];
            const u32 local_in_chunk = (u32)chunk_type_written[c * FACE_TYPE_AMOUNT + t]++;
            const u32 face_index_in_type = base_in_type + local_in_chunk;

            const u32 dst_v = type_vertex_start[t] + face_index_in_type * 4u;
            const u32 dst_i = type_index_start[t] + face_index_in_type * 3u;
            const u16 base_vertex = (u16)((face_index_in_type % MAX_FACES_PER_BATCH) * 4u);

            write_face_to_buffers(dst_v, dst_i, pos_offset, fs, base_vertex);
        }
    }
}
