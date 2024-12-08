#pragma once

#include "nums.h"

typedef void (*SoundCallback)(i16* sound_buffer, size_t sample_count);

bool sound_init(SoundCallback sound_cb, size_t sample_count);
bool sound_is_initialized(void);
void sound_play(void);
void sound_pause(void);
void sound_lock(void);
void sound_unlock(void);

