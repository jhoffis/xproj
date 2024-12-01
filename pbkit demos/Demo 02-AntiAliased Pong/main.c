//Demo 02-AntiAliased Pong

#include <hal/input.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include <openxdk/debug.h>

#include "pbKit.h"
#include "outer.h"

#include "detect_pkt.h"
#include "inp2eth_keys.h"

#include "string.h"
#include "stdio.h"
#include <stdlib.h>



//under that displacement, a stick will be considered in neutral position
#define DEADZONE_JOYPAD		16

#define DEADZONE_WIIMOTE_X	16
#define DEADZONE_WIIMOTE_Z_UP	-20
#define DEADZONE_WIIMOTE_Z_DOWN	-4

#define ABSOLUTE_WIIMOTE	0
#define ABSOLUTE_JOYPAD		1
#define RELATIVE		2

#define QUIT			0
#define POWEROFF		1	//unused, dunno how to poweroff xb1...
#define CONTINUE		2

//These are the variables detect_pkt.c module will update (eventually)
//with keys states and mouse relative position (from center of screen)
char		msex,msey;
char		keymap[128]; //see index constants in inp2eth_keys.h

void image_bitblt(void)
{
	int	BytesPerPixel=4;
	DWORD	EncodedBpp=0xa;
	DWORD	SrcPitch,DstPitch;
	DWORD	SrcAddr,DstAddr;
	DWORD	sx,sy,dx,dy,h,w;
	DWORD	*p;
	
	SrcPitch=DstPitch=pb_back_buffer_pitch();
	SrcAddr=(DWORD)pb_extra_buffer(0);
	DstAddr=0; //base dma addr=last targetted buffer addr
	
	sx=sy=dx=dy=0;
	w=pb_back_buffer_width();
	h=pb_back_buffer_height();
	
	switch(BytesPerPixel)
	{
		case 1:
			EncodedBpp=1;
			break;
		case 2:
			EncodedBpp=4;
			break;
		case 4:
			EncodedBpp=0xa;
			break;
	}

	p=pb_begin();
	pb_push4to(	SUBCH_4,
			p,			
			NV_IMAGE_BLIT_POINT_IN,
			EncodedBpp,
			(DstPitch<<16)|(SrcPitch&0xffff),
			SrcAddr,
			DstAddr	);
	p+=5;
	pb_end(p);

	p=pb_begin();
	pb_push3to(	SUBCH_3,
			p,			
			NV_IMAGE_BLIT_POINT_IN,
			(sx&0xffff)|(sy<<16),
			(dx&0xffff)|(dy<<16),
			(w&0xffff)|(h<<16)	);
	p+=4;
	pb_end(p);
}

void antialiased_texture_copy(void)
{
	float xScale  = 2.0f;
	float yScale  = 2.0f;
	float fLeft   = (float)0;
	float fTop    = (float)0;
	float fRight  = (float)2*pb_back_buffer_width(); //we draw 1 triangle that covers all screen (will be clipped)
	float fBottom = (float)2*pb_back_buffer_height();
	float uAdjust = 0.0f;
	float vAdjust = 0.0f;

	DWORD *p;
	
	int i;
	
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BEGIN_END,TRIANGLES); p+=2; //triangle list (beginning of list)
	pb_push(p++,0x40000000|NV20_TCL_PRIMITIVE_3D_VERTEX_DATA,12); //bit 30 means all params go to same register 0x1818
	// Vertex 0
	*((float *)(p++))=fLeft;
	*((float *)(p++))=fTop;
	*((float *)(p++))=uAdjust;
	*((float *)(p++))=vAdjust;
	// Vertex 1
	*((float *)(p++))=fRight;
	*((float *)(p++))=fTop;
	*((float *)(p++))=uAdjust + pb_back_buffer_width()*xScale;
	*((float *)(p++))=vAdjust;
	// Vertex 2
	*((float *)(p++))=fLeft;
	*((float *)(p++))=fBottom;
	*((float *)(p++))=uAdjust;
	*((float *)(p++))=vAdjust + pb_back_buffer_height()*yScale;
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=STOP; //triggers the drawing (end of list)
	pb_end(p);
}

void prepare_anti_aliasing(void)
{
	DWORD			*p;
	int			i,j;

	//Here, we get everything ready for 1 fast texture copy (per frame)
	//from static extra frame buffer, into rotating back buffer.
	//Strong anti-aliasing filters are setup for the texture copy
	//(gaussian cubic)

	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_CULL_FACE_ENABLE,0); p+=2;//CullModeEnable=FALSE
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_DITHER_ENABLE,1); p+=2; //DitherEnable=TRUE
	pb_push2(p,NV20_TCL_PRIMITIVE_3D_POINT_PARAMETERS_ENABLE,0,0); p+=3; //PointScaleEnable=FALSE & PointSpriteEnable=FALSE
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_POINT_SIZE,8); p+=2; //PointSize=1.0f
	pb_end(p);


	//Draws some broad ugly diagonals in texture in order to detect the anti-aliasing effect
	//p=pb_extra_buffer(0);
	//for(i=0;i<256;i++) for(j=0;j<256;j++) *(p+i*256+j)=((((i&~1)+(j&~1))&16)*15)*256+((((j&~1)-(i&~1))&16)*15)*65536+255;

	//attach extra buffer memory area to texture stage 0 (used in pixel shader)
	//extra buffers and back buffers have same sizes and pitches
	p=pb_begin();
	pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),((DWORD)pb_extra_buffer(0))&0x03FFFFFF,0x0001122a); p+=3; //set stage 0 texture address & format
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),pb_back_buffer_pitch()<<16); p+=2; //set stage 0 texture pitch (pitch<<16)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(pb_back_buffer_width()<<16)|pb_back_buffer_height()); p+=2; //set stage 0 texture width & height ((witdh<<16)|height)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00030303); p+=2;//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); p+=2; //set stage 0 texture enable flags
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x04074000); p+=2; //set stage 0 texture filters (AA!)
	pb_end(p);

	//Note that 0x02022000 (for TX_FILTER) will disable anti-aliasing.
	//For 2D games, use this non-AA value for 2D direct texture fast copy using same technic
	//or copy with CPU using an assembler routine optimized with MMX insructions (very efficient)

	//EncodedFormat=(log2depth<<28)| 	//log2 sizes used if sizes are power of 2 (non swizzled)
	//		(log2height<<24)|
	//		(log2width<<20)|
	//		(MipMapLevels<<16)|
	//		(Format<<8)|
	//		((2+Bit4Flag)<<4)|	//meaning of bit 5 & 4 is unknown
	//		((2+CubeMapFlag)<<2)|	//meaning of bit 3 is unknown
	//		(1+Bit0Flag);		//meaning of bit 0 & 1 is unknown

	//EncodedFilters=	(((DWORD)bias)&0x1FFF)|		//bits 12-0
	//			(MagFilter<<24)|		//bits 26-24 4=quincunx
	//			MipMinFilter|			//bits 18-16 7\_gaussian cubic (1 pixel=mean value of 3x3 pixel grid around it)
	//			FiltersType|			//bits 14-13 4/
	//			ColorsSigns;			//bits 31-28
	//for lesser filters: if minfilter=point (mipfilter: 1=none 3=point 5=linear) else (mipfilter: 2=none 4=point 6=linear)
	
	//neutralize other texture stages:
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1),0x0003ffc0); p+=2;//set stage 1 texture enable flags (bit30 disabled)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2),0x0003ffc0); p+=2;//set stage 2 texture enable flags (bit30 disabled)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3),0x0003ffc0); p+=2;//set stage 3 texture enable flags (bit30 disabled)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),0x00030303); p+=2;//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),0x00030303); p+=2;//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),0x00030303); p+=2;//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1),0x02022000); p+=2;//set stage 1 texture filters (no AA, stage not even used)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2),0x02022000); p+=2;//set stage 2 texture filters (no AA, stage not even used)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3),0x02022000); p+=2;//set stage 3 texture filters (no AA, stage not even used)
	pb_end(p);
	
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_LIGHTING_ENABLE,0); p+=2; //(lighting related)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_SEPARATE_SPECULAR_ENABLE,0); p+=2; //(lighting related)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_LIGHT_CONTROL,0x00020001); p+=2; //(specular flags=0x0002)
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_LIGHT_MODEL_TWO_SIDE_ENABLE,0); p+=2; //TwoSidedLighting=FALSE
	pb_end(p);

	//fog (untested)
	//p=pb_begin();
	//pb_push1(p,NV20_TCL_PRIMITIVE_3D_FOG_ENABLE,1); p+=2; //FogEnable=TRUE
	//pb_push1(p,NV20_TCL_PRIMITIVE_3D_FOG_COORD_DIST,0); p+=2; //FogRange (2=enable, 1=disable)
	//pb_push1(p,NV20_TCL_PRIMITIVE_3D_FOG_MODE,0x2601); p+=2; //FogTable (0x2601=linear 0x800=exp 0x801=exp2)
	//pb_push3(p,NV20_TCL_PRIMITIVE_3D_FOG_EQUATION_CONSTANT,1+e*s,-s,0); p+=4; //FogTableParameters max(scale)=8192.0f scale=1/(end-beginning)
	//pb_push2(p,NV20_TCL_PRIMITIVE_3D_RC_FINAL0,0x130C0300,0x00001c80); p+=3;//PSFinalCombinerIn=ABCD,EFG
	//[NV20_TCL_PRIMITIVE_3D_RC_FINAL0]=0x130E0300 if specular is activated
	//pb_end(p);
	
	//No fog
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_FOG_ENABLE,0); p+=2; //FogEnable=FALSE
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_FOG_COORD_DIST,0); p+=2; //FogRange=disable fog
	pb_push2(p,NV20_TCL_PRIMITIVE_3D_RC_FINAL0,0x0000000c,0x00001c80); p+=3;//PSFinalCombinerIn=abcd,efg
	//[NV20_TCL_PRIMITIVE_3D_RC_FINAL0]=0x0000000e if specular is activated
	pb_end(p);

	//Vertex and pixel shaders initialization:
	//Try to update pixel shaders manually if you change it 
	//often: it's much faster than using function pb_push_mcode()
	//because most of ps registers won't change over time.
	//Also, for pixel shaders that need more than 1 stage
	//pb_pcode2mcode won't help you. You have to set it up manually.
	//(not sure it's even worthy to improve pb_pcode2mcode for ps)
	//On the other hand pb_pcode2mcode is complete for vertex shaders.

	//To get an idea about what constants you need
	//and which vertex shader registers you should target
	//take a look at intermediary assembler file vs.psh

	//set vertex shader constants
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID,96); p+=2; //set cursor in order to load data into C0 quaternion (and following ones) (0=C-96)
	pb_push4f(p,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,1.0f,1.0f,1.0f,1.0f); p+=5; //loads constant C0=(Xscale,YScale,Zscale,Wscale) into GPU
	pb_push4f(p,NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X,0,0,0,0); p+=5; //loads constant C1=(Xbias,Ybias,Zbias,Wbias) into GPU
	pb_end(p);

	//sets vertex feed configuration
	p=pb_begin();
	pb_push(p++,NV20_TCL_PRIMITIVE_3D_VERTEX_ATTR(0),16);
	for(i=0;i<16;i++) *(p++)=2;	//resets array (nothing goes to v0-v15)
	pb_end(p);
	//declares that 2 floats will go into v0 (2D position)
	//declares that 2 floats will go into v7 (2D texture coordinates)
	//(check your intermediate assembler file .vsh to see what ATTR:n is expected)
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_VERTEX_ATTR(0),0x22); p+=2;
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_VERTEX_ATTR(7),0x22); p+=2;
	pb_end(p);

	//read vs.cg for hints about how to create vs.h
	#include "vs.h"
	p=pb_begin();
	p=pb_push_mcode(p,pb_pcode2mcode(g_vs11_main)); //complete! (but encodes separate instructions)
	pb_end(p);

	//read ps.cg for hints about how to create ps.h
	#include "ps.h"
	p=pb_begin();
	p=pb_push_mcode(p,pb_pcode2mcode(g_ps11_main)); //incomplete (just sets 1 stage r0=a*b+c*d with 'tex t0' supported)
	pb_end(p);
}


int pong(void)
{
	int			x,y,w,h,dx,dy,oldx,oldy;
	int			x1,y1,w1,h1,dx1,dy1,oldx1,oldy1;
	int			x2,y2,w2,h2,dx2,dy2,oldx2,oldy2;
	int			xmina,xmaxa,ymina,ymaxa;
	int			xminb,xmaxb,yminb,ymaxb;
	int 			i, power_off, quit;
	int			players,mode;
	int			score1,score2,scorey;
	int			random_side;
	int			needs_to_be_released;
	int			blink,right_goal,left_goal;
	int			max_score,speedup_soon;
	int			antialiasing=1;

	max_score=10;
	blink=0;
	score1=0;
	score2=0;

	needs_to_be_released=1;

	power_off=0;
	quit=0;

	players=1;
	mode=RELATIVE;

	w=20;
	h=20;
	x=640/2-w/2;
	y=480/2-h/2;

	//menu loop
	while(1)
	{
		pb_wait_for_vbl(); //because we want strong sync with joypad/wiimote

		pb_reset(); //new frame, let's start from push buffer head again

		//we draw in non anti-aliased static extra frame buffer now
		pb_target_extra_buffer(0); //set target for pixel rendering AND image bitblt

		pb_erase_depth_stencil_buffer(0,0,640,480); //clear depth stencil buffer (MANDATORY)



		pb_fill(0,0,640,480,0x000000); //clear frame (optional)

		pb_erase_text_screen();

		pb_print("                            PONG\n"); 
		//pb_print("                A gift for cute <girlfriend name>"); // ;)
		pb_print("\n");

		y+=h/2;

		if ((3*25<=y)&&(y<=4*25))
			pb_print("   < 1 Player  with Joypad              (absolute mode) >\n\n");
		else
			pb_print("     1 Player  with Joypad              (absolute mode)  \n\n");

		if ((5*25<=y)&&(y<=6*25))
			pb_print("   < 1 Player  with Wiimote             (absolute mode) >\n\n");
		else
			pb_print("     1 Player  with Wiimote             (absolute mode)  \n\n");

		if ((7*25<=y)&&(y<=8*25))
			pb_print("   < 1 Player  with Joypad  or Wiimote  (relative mode) >\n\n");
		else
			pb_print("     1 Player  with Joypad  or Wiimote  (relative mode)  \n\n");

		if ((9*25<=y)&&(y<=10*25))
			pb_print("   < 2 Players with Joypads or Wiimotes (relative mode) >\n\n");
		else
			pb_print("     2 Players with Joypads or Wiimotes (relative mode)  \n\n");

		if ((11*25<=y)&&(y<=12*25))
			pb_print("   < 2 Players with Wiimotes            (absolute mode) >\n\n");
		else
			pb_print("     2 Players with Wiimotes            (absolute mode)  \n\n");

		if ((13*25<=y)&&(y<=14*25))
			pb_print("   < 2 Players with Joypads             (absolute mode) >\n\n");
		else
			pb_print("     2 Players with Joypads             (absolute mode)  \n\n");

		y-=h/2;

		pb_print("    Use right stick and press A to select or Y to quit\n");
		pb_print("         or wiimote and press B to select entry");

		pb_draw_text_screen();

		pb_fill(x,y,w,h,0x000000);
		pb_fill(x+1,y+1,w-2,h-2,0xFFFFFF);

		detect_pkt(keymap,&msex,&msey);

		//joypad(s) states reading
		XInput_GetEvents();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) antialiasing=0;
		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) antialiasing=1;

		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) pb_show_debug_screen();
		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) pb_show_front_screen();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_Y]) return QUIT;

		if (((g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A])||(keymap[WIIMOTE1_B]))&&(needs_to_be_released==0))
		{
			y+=h/2;

			if ((3*25<=y)&&(y<=4*25))
			{
				players=1;
				mode=ABSOLUTE_JOYPAD;
				break;
			}

			if ((5*25<=y)&&(y<=6*25))
			{
				players=1;
				mode=ABSOLUTE_WIIMOTE;
				break;
			}

			if ((7*25<=y)&&(y<=8*25))
			{
				players=1;
				mode=RELATIVE;
				break;
			}

			if ((9*25<=y)&&(y<=10*25))
			{
				players=2;
				mode=RELATIVE;
				break;
			}

			if ((11*25<=y)&&(y<=12*25))
			{
				players=2;
				mode=ABSOLUTE_WIIMOTE;
				break;
			}

			if ((13*25<=y)&&(y<=14*25))
			{
				players=2;
				mode=ABSOLUTE_JOYPAD;
				break;
			}

			y-=h/2;

			needs_to_be_released=1;
		}

		if (((g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A])==0)&&(keymap[WIIMOTE1_B]==0)) needs_to_be_released=0;

		dy=-g_Pads[0].sRThumbY/256;

		if (dy<-DEADZONE_JOYPAD) y+=(dy+DEADZONE_JOYPAD)/8;
		if (dy>DEADZONE_JOYPAD) y+=(dy-DEADZONE_JOYPAD)/8;
		if (dx<-DEADZONE_JOYPAD) x+=(dx+DEADZONE_JOYPAD)/8;
		if (dx>DEADZONE_JOYPAD) x+=(dx-DEADZONE_JOYPAD)/8;

		//relative wiimote displacement (works slower but gets rid of noise well)
		if (msey<DEADZONE_WIIMOTE_Z_UP) y+=(msey-DEADZONE_WIIMOTE_Z_UP)/2;
		if (msey>DEADZONE_WIIMOTE_Z_DOWN) y+=(msey-DEADZONE_WIIMOTE_Z_DOWN)/2;

		if (x<0) x=0;
		if (y<0) y=0;
		if (x>640-w) x=640-w;
		if (y>480-h) y=480-h;

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
		};
	}

	//wait until Joypad A is released and Wiimote B is released (or never pressed)
	do { XInput_GetEvents(); detect_pkt(keymap,&msex,&msey); } while ((g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A])||(keymap[WIIMOTE1_B]));

	random_side=y&1;

	//puck and players
	w=20;
	h=20;
	x=640/2-w/2;
	y=480/2-h/2;
	oldx=x;
	oldy=y;

	w1=20;
	h1=100;
	x1=100-w1/2;
	y1=480/2-h1/2;
	oldx1=x1;
	oldy1=y1;

	w2=20;
	h2=100;
	x2=540-w2/2;
	y2=480/2-h2/2;
	oldx2=x2;
	oldy2=y2;

	speedup_soon=0;

	if (random_side)
	{
		//for plasma, better avoid graphics always staying at same place
		scorey=450;
		dx=2;
		dy=2;
	}
	else
	{
		scorey=30;
		dx=-2;
		dy=2;
	}

	//game loop
	while(1)
	{
		blink=pb_wait_for_vbl(); //because we want strong sync with joypad/wiimote

		pb_reset(); //new frame, let's start from push buffer head again

		//we draw in non anti-aliased static extra frame buffer now
		pb_target_extra_buffer(0); //set target for pixel rendering AND image bitblt
		

		pb_erase_depth_stencil_buffer(0,0,640,480); //clear depth stencil buffer (MANDATORY)

		pb_fill(0,0,640,480,0x000000); //clear frame (optional)

		//display scores
		for(i=0;i<max_score;i++)
		{
			if ((score1>i)&&((blink&32)||(score1<max_score)))
				pb_fill(640/2-10-(i+2)*25,scorey-10,20,20,0xFFFFFF);
			else
				pb_fill(640/2-5-(i+2)*25,scorey-5,10,10,0xFFFFFF);

			if ((score2>i)&&((blink&32)||(score2<max_score)))
				pb_fill(640/2-10+(i+2)*25,scorey-10,20,20,0xFFFFFF);
			else
				pb_fill(640/2-5+(i+2)*25,scorey-5,10,10,0xFFFFFF);
		}

		pb_fill(x,y,w,h,0xFFFFFF);
		pb_fill(x1,y1,w1,h1,0xFFFFFF);
		pb_fill(x2,y2,w2,h2,0xFFFFFF);

		detect_pkt(keymap,&msex,&msey);

		//joypad(s) states reading
		XInput_GetEvents();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) antialiasing=0;
		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) antialiasing=1;

		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) pb_show_debug_screen();
		//if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) pb_show_front_screen();

		if ((score1==max_score)||(score2==max_score))
			if ((g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A])||(keymap[WIIMOTE1_B])) break;

		dx1=g_Pads[0].sRThumbX/256;
		dy1=-g_Pads[0].sRThumbY/256;

		if (mode==RELATIVE)
		{
			if (dx1<-DEADZONE_JOYPAD) x1+=(dx1+DEADZONE_JOYPAD)/8;
			if (dx1>DEADZONE_JOYPAD) x1+=(dx1-DEADZONE_JOYPAD)/8;
			if (dy1<-DEADZONE_JOYPAD) y1+=(dy1+DEADZONE_JOYPAD)/8;
			if (dy1>DEADZONE_JOYPAD) y1+=(dy1-DEADZONE_JOYPAD)/8;
		}

		if (mode==ABSOLUTE_JOYPAD)
		{
			x1=100-w1/2+dx1*50/256;
			y1=480/2-h1/2+dy1*480/256;
		}

		if (players==1)
		{
			//AI!
			if (x>640/2)
				dy2=(y+w/2)-(y2+w2/2);
			else
				dy2=(480/2)-(y2+w2/2);
			dy2-=(3*dy2/4);
			y2+=dy2;
		}
		else
		{
			dx2=g_Pads[1].sRThumbX/256;
			dy2=-g_Pads[1].sRThumbY/256;

			if (mode==RELATIVE)
			{
				if (dx2<-DEADZONE_JOYPAD) x2+=(dx2+DEADZONE_JOYPAD)/8;
				if (dx2>DEADZONE_JOYPAD) x2+=(dx2-DEADZONE_JOYPAD)/8;
				if (dy2<-DEADZONE_JOYPAD) y2+=(dy2+DEADZONE_JOYPAD)/8;
				if (dy2>DEADZONE_JOYPAD) y2+=(dy2-DEADZONE_JOYPAD)/8;
			}

			if (mode==ABSOLUTE_JOYPAD)
			{
				x2=540-w2/2+dx2*50/256;
				y2=480/2-h2/2+dy2*480/256;
			}
		}

		//relative wiimote displacement (works slower but gets rid of noise well)
		if (mode==RELATIVE)
		{
			if (players==2)
			{
				if (keymap[WIIMOTE1_TURNLEFT]) x1-=2;
				if (keymap[WIIMOTE1_TURNRIGHT]) x1+=2;
				if (keymap[WIIMOTE2_TURNLEFT]) x2-=2;
				if (keymap[WIIMOTE2_TURNRIGHT]) x2+=2;
				if (msex<DEADZONE_WIIMOTE_Z_UP) y2+=(msex-DEADZONE_WIIMOTE_Z_UP)/2;
				if (msex>DEADZONE_WIIMOTE_Z_DOWN) y2+=(msex-DEADZONE_WIIMOTE_Z_DOWN)/2;
				if (msey<DEADZONE_WIIMOTE_Z_UP) y1+=(msey-DEADZONE_WIIMOTE_Z_UP)/2;
				if (msey>DEADZONE_WIIMOTE_Z_DOWN) y1+=(msey-DEADZONE_WIIMOTE_Z_DOWN)/2;
			}
			else
			{
				if (msex<-DEADZONE_WIIMOTE_X) x1+=(msex+DEADZONE_WIIMOTE_X)/8;
				if (msex>DEADZONE_WIIMOTE_X) x1+=(msex-DEADZONE_WIIMOTE_X)/8;
				if (msey<DEADZONE_WIIMOTE_Z_UP) y1+=(msey-DEADZONE_WIIMOTE_Z_UP)/2;
				if (msey>DEADZONE_WIIMOTE_Z_DOWN) y1+=(msey-DEADZONE_WIIMOTE_Z_DOWN)/2;
			}
		}

		//may suffer from noise but is instant positioning (unless mse2eth is used)
		if (mode==ABSOLUTE_WIIMOTE)
		{
			if (players==2)
			{
				//applying minimal numeric filtering
				//in order to attenuate noise (a bit)
				static int lastx=0,lasty=0;
				int filteredx,filteredy,newx,newy;

				if (keymap[WIIMOTE1_TURNLEFT]) x1=100-w/2-20;
				if (keymap[WIIMOTE1_TURNRIGHT]) x1=100-w/2+20;
				if (keymap[WIIMOTE2_TURNLEFT]) x2=540-w/2-20;
				if (keymap[WIIMOTE2_TURNRIGHT]) x2=540-w/2+20;

				newx=msex;
				newy=msey;
				filteredx=(lastx+newx)/2;
				filteredy=(lasty+newy)/2;
				y2=480/2-h/2+(filteredx+8)*6; //msex is player2's y
				y1=480/2-h/2+(filteredy+8)*6;
				lastx=filteredx;
				lasty=filteredy;		
			}
			else
			{
				//applying minimal numeric filtering
				//in order to attenuate noise (a bit)
				static int lastx=0,lasty=0;
				int filteredx,filteredy,newx,newy;

				newx=msex;
				newy=msey;
				filteredx=(lastx+newx)/2;
				filteredy=(lasty+newy)/2;
				x1=100-w/2+filteredx/2;
				y1=480/2-h/2+(filteredy+8)*6;
				lastx=filteredx;
				lasty=filteredy;		
			}
		}

		//move puck
		x+=dx;
		y+=dy;
		
		//any collision?

		//A=puck area (past puck to present puck)
		xmina=x;		
		xmaxa=x+w;		
		ymina=y;		
		ymaxa=y+h;		
		if (oldx<xmina) xmina=oldx;		
		if (oldx+w>xmaxa) xmaxa=oldx+w;		
		if (oldy<ymina) ymina=oldy;		
		if (oldy+h>ymaxa) ymaxa=oldy+h;
		
		if (dx<0) //only if puck is going to goal 
		{
			//B=player1 (past player1 to present player1)
			xminb=x1;		
			xmaxb=x1+w1;		
			yminb=y1;		
			ymaxb=y1+h1;		
			if (oldx1<xminb) xminb=oldx1;		
			if (oldx1+w1>xmaxb) xmaxb=oldx1+w1;		
			if (oldy1<yminb) yminb=oldy1;		
			if (oldy1+h1>ymaxb) ymaxb=oldy1+h1;

			//B=intersection(A,B)
			if (xminb<xmina) xminb=xmina;
			if (xmaxb>xmaxa) xmaxb=xmaxa;
			if (yminb<ymina) yminb=ymina;
			if (ymaxb>ymaxa) ymaxb=ymaxa;
	
			//B not empty => player1 hits puck
			if ((xminb<xmaxb)&&(yminb<ymaxb))
			{
				dx=-dx;
				if (x<x1+w1) x=2*(x1+w1)-x; 

				if (speedup_soon) speedup_soon++;
				if (speedup_soon==3)
				{
					dx++;
					if (dy<0) dy=dx; else dy=-dx;
					speedup_soon=0;
				}

				//has player1 tried to slow down puck?
				if ((oldx1-x1>1)&&(dx>=3)) dx-=2; //slow down puck

				//has player1 tried to accelerate puck?
				if ((x1-oldx1>1)&&(dx<w)) dx++; //accelerate puck

				//vertical move?
				if ((oldy1-y1>2)&&(dy>-w)&&(dy<0)) dy--;
				if ((y1-oldy1>2)&&(dy<w)&&(dy>0)) dy++;
			}
		}

		if (dx>0) //only if puck is going to goal 
		{
			//B=player2 (past player2 to present player2)
			xminb=x2;		
			xmaxb=x2+w2;		
			yminb=y2;		
			ymaxb=y2+h2;		
			if (oldx2<xminb) xminb=oldx2;		
			if (oldx2+w2>xmaxb) xmaxb=oldx2+w2;		
			if (oldy2<yminb) yminb=oldy2;		
			if (oldy2+h2>ymaxb) ymaxb=oldy2+h2;		

			//B=intersection(A,B)
			if (xminb<xmina) xminb=xmina;
			if (xmaxb>xmaxa) xmaxb=xmaxa;
			if (yminb<ymina) yminb=ymina;
			if (ymaxb>ymaxa) ymaxb=ymaxa;
	
			//B not empty => player2 hits puck
			if ((xminb<xmaxb)&&(yminb<ymaxb))
			{
				dx=-dx;
				if (x>x2-w) x=2*(x2-w)-x; 

				if (speedup_soon) speedup_soon++;
				if (speedup_soon==3)
				{
					dx--;
					if (dy<0) dy=dx; else dy=-dx;
					speedup_soon=0;
				}

				//has player2 tried to slow down puck?
				if ((x2-oldx2>1)&&(dx<=-3)) dx+=2; //slow down puck

				//has player2 tried to accelerate puck?
				if ((oldx2-x2>1)&&(dx>-w)) dx--; //accelerate puck

				//vertical move?
				if ((oldy2-y2>2)&&(dy>-w)&&(dy<0)) dy--;
				if ((y2-oldy2>2)&&(dy<w)&&(dy>0)) dy++;
			}
		}

		//puck speed limit
		if (dx>0) if (dx>w) dx=w;
		if (dx<0) if (dx<-w) dx=-w;
		if (dy>0) if (dy>w) dy=w;
		if (dy<0) if (dy<-w) dy=-w;

		//puck hits any wall?
		left_goal=0;
		right_goal=0;
		if (x<0) { x=-x; dx=-dx; left_goal=1; }
		if (y<0) { y=-y; dy=-dy; }
		if (x>640-w) { x=2*(640-w)-x; dx=-dx; right_goal=1; }
		if (y>480-h) { y=2*(480-h)-y; dy=-dy; }

		//keep player1 inside limits
		if (x1<60) x1=60;
		if (y1<0) y1=0;
		if (x1>140-w1) x1=140-w1;
		if (y1>480-h1) y1=480-h1;

		//keep player2 inside limits
		if (x2<500) x2=500;
		if (y2<0) y2=0;
		if (x2>600-w2) x2=600-w2;
		if (y2>480-h2) y2=480-h2;

		oldx=x;
		oldy=y;
		oldx1=x1;
		oldy1=y1;
		oldx2=x2;
		oldy2=y2;

		if ((left_goal)&&(score1<max_score)&&(score2<max_score)) score2++;
		if ((right_goal)&&(score1<max_score)&&(score2<max_score)) score1++;

		if ((left_goal)||(right_goal)) speedup_soon=1;

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
		};
	}

	return CONTINUE;
}




void XBoxStartup(void)
{
	int		i;

	XInput_Init();

	pb_extra_buffers(1);
	//we want 1 extra frame buffer that will be the non-AntiAliased back buffer
	//(an AntiAliased texture copy will be done from static non-AA back buffer towards AA rotating back buffer)
	
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

	prepare_anti_aliasing();

	detect_pkt_init();
	detect_myip(); //change your IP in there (located in detect_pkt.c)

	//Because openxdk frame buffer is not allocated in memory, third frame buffer
	//overlaps openxdk frame buffer. To avoid graphic interference, skip upper lines.
	//Note that scrolling up will produce an interference, but won't prevent reading.
	for(i=0;i<22;i++) debugPrint("\n"); //if you use double font size, skip 11 only
	//need to call pb_show_debug_screen() to see it, revert with pb_show_front_buffer()
	//debugPrint("Debug screen (Press X and B to come here or leave from here):\n\n");
	debugPrint("Debug screen (Press X and B to turn antialiasing off/on):\n\n");
	
	msex=-12; //inside deadzone of wiimote & joypad, if no data comes, won't count
	msey=-12; //inside deadzone of wiimote & joypad, if no data comes, won't count
	for(i=0;i<128;i++) keymap[i]=0;

	while(pong()==CONTINUE);

	detect_pkt_quit();

	pb_kill();

	XInput_Quit();

	XReboot();
}
