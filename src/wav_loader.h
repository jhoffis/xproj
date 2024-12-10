#pragma once
#include "nums.h"

typedef struct {
    u8 chunk_id[4];        // "riff"
    u32 chunk_size;        // size of the rest of the file minus 8 bytes
    u8 format[4];         // "wave"
    
    // "fmt " subchunk
    u8 subchunk1_id[4];    // "fmt "
    u32 subchunk1_size;    // size of the fmt chunk (16 for pcm)
    u16 audio_format;      // pcm = 1, other values indicate compression
    u16 num_channels;      // mono = 1, stereo = 2, etc.
    u32 sample_rate;       // 8000, 44100, etc.
    u32 byte_rate;         // == samplerate * numchannels * bitspersample/8
    u16 block_align;       // == numchannels * bitspersample/8
    u16 bits_per_sample;    // 8 bits = 8, 16 bits = 16, etc.
    
    // "data" subchunk
    u8 subchunk2_id[4];    // "data"
    u32 subchunk2_size;    // == numsamples * numchannels * bitspersample/8
    // Actual sound data starts here
} wav_header;

void* load_wav(const char* filename);

