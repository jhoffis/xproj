#include "ui.h"

#include "pbkit/pbkit.h"
#include "shader.h"
#include "src/allocator.h"
#include "src/mvp.h"

#define UI_ATTR_POSITION 0
#define UI_ATTR_TEXCOORD 9

static u32    *ui_indices;
static f32_v2 *ui_uv;
static f32_v2 *ui_pos;

static inline u32 pack_u16(u16 a, u16 b) { return (u32)a | ((u32)b << 16); }

// Bind stage0 texture exactly like terrain.h does.
// This keeps your “how we deal with textures” consistent.
static void ui_bind_texture0(const image_data *img) {
    u8 u = fast_log2((u32)img->w);
    u8 v = fast_log2((u32)img->h);

    int channels = 2;
    DWORD format =
        ((channels & NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) |
         (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR * NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE)) |
        (((2 << 4) & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY)) |
        ((NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8 << 8) & NV097_SET_TEXTURE_FORMAT_COLOR) |
        ((1 << 16) & NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS) |
        ((u << 20) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U) |
        ((v << 24) & NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);

    DWORD filter = 0x01014000; // nearest (same as your terrain path)
    DWORD control_enable = NV097_SET_TEXTURE_CONTROL0_ENABLE | NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP;

    u32 *p = pb_begin();
    p = pb_push2(p, NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0), img->addr26bits, format);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0), img->pitch << 16);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0), (img->w << 16) | img->h);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_WRAP(0), 0x00010101);
    // p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0), control_enable);
	p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0), 0x4003ffc4);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_FILTER(0), filter);
    pb_end(p);

    // Disable other stages (same as your terrain path)
    p = pb_begin();
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1), 0x0003ffc0);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2), 0x0003ffc0);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3), 0x0003ffc0);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),   0x00030303);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),   0x00030303);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),   0x00030303);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_FILTER(1), 0x02022000);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_FILTER(2), 0x02022000);
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_FILTER(3), 0x02022000);
    pb_end(p);
}

void init_ui(void) {
    ui_uv = xMmAllocateContiguousMemoryEx(8 * sizeof(f32_v2), 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    ui_pos = xMmAllocateContiguousMemoryEx(4 * sizeof(f32_v3), 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    ui_indices = x_aligned_malloc(6 * sizeof(u16), 16);

    ui_indices[0] = pack_u16(0, 1);
    ui_indices[1] = pack_u16(2, 2);
    ui_indices[2] = pack_u16(3, 0);

    ui_uv[0] = (f32_v2){1.f, 0.f};
    ui_uv[1] = (f32_v2){0.f, 0.f};
    ui_uv[2] = (f32_v2){0.f, 1.f};
    ui_uv[3] = (f32_v2){1.f, 1.f};

    ui_pos[0] = (f32_v2){.9f,  .5f};
    ui_pos[1] = (f32_v2){-.9f, .5f};
    ui_pos[2] = (f32_v2){-.9f, -.5f};
    ui_pos[3] = (f32_v2){.9f,  -.5f};
    // ui_pos[0] = (f32_v2){-.9f, -.9f};
    // ui_pos[1] = (f32_v2){.9f,  -.9f};
    // ui_pos[2] = (f32_v2){.9f,  .9f};
    // ui_pos[3] = (f32_v2){-.9f, .9f};
}

static inline float ui_px_to_ndc_x(float x_px, float fb_w) {
    return (x_px / fb_w) * 2.0f - 1.0f;
}
static inline float ui_px_to_ndc_y(float y_px, float fb_h) {
    // y-down pixels → y-up clip
    return 1.0f - (y_px / fb_h) * 2.0f;
}

void ui_sprite(const image_data *img,
               const f32 x,
               const f32 y,
               const f32 w,
               const f32 h,
               const f32 scale,
               const anchor_e anchor) {
    f32 uvx0;
    f32 uvy0;
    f32 uvx1;
    f32 uvy1;
    f32 x0;
    f32 x1;
    f32 y0;
    f32 y1;
    switch (anchor) {
        case anchor_tl:
            uvx0 = w;
            uvy0 = 0;
            uvx1 = 0;
            uvy1 = h;
            x0 = x;
            x1 = x+(w * (f32)img->w * scale);
            y0 = y;
            y1 = y+(h * (f32)img->h * scale);
            break;
        case anchor_bl:
            uvx0 = w;
            uvy0 = h;
            uvx1 = 0;
            uvy1 = 0;
            x0 = x;
            x1 = x+(w * (f32)img->w * scale);
            y0 = screen_height - y;
            y1 = y0 - (h * (f32)img->h * scale);
            break;
        case anchor_bm:
            uvx0 = w;
            uvy0 = h;
            uvx1 = 0;
            uvy1 = 0;
            f32 wa = (w * (f32)img->w * scale) / 2;
            f32 xa = (f32)screen_width / 2 + x;
            x0 = xa - wa;
            x1 = xa + wa;
            y0 = screen_height - y;
            y1 = y0 - (h * (f32)img->h * scale);
            break;
        }

    init_shader(SHADER_UI);
    
    u32 *p;
    p = pb_begin();
    p = pb_push1(p, NV097_SET_DEPTH_TEST_ENABLE, 0);
    p = pb_push1(p, NV097_SET_DEPTH_MASK, 0);
    p = pb_push1(p, NV097_SET_CULL_FACE_ENABLE, 0);
    p = pb_push1(p, NV097_SET_ALPHA_TEST_ENABLE, 1);
    p = pb_push1(p, NV097_SET_BLEND_ENABLE, 1);
    p = pb_push1(p, NV097_SET_COLOR_MASK, 0x01010101);
    pb_end(p);
    p = pb_begin();
    // p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 0);
    // f32 constants_0[4] = {0, 0, 0, 1};
    // pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
    // memcpy(p, constants_0, 4*sizeof(f32)); p+=4;

    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 16);
    for (int i = 0; i < 16; i++) {
        *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    }
    pb_end(p);

    ui_bind_texture0(img);

    p = pb_begin();
    pb_push1(p, NV20_TCL_PRIMITIVE_3D_BEGIN_END, QUADS); p += 2;

#define UI_TEX0_2F (NV097_SET_VERTEX_DATA2F_M + UI_ATTR_TEXCOORD * 2 * sizeof(f32))
    // Vertex 0
    pb_push(p++, UI_TEX0_2F, 2);
    *((f32 *)(p++)) = uvx1;
    *((f32 *)(p++)) = uvy0;
    pb_push(p++, NV097_SET_VERTEX4F, 4);
    *((f32 *)(p++)) = x0;
    *((f32 *)(p++)) = y0;
    *((f32 *)(p++)) = 0.0f;
    *((f32 *)(p++)) = 1.0f;

    // Vertex 1
    pb_push(p++, UI_TEX0_2F, 2);
    *((f32 *)(p++)) = uvx0;
    *((f32 *)(p++)) = uvy0;
    pb_push(p++, NV097_SET_VERTEX4F, 4);
    *((f32 *)(p++)) = x1;
    *((f32 *)(p++)) = y0;
    *((f32 *)(p++)) = 0.0f;
    *((f32 *)(p++)) = 1.0f;

    // Vertex 2
    pb_push(p++, UI_TEX0_2F, 2);
    *((f32 *)(p++)) = uvx0;
    *((f32 *)(p++)) = uvy1;
    pb_push(p++, NV097_SET_VERTEX4F, 4);
    *((f32 *)(p++)) = x1;
    *((f32 *)(p++)) = y1;
    *((f32 *)(p++)) = 0.0f;
    *((f32 *)(p++)) = 1.0f;

    // Vertex 3
    pb_push(p++, UI_TEX0_2F, 2);
    *((f32 *)(p++)) = uvx1;
    *((f32 *)(p++)) = uvy1;
    pb_push(p++, NV097_SET_VERTEX4F, 4);
    *((f32 *)(p++)) = x0;
    *((f32 *)(p++)) = y1;
    *((f32 *)(p++)) = 0.0f;
    *((f32 *)(p++)) = 1.0f;
#undef UI_TEX0_2F

    pb_push(p++, NV20_TCL_PRIMITIVE_3D_BEGIN_END, 1);
    *(p++) = STOP;

    pb_end(p);
}

