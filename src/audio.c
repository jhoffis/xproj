/**** gsound.c (nxdk) ****/

#include "audio.h"
#include "SDL_timer.h"
#include <hal/audio.h>
#include <SDL_audio.h>
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

bool xaudio_init(SoundCallback sound_cb, size_t sample_count) {
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

bool xaudio_is_initialized(void) {
    return LInitialized;
}

void xaudio_play(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    XAudioPlay();
}

void xaudio_pause(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    XAudioPause();
}

void xaudio_lock(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    EnterCriticalSection(&LCriticalSection);
}

void xaudio_unlock(void) {
    if (!LInitialized) return; // FIXME "The sound system is not initialized"
    LeaveCriticalSection(&LCriticalSection);
}


/*
 *  SDL AUDIO
 */
#define PI2 6.28318530718
float LAudio_time = 0;
float LAudio_freq = 440;

void _xoxo_callback(void* userdata, Uint8* stream, int len) {
	short * snd = (short*) stream;
	len /= sizeof(*snd);
	for(int i = 0; i < len; i++) //Fill array with frequencies, mathy-math stuff
	{
		snd[i] = 32000 * sin(LAudio_time);
		
		LAudio_time += LAudio_freq * PI2 / 48000.0;
		if(LAudio_time >= PI2)
			LAudio_time -= PI2;
	}
}

bool sdl_audio_init(void) {
    
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.freq = 48000; //declare specs
	desired.format = AUDIO_S16SYS;
	desired.channels = 1;
	desired.samples = 4096;
	desired.callback = _xoxo_callback;
	desired.userdata = NULL;
    SDL_AudioSpec *obtained = MmAllocateContiguousMemoryEx(sizeof(SDL_AudioSpec), 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memset(obtained, 0, sizeof(SDL_AudioSpec));

    SDL_OpenAudio(&desired, obtained);
    SDL_Delay(1000);
    SDL_PauseAudio(0);
  //Open audio, if error, print
	// int id;
	// if ((id = SDL_OpenAudioDevice(NULL, 0, &desired, obtained, SDL_AUDIO_ALLOW_ANY_CHANGE)) <= 0) {
 //        return false;
	// }
	//
	// /* Start playing, "unpause" */
	// SDL_PauseAudioDevice(id, 0);

	while(true) //Stall for time while audio plays
	{

    	  //Play A
          LAudio_freq = 440;

          SDL_Delay(3000);
	  //Play middle C
          LAudio_freq = 261.6256;
          SDL_Delay(3000);
          break;

          //if needed, you can do cool stuff here, like change frequency for different notes: 
          //https://en.wikipedia.org/wiki/Piano_key_frequencies

          //Another cool thing:
          /*
	  while(true)
	  {
	    for(freq = 440; freq < 880; freq += 10)
	        SDL_Delay(10);
	    for(freq = 870; freq > 450; freq -= 10)
	        SDL_Delay(10);
	  } */
	}
    return true;
}
