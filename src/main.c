#include <hal/debug.h>
#include <hal/video.h>
#include <SDL.h>
#include <pbkit/pbkit.h>
#include <windows.h>

#include "file_util.h"
#include "nums.h"
#include "shader.h"
#include "xboxkrnl/xboxkrnl.h"
#include "math3d.h"
#include "cube.h"

typedef struct {
    float pos[3];
    float color[3];
} __attribute__((packed)) ColoredVertex;

static const ColoredVertex verts[] = {
    //  X     Y     Z       R     G     B
    {{-1.0, -0.9,  1.0}, { 0.5,  1.0,  0.0}}, /* Foreground triangle */
    {{ 0.0,  1.0,  1.0}, { 0.0,  1.0,  0.0}},
    {{ 1.0, -1.0,  1.0}, { 0.0,  0.0,  1.0}},
};
static const ColoredVertex verts2[] = {
    //  X     Y     Z       R     G     B
    {{-1.0, -0.7,  1.0}, { 0.5,  1.0,  0.0}}, /* Foreground triangle */
    {{ 0.0,  0.5,  1.0}, { 0.0,  1.0,  0.0}},
    {{ 1.0, -1.0,  1.0}, { 0.0,  0.0,  1.0}},
};
static const ColoredVertex verts3[] = {
    //  X     Y     Z       R     G     B
    {{-0.8, -0.6,  1.0}, { 1.0,  1.0,  0.0}}, /* Foreground triangle */
    {{ 0.0,  0.5,  1.0}, { 0.0,  1.0,  0.0}},
    {{ 1.0, -1.2,  1.0}, { 0.0,  0.0,  1.0}},
};

MATRIX m_model, m_view, m_proj, m_mvp;
VECTOR v_obj_rot     = {  0,   0,   0,  1 };
VECTOR v_obj_scale   = {  1,   1,   1,  1 };
VECTOR v_obj_pos     = {  0,   0,   0,  1 };
VECTOR v_cam_loc     = {  0,   0, 165,  1 };
VECTOR v_cam_rot     = {  0,   0,   0,  1 };

static u32 *alloc_vertices;
static u32 *alloc_vertices2;
static u32 *alloc_vertices3;
static u32 *alloc_vertices_cube;
static u32  num_vertices;
static f32  m_viewport[4][4];

SDL_GameController *pad = NULL;
bool pbk_init = false, sdl_init = false;
u32 width = 1280, height = 720;

void wait_then_cleanup() {
    Sleep(5000);

    if (pbk_init) {
        pb_kill();
    }
    if (pad != NULL) {
        SDL_GameControllerClose(pad);
    }
    if (sdl_init) {
        SDL_Quit();
    }
}

static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max);

int main(void)
{
    SDL_Event e;

    i32       start, last, now;
    i32       fps, frames;
    start = now = last = GetTickCount();
    frames = fps = 0;
    int f3key = 0;

    if (!XVideoSetMode(width, height, 32, 60)) {
        XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
        debugPrint("720p is not enabled! Please enable it!");
        wait_then_cleanup();
        return 1;
    }

    sdl_init = SDL_Init(SDL_INIT_GAMECONTROLLER) == 0;
    if (!sdl_init) {
        debugPrint("SDL_Init failed: %s\n", SDL_GetError());
        wait_then_cleanup();
        return 1;
    }

    pbk_init = pb_init() == 0;
    if (!pbk_init) {
        debugPrint("pbkit init failed\n");
        wait_then_cleanup();
        return 1;
    }

    pb_show_front_screen();

    // int width, height, channels;
    // unsigned char* image_data = stbi_load("image.png", &width, &height, &channels, STBI_rgb_alpha);
    // if (!image_data) {
    //     printf("Failed to load image\n");
    //     return 1;
    // }

    ImageData img = load_image("D:\\testimg.png");
    u8 imggg[] = {
                 255,   0,   0, 0xff, 
                 255, 110,   0, 0xff,
                 0,   255,   0, 0xff,
                 100,     0, 255, 0xff};
    void *textureAddr = MmAllocateContiguousMemoryEx(16, 0, MAX_MEM_64, 0, 0x404);
    memcpy(textureAddr, img.image, 4*4); // TODO use img.length (whatever that is...)

    init_shader_old();
    
    //0, 0x3ffb000 means anywhere in the memory that is less than 64 mb.
    // https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
    alloc_vertices = MmAllocateContiguousMemoryEx(sizeof(verts), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(alloc_vertices, verts, sizeof(verts));
    alloc_vertices2 = MmAllocateContiguousMemoryEx(sizeof(verts2), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(alloc_vertices2, verts2, sizeof(verts2));
    alloc_vertices3 = MmAllocateContiguousMemoryEx(sizeof(verts3), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(alloc_vertices3, verts3, sizeof(verts3));
    alloc_vertices_cube = MmAllocateContiguousMemoryEx(sizeof(cube_vertices), 0, MAX_MEM_64, 0, 0x404);
    memcpy(alloc_vertices_cube, cube_vertices, sizeof(cube_vertices));
    num_vertices = sizeof(verts)/sizeof(verts[0]);

    /* Create view matrix (our camera is static) */
    create_world_view(m_view, v_cam_loc, v_cam_rot);

    /* Create projection matrix */
    create_view_screen(m_proj, (float)width/(float)height, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10000.0f);

    /* Create viewport matrix, combine with projection */
    matrix_viewport(m_viewport, 0, 0, width, height, 0, 65536.0f);
    matrix_multiply(m_proj, m_proj, (float*)m_viewport);

    // default surface color anyway but...
    pb_set_color_format(NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8, false);

    i32 sw = 0;
    f32 obj_rotationX = 0;
    f32 obj_rotationY = 0;
    for (;;) {
        pb_wait_for_vbl();
        pb_target_back_buffer();
        pb_reset();
        pb_erase_depth_stencil_buffer(0, 0, width, height);
        pb_fill(0, 0, width, height, 0xff0E060C);
        pb_erase_text_screen();

        /* Tilt and rotate the object a bit */
        v_obj_rot[0] = obj_rotationX/1000.0f * M_PI * -0.25f;
        v_obj_rot[1] = obj_rotationY/1000.0f * M_PI * -0.25f;

        /* Create local->world matrix given our updated object */
        matrix_unit(m_model);
        matrix_rotate(m_model, m_model, v_obj_rot);
        matrix_scale(m_model, m_model, v_obj_scale);
        matrix_translate(m_model, m_model, v_obj_pos);

        init_shader(0);
        while(pb_busy()) {
            /* Wait for completion... */
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_CONTROLLERDEVICEADDED) {
                SDL_GameController *new_pad = SDL_GameControllerOpen(e.cdevice.which);
                if (pad == NULL) {
                    pad = new_pad;
                }
            } else if (e.type == SDL_CONTROLLERDEVICEREMOVED) {
                if (pad == SDL_GameControllerFromInstanceID(e.cdevice.which)) {
                    pad = NULL;
                }
                SDL_GameControllerClose(SDL_GameControllerFromInstanceID(e.cdevice.which));
            } else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
                if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                    pad = (SDL_GameControllerFromInstanceID(e.cdevice.which));
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_F3) {
                    f3key = 1;
                }
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_F3) {
                    f3key = 0;
                }
            }
        }

        SDL_GameControllerUpdate();
        if (pad == NULL) {
            pb_print("Press start on a controller to test\n");
        }
        else {

            i16 look_x_axis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTX);
            if (look_x_axis > 2000 || look_x_axis < -2000) {
                // x is y because rotation ok?!
                obj_rotationY += (float) (look_x_axis) / 1000.f;
            }
            i16 look_y_axis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTY);
            if (look_y_axis > 2000 || look_y_axis < -2000) {
                obj_rotationX += (float) (look_y_axis) / 1000.f;
            }
            if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A)) {
                obj_rotationX = 0;
                obj_rotationY = 0;
            }

            pb_print(
                    "Testing Controller %d.\n"
                    "Press Start on another controller to test\n\n"
                    "Axis:\n"
                    "- Lstick: x=%d, y=%d\n"
                    "- Rstick: x=%d, y=%d\n"
                    "- Ltrig: %d\n"
                    "- Rtrig: %d\n"
                    "Buttons:\n"
                    "- A:%d B:%d X:%d Y:%d\n"
                    "- Back:%d Start:%d White:%d Black:%d\n"
                    "- Up:%d Down:%d Left:%d Right:%d\n"
                    "- Lstick:%d, Rstick:%d\n"
                    "- Vendor: %04x Product: %04x\n"
                    "- F3: %d\n"
                    "- Obj rot X: %d, Y: %d\n",
                    SDL_GameControllerGetPlayerIndex(pad),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTX),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTY),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT),
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_Y),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSTICK),
                    SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSTICK),
                    SDL_GameControllerGetVendor(pad), 
                    SDL_GameControllerGetProduct(pad),
                    f3key,
                    (int) obj_rotationX,
                    (int) obj_rotationY
                        );

            SDL_GameControllerRumble(pad, SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) * 2,
                    SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) * 2,
                    100);
        }
        if (fps > 0) {
            pb_print("FPS: %d", fps);
        }
        pb_draw_text_screen();

        // {
        //     u32 *p = pb_begin();
        //
        //     /* Set shader constants cursor at C0 */
        //     p = pb_push1(p, NV097_SET_TRANSFORM_CONSTANT_LOAD, 96);
        //
        //     /* Send the transformation matrix */
        //     pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
        //     memcpy(p, m_viewport, 16*4); p+=16;
        //
        //     float constants_0[2] = {0, 1,};
        //     pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 8);
        //     memcpy(p, constants_0, 8); p+=8;
        //
        //     /* Clear all attributes */
        //     pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT,16);
        //     for(u8 i = 0; i < 16; i++) {
        //         *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
        //     }
        //     pb_end(p);
        //
        //     /* Set vertex position attribute */
        //     set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
        //             3, sizeof(ColoredVertex), &alloc_vertices[0]);
        //
        //     /* Set vertex diffuse color attribute */
        //     // set_attrib_pointer(3, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
        //     //         3, sizeof(ColoredVertex), &alloc_vertices[3]);
        //
        //     /* Begin drawing triangles */
        //     draw_arrays(NV097_SET_BEGIN_END_OP_TRIANGLES, 0, num_vertices);

            /* Set vertex position attribute */
            // set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            //         3, sizeof(ColoredVertex), &alloc_vertices3[0]);
            //
            // /* Set vertex diffuse color attribute */
            // set_attrib_pointer(3, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
            //         3, sizeof(ColoredVertex), &alloc_vertices3[3]);
            //
            // /* Begin drawing triangles */
            // draw_arrays(NV097_SET_BEGIN_END_OP_TRIANGLES, 0, num_vertices);
        // }
        init_shader(1);
        {
        /*
         * Setup texture stages
         */

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
            u32 *p = pb_begin();
        p = pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),(DWORD)textureAddr & 0x03ffffff,0x0001122a); //set stage 0 texture address & format
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),4<<16); //set stage 0 texture pitch (pitch<<16)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(2<<16)|2); //set stage 0 texture width & height ((witdh<<16)|height)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00030303);//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); //set stage 0 texture enable flags
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x04074000); //set stage 0 texture filters (AA!)
        pb_end(p);

        /* Disable other texture stages */
        p = pb_begin();
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1),0x0003ffc0);//set stage 1 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2),0x0003ffc0);//set stage 2 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3),0x0003ffc0);//set stage 3 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),0x00030303);//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),0x00030303);//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),0x00030303);//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1),0x02022000);//set stage 1 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2),0x02022000);//set stage 2 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3),0x02022000);//set stage 3 texture filters (no AA, stage not even used)
        pb_end(p);



        p = pb_begin();
            /* Set shader constants cursor at C0 */
            p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

            /* Send the transformation matrix */
            // pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
            // memcpy(p, m_viewport, 16*4); p+=16;

            /* Send the model matrix */
            pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
            memcpy(p, m_model, 16*4); p+=16;

            /* Send the view matrix */
            pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
            memcpy(p, m_view, 16*4); p+=16;

            /* Send the projection matrix */
            pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
            memcpy(p, m_proj, 16*4); p+=16;

            /* Send camera position */
            // pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
            // memcpy(p, v_cam_loc, 4*4); p+=4;

            // float constants_0[2] = {0, 1,};
            // pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 8);
            // memcpy(p, constants_0, 8); p+=8;

            /* Clear all attributes */
            pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT,16);
            for(u8 i = 0; i < 16; i++) {
                *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
            }
            pb_end(p);

            /*
             * Setup vertex attributes
             */

            /* Set vertex position attribute */
            set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
                    3, sizeof(Vertex), &alloc_vertices_cube[0]);

            /* Set vertex diffuse color attribute */
            set_attrib_pointer(3, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
                    3, sizeof(Vertex), &alloc_vertices_cube[3]);

            /* Set texture coordinate attribute */
            set_attrib_pointer(9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F,
                           2, sizeof(Vertex), &alloc_vertices_cube[6]);

            /* Begin drawing triangles */
            draw_indexed();
        }
        while (pb_busy());
        while (pb_finished());

        frames++;

        /* Latch FPS counter every second */
        now = GetTickCount();
        if ((now-last) > 1000) {
            fps = frames;
            frames = 0;
            last = now;
        }
    }

    pb_kill();
    return 0;
}

/* Construct a viewport transformation matrix */
static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max)
{
    memset(out, 0, 4*4*sizeof(float));
    out[0][0] = width/2.0f;
    out[1][1] = height/-2.0f;
    out[2][2] = z_max - z_min;
    out[3][3] = 1.0f;
    out[3][0] = x + width/2.0f;
    out[3][1] = y + height/2.0f;
    out[3][2] = z_min;
}

