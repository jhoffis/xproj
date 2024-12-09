/**** gsound.c (nxdk) ****/

#include "audio.h"
#include <hal/audio.h>
#include <SDL_mixer.h>
#include <string.h>
#include <windows.h>

#define BUFFER_COUNT 3

static volatile u8* AudioMMIO = (u8*)0xFEC00000;
static bool LInitialized;
static SoundCallback LSoundCallback;
static i16* LBuffers;
static size_t LNextBuffer;
static size_t LSampleCount;
static DWORD LSleepCount;
static DWORD LAudioThreadID;
static u8 LLastDescriptorIndex;
static CRITICAL_SECTION LCriticalSection;

#define CURRENT_PCM_OUT_INDEX AudioMMIO[0x114]

static void push_audio(void) {
    XAudioProvideSamples((unsigned char*)(LBuffers + LNextBuffer*LSampleCount), (unsigned short)(LSampleCount*2), false);
    LNextBuffer = (LNextBuffer + 1)%BUFFER_COUNT;
    LSoundCallback(LBuffers + LNextBuffer*LSampleCount, LSampleCount);
}

static DWORD __stdcall audio_thread(LPVOID parameter) {
    (void)parameter;
    for (;;) {
        u8 Index;
        Sleep(LSleepCount);
        Index = CURRENT_PCM_OUT_INDEX;
        if (Index != LLastDescriptorIndex) {
            LLastDescriptorIndex = Index;
            EnterCriticalSection(&LCriticalSection);
            push_audio();
            LeaveCriticalSection(&LCriticalSection);
        }
    }
}

bool sound_init(SoundCallback sound_cb, size_t sample_count) {
    if (LInitialized) return false; // FIXME "The sound system is already initialized"
    if (sound_cb == NULL) return false; // FIXME "Missing sound callback"
    if (sample_count < 1024) return false; // FIXME "Buffer size too small"
    if (sample_count >= 32767) return false; // FIXME "Buffer size too large"
    InitializeCriticalSection(&LCriticalSection);
    LSleepCount = sample_count/(48*3);
    LSoundCallback = sound_cb;
    LSampleCount = sample_count;
    LBuffers = MmAllocateContiguousMemoryEx(BUFFER_COUNT*sample_count*2, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memset(LBuffers, 0, BUFFER_COUNT*sample_count*2);
    LNextBuffer = 0;
    XAudioInit(16, 2, NULL, NULL);
    LLastDescriptorIndex = 0xFF;
    SetThreadPriority(CreateThread(NULL, 1024, audio_thread, NULL, 0, &LAudioThreadID), THREAD_PRIORITY_TIME_CRITICAL);
    for (int i = 0; i < BUFFER_COUNT; ++i) {
        push_audio();
    }
    XAudioPlay();
    LInitialized = true;
    return true;
}

bool sound_is_initialized(void) {
    return LInitialized;
}

void sound_play(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    XAudioPlay();
}

void sound_pause(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    XAudioPause();
}

void sound_lock(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    EnterCriticalSection(&LCriticalSection);
}

void sound_unlock(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    LeaveCriticalSection(&LCriticalSection);
}
