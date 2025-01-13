#include "cube.h"
#include "world.h"
#include <stdlib.h>

static image_data *block_textures;

void init_cubes(void) {
    block_textures = calloc(FACE_TYPE_AMOUNT, sizeof(image_data));
    block_textures[FACE_TYPE_GRASS_TOP] = load_image("grass");
    block_textures[FACE_TYPE_GRASS_SIDE] = load_image("grass_side");
    block_textures[FACE_TYPE_DIRT] = load_image("dirt");
    block_textures[FACE_TYPE_COBBLESTONE] = load_image("cobblestone");
}

void destroy_cubes(void) {
    free(block_textures);
}

u32 convert_block_to_face_type(u32 type, u8 direction) {
    if (type == BLOCK_TYPE_GRASS) {
        if (direction == FACE_DIRECTION_UP) return FACE_TYPE_GRASS_TOP;
        if (direction == FACE_DIRECTION_DOWN) return FACE_TYPE_DIRT;
        return FACE_TYPE_GRASS_SIDE;
    }
    type += 1; // grass_side
    return type;
}

image_data *get_cube_texture(u32 type) {
    return &block_textures[type];
}
