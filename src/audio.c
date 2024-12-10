/**** 
 * TODO:
 * FOR EMULATOR
 *     gsound.c (nxdk) 
 * FOR XBOX
 *     xaudio raw?
 * FOR PC
 *     openal?
 ****/

#include "audio.h"
#include <hal/audio.h>
#include <string.h>
#include <windows.h>

#define BUFFER_COUNT 3

static volatile u8* l_audio_MMIO = (u8*)0xFEC00000;
#define CURRENT_PCM_OUT_INDEX l_audio_MMIO[0x114]

static bool l_xaudio_initialized;
static sound_callback l_sound_callback;
static i16* l_buffers;
static size_t l_next_buffer;
static size_t l_sample_count;
static DWORD l_sleep_count;
static DWORD l_audio_thread_id;
static u8 l_last_descriptor_index;
static CRITICAL_SECTION l_critical_section;


static void xaudio_push(void) {
    XAudioProvideSamples((u8*)(l_buffers + l_next_buffer*l_sample_count), (u16)(l_sample_count*2), false);
    l_next_buffer = (l_next_buffer + 1)%BUFFER_COUNT;
    l_sound_callback(l_buffers + l_next_buffer*l_sample_count, l_sample_count);
}

static DWORD __stdcall xaudio_thread(LPVOID parameter) {
    (void)parameter;
    for (;;) {
        Sleep(l_sleep_count);
        u8 index = CURRENT_PCM_OUT_INDEX;
        if (index != l_last_descriptor_index) {
            l_last_descriptor_index = index;
            EnterCriticalSection(&l_critical_section);
            xaudio_push();
            LeaveCriticalSection(&l_critical_section);
        }
    }
}

bool xaudio_init(sound_callback sound_cb, size_t sample_count) {
    if (l_xaudio_initialized) return false; // FIXME "The sound system is already initialized"
    if (sound_cb == NULL) return false; // FIXME "Missing sound callback"
    if (sample_count < 1024) return false; // FIXME "Buffer size too small"
    if (sample_count >= 32767) return false; // FIXME "Buffer size too large"
    InitializeCriticalSection(&l_critical_section);
    l_sleep_count = sample_count/(48*3);
    l_sound_callback = sound_cb;
    l_sample_count = sample_count;
    l_buffers = MmAllocateContiguousMemoryEx(BUFFER_COUNT*sample_count*2, 0, MAX_MEM_64, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memset(l_buffers, 0, BUFFER_COUNT*sample_count*2);
    l_next_buffer = 0;
    XAudioInit(16, 2, NULL, NULL);
    l_last_descriptor_index = 0xFF;
    SetThreadPriority(CreateThread(NULL, 1024, xaudio_thread, NULL, 0, &l_audio_thread_id), THREAD_PRIORITY_TIME_CRITICAL);
    for (int i = 0; i < BUFFER_COUNT; ++i) {
        xaudio_push();
    }
    XAudioPlay();
    l_xaudio_initialized = true;
    return true;
}

bool xaudio_is_initialized(void) {
    return l_xaudio_initialized;
}

void xaudio_play(void) {
    if (!l_xaudio_initialized) return; // FIXME "The sound system is not initialized"
    XAudioPlay();
}

void xaudio_pause(void) {
    if (!l_xaudio_initialized) return; // FIXME "The sound system is not initialized"
    XAudioPause();
}

void xaudio_lock(void) {
    if (!l_xaudio_initialized) return; // FIXME "The sound system is not initialized"
    EnterCriticalSection(&l_critical_section);
}

void xaudio_unlock(void) {
    if (!l_xaudio_initialized) return; // FIXME "The sound system is not initialized"
    LeaveCriticalSection(&l_critical_section);
}


/*
 *  SDL AUDIO
 */
#define PI2 6.28318530718
static bool l_sdl_initialized;
static float l_audio_time = 0;
static float l_audio_freq = 440;
static SDL_AudioSpec* l_digi_audiospec = NULL;

static void _xoxo_callback(void* userdata, Uint8* stream, int len) {
	short * snd = (short*) stream;
	len /= sizeof(*snd);
	for(int i = 0; i < len; i++) //Fill array with frequencies, mathy-math stuff
	{
		snd[i] = 32000 * sin(l_audio_time);
		
		l_audio_time += l_audio_freq * PI2 / 48000.0;
		if(l_audio_time >= PI2)
			l_audio_time -= PI2;
	}
}

bool sdl_audio_init(void) {

    if (!l_sdl_initialized) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            return false;
        }
        l_sdl_initialized = true;
    }

	SDL_AudioFormat desired_audioformat;
	SDL_version version;
	SDL_GetVersion(&version);
	if (version.major <= 2 && version.minor <= 0 && version.patch <= 3) {
		// In versions before 2.0.4, 16-bit audio samples don't work properly (the sound becomes garbled).
		// See: https://bugzilla.libsdl.org/show_bug.cgi?id=2389
		// Workaround: set the audio format to 8-bit, if we are linking against an older SDL2 version.
		desired_audioformat = AUDIO_U8;
		printf("Your SDL.dll is older than 2.0.4. Using 8-bit audio format to work around resampling bug.");
	} else {
		desired_audioformat = AUDIO_S16SYS;
	}

	SDL_AudioSpec *desired;
	desired = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
	memset(desired, 0, sizeof(SDL_AudioSpec));
    desired->freq = 44100; //declare specs
	desired->format = desired_audioformat;
	desired->channels = 2;
	desired->samples = 1024;
	desired->callback = _xoxo_callback;
	desired->userdata = NULL;
    
    if (SDL_OpenAudio(desired, NULL) != 0) {
		// sdlperror("init_digi: SDL_OpenAudio");
		//quit(1);
		// digi_unavailable = 1;
		return false;
	}
    SDL_PauseAudio(0);
	l_digi_audiospec = desired;
    // SDL_AudioSpec *obtained = {0};
    // malloc(sizeof(SDL_AudioSpec));
    // memset(obtained, 0, sizeof(SDL_AudioSpec));

    // if (SDL_OpenAudio(&desired, obtained) == -1) {
    //     return false;
    // }
    // LAudio_freq = obtained->freq;

	// SDL_PauseAudio(0); // unpause
//
//     while (1) {
// SDL_Delay(3000);
//     }
	
  //Open audio, if error, print
	// int id;
	// if ((id = SDL_OpenAudioDevice(NULL, 0, &desired, obtained, SDL_AUDIO_ALLOW_ANY_CHANGE)) <= 0) {
 //        return false;
	// }
	//
	/* Start playing, "unpause" */
	// SDL_PauseAudioDevice(id, 0);
    // SDL_UnlockAudio();

	// while(true) //Stall for time while audio plays
	// {
	//
 //    	  //Play A
 //          LAudio_freq = 440;
	//
 //          SDL_Delay(3000);
	//   //Play middle C
 //          LAudio_freq = 261.6256;
 //          SDL_Delay(3000);
 // //          break;

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
	// }
    return true;
}

bool sdl_is_initialized(void) {
    return l_sdl_initialized;
}
