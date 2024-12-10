#include "wav_loader.h"
#include "xboxkrnl/xboxkrnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* load_wav(const char* filename, wav_header* header) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Read the WAV header
    if (fread(header, sizeof(wav_header), 1, file) != 1) {
        perror("Error reading header");
        fclose(file);
        return NULL;
    }

    // Validate the header (optional, but recommended)
    if (strncmp(header->riff, "RIFF", 4) != 0 || strncmp(header->wave, "WAVE", 4) != 0) {
        fprintf(stderr, "Invalid WAV file\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for the audio data
    void* audioData = MmAllocateContiguousMemoryEx(sizeof(header->data_size), 0, MAX_MEM_64, 0, 0x404);
    if (!audioData) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the audio data
    if (fread(audioData, 1, header->data_size, file) != header->data_size) {
        perror("Error reading audio data");
        free(audioData);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return audioData;
}
