#include "ui.h"

#include "pbkit/pbkit.h"
#include "shader.h"
#include "src/mvp.h"

#define UI_ATTR_POSITION 0
#define UI_ATTR_TEXCOORD 9

static u32 ui_indices[3];
static f32 uv[8];
static f32_v2 pos[4];

static inline u32 pack_u16(u16 a, u16 b) { return ((u32)a << 16) | (u32)b; }

static inline u8 ui_fast_log2(u32 v) {
    u8 r = 0;
    while (v > 1) { v >>= 1; r++; }
    return r;
}

// Bind stage0 texture exactly like terrain.h does.
// This keeps your “how we deal with textures” consistent.
static void ui_bind_texture0(const image_data *img) {
    u8 u = ui_fast_log2((u32)img->w);
    u8 v = ui_fast_log2((u32)img->h);

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
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0), control_enable);
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
    ui_indices[0] = pack_u16((u16)(0), (u16)(1));
    ui_indices[1] = pack_u16((u16)(2), (u16)(2));
    ui_indices[2] = pack_u16((u16)(3), (u16)(0));
    uv[0] = 0.; uv[1] = 0.;
    uv[2] = 1.; uv[3] = 0.;
    uv[4] = 1.; uv[5] = 1.;
    uv[6] = 0.; uv[7] = 1.;
}

static inline float ui_px_to_ndc_x(float x_px, float fb_w) {
    return (x_px / fb_w) * 2.0f - 1.0f;
}
static inline float ui_px_to_ndc_y(float y_px, float fb_h) {
    // y-down pixels → y-up clip
    return 1.0f - (y_px / fb_h) * 2.0f;
}

void ui_sprite(const image_data *img, f32 x, f32 y, f32 w, f32 h) {
    float x0 = ui_px_to_ndc_x(x, screen_width);
    float x1 = ui_px_to_ndc_x(x + w, screen_width);
    float y0 = ui_px_to_ndc_y(y, screen_height);
    float y1 = ui_px_to_ndc_y(y + h, screen_height);
    // TL, TR, BR, BL
    // pos[0].x = 0+1;   pos[0].y = 0+1;
    // pos[1].x = 0;     pos[1].y = 0+1;  
    // pos[2].x = 0;     pos[2].y = 0;
    // pos[3].x = 0+1;   pos[3].y = 0;
    pos[0].x = x0;   pos[0].y = y0;
    pos[1].x = x1;   pos[1].y = y0;  
    pos[2].x = x1;   pos[2].y = y1;
    pos[3].x = x0;   pos[3].y = y1;

    init_shader(SHADER_UI);
    
    u32 *p;
    p = pb_begin();
    p = pb_push1(p, NV097_SET_CULL_FACE_ENABLE, 0);
    p = pb_push1(p, NV097_SET_DEPTH_TEST_ENABLE, 0);
    p = pb_push1(p, NV097_SET_STENCIL_TEST_ENABLE, 0);
    p = pb_push1(p, NV097_SET_COLOR_MASK,
            NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE |
            NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE |
            NV097_SET_COLOR_MASK_RED_WRITE_ENABLE |
            NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE);
    pb_end(p);
    p = pb_begin();
    p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

    f32 constants_0[1] = {1, };
    pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 1);
    memcpy(p, constants_0, 1*sizeof(f32)); p+=1;

    // pb_end(p);
    // p = pb_begin();
    pb_push(p++,NV097_SET_VERTEX_DATA_ARRAY_FORMAT,3);
    for(size_t i = 0; i < 3; i++) {
        *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    }
    pb_end(p);


    // Bind the texture like terrain does
    ui_bind_texture0(img);

    // Bind streams: position + texcoord
    set_attrib_pointer(UI_ATTR_POSITION, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
                       2, sizeof(f32_v2), pos);

    set_attrib_pointer(UI_ATTR_TEXCOORD, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
                       2, sizeof(f32_v2), uv);

    draw_indexed(3, ui_indices);
}

