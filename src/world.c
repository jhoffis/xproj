#include "world.h"
#include "random.h"

/*
 * Single zeroed pool of chunks.
 * Greedy meshing.
 * Calculate and store faces of the chunk.
 * Store those meshes further away on disk.
 * Load the faces you need to the gpu.
 */

chunk_data test_chunk = {0};

void generate_chunk(i32 x, i32 y) {
    for (int X = 0; X < 16; X++) {
        for (int Z = 0; Z < 16; Z++) {
            u64 y_ran = lehmer32_seeded(1032487 + 100*X*Z);
            y_ran = 0; // y_ran % 1; 
            y_ran += 1;
            for (int Y = 0; Y < 16; Y++) {
                if (Y < y_ran) {
                    test_chunk.cubes[X][Y][Z].type = GRASSTYPE;
                } else {
                    test_chunk.cubes[X][Y][Z].type = AIRTYPE;
                }
            }
        }
    }
}
