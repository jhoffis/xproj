#include "wav_loader.h"
#include "xboxkrnl/xboxkrnl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const u32 ALLOC_SIZE = 64 * 1024;

static char* path_name(const char *name) {
    char *path = malloc(100);
    strcpy(path, "D:\\");
    strcat(path, name);
    strcat(path, ".wav");
    return path;
}

static wav_file* load_wav(const char* filename, u32 start_index, u32 max_alloc_size) {
    char *fixed_name = path_name(filename);
    FILE* file = fopen(fixed_name, "rb");
    if (!file) {
        perror("Error opening file");
        free(fixed_name); // Assuming path_name allocates memory
        return NULL;
    } 
    wav_header header = {0};

    // Read the WAV header
    if (fread(&header, sizeof(wav_header), 1, file) != 1) {
        perror("Error reading header");
        fclose(file);
        free(fixed_name);
        return NULL; 
    }

    // Validate the header
    if (header.chunk_id != RIFF_ID || header.format != WAVE_ID ||
            header.subchunk1_id != FMT_ID || header.subchunk2_id != DATA_ID) {
        fprintf(stderr, "Invalid WAV file\n");
        fclose(file);
        free(fixed_name);
        return NULL; 
    }

    // Allocate memory for the audio data
    u32 alloc_size = max_alloc_size;
    if (alloc_size > header.subchunk2_size - start_index) {
        alloc_size = header.subchunk2_size - start_index;
    }

    void* audio_data = MmAllocateContiguousMemoryEx(alloc_size, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!audio_data) {
        perror("Memory allocation failed");
        fclose(file);
        free(fixed_name);
        return NULL; 
    }

    // Move the file pointer to the correct start position within the data chunk
    if (fseek(file, start_index, SEEK_CUR) != 0) {
        perror("Error seeking in file");
        MmFreeContiguousMemory(audio_data);
        fclose(file);
        free(fixed_name);
        return NULL; 
    }

    // Read the audio data
    if (fread(audio_data, 1, alloc_size, file) != alloc_size) {
        perror("Error reading audio data");
        MmFreeContiguousMemory(audio_data);
        fclose(file);
        free(fixed_name);
        return NULL; 
    }

    fclose(file);
    wav_file* wav = malloc(sizeof(wav_file));
    if (!wav) {
        perror("Memory allocation for wav_file failed");
        free(fixed_name);
        return NULL;
    }
    *wav = (wav_file){header, filename, audio_data, alloc_size};
    return wav;
}

wav_entity *create_wav_entity(const char *filename) {
    wav_file *wav = load_wav(filename, 0, 2*ALLOC_SIZE);
    if (wav == NULL) {
        // FIXME fail;
        return NULL;
    }
    wav_entity *entity = malloc(sizeof(wav_entity));
    entity->header = wav->header;
    entity->filename = strdup(filename); 
    entity->cursor = malloc(sizeof(u32));
    *entity->cursor = 0;

    if (wav->loaded_data_size < ALLOC_SIZE) {
        entity->current_data_size = wav->loaded_data_size;
        entity->current_data = malloc(entity->current_data_size);
        memcpy(entity->current_data, wav->loaded_data, entity->current_data_size); 
        entity->loaded_cursor = 0;
        entity->next_data_size = 0;
        return entity;
    }
    entity->current_data_size = ALLOC_SIZE;
    entity->current_data = malloc(entity->current_data_size);
    memcpy(entity->current_data, wav->loaded_data, entity->current_data_size); 

    if (wav->loaded_data_size - ALLOC_SIZE < ALLOC_SIZE) {
        entity->next_data_size = wav->loaded_data_size - ALLOC_SIZE;
        entity->next_data = malloc(entity->next_data_size);
        memcpy(entity->next_data, &wav->loaded_data[ALLOC_SIZE], entity->next_data_size); 
        entity->loaded_cursor = 0;
        return entity;
    }
    entity->next_data_size = ALLOC_SIZE;
    entity->next_data = malloc(entity->next_data_size);
    memcpy(entity->next_data, &wav->loaded_data[ALLOC_SIZE], entity->next_data_size); 
    entity->loaded_cursor = 2*ALLOC_SIZE;
    
    // TODO delete wav file.
    return entity;
}

/*
 *  Load after having filled the sample buffer
 */
bool load_next_wav_buffer(wav_entity *entity) {
    free(entity->current_data);
    if (entity->next_data_size == 0) {
        entity->current_data = NULL;
        return false;
    }
    entity->current_data = entity->next_data;
    entity->current_data_size = entity->next_data_size;
    if (entity->loaded_cursor == 0) {
        entity->next_data_size = 0;
        entity->next_data = NULL;
        return true;
    }
    wav_file *wav = load_wav(entity->filename, entity->loaded_cursor, ALLOC_SIZE);
    if (wav == NULL) {
        entity->next_data_size = 0;
        entity->next_data = NULL;
        return true;
    }

    if (wav->loaded_data_size < ALLOC_SIZE) {
        entity->loaded_cursor = 0;
        entity->next_data_size = wav->loaded_data_size;
    } else {
        entity->loaded_cursor += ALLOC_SIZE; 
        entity->next_data_size = ALLOC_SIZE;
    }
    entity->next_data = malloc(entity->next_data_size);
    memcpy(entity->next_data, wav->loaded_data, entity->next_data_size); 

    return true;
}








