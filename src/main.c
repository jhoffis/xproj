#include <hal/debug.h>
#include <hal/video.h>
#include <SDL.h>
#include <windows.h>

#include "audio.h"
#include "file_util.h"
#include "nums.h"
#include "random.h"
#include "shader.h"
#include "terrain.h"
#include "xboxkrnl/xboxkrnl.h"
#include "math3d.h"
#include "cube.h"
#include "mvp.h"


#include "nxdk_wav.h"


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

void testSound(i16* sound_buffer, size_t sample_count) {
    // guess: put the sound into the buffer?
    // memcpy(sound_buffer, nxdk_wav_h_bin, sample_count);
    // int is_final = (voice_pos+buffer_size) >= voice_len;
    // int chunk_size = MIN(voice_len-voice_pos, buffer_size);
    //
    // memcpy(buffers[current_buf], voice_data+voice_pos, chunk_size);
    // XAudioProvideSamples(buffers[current_buf], chunk_size, is_final);
    //
    // if (is_final) {
    //     voice_pos = 0;
    // } else {
    //     voice_pos = voice_pos+chunk_size;
    // }
    //
    // current_buf = (current_buf+1) % NUM_BUFFERS;
    // callback_count++;
   static float phase = 0.0f;       // Keep track of the waveform phase
    const float frequency = 440.0f; // Frequency in Hz (e.g., A4)
    const float sample_rate = 48000.0f; // Audio sample rate in Hz
    const float amplitude = 30000.0f;   // Amplitude (scaled to i16 range)

    for (size_t i = 0; i < sample_count; i++) {
        sound_buffer[i] = (i16)(amplitude * sinf(phase));
        phase += 2.0f * M_PI * frequency / sample_rate;
        if (phase > 2.0f * M_PI) {
            phase -= 2.0f * M_PI; // Wrap phase to avoid floating-point overflow
        }
    }
}

int main(void)
{
    SDL_Event e;

    i32       start, last, now;
    i32       fps, frames;
    start = now = last = GetTickCount();
    frames = fps = 0;
    int f3key = 0;

    if (!XVideoSetMode(width, height, 32, REFRESH_60HZ)) {
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

    sound_init(testSound, 16512); // nxdk_wav_h_bin_len);

    ImageData img = load_image("grass");

    alloc_vertices_cube = MmAllocateContiguousMemoryEx(sizeof(cube_vertices), 0, MAX_MEM_64, 0, 0x404);
    memcpy(alloc_vertices_cube, cube_vertices, sizeof(cube_vertices));


    /* Create projection matrix */
    create_view_screen(m_proj, (float)width/(float)height, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10000.0f);

    /* Create viewport matrix, combine with projection */
    matrix_viewport(m_viewport, 0, 0, width, height, 0, 65536.0f);
    matrix_multiply(m_proj, m_proj, (float*)m_viewport);

    // default surface color anyway but...
    pb_set_color_format(NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8, false);
    

    f32 move_speed = 5;
    i32 sw = 0;
    f32 obj_rotationX = 0;
    f32 obj_rotationY = 0;
    f32 cam_rotationX = 0;
    f32 cam_rotationY = 0;
    f32 cam_posX = 0;
    f32 cam_posY = 0;
    f32 cam_posZ = 0;
    for (;;) {

        sound_play();

        pb_wait_for_vbl();
        pb_target_back_buffer();
        pb_reset();
        pb_erase_depth_stencil_buffer(0, 0, width, height);
        pb_fill(0, 0, width, height, 0xff0E060C);
        pb_erase_text_screen();

        v_cam_rot[0] = cam_rotationX;
        v_cam_rot[1] = cam_rotationY;
        v_cam_loc[1] = cam_posY;

        float x = sin(cam_rotationY) * move_speed;
        float z = cos(cam_rotationY) * move_speed;

        // Forward and backwards + side to side
        v_cam_loc[0] += (x * cam_posZ) + (z * cam_posX);
        v_cam_loc[2] += (z * cam_posZ) - (x * cam_posX);


        /* Create view matrix (our camera is static) */
        create_world_view(m_view, v_cam_loc, v_cam_rot);

        while (pb_busy()) {}

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
                cam_rotationY -= (float) (look_x_axis) / 1000000.f;
            }
            i16 look_y_axis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTY);
            if (look_y_axis > 2000 || look_y_axis < -2000) {
                cam_rotationX -= (float) (look_y_axis) / 1000000.f;
            }
            i16 walk_x_axis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
            if (walk_x_axis > 2000 || walk_x_axis < -2000) {
                // x is y because rotation ok?!
                cam_posX = (float) (walk_x_axis) / 32767.f;
            } else {
                cam_posX = 0;
            }

            i16 walk_y_axis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);
            if (walk_y_axis > 2000 || walk_y_axis < -2000) {
                cam_posZ = (float) (walk_y_axis) / 32767.f;
            } else {
                cam_posZ = 0;
            }
            if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A) | SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
                cam_posY += 5;
            }
            if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
                cam_posY -= 5;
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

        render_terrain(img);

        // render text in front of everything
        pb_draw_text_screen();
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

