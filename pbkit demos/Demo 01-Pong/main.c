//Demo 01-Pong (hardware accelerated)

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

if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) pb_show_debug_screen();
if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) pb_show_front_screen();

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
		dx=0;

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


	detect_pkt_init();
	detect_myip(); //change your IP in there (located in detect_pkt.c)

	//Because openxdk frame buffer is not allocated in memory, third frame buffer
	//overlaps openxdk frame buffer. To avoid graphic interference, skip upper lines.
	//Note that scrolling up will produce an interference, but won't prevent reading.
	for(i=0;i<22;i++) debugPrint("\n"); //if you use double font size, skip 11 only
	//need to call pb_show_debug_screen() to see it, revert with pb_show_front_buffer()
	debugPrint("Debug screen (Press X and B to come here or leave from here):\n\n");

	msex=-12; //inside deadzone of wiimote & joypad, if no data comes, won't count
	msey=-12; //inside deadzone of wiimote & joypad, if no data comes, won't count
	for(i=0;i<128;i++) keymap[i]=0;

	while(pong()==CONTINUE);

	detect_pkt_quit();

	pb_kill();

	XInput_Quit();

	XReboot();
}
