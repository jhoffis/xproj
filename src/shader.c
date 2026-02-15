#include "shader.h"
#include "allocator.h"
// #include "hal/debug.h"
#include <stdlib.h>
#include <string.h>

#include <xmmintrin.h> // For SSE (128-bit SIMD)
#include <strings.h> // for ffs()
#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))
typedef float f32_may_alias __attribute__((__may_alias__));

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

static inline u32 sse_lead_words_to_align(const u32 *dst)
{
    return ((u32)((16 - ((uintptr_t)dst & 15)) & 15)) >> 2;
}

static inline void copy_40_u32_store_ps_aligned(u32 *dst, const u32 *src)
{
    const f32_may_alias *s = (const f32_may_alias *)src;
    f32_may_alias *d = (f32_may_alias *)dst;
    __m128 v0 = _mm_loadu_ps((const float *)&s[0]);
    __m128 v1 = _mm_loadu_ps((const float *)&s[4]);
    __m128 v2 = _mm_loadu_ps((const float *)&s[8]);
    __m128 v3 = _mm_loadu_ps((const float *)&s[12]);
    __m128 v4 = _mm_loadu_ps((const float *)&s[16]);
    __m128 v5 = _mm_loadu_ps((const float *)&s[20]);
    __m128 v6 = _mm_loadu_ps((const float *)&s[24]);
    __m128 v7 = _mm_loadu_ps((const float *)&s[28]);
    __m128 v8 = _mm_loadu_ps((const float *)&s[32]);
    __m128 v9 = _mm_loadu_ps((const float *)&s[36]);
    _mm_store_ps((float *)&d[0], v0);
    _mm_store_ps((float *)&d[4], v1);
    _mm_store_ps((float *)&d[8], v2);
    _mm_store_ps((float *)&d[12], v3);
    _mm_store_ps((float *)&d[16], v4);
    _mm_store_ps((float *)&d[20], v5);
    _mm_store_ps((float *)&d[24], v6);
    _mm_store_ps((float *)&d[28], v7);
    _mm_store_ps((float *)&d[32], v8);
    _mm_store_ps((float *)&d[36], v9);
}

static inline void copy_36_u32_store_ps_aligned(u32 *dst, const u32 *src)
{
    const f32_may_alias *s = (const f32_may_alias *)src;
    f32_may_alias *d = (f32_may_alias *)dst;
    __m128 v0 = _mm_loadu_ps((const float *)&s[0]);
    __m128 v1 = _mm_loadu_ps((const float *)&s[4]);
    __m128 v2 = _mm_loadu_ps((const float *)&s[8]);
    __m128 v3 = _mm_loadu_ps((const float *)&s[12]);
    __m128 v4 = _mm_loadu_ps((const float *)&s[16]);
    __m128 v5 = _mm_loadu_ps((const float *)&s[20]);
    __m128 v6 = _mm_loadu_ps((const float *)&s[24]);
    __m128 v7 = _mm_loadu_ps((const float *)&s[28]);
    __m128 v8 = _mm_loadu_ps((const float *)&s[32]);
    _mm_store_ps((float *)&d[0], v0);
    _mm_store_ps((float *)&d[4], v1);
    _mm_store_ps((float *)&d[8], v2);
    _mm_store_ps((float *)&d[12], v3);
    _mm_store_ps((float *)&d[16], v4);
    _mm_store_ps((float *)&d[20], v5);
    _mm_store_ps((float *)&d[24], v6);
    _mm_store_ps((float *)&d[28], v7);
    _mm_store_ps((float *)&d[32], v8);
}

static inline void copy_u32_sse_unrolled_120(u32 *dst, const u32 *src)
{
    const u32 lead = sse_lead_words_to_align(dst);
    switch (lead) {
        case 0:
            __builtin_prefetch(&src[40], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 0, src + 0);
            __builtin_prefetch(&src[80], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 40, src + 40);
            __builtin_prefetch(&src[120], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 80, src + 80);
            break;
        case 1:
            dst[0] = src[0];
            __builtin_prefetch(&src[41], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 1, src + 1);
            __builtin_prefetch(&src[81], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 41, src + 41);
            copy_36_u32_store_ps_aligned(dst + 81, src + 81);
            dst[117] = src[117];
            dst[118] = src[118];
            dst[119] = src[119];
            break;
        case 2:
            dst[0] = src[0];
            dst[1] = src[1];
            __builtin_prefetch(&src[42], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 2, src + 2);
            __builtin_prefetch(&src[82], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 42, src + 42);
            copy_36_u32_store_ps_aligned(dst + 82, src + 82);
            dst[118] = src[118];
            dst[119] = src[119];
            break;
        default:
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            __builtin_prefetch(&src[43], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 3, src + 3);
            __builtin_prefetch(&src[83], 0, 1);
            copy_40_u32_store_ps_aligned(dst + 43, src + 43);
            copy_36_u32_store_ps_aligned(dst + 83, src + 83);
            dst[119] = src[119];
            break;
    }
    _mm_sfence();
}

static inline void copy_u32_sse(u32 *dst, const u32 *src, u32 count_words)
{
    u32 j = 0;
    u32 lead = sse_lead_words_to_align(dst);
    if (lead > count_words) lead = count_words;

    for (; j < lead; ++j) {
        dst[j] = src[j];
    }

    {
        const f32_may_alias *s = (const f32_may_alias *)(src + j);
        f32_may_alias *d = (f32_may_alias *)(dst + j);
        u32 vec_words = ((count_words - j) >> 2) << 2;
        u32 k = 0;
        for (; k + 40 <= vec_words; k += 40) {
            __m128 v0 = _mm_loadu_ps((const float *)&s[k + 0]);
            __m128 v1 = _mm_loadu_ps((const float *)&s[k + 4]);
            __m128 v2 = _mm_loadu_ps((const float *)&s[k + 8]);
            __m128 v3 = _mm_loadu_ps((const float *)&s[k + 12]);
            __m128 v4 = _mm_loadu_ps((const float *)&s[k + 16]);
            __m128 v5 = _mm_loadu_ps((const float *)&s[k + 20]);
            __m128 v6 = _mm_loadu_ps((const float *)&s[k + 24]);
            __m128 v7 = _mm_loadu_ps((const float *)&s[k + 28]);
            __m128 v8 = _mm_loadu_ps((const float *)&s[k + 32]);
            __m128 v9 = _mm_loadu_ps((const float *)&s[k + 36]);
            _mm_store_ps((float *)&d[k + 0], v0);
            _mm_store_ps((float *)&d[k + 4], v1);
            _mm_store_ps((float *)&d[k + 8], v2);
            _mm_store_ps((float *)&d[k + 12], v3);
            _mm_store_ps((float *)&d[k + 16], v4);
            _mm_store_ps((float *)&d[k + 20], v5);
            _mm_store_ps((float *)&d[k + 24], v6);
            _mm_store_ps((float *)&d[k + 28], v7);
            _mm_store_ps((float *)&d[k + 32], v8);
            _mm_store_ps((float *)&d[k + 36], v9);
        }
        for (; k < vec_words; k += 4) {
            __m128 v = _mm_loadu_ps((const float *)&s[k]);
            _mm_store_ps((float *)&d[k], v);
        }
        j += vec_words;
    }

    for (; j < count_words; ++j) {
        dst[j] = src[j];
    }
    _mm_sfence();
}


static u32 draw_indexed_full(u32 num_cube_indices, const u32 *cube_indices) {
#define MAX_BATCH 120
    const u32 full_words = (num_cube_indices / MAX_BATCH) * MAX_BATCH;
    u32 base_offset = 0;
    u32 *p;

    while (base_offset < full_words) {
        p = pb_begin();
        p = pb_push1(p, NV097_SET_BEGIN_END, g_render_method);
        pb_push(p++, 0x40000000 | NV20_TCL_PRIMITIVE_3D_INDEX_DATA, MAX_BATCH);

        copy_u32_sse_unrolled_120(p, &cube_indices[base_offset]);

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

    u32 *p;

    // draw full blocks, then advance the pointer for the remainder
    const u32 consumed = draw_indexed_full(num_cube_indices, cube_indices);
    cube_indices += consumed;
    num_cube_indices -= consumed;

    for (u32 i = 0; i < num_cube_indices; i += MAX_BATCH) {
        u32 num_this_batch = MIN(MAX_BATCH, num_cube_indices - i);
        p = pb_begin();
        p = pb_push1(p, NV097_SET_BEGIN_END, g_render_method);
        pb_push(p++, 0x40000000 | NV20_TCL_PRIMITIVE_3D_INDEX_DATA, num_this_batch);

        copy_u32_sse(p, &cube_indices[i], num_this_batch);

        p += num_this_batch;
        p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
        pb_end(p);
    }
    
    // debugPrint("spot5\n");
}
