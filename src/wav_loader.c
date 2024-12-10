#include "wav_loader.h"
#include "xboxkrnl/xboxkrnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* path_name(const char *name) {
    char *path = malloc(100);
    strcpy(path, "D:\\");
    strcat(path, name);
    strcat(path, ".wav");
    return path;
}

void* load_wav(const char* filename) {
    char *fixed_name = path_name(filename);
    FILE* file = fopen(fixed_name, "rb");
    free((void*)fixed_name);
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    wav_header header = {0};
    // Read the WAV header
    if (fread(&header, sizeof(wav_header), 1, file) != 1) {
        perror("Error reading header");
        fclose(file);
        return NULL;
    }

    // Validate the header (optional, but recommended)
    if (strncmp((const char*)header.chunk_id, "RIFF", 4) != 0 ||
        strncmp((const char*)header.format, "WAVE", 4) != 0) {
        fprintf(stderr, "Invalid WAV file\n");
        fclose(file);
        return NULL;
    }


    // Allocate memory for the audio data
    void* audio_data = MmAllocateContiguousMemoryEx(sizeof(header.subchunk2_size), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!audio_data) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }
    // Read the audio data
    if (fread(audio_data, 1, header.subchunk2_size, file) != header.subchunk2_size) {
        perror("Error reading audio data");
        free(audio_data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return audio_data;
}

