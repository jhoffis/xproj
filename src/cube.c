#include "cube.h"
#include "world.h"
#include <stdlib.h>

static image_data *block_textures;

void init_cubes(void) {
    block_textures = calloc(BLOCK_TYPE_MASK, sizeof(image_data));
    block_textures[0] = load_image("grass");
    block_textures[1] = load_image("grass_side");
    block_textures[2] = load_image("dirt");
}

void destroy_cubes(void) {
    free(block_textures);
}

image_data *get_cube_texture(u32 type, u8 direction) {
    if (type == BLOCK_TYPE_GRASS) {
        if (direction == FACE_DIRECTION_UP) return &block_textures[type];
        if (direction == FACE_DIRECTION_DOWN) return get_cube_texture(BLOCK_TYPE_DIRT, direction);
        return &block_textures[type + 1]; // grass_side
    }
    // otherwise jump:
    type += 1; // grass_side
    return &block_textures[type];
}
