#include "shader.h"
#include "allocator.h"
// #include "hal/debug.h"
#include <stdlib.h>
#include <string.h>

#include <xmmintrin.h> // For SSE (128-bit SIMD)
#include <strings.h> // for ffs()
#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

u8 g_render_method = TRIANGLES;

typedef void (*fragment_shader_setup_fn)(u32 **pp);

/* VS blob arrays and PS setup functions are auto-generated at build time */
#include "shader_data_gen.inc"

typedef struct shader_variant_data {
    const u32 *vs_program_src;
    size_t shader_size;
    fragment_shader_setup_fn setup_fragment_shader;
} shader_variant_data;

#define SHADER_VARIANT_DATA_ROW(enum_name, name) \
    [enum_name] = { \
        .vs_program_src = g_vs_program_##name, \
        .shader_size = sizeof(g_vs_program_##name), \
        .setup_fragment_shader = setup_fragment_shader_##name, \
    },
static const shader_variant_data g_shader_variant_data[SHADER_COUNT] = {
    SHADER_VARIANT_TABLE(SHADER_VARIANT_DATA_ROW)
};
#undef SHADER_VARIANT_DATA_ROW

void init_shader(shader_variant which) {
    u32 *p;

    u32 *vs_program = NULL;
    shader_variant_data variant_data;

    if ((u32)which >= SHADER_COUNT) {
        return;
    }

    variant_data = g_shader_variant_data[which];
    if (!variant_data.vs_program_src || variant_data.shader_size == 0 || !variant_data.setup_fragment_shader) {
        return;
    }

    vs_program = (u32 *)xmalloc(variant_data.shader_size);
    if (!vs_program) {
        return;
    }
    memcpy(vs_program, variant_data.vs_program_src, variant_data.shader_size);

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
    for (u32 i = 0; i < variant_data.shader_size / 16; i++) {
        p = pb_begin();
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &vs_program[i*4], 4*4);
        p+=4;
        pb_end(p);
    }

    /* Setup fragment shader */
    p = pb_begin();
    variant_data.setup_fragment_shader(&p);
    pb_end(p);

    if (vs_program) {
        xfree(vs_program);
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
void draw_arrays(u32 mode, s32 start, s32 count)
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

static inline void* align_and_zero(void* ptr) {
    char* p = (char*)ptr;
    
    if (((uintptr_t)p) % 16 == 4) {
        return p;  // Already aligned correctly
    }

    // For very small gaps (1-3 bytes), individual stores are fastest
    if (((uintptr_t)p) % 16 >= 13 || ((uintptr_t)p) % 16 <= 3) {
        do {
            *p++ = 0;
        } while (((uintptr_t)p) % 16 != 4);
        return p;
    }

    // For larger gaps, use SSE
    __m128 zero = _mm_setzero_ps();
    
    // Store 16 bytes of zeros
    _mm_storeu_ps((float*)p, zero);
    
    // Advance to next aligned position
    p += (16 - ((uintptr_t)p % 16 - 4) % 16);
    return p;
}


static u32 draw_indexed_full(u32 num_cube_indices, const u32 *cube_indices) {
#define MAX_BATCH 120
    const u32 full_words = (num_cube_indices / MAX_BATCH) * MAX_BATCH;
    u32 base_offset = 0;
    __m128 indices1, indices2, indices3, indices4, indices5;
    __m128 indices6, indices7, indices8, indices9, indices10;
    u32 *p;

    while (base_offset < full_words) {
        pb_align();
        p = pb_begin();
        p = pb_push1(p, NV097_SET_BEGIN_END, g_render_method);
        pb_push(p++, 0x40000000 | NV20_TCL_PRIMITIVE_3D_INDEX_DATA, MAX_BATCH);

        __builtin_prefetch(&cube_indices[base_offset + 40], 0, 1);

        indices1  = *((__m128 *)&cube_indices[base_offset + 0]);
        indices2  = *((__m128 *)&cube_indices[base_offset + 4]);
        indices3  = *((__m128 *)&cube_indices[base_offset + 8]);
        indices4  = *((__m128 *)&cube_indices[base_offset + 12]);
        indices5  = *((__m128 *)&cube_indices[base_offset + 16]);
        indices6  = *((__m128 *)&cube_indices[base_offset + 20]);
        indices7  = *((__m128 *)&cube_indices[base_offset + 24]);
        indices8  = *((__m128 *)&cube_indices[base_offset + 28]);
        indices9  = *((__m128 *)&cube_indices[base_offset + 32]);
        indices10 = *((__m128 *)&cube_indices[base_offset + 36]);

        _mm_store_ps((float *)&p[0],  indices1);
        _mm_store_ps((float *)&p[4],  indices2);
        _mm_store_ps((float *)&p[8],  indices3);
        _mm_store_ps((float *)&p[12], indices4);
        _mm_store_ps((float *)&p[16], indices5);
        _mm_store_ps((float *)&p[20], indices6);
        _mm_store_ps((float *)&p[24], indices7);
        _mm_store_ps((float *)&p[28], indices8);
        _mm_store_ps((float *)&p[32], indices9);
        _mm_store_ps((float *)&p[36], indices10);

        __builtin_prefetch(&cube_indices[base_offset + 80], 0, 1);

        indices1  = *((__m128 *)&cube_indices[base_offset + 40]);
        indices2  = *((__m128 *)&cube_indices[base_offset + 44]);
        indices3  = *((__m128 *)&cube_indices[base_offset + 48]);
        indices4  = *((__m128 *)&cube_indices[base_offset + 52]);
        indices5  = *((__m128 *)&cube_indices[base_offset + 56]);
        indices6  = *((__m128 *)&cube_indices[base_offset + 60]);
        indices7  = *((__m128 *)&cube_indices[base_offset + 64]);
        indices8  = *((__m128 *)&cube_indices[base_offset + 68]);
        indices9  = *((__m128 *)&cube_indices[base_offset + 72]);
        indices10 = *((__m128 *)&cube_indices[base_offset + 76]);

        _mm_store_ps((float *)&p[40], indices1);
        _mm_store_ps((float *)&p[44], indices2);
        _mm_store_ps((float *)&p[48], indices3);
        _mm_store_ps((float *)&p[52], indices4);
        _mm_store_ps((float *)&p[56], indices5);
        _mm_store_ps((float *)&p[60], indices6);
        _mm_store_ps((float *)&p[64], indices7);
        _mm_store_ps((float *)&p[68], indices8);
        _mm_store_ps((float *)&p[72], indices9);
        _mm_store_ps((float *)&p[76], indices10);

        __builtin_prefetch(&cube_indices[base_offset + 120], 0, 1);

        indices1  = *((__m128 *)&cube_indices[base_offset + 80]);
        indices2  = *((__m128 *)&cube_indices[base_offset + 84]);
        indices3  = *((__m128 *)&cube_indices[base_offset + 88]);
        indices4  = *((__m128 *)&cube_indices[base_offset + 92]);
        indices5  = *((__m128 *)&cube_indices[base_offset + 96]);
        indices6  = *((__m128 *)&cube_indices[base_offset + 100]);
        indices7  = *((__m128 *)&cube_indices[base_offset + 104]);
        indices8  = *((__m128 *)&cube_indices[base_offset + 108]);
        indices9  = *((__m128 *)&cube_indices[base_offset + 112]);
        indices10 = *((__m128 *)&cube_indices[base_offset + 116]);

        _mm_store_ps((float *)&p[80],  indices1);
        _mm_store_ps((float *)&p[84],  indices2);
        _mm_store_ps((float *)&p[88],  indices3);
        _mm_store_ps((float *)&p[92],  indices4);
        _mm_store_ps((float *)&p[96],  indices5);
        _mm_store_ps((float *)&p[100], indices6);
        _mm_store_ps((float *)&p[104], indices7);
        _mm_store_ps((float *)&p[108], indices8);
        _mm_store_ps((float *)&p[112], indices9);
        _mm_store_ps((float *)&p[116], indices10);

        _mm_sfence();

        p += MAX_BATCH;
        p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
        pb_end(p);

        base_offset += MAX_BATCH;
    }

    return base_offset; // <-- words consumed, NOT "remaining"
#undef MAX_BATCH
}

void draw_indexed(u32 num_cube_indices, u32 *cube_indices) {
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_BATCH 120

    __m128 indices1, indices2, indices3, indices4, indices5;
    __m128 indices6, indices7, indices8, indices9, indices10;
    u32 *p;

    // draw full blocks, then advance the pointer for the remainder
    const u32 consumed = draw_indexed_full(num_cube_indices, cube_indices);
    cube_indices += consumed;
    num_cube_indices -= consumed;

    for (u32 i = 0; i < num_cube_indices; i += MAX_BATCH) {
        pb_align();
        u32 num_this_batch = MIN(MAX_BATCH, num_cube_indices - i);
        p = pb_begin();
        p = pb_push1(p, NV097_SET_BEGIN_END, g_render_method);
        pb_push(p++, 0x40000000 | NV20_TCL_PRIMITIVE_3D_INDEX_DATA, num_this_batch);

        u32 base_offset = i;

        // Prefetch the first chunk
        __builtin_prefetch(&cube_indices[base_offset + 20], 0, 1);

        u32 j = 0;
        for (; j + 40 <= num_this_batch; j += 40) {
            __builtin_prefetch(&cube_indices[base_offset + j + 40], 0, 1);

            indices1  = *((__m128 *)&cube_indices[base_offset + j]);
            indices2  = *((__m128 *)&cube_indices[base_offset + j + 4]);
            indices3  = *((__m128 *)&cube_indices[base_offset + j + 8]);
            indices4  = *((__m128 *)&cube_indices[base_offset + j + 12]);
            indices5  = *((__m128 *)&cube_indices[base_offset + j + 16]);
            indices6  = *((__m128 *)&cube_indices[base_offset + j + 20]);
            indices7  = *((__m128 *)&cube_indices[base_offset + j + 24]);
            indices8  = *((__m128 *)&cube_indices[base_offset + j + 28]);
            indices9  = *((__m128 *)&cube_indices[base_offset + j + 32]);
            indices10 = *((__m128 *)&cube_indices[base_offset + j + 36]);
            // indices1 = _mm_loadu_ps((float *)&cube_indices[base_offset + j]);
            // indices2 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 4]);
            // indices3 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 8]);
            // indices4 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 12]);
            // indices5 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 16]);
            // indices6 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 20]);
            // indices7 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 24]);
            // indices8 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 28]);
            // indices9 = _mm_loadu_ps((float *)&cube_indices[base_offset + j + 32]);
            // indices10 = _mm_loadu_ps((float *)&cube_indices[base_offset + j+ 36]);

            _mm_store_ps((float *)&p[j],      indices1);
            _mm_store_ps((float *)&p[j + 4],  indices2);
            _mm_store_ps((float *)&p[j + 8],  indices3);
            _mm_store_ps((float *)&p[j + 12], indices4);
            _mm_store_ps((float *)&p[j + 16], indices5);
            _mm_store_ps((float *)&p[j + 20], indices6);
            _mm_store_ps((float *)&p[j + 24], indices7);
            _mm_store_ps((float *)&p[j + 28], indices8);
            _mm_store_ps((float *)&p[j + 32], indices9);
            _mm_store_ps((float *)&p[j + 36], indices10);
        }

        for (; j + 4 <= num_this_batch; j += 4) {
            indices1 = *((__m128 *)&cube_indices[base_offset + j]);
            _mm_store_ps((float *)&p[j], indices1);
        }

        // Handle remaining indices (not divisible by 16)
        for (; j < num_this_batch; ++j) {
            p[j] = cube_indices[base_offset + j];
        }

        _mm_sfence();

        p += num_this_batch;
        p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
        pb_end(p);
    }
    
    // debugPrint("spot5\n");
}
