#include "shader.h"
#include "cube.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h> // for ffs()
#include <pbkit/pbkit.h>
#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))


void init_shader(i32 which) {
    u32 *p;

    u32 *vs_program = NULL;
    size_t shader_size;

    switch (which) {
        case 0: 
            u32 temp[] = {
                #include "vs.inl"
            };
            shader_size = sizeof(temp);
            vs_program = (u32 *)malloc(shader_size);
            if (vs_program) {
                memcpy(vs_program, temp, shader_size); // Copy shader data
            }
            break;
        case 1: 
            u32 temp2[] = {
                #include "vs2.inl"
            };
            shader_size = sizeof(temp2);
            vs_program = (u32 *)malloc(shader_size);
            if (vs_program) {
                memcpy(vs_program, temp2, shader_size); // Copy shader data
            }
            break;
        default:
            // debugPrint("Invalid shader selection!\n");
            break;
    }

    p = pb_begin();
    // Set run address of shader
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);

    // Set execution mode
    p = pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
                 MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
                 | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));

    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);

    pb_end(p);

    /* Set cursor and begin copying program */
    p = pb_begin();
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);

    pb_end(p);

    /* Copy program instructions (16-bytes each) */
    for (u32 i = 0; i < shader_size / 16; i++) {
        p = pb_begin();
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &vs_program[i*4], 4*4);
        p+=4;
        pb_end(p);
    }

    /* Setup fragment shader */
    p = pb_begin();
    switch (which) {
        case 0:
#include "ps.inl"
            break;
        case 1:
#include "ps2.inl"
            break;
    }
    pb_end(p);

    if (vs_program) {
        free(vs_program);
    }
}

/* Set an attribute pointer */
void set_attrib_pointer(u32 index, u32 format, u32 size, u32 stride, const void* data)
{
    uint32_t *p = pb_begin();
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + index*4,
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, format) | \
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, size) |  \
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, stride));
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + index*4, (uint32_t)data & 0x03ffffff);
    pb_end(p);
}

/* Send draw commands for the triangles */
void draw_arrays(u32 mode, i32 start, i32 count)
{
    uint32_t *p = pb_begin();
    p = pb_push1(p, NV097_SET_BEGIN_END, mode);

    p = pb_push1(p, 0x40000000|NV097_DRAW_ARRAYS, //bit 30 means all params go to same register 0x1810
                 MASK(NV097_DRAW_ARRAYS_COUNT, (count-1)) | MASK(NV097_DRAW_ARRAYS_START_INDEX, start));

    p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    pb_end(p);
}

u32 pack_u16_to_u32(u16 a, u16 b) {
    return ((u32) a << 16) | (u32) b;
}

void pack_u16_list(u32 * out, u16 *list, u32 size) {
    for (u32 i = 0; i < size; i += 2) {
        out[i / 2] = pack_u16_to_u32(list[i], list[i + 1]);
    }
}
/*
 *  First draw the demo and then try to get a model from the disk. 
 *  But draw a cube instead - just so it's easier to see what it's doing.
 */
void draw_indexed() {
    #define MIN(a,b) ((a)<(b)?(a):(b))
    #define MAX_BATCH 120

    u32 *p;
    u32 num_this_batch;
    // u32 num_indices = sizeof(
    // for (u32 i = 0; i < num_cube_indices / 2; i += num_this_batch) {
        num_this_batch = MIN(MAX_BATCH, num_cube_indices); // - i);

        //What are the indices?
        p = pb_begin();
        p = pb_push1(p, NV097_SET_BEGIN_END, TRIANGLES);
        pb_push(p++, 0x40000000|NV20_TCL_PRIMITIVE_3D_INDEX_DATA, num_this_batch);

        // send indices
        memcpy(p, &cube_indices[0], num_this_batch * sizeof(u32));
        p += num_this_batch;

        p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
        pb_end(p);
    // }
}
