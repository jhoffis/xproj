//Demo 04-Initial Fantasy

#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include <openxdk/debug.h>
#include <xboxkrnl/xboxkrnl.h> //for MmFreeContiguousMemory

#include "pbKit.h"


#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#include "math3D.h"

#include "graphics.h"

#include "mesh.h"

#define USE_V_BATCHES //first speed optimization, enqueue a vertex batch order (like dma-tag 'ref' on ps2)

//#define USE_I_BATCHES //technic simulation, enqueue index batch order (saves a lots of memory, just simulated here) 

#define WAIT	//force waitings to allow precise loop performance time calculation (remove that to gain more speed later)

//under that displacement, a stick will be considered in neutral position
#define DEADZONE_JOYPAD		16

int model=0;

unsigned int cpu_ticks(void) 
{
	unsigned int v;
	__asm__ __volatile__ ("rdtsc":"=a" (v));
	//edx:eax holds cpu ticks count after 'rdtsc'
	return v;
}

int verbose=0;


void draw_triangle_list(DWORD *vertices,int num_vertices)
{
static	int	overflow_estimation=0;
	DWORD 	*p;
	DWORD	size;
#ifdef USE_V_BATCHES
	DWORD	offset;
	DWORD	num_vertices_this_batch;
#endif

#ifdef USE_I_BATCHES
	DWORD	index_upper,index_lower,i;
#endif

	//Useless if normal method is used and necessary for other ones:
	//we set addresses for V0,V3 and v7 feeds
	//Strides (+4*8) have already been set by prepare_shaders()
	p=pb_begin();
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_VB_POINTER_ATTR0_POS+0*4,1); //v0 feed address
	*(p++)=((DWORD)(&vertices[0]))&0x03ffffff;
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_VB_POINTER_ATTR0_POS+3*4,1); //v3 feed address
	*(p++)=((DWORD)(&vertices[3]))&0x03ffffff;
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_VB_POINTER_ATTR0_POS+7*4,1); //v7 feed address
	*(p++)=((DWORD)(&vertices[6]))&0x03ffffff;
	pb_end(p);
	
#ifdef USE_V_BATCHES
	//vertex batches method: fast (like dma 'ref' tags on ps2)
	//we enqueue 1 dword that triggers up to 256 vertices transferts
	p=pb_begin();
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=TRIANGLES; //(beginning of list)

	size=(num_vertices+251)/252;
	pb_push(p++,0x40000000|NV20_TCL_PRIMITIVE_3D_VB_VERTEX_BATCH,size); //bit 30 means all params go to same register 0x1810
	offset=0;
	while(size)
	{
		if (size>1) 
			num_vertices_this_batch=252; 
		else 
			num_vertices_this_batch=num_vertices%252;
		
		*(p++)=((num_vertices_this_batch-1)<<24)|offset;
		
		offset+=252;
		size--;
	}

	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=STOP; //triggers the drawing (end of list)

	pb_end(p);

	overflow_estimation+=num_vertices;
	if (overflow_estimation>252*8*16000) //depends on push buffer size
	{
		overflow_estimation=0;
		pb_reset(); //jump back to push buffer head to avoid buffer overflow
	}

#else

#ifdef USE_I_BATCHES
	//index batches method: fast
	//we will enqueue 1 dword for every 2 indices (so we need much less pb_reset calls)

	p=pb_begin();
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=TRIANGLES; //(beginning of list)

	//normally you have to read indices from faces list of mesh
	//but here we will just simulate by incrementing counters
	//for educational (This technic is not compatible with ps2)
	
	//This method speed is the vertex batches method speed
	//multiplied by the ratio num_vertices/num_faces where
	//num_vertices is the number of vertices required by the
	//vertex batches method, depending on the mesh structure.
	//(i.e more than the actual number of vertices in the mesh)
	
	index_upper=1; //upper 16 bits is following index
	index_lower=0;
	if (num_vertices&1) //need to handle special case of odd numbers, since indices are handled by pairs
	{
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_INDEX_DATA+4,size); //no constant yet in nouveau header for 0x1804 ('lonely' index data register)
		*(p++)=index_lower;
		num_vertices--; //make the number even now
		index_lower++;
		index_upper++;
	}
	size=num_vertices/2; //xyz,normal,uv
	pb_push(p++,0x40000000|NV20_TCL_PRIMITIVE_3D_INDEX_DATA,size); //bit 30 means all params go to same register 0x1800
	//let's simulate the flow of face indices
	for(i=0;i<size;i++)
	{
		*(p++)=(index_upper<<16)|index_lower;
		index_upper+=2;
		index_lower+=2;
	}

	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=STOP; //triggers the drawing (end of list)

	pb_end(p);

	overflow_estimation+=num_vertices;
	if (overflow_estimation>16*16000)  //depends on push buffer size
	{
		overflow_estimation=0;
		pb_reset(); //jump back to push buffer head to avoid buffer overflow
	}

#else
	//normal method:  slow (may require to call pb_reset often to avoid push buffer overflow)
	//we will enqueue 3+3+2 dwords for each vertice

	p=pb_begin();
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=TRIANGLES; //(beginning of list)

	size=num_vertices*(3+3+2); //xyz,normal,uv
	pb_push(p++,0x40000000|NV20_TCL_PRIMITIVE_3D_VERTEX_DATA,size); //bit 30 means all params go to same register 0x1818
	memcpy(p,vertices,size*4); p+=size;

	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=STOP; //triggers the drawing (end of list)

	pb_end(p);

	overflow_estimation+=num_vertices;
	if (overflow_estimation>16000) //depends on push buffer size
	{
		overflow_estimation=0;
		pb_reset(); //jump back to push buffer head to avoid buffer overflow
	}

#endif

#endif
}



int test(void)
{
	int 			x=320,y=240,z=0;
	int			i,dx,dy,dz,n;
	DWORD			*p;
	int			perf; //performance bar length (time spent rendering in a frame)
	float			aspect=1.78f; //16/9, 4/3=1.33f

	DWORD			*ptr_dma_buffer;	//prepared vertices(uv,normal,xyz) data
	DWORD			*vertices;
	DWORD			dma_size;
	DWORD			num_vertices;
	DWORD			num_batches;
	int			frames=0;
#ifdef USE_V_BATCHES
	//65535 is the upper limit of vertex batch technic (will enqueue 255 commands)
	int			max_vertices_per_batch=65535; //keep it multiple of 3
#else
	//Avoid too big pb_begin=>pb_end blocks (but theoretical limit of index batch method is 32M indices)
	int			max_vertices_per_batch=252; //keep it multiple of 3
#endif

	int			done=0;

	int			max;

	DWORD			frame_start_ticks;
	DWORD			frame_end_ticks;
	DWORD			loop_start_ticks;
	DWORD			frame_ticks;

	int			antialiasing=0;
	int			debug_screen=0;
	int			depth_screen=0;

	int			texture;
	
	VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
	VECTOR object_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

	VECTOR camera_position = { 0.00f, 0.00f, 100.00f, 1.00f };
	VECTOR camera_rotation = { 0.00f, 0.00f, 0.00f,   1.00f };

	VECTOR light_direction = { 0.577f,0.577f,-0.577f, 0.00f };
	VECTOR light_color     = { 1.00f, 1.00f, 1.00f,   0.00f };

	MATRIX			local_world;
	MATRIX			local_light;
	MATRIX			world_view;
	MATRIX			view_screen;
	MATRIX			local_screen;

	//clear screen
	pb_wait_for_vbl();
	frame_start_ticks=cpu_ticks(); //remember when vblank events occured
	pb_reset(); //do it at beginning of the frame
	pb_fill(0,0,640,480,0);
	while(pb_finished());
	pb_wait_for_vbl();
	frame_ticks=cpu_ticks()-frame_start_ticks; //around 12 Millions cpu ticks
	pb_reset(); //do it at beginning of the frame
	pb_fill(0,0,640,480,0);
	while(pb_finished());

	if (load_data()!=1) 
	{
		debugPrint("Can't load .3ds and .bmp files from /meshes sub-directory.\n\n");
		debugPrint("Press Y to quit.\n");
		pb_show_debug_screen();
		while(done==0)
		{
			XInput_GetEvents();
			if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_Y]) done=1;
		}
		return 0;
	}

	//objects names seem to be limited to 10 chars in 3ds files (truncate)

	vertices=NULL;
	if (model==1) num_vertices=get_mesh_batches("SpaceFight",&vertices,max_vertices_per_batch);
	if (model==2) num_vertices=get_mesh_batches("Fuselage",&vertices,max_vertices_per_batch);
	if (num_vertices==0) return 0;
	if (vertices==NULL) return 0;

	if (model==1) texture=get_mesh_texture("SpaceFight");
	if (model==2) texture=get_mesh_texture("Fuselage");
	if (texture==0)
	{
		free(vertices);
		return 0;
	}

	//ooPo's math3D lib, ported from ps2sdk
	
	//create the view_screen matrix.
	create_view_screen(view_screen, aspect, -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	max=num_vertices;

	perf=0;

	while(done==0)
	{
		//per frame stuff:
		
 		pb_wait_for_vbl(); //because we want to count ticks from start of frame (remove it when trying triple buffering technic)

		frame_start_ticks=cpu_ticks(); //remember when vblank events occured
		frames++;

		pb_reset(); //new frame, let's start from push buffer head again

		//we draw in non anti-aliased static extra frame buffer now
		pb_target_extra_buffer(0); //set target for pixel rendering AND image bitblt
		pb_erase_depth_stencil_buffer(0,0,640,480); //clear depth stencil buffer (MANDATORY)
		pb_fill(0,0,640,480,0x0000ff); //clear frame (optional, also, this call and previous one can be merged: flag=0xf3)
		pb_erase_text_screen();
		set_source_extra_buffer(1); //so we read texture from extra buffer #1
		set_filters_off();
		set_transparency_off();

		if (perf) pb_fill(0,50,perf,1,0xffff00);


		//per object stuff:

		object_rotation[1]=(x-640/2)/80.0f;
		while (object_rotation[1] > 3.14f) { object_rotation[1] -= 6.28f; }
		while (object_rotation[1] < -3.14f) { object_rotation[1] += 6.28f; }

		object_rotation[0]=(y-480/2)/80.0f;
		while (object_rotation[0] > 3.14f) { object_rotation[0] -= 6.28f; }
		while (object_rotation[0] < -3.14f) { object_rotation[0] += 6.28f; }

		camera_position[_X]=0.0f+0*(y-480/2)/1.0f;
		camera_position[_Y]=0.0f+0*(y-480/2)/1.0f;
		camera_position[_Z]=500.0f+z*1.0f;

		//create the local_world matrix.
		create_local_world(local_world, object_position, object_rotation);

		//create the local_light matrix.
		create_local_light(local_light, object_rotation);
		//rotate light_dirs with local_light
   
		//create the world_view matrix.
		create_world_view(world_view, camera_position, camera_rotation);

		//create the local_screen matrix.
		create_local_screen(local_screen, local_world, world_view, view_screen);

		//Uploads qwords C0-C13 (per object stuff : matrices, lights, etc...)
		//See vs.vsh for the mapping of constant parameter names to constants indices
		//Note that 'def' instructions will generate automatically upload commands
		//in the sequence created by pb_pcode2mcode, thus, you don't need to upload them
		//Caution, if you no longer use a named constant parameter, it won't
		//be mapped any longer and all incices will be lifted accordingly
		p=pb_begin();
		pb_push1(p,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID,96); p+=2; //set shader constants cursor at C0
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,16); 	//loads C0-C3
		memcpy(p,local_screen,16*4); p+=16;
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,16); 	//loads C4-C7
		memcpy(p,local_light,16*4); p+=16;
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,4); 	//loads C8
		memcpy(p,light_direction,4*4); p+=4;
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,4); 	//loads C9
		memcpy(p,light_color,4*4); p+=4;			
		
		pb_push(p++,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,8); 	//loads C10-C11

		*((float *)(p++))=500.0f; //final 2D scales (better apply this at the very end, in order to not disrupt clipping)
		*((float *)(p++))=500.0f;
		*((float *)(p++))=65536.0f; //actually we can occupy 24 bits of the depth field, but don't overflow
		*((float *)(p++))=1.0f; //don't change it (or unused if you only scale xyz)

		*((float *)(p++))=640.0f/2; //final 2D decals
		*((float *)(p++))=480.0f/2;
		*((float *)(p++))=65536.0f; //getting away from (0,0,0) is good for Z-buffering
		*((float *)(p++))=0.0f; //don't change it (or unused if you only decal xyz)

		pb_end(p);

#ifdef WAIT
		while(pb_busy());
		//we have finished the per frame/object stuff here
#endif
		//per batch stuff:
		loop_start_ticks=cpu_ticks();

		num_batches=(max+max_vertices_per_batch-1)/max_vertices_per_batch;
		ptr_dma_buffer=(DWORD *)vertices;
		while(num_batches) //partial progressive method
		{
			if (num_batches>1)
				n=max_vertices_per_batch;
			else
				n=max%max_vertices_per_batch;

			draw_triangle_list(ptr_dma_buffer,n);

			ptr_dma_buffer+=n*(3+3+2);
			num_batches--;
		}

#ifdef WAIT
		while(pb_busy());
		//We have finished rendering here
#endif
		frame_end_ticks=cpu_ticks();

		//let's calculate next performance bar length
		perf=(int)(640.0f*(frame_end_ticks-frame_start_ticks)/frame_ticks);

		pb_print("Z=%d %d verts (prep:%d%% loop:%d%% %f v/f max)\n",
			(int)camera_position[_Z],
			max,
			(int)(100.0f*(loop_start_ticks-frame_start_ticks)/frame_ticks),
			(int)(100.0f*(frame_end_ticks-loop_start_ticks)/frame_ticks),
			(max*((float)frame_ticks)/(frame_end_ticks-loop_start_ticks))
			);

/*
pb_print("%f %f %f %f\n",m[4*0+0],m[4*0+1],m[4*0+2],m[4*0+3]);
pb_print("%f %f %f %f\n",m[4*1+0],m[4*1+1],m[4*1+2],m[4*1+3]);
pb_print("%f %f %f %f\n",m[4*2+0],m[4*2+1],m[4*2+2],m[4*2+3]);
pb_print("%f %f %f %f\n",m[4*3+0],m[4*3+1],m[4*3+2],m[4*3+3]);
*/

//verbose=0;
//if (verbose) get_mesh_vertices("x",NULL);
//verbose=0;	
		pb_draw_text_screen();

		XInput_GetEvents();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_Y]) done=1;

		if (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_X]) max=((max/252)-1)*252;
		if (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_B]) max=((max/252)+1)*252;

		//Doing antialiasing would require to switch shaders
		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) antialiasing=0;
		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) antialiasing=1;

		if (g_Pads[0].PressedButtons.usDigitalButtons&XPAD_START) 
		{
			if (depth_screen)
			{
				depth_screen=0;
				debug_screen=0;
				pb_show_front_screen();
			}
			else
			{
				depth_screen=1;
				debug_screen=0;
				pb_show_depth_screen();
			}
		}
		
		if (g_Pads[0].PressedButtons.usDigitalButtons&XPAD_BACK)
		{
			if (debug_screen)
			{
				depth_screen=0;
				debug_screen=0;
				pb_show_front_screen();
			}
			else
			{
				depth_screen=0;
				debug_screen=1;
				pb_show_debug_screen();
			}
		}

		if (max<252) max=252;
		if (max>num_vertices) max=num_vertices;

		dx=g_Pads[0].sRThumbX/256;
		dy=-g_Pads[0].sRThumbY/256;
		dz=-g_Pads[0].sLThumbY/256;

		if (dy<-DEADZONE_JOYPAD) y+=(dy+DEADZONE_JOYPAD)/8;
		if (dy>DEADZONE_JOYPAD) y+=(dy-DEADZONE_JOYPAD)/8;
		if (dx<-DEADZONE_JOYPAD) x+=(dx+DEADZONE_JOYPAD)/8;
		if (dx>DEADZONE_JOYPAD) x+=(dx-DEADZONE_JOYPAD)/8;
		if (dz<-DEADZONE_JOYPAD) z+=(dz+DEADZONE_JOYPAD)/8;
		if (dz>DEADZONE_JOYPAD) z+=(dz-DEADZONE_JOYPAD)/8;

		if (x<0) x=0;
		if (y<0) y=0;
		if (x>640) x=640;
		if (y>480) y=480;
	
	
		set_source_extra_buffer(0); //for incoming antialiasing texture copy
		set_filters_on();
		set_transparency_off();	

		//now, we draw in usual rotating back buffer an AntiAliased copy
		//of our non-AntiAliased static extra frame buffer (we just drew in it)
		pb_target_back_buffer(); //set target for pixel rendering AND image bitblt
		if (antialiasing)
			antialiased_texture_copy();
		else
			image_bitblt(); //just do an hardware bitblt!
		//you may eventually draw here something (non anti-aliased HUD for example)
		
		//we declare this frame finished (next VBLank will swap screens automatically)
		while (pb_finished()) //no free buffer available, wait and retry later
		{
			//Should never happen if you use pb_wait_for_vbl for each frame
			//Otherwise, it gives an information : you are too much in advance
			//so, you can try drawing more details in each frame. Functions
			//pb_get_vbl_counter() can be used to survey your advance.
		}
	}

	if (vertices) MmFreeContiguousMemory((PVOID)vertices);

	return 0;
}





void XBoxStartup(void)
{
	int		i;

	XInput_Init();

	pb_extra_buffers(2);
	//we want 1 extra frame buffer (#0) that will be the non-AntiAliased back buffer
	//(an AntiAliased texture copy can be done from static non-AA back buffer towards AA rotating back buffer)
	//we want 1 extra frame buffer (#1) that will hold texture
	
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
			debugPrint("pb_init Error\n");
			XSleep(2000);
			XReboot();
			return;
	}

	prepare_shaders();

	//Because openxdk frame buffer is not allocated in memory, third frame buffer
	//overlaps openxdk frame buffer. To avoid graphic interference, skip upper lines.
	//Note that scrolling up will produce an interference, but won't prevent reading.
	for(i=0;i<22;i++) debugPrint("\n"); //if you use double font size, skip 11 only
	//need to call pb_show_debug_screen() to see it, revert with pb_show_front_buffer()
	debugPrint("Debug screen (Press BACK to come here or leave from here):\n\n");
	//debugPrint("Debug screen (Press X and B to turn antialiasing off/on):\n\n");

	pb_show_debug_screen();

	debugPrint("\nChoose the model :\n\n");
	debugPrint("A - SpaceFighter (500 faces, 1500 vertices)\n\n");
	debugPrint("B - Fuselage (30000 faces, 90000 vertices)\n\n");

	while(model==0)
	{
		XInput_GetEvents();
		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A]) model=1;
		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) model=2;
	}

	pb_show_front_screen();

	while(test());

	pb_kill();

	XInput_Quit();

	XReboot();
}
