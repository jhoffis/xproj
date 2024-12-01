#include <stdio.h>
#include <string.h>

#include "pbKit.h"
#include "qbert.h"

#define ABS(x) ((x>0)?(x):(-(x)))
#define SGN(x) ((!(x))?(0):(  ((x>0)?(1):(-1))  ))
#define MAX(a,b) ((a>b)?(a):(b))
#define MIN(a,b) ((a<b)?(a):(b))


extern int slx[];
extern int sly[];

extern unsigned int pal[16];

char *Demostr="222222868686868686428442424268884248";

int antialiasing=0;

int freezetime=0;
int tmx,tmy,tmlx,tmly;
int P2[7][7];
int Ntt[7],Tt[7],C[4];
int Pp[14],G[7],D[7],P[14];
int Pl[7],Px[7],Py[7],Xt[7],Yt[7],M[7],Dx[7],Dy[7],Xd[7],Yd[7];
int Oldlv=1;
int Demotab[40][2];
int Lndemo;
int quit=0;
int Demo=1;
int Demon;

int tv,tv1,tv2,tv3,tv4;

int ii,jj,I,F,Lives,Lv,Rx,Ry,Ary,J,Z,Rd,W,OldI;
long Sc,Stl;
int Tm,Ft,X,Y,Ex,Ey,Cc,E;
int Dmx,Dmy,Nbec,Nc,Ncb,Npl,N,Xo,Yo;
int Pouf,Xpf,Ypf,Uggd,Uggg,Slick,Coily,Egg,Egg2,Cegg,Cegg2;
int Plt,Nt,Stand,Nm,Nom,Freeze,T,Dd,No,tmp,Nwtab;

char r;




void Sp(int No,int Xo,int Yo,int mirror,int n)
{
	Spr(Px[No]+Xo,Py[No]+Yo,mirror,n);
}


void Life(void)
{
	Spr(20,60,0,14+C[Ncb-1]);
	if (Lives!=0)
		for(J=1;J<Lives+1;J++)
			Spr(8+20*(J-1),72,1,1);
}

void afftext8(int x,int y,char *s)
{
	pb_printat(y,x,s);
}


void View(void)
{
	static int first_time=1;
	
	if (first_time==0)
	{
		//finish last frame:

		pb_draw_text_screen();
	
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
	}
		
	//we declare this frame finished (next VBLank will swap screens automatically)
	while (pb_finished()) //no free buffer available, wait and retry later
	{
		//Should never happen if you use pb_wait_for_vbl for each frame
		//Otherwise, it gives an information : you are too much in advance
		//so, you can try drawing more details in each frame. Functions
		//pb_get_vbl_counter() can be used to survey your advance.
	};
	
	
	//begin new frame :
	
	pb_wait_for_vbl(); //because we want strong sync with joypad/wiimote
	pb_reset(); //new frame, let's start from push buffer head again

	//we draw in non anti-aliased static extra frame buffer now
	pb_target_extra_buffer(0); //set target for pixel rendering AND image bitblt
	pb_erase_depth_stencil_buffer(0,0,640,480); //clear depth stencil buffer (MANDATORY)
	pb_fill(0,0,640,480,0x000000); //clear frame (optional, also, this call and previous one can be merged: flag=0xf3)
	pb_erase_text_screen();
	set_source_extra_buffer(1); //so we copy from extra buffer #1 where sprites are all drawn
	set_filters_off();
	set_transparency_on();
	
	first_time=0;
}


void Score(int bonus)
{
	char s[10];
	int i,x,y;
	
	if (T==0)
	{
		T= -1;
	}

	if (Demo)
		afftext8(10,0,"DEMO (PRESS A)");
	
	sprintf(s,"%07ld0",Sc);
	afftext8(0,0,s);
	
	if (Sc/Stl<(Sc+(long)bonus)/Stl)
	{
		Stl=Stl+2000L;
		if (Lives<3)
		{
			afftext8(14,0,"BONUS!");

			T=16;
			Lives++;
			Life();
		}
	}
	Sc=Sc+(long)bonus;
	if (Sc>9999999L)
		Sc=9999999L;


	if (Lv==10)
		sprintf(s,"LEVEL 10");
	else
		sprintf(s,"LEVEL  %d",Lv);
	afftext8(0,6,s);

	sprintf(s,"ROUND  %d",Rd);
	afftext8(0,7,s);
	
	
	//lifts
	
	updateliftcolor();
	
	for(i=0;i<7;i++)
	{
		if ((G[i] > -1)&&(G[i]!=Plt))
		{
			y=i;
			x= -1;
			Spr(-16+160+16*x-16*y,13+24+24*x+24*y,0,13);
		}
	}

	for(i=0;i<7;i++)
	{
		if ((D[i] > -1)&&(D[i]!=Plt))
		{
			y= -1;
			x=i;
  			Spr(160+16*x-16*y,13+24+24*x+24*y,0,13);
		}
	}
}




int randint(int n)
{
	static DWORD seed=0x1234;
		
	seed=seed*0x41C64E6D+0x3039;
	return (seed%n);
}



void Qbert2(void)
{
	if (Dy[0]==0) tv1= 1; else tv1=0;
	if (Xt[0]+Yt[0]==3) tv= -1; else tv=0;
	if (M[0]<23-tv)
		Sp(0,-8+Xd[0],6+Yd[0],tv1,4);
}

void Sam2(void)
{
	if (Dy[3]==0) tv1= 1; else tv1=0;
	if (Xt[3]+Yt[3]==3) tv= -1; else tv=0;
	if (M[3]<23-tv)
	{
		if (Slick)
			Sp(3,-8+Xd[3],6+Yd[3],tv1,31);
		else
			Sp(3,-8+Xd[3],4+Yd[3],tv1,8);
	}
}

void Coily2_2(void)
{
	if (Dy[2]==0) tv1= 1; else tv1=0;
	if (Xt[2]+Yt[2]==3) tv= -1; else tv=0;
	if (Egg==0 && M[2]<23-2*tv)
		Sp(2,-8+Xd[2],4+Yd[2],tv1,21);
}

void Coily2_1(void)
{
	if (Dy[1]==0) tv1= 1; else tv1=0;
	if (Xt[1]+Yt[1]==3) tv= -1; else tv=0;
	if (Egg2==0 && M[1]<23-2*tv && Coily)
		Sp(1,-8+Xd[1],4+Yd[1],tv1,21);
}

void Pos(int No)
{
	if (No==5 || No==6)
	{
		if (No==5)
		{
			Px[5]=264-Xt[5]*32-16*Yt[5];
			Py[5]=180-Yt[5]*24;
		}
		else
		{
			Px[6]=56+Yt[6]*32+16*Xt[6];
			Py[6]=180-Xt[6]*24;
		}
	}
	else
	{
		Px[No]=160+16*Xt[No]-16*Yt[No];
		Py[No]=24+24*Xt[No]+24*Yt[No];
	}
}

void Guide(int No,int Nk)
{
	Rx=SGN(Xt[Nk]-Xt[No]);
	Ry=SGN(Yt[Nk]-Yt[No]);
	if (Rx==0 && Ry==0)
	{
		Rx=Dx[Nk];
		Ry=Dy[Nk];
	}
	if (ABS(Xt[Nk]-Xt[No])<ABS(Yt[Nk]-Yt[No]) && Rx && Ry)
		Rx=0;
	if (ABS(Xt[Nk]-Xt[No])>ABS(Yt[Nk]-Yt[No]) && Rx && Ry)
		Ry=0;
	if (ABS(Xt[Nk]-Xt[No])==ABS(Yt[Nk]-Yt[No]) && Rx && Ry)
	{
	    if (Xt[No]+Yt[No]==6)
    	{
			if (Xt[No]+Yt[No]+Rx>6)
				Rx=0;
			else
				Ry=0;
		}
		else
		{
			if (Xt[No]<Yt[No])
				Rx=0;
			else
				Ry=0;
		}
	}
}

void Ugg_g(void)
{
	if (Uggg)
	{
		if ((M[6]>0 && M[6]<6) || (M[6]==0 && Yd[6]==0))
		{
			if (Dy[6]==0)
				Sp(6,-25+Xd[6],12+Yd[6],1,28);
			else
				Sp(6,-24+Xd[6],10+Yd[6],1,34);
		}
		else
		{
			if (Dy[6]==0)
				Sp(6,-25+Xd[6],13+Yd[6],1,25);
			else
				Sp(6,-24+Xd[6],10+Yd[6],1,35);
		}
	}
	else
	{
		if (M[6]==0)
		{
			if (Dy[6]==0)
				Sp(6,-25+Xd[6],18+Yd[6],1,29);
			else
				Sp(6,-27+Xd[6],14+Yd[6],1,27);
		}
		else
		{
			if (Dy[6]==0)
				Sp(6,-25+Xd[6],20+Yd[6],1,26);
			else
				Sp(6,-27+Xd[6],15+Yd[6],1,24);
		}
	}
}

void Ugg_d(void)
{
	if (Uggd)
	{
		if ((M[5]>0 && M[5]<6) || (M[5]==0 && Yd[5]==0))
		{
			if (Dx[5]==0)
				Sp(5,-7+Xd[5],12+Yd[5],0,28);
			else
				Sp(5,-8+Xd[5],10+Yd[5],0,34);
		}
		else
		{
			if (Dx[5]==0)
				Sp(5,-7+Xd[5],13+Yd[5],0,25);
			else
				Sp(5,-8+Xd[5],10+Yd[5],0,35);
		}
	}
	else
	{
		if (M[5]>0 && M[5]<6 || (M[5]==0 && Yd[5]==0))
		{
			if (Dx[5]==0)
				Sp(5,-7+Xd[5],18+Yd[5],0,29);
			else
				Sp(5,-5+Xd[5],14+Yd[5],0,27);
		}
		else
		{
			if (Dx[5]==0)
				Sp(5,-7+Xd[5],20+Yd[5],0,26);
			else
				Sp(5,-5+Xd[5],15+Yd[5],0,24);
		}
	}
}

void Red_ball(void)
{
	if (Coily)
	{
		if (Egg2)
			Sp(1,-8+Xd[1],3+Yd[1],0,11);
		else
		{
			if (M[1]<9 || Dx[1]+Dy[1]>-1)
			{
				if ((Dx[1]==1 || Dy[1]==1) && M[1]==0) tv1= 1; else tv1=0;
				if (M[1]>0) tv2= 1; else tv2=0;
				if (M[1]>0 && (Dx[1]==1 || Dy[1]==1)) tv3= 1; else tv3=0;
				if (Dy[1]==0) tv4= 1; else tv4=0;
				Sp(1,-8+Xd[1],4+Yd[1],tv4,6+tv1+15*tv2+tv3);
			}
		}
	}
	else
		Sp(1,-8+Xd[1],3+Yd[1],0,10);
}

void Sam(void)
{
	if (M[3]<9 || Dx[3]+Dy[3]>0)
	{
		if (Slick)
		{
			if ((Dx[3]== -1 || Dy[3]== -1) && M[3]==0) tv1= 1; else tv1=0;
			if (M[3]>0) tv2= 1; else tv2=0;
			if (M[3]>0 && (Dx[3]== -1 || Dy[3]== -1)) tv3= 1; else tv3=0;
			if (Dy[3]==0) tv4= 1; else tv4=0;
			Sp(3,-8+Xd[3],4-2*tv2+Yd[3],tv4,32+tv1-2*tv2+tv3);
		}
		else
		{
			if ((Dx[3]== 1 || Dy[3]== 1) && M[3]==0) tv1= 1; else tv1=0;
			if (M[3]>0) tv2= 1; else tv2=0;
			if (M[3]>0 && (Dx[3]== 1 || Dy[3]== 1)) tv3= 1; else tv3=0;
			if (Dy[3]==0) tv4= 1; else tv4=0;
			Sp(3,-8+Xd[3],4-2*tv2+Yd[3],tv4,2+tv1+6*tv2+tv3);
		}
	}
}

void Green_ball(void)
{
	Sp(4,-8+Xd[4],3+Yd[4],0,12);
}

void Coilyf(void)
{
	if (Egg)
		Sp(2,-8+Xd[2],3+Yd[2],0,11);
	else
	{
		if (M[2]<9 || Dx[2]+Dy[2]>-1)
		{
			if ((Dx[2]==1 || Dy[2]==1) && M[2]==0) tv1= 1; else tv1=0;
			if (M[2]>0) tv2= 1; else tv2=0;
			if (M[2]>0 && (Dx[2]==1 || Dy[2]==1)) tv3= 1; else tv3=0;
			if (Dy[2]==0) tv4= 1; else tv4=0;
			Sp(2,-8+Xd[2],4+Yd[2],tv4,6+tv1+15*tv2+tv3);
		}
	}
}

void Qbert(void)
{
	if (M[0]<9 || Dx[0]+Dy[0]>0)
	{
		if ((Dx[0]==1 || Dy[0]==1) && M[0]==0) tv1= 1; else tv1=0;
		if (M[0]>0) tv2= 1; else tv2=0;
		if (M[0]>0 && (Dx[0]==1 || Dy[0]==1)) tv3= 1; else tv3=0;
		if (Dy[0]==0) tv4= 1; else tv4=0;
		Sp(0,-8+Xd[0],4-2*tv2+Yd[0],tv4,tv1+4*tv2+tv3);
	}
}

void Killed(int Nk)
{
	if (Ntt[Nk]!=Nbec)
	{
		for(J=Ntt[Nk];J<Nbec;J++)
		{
			Tt[J]=Tt[J+1];
			Ntt[Tt[J]]--;
		}
	}
	Nbec--;
	Xpf=Px[Nk]+Xd[Nk]-8;
	Ypf=Py[Nk]+Yd[Nk]+4;
	Xt[Nk]=0;
	Yt[Nk]=0;
	M[Nk]= -1;
	if (Nk!=0) M[Nk]= -99;
	Xd[Nk]=0;
	Yd[Nk]= -40;
	if (Nk==5 || Nk==6)
	{
		if (Nk==5)
			Xd[5]=40;
		else
			Xd[6]= -40;
		Yd[Nk]=20;
	}
	Dy[Nk]=1;
	Dx[Nk]=0;
	Pouf=0;
	if (Nk==3 && M[0]>-1)
		Score(30);
	if (Nk==2)
		Cegg=48-Nt;
	if (Nk==1)
		Cegg2=48-Nt;
	if (((Nk==2 && Egg==0) || (Nk==1 && Egg2==0 && Coily)) && M[0]>-1)
		Score(50);
	if (Nk==0)
	{
		Stand=0;
		Freeze=0;
	}
	Pos(Nk);
}





void Flip(void)
{
	if ((I==0 && C[1+P2[Xt[I]][Yt[I]]]!=16) || 
		(I==3 && C[P2[Xt[I]][Yt[I]]]!=C[0]))
	{
		Z=P2[Xt[I]][Yt[I]];
		Z++;
		if (I==3)
		{
			if (C[Z]==17 || C[Z]==16)
				Nc--;
			Z=0;
		}
		if (C[Z]==17)
		{
			Z=0;
			Nc--;
		}
		if (C[Z+1]>15)
			Nc++;
		P2[Xt[I]][Yt[I]]=Z;
		
		if (Z!=0)
			Score(1);
		if (Nc==28)
			W=2;
	}
}





void Afftime(void)
{
	char s[10];

	sprintf(s,"TIME %02d",Tm/10);	
	afftext8(13,1,s);
}











