//Demo 03-Q*Bert

#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include <openxdk/debug.h>

#include "pbKit.h"
#include "outer.h"


#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#include "qbert.h"






void XBoxStartup(void)
{
	int		i;

	XInput_Init();

	pb_extra_buffers(2);
	//we want 1 extra frame buffer (#0) that will be the non-AntiAliased back buffer
	//(an AntiAliased texture copy will be done from static non-AA back buffer towards AA rotating back buffer)
	//we want 1 extra frame buffer (#1) that will hold sprites, ready to copy.
	
	switch(pb_init())
	{
		case 0: break; //no error

		case -4:		
			debugPrint("IRQ3 already handled. Can't install GPU interrupt.\n");
			debugPrint("You must apply the patch and compile again OpenXDK.\n");
			debugPrint("Also be sure to call pb_init() before any other call.\n");
			XSleep(2000);
			XReboot();
			return;

		case -5:		
			debugPrint("Unexpected PLL configuration. Report this issue please.\n");
			XSleep(2000);
			XReboot();
			return;
		
		default:
			debugPrint("pb_init Error %d\n");
			XSleep(2000);
			XReboot();
			return;
	}

	prepare_texture_mapping();

	//Because openxdk frame buffer is not allocated in memory, third frame buffer
	//overlaps openxdk frame buffer. To avoid graphic interference, skip upper lines.
	//Note that scrolling up will produce an interference, but won't prevent reading.
	for(i=0;i<22;i++) debugPrint("\n"); //if you use double font size, skip 11 only
	//need to call pb_show_debug_screen() to see it, revert with pb_show_front_buffer()
	//debugPrint("Debug screen (Press X and B to come here or leave from here):\n\n");
	debugPrint("Debug screen (Press X and B to turn antialiasing off/on):\n\n");
	

	prepare_sprites(); //draw needed sprites in extra buffer #1

	while(game());

	pb_kill();

	XInput_Quit();

	XReboot();
}
