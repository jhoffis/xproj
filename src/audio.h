#pragma once

#include "nums.h"
#include <SDL.h>

typedef void (*sound_callback)(i16* sound_buffer, size_t sample_count);

bool xaudio_init(sound_callback sound_cb, size_t sample_count);
bool xaudio_is_initialized(void);
void xaudio_play(void);
void xaudio_pause(void);
void xaudio_lock(void);
void xaudio_unlock(void);

bool sdl_audio_init(void);
bool sdl_is_initialized(void);
