#pragma once
#include "nums.h"

#define RIFF_ID 0x46464952  // 'RIFF' in little endian
#define WAVE_ID 0x45564157  // 'WAVE' in little endian
#define FMT_ID  0x20746D66  // 'fmt ' in little endian
#define DATA_ID 0x61746164  // 'data' in little endian

typedef struct {
    u32 chunk_id;        // "RIFF" in little endian
    u32 chunk_size;      // (in bytes) size of the rest of the file minus 8 bytes
    u32 format;          // "WAVE" in little endian
    
    // "fmt " subchunk
    u32 subchunk1_id;    // "fmt " in little endian
    u32 subchunk1_size;  // size of the fmt chunk (16 for pcm)
    u16 audio_format;    // pcm = 1, other values indicate compression
    u16 num_channels;    // mono = 1, stereo = 2, etc.
    u32 sample_rate;     // 8000, 44100, etc.
    u32 byte_rate;       // == samplerate * numchannels * bitspersample/8
    u16 block_align;     // == numchannels * bitspersample/8
    u16 bits_per_sample; // 8 bits = 8, 16 bits = 16, etc.
    
    // "data" subchunk
    u32 subchunk2_id;    // "data" in little endian
    u32 subchunk2_size;  // (in bytes) == numsamples * numchannels * bitspersample/8
    // Actual sound data starts here
} wav_header;

typedef struct {
    wav_header header;
    char *filename;
    void* current_data;
    void* next_data;
    u32 current_data_size;
    u32 next_data_size;
    u32 *cursor;
    u32 loaded_cursor;
} wav_entity;

// wav_file load_wav(const char* filename);
wav_entity *create_wav_entity(const char *filename);
bool load_next_wav_buffer(wav_entity *entity);
void free_wav_entity(wav_entity *entity);
