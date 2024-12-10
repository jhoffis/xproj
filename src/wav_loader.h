#pragma once
#include "nums.h"

typedef struct {
    i8 riff[4];        // "RIFF"
    i8 wave[4];        // "WAVE"
    i8 fmt[4];         // "fmt "
    i8 data[4];          // "data"
    u32 data_size;      // Size of the data chunk
    u32 chunk_size;     // File size - 8 bytes
    u32 subchunk1_size; // Size of the fmt chunk
    u32 sample_rate;    // Sample rate
    u32 byte_rate;      // Byte rate
    u16 audio_format;   // Audio format (1 for PCM)
    u16 num_channels;   // Number of channels
    u16 block_align;    // Block align
    u16 bits_per_sample; // Bits per sample
} wav_header;

void *load_wav(const char *filename, wav_header *header);
