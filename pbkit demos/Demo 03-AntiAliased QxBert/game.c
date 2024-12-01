#include <stdio.h>
#include <string.h>

#include <hal/input.h>
#include <hal/xbox.h>
#include <openxdk/debug.h>

#include "pbKit.h"
#include "qbert.h"

//under that displacement, a stick will be considered in neutral position
#define DEADZONE_JOYPAD		16


#define ABS(x) ((x>0)?(x):(-(x)))
#define SGN(x) ((!(x))?(0):(  ((x>0)?(1):(-1))  ))
#define MAX(a,b) ((a>b)?(a):(b))
#define MIN(a,b) ((a<b)?(a):(b))


extern char *Demostr;

extern int antialiasing;

extern int freezetime;
extern int tmx,tmy,tmlx,tmly;
extern int P2[7][7];
extern int Ntt[7],Tt[7],C[4];
extern int Pp[14],G[7],D[7],P[14];
extern int Pl[7],Px[7],Py[7],Xt[7],Yt[7],M[7],Dx[7],Dy[7],Xd[7],Yd[7];
extern int Oldlv;
extern int Demotab[40][2];
extern int Lndemo;
extern int quit;
extern int Demo;
extern int Demon;

extern int tv,tv1,tv2,tv3,tv4;

extern int ii,jj,I,F,Lives,Lv,Rx,Ry,Ary,J,Z,Rd,W,OldI;
extern long Sc,Stl;
extern int Tm,Ft,X,Y,Ex,Ey,Cc,E;
extern int Dmx,Dmy,Nbec,Nc,Ncb,Npl,N,Xo,Yo;
extern int Pouf,Xpf,Ypf,Uggd,Uggg,Slick,Coily,Egg,Egg2,Cegg,Cegg2;
extern int Plt,Nt,Stand,Nm,Nom,Freeze,T,Dd,No,tmp,Nwtab;

extern char r;



int game()
{
	int pause;
	int i,dx,dy;
	char s[40];

	Demo=1;
play:
	pause=0;
	Lndemo=strlen(Demostr);
	for(ii=0;ii<Lndemo;ii++)
	{
		Demotab[ii][0]=Demotab[ii][1]=0;
		switch(Demostr[ii])
		{
			case '4':
				Demotab[ii][0]= -1;
				break;
			case '6':
				Demotab[ii][0]= 1;
				break;
			case '8':
				Demotab[ii][1]= -1;
				break;
			case '2':
				Demotab[ii][1]= 1;
				break;
		}
	}
	
	
	View();

	Lv=Oldlv;
	if (Demo) Lv=1;

	W=0;
	Lives=3;
	Rd=1;
	Sc=0L;
	Stl=1000L;

  do
  {
	if (Demo==0) 
		afftext8(23,4,"GET READY");
	else
		afftext8(23,4,"  DEMO   ");
	if (Lv!=10)
	{
		sprintf(s,"LEVEL %d",Lv);
		afftext8(24,6,s);
		sprintf(s,"ROUND %d",Rd);
		afftext8(24,8,s);
	}
	else
	{
		afftext8(22,6,"FINAL LEVEL");
		afftext8(23,8,"HURRY UP!");
  		Tm=900;
		Ft=0;
	}

	View();

	for(i=0;i<180;i++) pb_wait_for_vbl();
		
	Demon=0;
	Dmx=0;
	Dmy=0;
	View();
	Nbec= -1;
	updatecubecolor();
	
	Nc=0;
	Ncb=3;
	if ((Lv==1)||(Lv==3)) Ncb--;
	if (Rd==4) Npl= -1; else Npl=0;
	C[0]=(Rd+2+Npl)%3;
	C[1]=(Rd+3+Npl)%3;
	C[2]=(Rd+1+Npl)%3;
	C[Ncb]=17;

	if (Lv<3) C[Ncb]=16;
	
	for(ii=0;ii<7;ii++)
		for(jj=0;jj<7;jj++)
			P2[ii][jj]=0;
	Npl=Lv;
	if (Lv>6) Npl=5;
	if (Lv==10) Npl=0;

	for(ii=0;ii<7;ii++) G[ii]=D[ii]= -1;
	for(ii=0;ii<14;ii++) P[ii]=Pp[ii]=0;

	for(I=0;I<Npl+1;I++)
	{
		F=0;
		
		do
		{
			N=randint(7);
			if ((I==0)&&(Lv==10)) N=0;
			if ((I+Rd)&1)
			{
				if (G[N]== -1)
				{
					G[N]=I;
					F=1;
				}
			}
			else
			{
				if (D[N]== -1)
				{
					D[N]=I;
					F=1;
				}
			}
		}while(!F);
			
		Yo= -1;
		Xo=N;
		if ((I+Rd)&1)
		{
			Yo=N;
			Xo= -1;
		}
	}
	
	Pouf= -1;
	Xpf=0;
	Ypf=0;

	Uggd=0;
	Uggg=0;
	Slick=0;
	Coily=0;
	
	for(ii=0;ii<7;ii++)
	{
		Xt[ii]=0;
		Yt[ii]=0;
		Xd[ii]=0;
		Dx[ii]=0;
		Dy[ii]=1;
		Yd[ii]= -32;
		M[ii]= -99;
	}

	M[0]= -12;
	Egg= -1;
	Egg2= -1;
	Nt=(Lv-1)*4+Rd;
	Cegg=48-Nt;
	Cegg2=48-Nt;
	T= -1;
	Yd[5]=20;
	Yd[6]=20;
	Xd[5]=40;
	Xd[6]= -40;
	Dx[6]=1;
	Dy[6]=0;

	draw_pyramid();
	Score(0);
	Life();

	Plt= -1;
	Stand=0;
	Xt[0]=0;
	Yt[0]=0;
	Nm=0;
	Nom=0;
	for(ii=0;ii<7;ii++) Pos(ii);
	Freeze=0;

	r=0;

	while(!quit)
	{
	  if (Freeze)
	  {
	 	freezetime++;
		if (freezetime>600)
			freezetime=Freeze=0;
	  }

	  r=0;
  
	  View();
	  pb_wait_for_vbl();//extra wait to slow down game
	  pb_wait_for_vbl();//extra wait to slow down game
	  draw_pyramid();
	  Score(0);
	  Life();
		
	  if ((W==1)||(W==2))
	  	quit=2;
	  else
	  {
		XInput_GetEvents();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) antialiasing=0;
		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) antialiasing=1;

//		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_X]) pb_show_debug_screen();
//		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_B]) pb_show_front_screen();

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_Y]) return 0;

		if (pause) afftext8(0,10,"PAUSE");

		if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A])
		{
			if (Demo)
			{
				Demo=0; 
				goto play;
			}
			else
			{
				afftext8(0,10,"PAUSE");				
				pause=1;
			}
		}
		else
		{
			if (pause)
			{
				while(1)					
				{
					XInput_GetEvents();
					if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A]==0) break;
				}
				while(1)					
				{
					XInput_GetEvents();
					if (g_Pads[0].PressedButtons.ucAnalogButtons[XPAD_A]) break;
				}
				pause=0;
			}
		}

		if (!Nm)
		{
			Nom=randint(6)+1;
			if (Nbec!=6)
				if (M[Nom]== -99)
					if  (
						Lv!=10 &&
						(Nom!=4 || (Nt>2 && Nbec>2)) &&
						(Nom!=3 || Nt>2) &&
						(Nom!=1 || (Nt!=3 && Nt!=5 && Nt!=6 && Nt!=10)) &&
						((Nom!=5 && Nom!=6) || 
						(Nt!=1 && Nt!=2 && Nt!=4 && Nt!=7 && Nt!=9 && Nt!=13 && Nt!=15))
						)
					{
						Nm=Nom;
						M[Nom]= -64+Nt;
						if (Nm==1)
						{
							if (randint(10)<Lv) Coily=1; else Coily=0;
						}
						if (Nm==3)
						{
							if (randint(10)<Lv) Slick=1; else Slick=0;
						}
						if (Nm==5)
						{
							if (randint(10)<Lv) Uggd=1; else Uggd=0;
						}
						if (Nm==6)
						{
							if (randint(10)<Lv) Uggg=1; else Uggg=0;
						}
					}
		}
		else
		{
			if (M[0]>-1 && (M[0]!=0 || Yd[0]>-16) && Freeze==0)
			{
				M[Nm]++;
				if (M[Nm]==0)
				{
					Nbec++;
					if (Nm!=5 && Nm!=6)
					{
						for(I=Nbec;I>0;I--)
						{
							Tt[I]=Tt[I-1];
							Ntt[Tt[I]]++;
						}
						Tt[0]=Nm;
						Ntt[Nm]=0;
					}
					else
					{
						Tt[Nbec]=Nm;
						Ntt[Nm]=Nbec;
					}
					Nm=0;
				}
			}
		}
		

		if (Nbec== -1)
		{
			M[0]++;
			if (M[0]==0)
			{
				Nbec=0;
				Tt[0]=0;
				Ntt[0]=0;
				if (W==0)
				{
					Lives--;
					if (Lives== -1)
						W=1;
					else
					{
						Pouf=0;
						Xpf=8+Lives*20;
						Ypf=70;
						Life();
					}
				}
			}
		}
		
		
		if (T>0) T--;

		if (M[0]==0)
		{
			if (Demo==0)
			{
				Ry=Rx=0;

				r=0;
				
				dx=g_Pads[0].sRThumbX/256;
				dy=-g_Pads[0].sRThumbY/256;

				if ((dx<-DEADZONE_JOYPAD)&&(dy<-DEADZONE_JOYPAD)) Rx=-1;
				if ((dx>DEADZONE_JOYPAD)&&(dy>DEADZONE_JOYPAD)) Rx=1;
				if ((dx>DEADZONE_JOYPAD)&&(dy<-DEADZONE_JOYPAD)) Ry=-1;
				if ((dx<-DEADZONE_JOYPAD)&&(dy>DEADZONE_JOYPAD)) Ry=1;
			}
			else
			{
				if ((Dmx==Xt[0])&&(Dmy==Yt[0]))
				{
					Rx=Demotab[Demon][0];
					Ry=Demotab[Demon][1];
					Demon=(Demon+1)%Lndemo;
					Dmx=Dmx+Rx;
					Dmy=Dmy+Ry;
				}
				else
				{
					Rx=SGN(Dmx-Xt[0]);
					Ry=0;
					if (Rx==0)
						Ry=SGN(Dmy-Yt[0]);
				}
			}
		}

		if (Xt[2]+Yt[2]==6 && M[2]==0 && Egg && Freeze==0)
		{
			Cegg--;
			if (Cegg==0) Egg=0;
			Yd[2]=0;
			if (Cegg<16)
				Yd[2]=(Cegg&1);
		}
	
		if (Coily && Xt[1]+Yt[1]==6 && M[1]==0 && Egg2 && Freeze==0)
		{
			Cegg2--;
			if (Cegg2==0) Egg2=0;
			Yd[1]=0;
			if (Cegg2<16)
				Yd[1]=(Cegg2&1);
		}


		for(I=0;I<7;I++)
		{
			if (M[I]>-1)
			{
				if ((I!=0 || Rx || Ry) && 
					(I!=2 || Egg==0 || Xt[2]+Yt[2]!=6 || M[0]<0) && 
					(I!=4 || Stand==0) && 
					(I!=1 || Egg2==0 || Xt[1]+Yt[1]!=6 || M[0]<0 || Coily==0))
				{
					if (Yd[I]==0 && M[I]==0)
					{
						if (I!=0)
						{
							Rx=0;
							Ry=0;
						}
						if (I==4 || (I==5 && Uggd==0) || (I==6 && Uggg==0))
						{
							Rx=0;
							Ry=0;
							if (randint(10)>4) Rx=1; else Ry=1;
						}
						if (I==1 || (I==2 && Egg) || (I==1 && Coily && Egg2))
						{
							Rx=0;
							Ry=0;
							if (Px[0]>Px[I]) Rx=1; else Ry=1;
						}
						if (I==5 && Uggd)
						{
							Rx=0;
							Ry=0;
							if (ABS(Px[5]-Px[0])<ABS(Py[5]-Py[0])) Ry=1;
							else  Rx=1;
						}
						if (I==6 && Uggg)
						{
							Rx=0;
							Ry=0;
							if (ABS(Px[6]-Px[0])<ABS(Py[6]-Py[0])) Rx=1;
							else Ry=1;
						}
        	    		
						if (I==3 && M[0]>-1)
						{
							if (Slick && M[4]>-1)
								Guide(3,4);
							else
							{
								do
								{
									Rx=randint(3)-1;
									if (Xt[3]+Rx<0 || Rx+Xt[3]+Yt[3]>6)
										Rx=0;
									if (Rx==0)
										Ry=randint(3)-1;
									if (Yt[3]+Ry<0 || Ry+Xt[3]+Yt[3]>6)
										Ry=0;
								}while((Rx==0)&&(Ry==0));
							}
						}

						if (M[0]<0 && ((I==2 && Egg==0) || I==3 || 
							(I==1 && Coily && Egg2==0)))
						{
							Rx=0;
							Ry=0;
							if (Xt[I]>Yt[I]) Rx=1; else Ry=1;
							if (Xt[I]+Yt[I]<5)
							{
								Xo=Rx;
								Rx= -Ry;
								Ry= -Xo;
							}
						}
					
						if (M[0]>-1 && ((I==2 && Egg==0) || 
							(I==1 && Egg2==0 && Coily)))
							Guide(I,0);
						
						M[I]=1;
						if ((I!=0)||(Rx)||(Ry))
						{
							Dy[I]=Ry;
							Dx[I]=Rx;
						}
					}
				}
				
				
				if ((I!=0 && Dx[I]+Dy[I]>0 && I!=5 && I!=6)) tv= -1; else tv=0;
				
				if (M[I]==32+19*tv)
				{
					Killed(I);
					if (I==2)
						Egg= -1;
					if (I==1 && Coily)
						Egg2= -1;
				}
			
				if (ABS(Yd[I])>1 && M[I]==0 && (I==0 || Freeze==0))
				{
					if (I==0 && Yd[0]== -32 && Nm!=0 && M[Nm]>-24)
					{
						M[Nm]= -24;
					}
					if (I==0 && W==3 && Yd[0]== -16)
					{
						Score(100+(Lv-1)*100+25*(Rd-1));
						W=0;
					}
					if (I==5 || I==6)
					{
						Yd[I]=Yd[I]-4;
						if (I==6) tv= -1; else tv=0;
						Xd[I]=Xd[I]-8*(1+2*tv);
					}
					else
						Yd[I]=Yd[I]+8;
					if (Yd[I]==0)
						Flip();
				}
			
				if (I==0 && Xt[0]<0)
				{
					if (M[0]==11 && G[Yt[0]]>-1)
					{
						M[0]=100;
						Plt=G[Yt[0]];
						P[Plt]= -1;
						Freeze=0;
						Yd[0]--;
						Xd[0]+=2;						
					}
				}
          	
				if (I==0 && Yt[0]<0)
				{
					if (M[0]==11 && D[Xt[0]]>-1)
					{
						M[0]=100;
						Plt=D[Xt[0]];
						P[Plt]= -1;
						Freeze=0;
						Yd[0]--;
						Xd[0]-=2;
					}
				}	
				
				if (M[I]==8 && (I==0 || Freeze==0))
				{
					M[I]=0;
					Xd[I]=0;
					Yd[I]=0;
					Xt[I]=Xt[I]+Dx[I];
					Yt[I]=Yt[I]+Dy[I];
					Pos(I);
					if (Xt[I]+Yt[I]>6 || Xt[I]<0 || Yt[I]<0)
						M[I]=8;
					else
					{
						Flip();
						if (I==4 && Xt[4]+Yt[4]>1)
						{
							if (randint(10)>Lv && M[0]>-1) Stand= -1;
							else Stand=0;
						}
					}
				}
	
				if (M[I]>0 && M[I]<100 && (I==0 || Freeze==0))
				{
					Dd=((M[I]/2)-3);
					if (M[I]!=8)
					{
						if (I==5 || I==6)
						{
							if (I==5)
							{
								if (Dy[5]==0) tv= -1; else tv=0;
								Xd[5]=Xd[5]-(2*Dd+4)+2*tv;
								if (Dx[5]==0) tv= -1; else tv=0;
								Yd[5]=Yd[5]-(Dd+2)+1+3*tv;
							}
							else
							{
								if (Dx[6]==0) tv= -1; else tv=0;
								Xd[6]=Xd[6]+(2*Dd+4)-2*tv;
								if (Dy[6]==0) tv= -1; else tv=0;
								Yd[6]=Yd[6]-(Dd+2)+1+3*tv;
							}
						}
						else
						{
							if (Dx[I]== -1 || Dy[I]==1) tv= -1; else tv=0;
							Xd[I]=Xd[I]+2*(1+2*tv);
							if (Dx[I]==1 || Dy[I]==1) tv= -1; else tv=0;
							Yd[I]=Yd[I]+Dx[I]+Dy[I]+2*Dd-4*tv;
						}
					}
					M[I]++;
				}
					
				if (I==5 || I==6)
				{
					if (I==5)
					{
						if (Dx[5]==0) tv= -1; else tv=0;
						Pl[5]=52-Yt[5]*8-M[5]*tv;
					}
					else
					{
						if (Dy[6]==0) tv= -1; else tv=0;
						Pl[6]=52-Xt[6]*8-M[6]*tv;
					}
				}
				else
				{
					if (Dx[I]== -1) tv1= -1; else tv1=0;
					if (Dy[I]==  0) tv2= -1; else tv2=0;
					if (Dy[I]== -1) tv3= -1; else tv3=0;
					if (Dx[I]==  0) tv4= -1; else tv4=0;				
					Pl[I]=Xt[I]*8-M[I]*(1+2*tv1)*tv2+Yt[I]*8-M[I]*(1+2*tv3)*tv4;
				}
			}
		}
	
	
		for(I=0;I<7;I++)
		{
			if (M[I]>-1)
			{
			
				if (Ntt[I]!=0)
				{
					if (Pl[I]<Pl[Tt[Ntt[I]-1]])
					{
						tmp=Tt[Ntt[I]-1];
						Tt[Ntt[I]-1]=Tt[Ntt[I]];
						Tt[Ntt[I]]=tmp;
						Ntt[Tt[Ntt[I]]]++;
						Ntt[I]--;
					}
				}
				
				if (Ntt[I]!=Nbec)
				{
					if (Pl[I]>Pl[Tt[Ntt[I]+1]])
					{
						tmp=Tt[Ntt[I]+1];
						Tt[Ntt[I]+1]=Tt[Ntt[I]];
						Tt[Ntt[I]]=tmp;
						Ntt[Tt[Ntt[I]]]--;
						Ntt[I]++;
					}
				}
				
				if (I!=0 && M[0]>-1 && M[0]<9)
				{
					if ((ABS(Xt[I]-Xt[0])<2 && ABS(Yt[I]-Yt[0])<2) || I==5 || I==6)
					{
						if ((I==5)||(I==6)) tv= -8; else tv=0;
						if (I==5) tv1= -8; else tv1=0;
						if (I==6) tv2= -8; else tv2=0;
						if (ABS(Px[I]+Xd[I]-tv1+tv2-Px[0]-Xd[0])+ABS(Py[I]+Yd[I]-tv-Py[0]-Yd[0])<12 && 
							(M[I]!=0 || Yd[I]>-16) && (M[0]!=0 || Yd[0]>-8))
						{
							if (I!=3 && I!=4)
							{
								Qbert();
								if (Dx[0]==1 || Dy[0]== -1) tv1= -1; else tv1=0;
								if (Dx[0]== -1 || Dy[0]==1) tv2=1; else tv2=0;
								Sp(0,-60-24*tv1+Xd[0],-16+Yd[0],tv2,23);
								View();
								for(i=0;i<60;i++) pb_wait_for_vbl();
								Killed(0);
							}
							else
								Killed(I);
							if (I==4)
							{
								Score(5);
								Freeze=1;
								freezetime=0;
							}
						}
					}
				}

				if (M[4]>-1 && I!=0 && I!=4)
				{
					if ((ABS(Xt[I]-Xt[4])<2 && ABS(Yt[I]-Yt[4])<2) || I==5 || I==6)
					{
						if ((I==5)||(I==6)) tv= -8; else tv=0;
						if (I==5) tv1= -8; else tv1=0;
						if (I==6) tv2= -8; else tv2=0;
						if (ABS(Px[I]+Xd[I]-tv1+tv2-Px[4]-Xd[4])+ABS(Py[I]+Yd[I]-tv-Py[4]-Yd[4])<12 &&
							 (M[I]!=0 || Yd[I]>-16) && (M[4]!=0 || Yd[4]>-16))
						{
							Killed(4);
							Stand=0;
						}
					}
				}
				
			}
		}
		
		if (Lv==10 && Tm<0 && !Ft)
		{
			Qbert();
			if (Dx[0]==1 || Dy[0]== -1) tv1= -1; else tv1=0;
			if (Dx[0]== -1 || Dy[0]==1) tv2= 1; else tv2=0;
			Sp(0,-60-24*tv1+Xd[0],-16+Yd[0],tv2,23);
			View();
			for(i=0;i<60;i++) pb_wait_for_vbl();
			Lives=0;
			Killed(0);
			Ft= -1;
		}
		
		if (Plt!= -1)
		{
			if (Dy[0]== -1) tv= -1; else tv=0;
			Xd[0]+=2*(1+2*tv);
			Yd[0]-=3;
			if (Dx[0]<0) tv= -1; else tv=0;
			Sp(0,Xd[0]+16*tv,13+Yd[0],0,13);
			
			if (16*Xt[0]-16*Yt[0]+Xd[0]==0)
			{
				if (Dx[0]== -1)
					G[Yt[0]]= -1;
				else
					D[Xt[0]]= -1;
					
				P[I]=0;
				Xt[0]=0;
				Yt[0]=0;
				M[0]=0;
				Yd[0]= -40;
				Xd[0]=0;
 				Pos(0);
				Plt= -1;
				Npl--;
			}
		}
		
		if (M[0]>99)
		{
			if (Xt[0]<0) tv1= -1; else tv1=0;
			if (Dy[0]==0) tv2= 1; else tv2=0;
			Sp(0,-2+Xd[0]+12*tv1,12+Yd[0],tv2,0);
		}
    
		if (Nbec>-1)
		{
			for(I=0;I<Nbec+1;I++)
			{
				No=Tt[I];
				if (M[No]>8 && Dy[No]+Dx[No]<0)
					switch(No)
					{
						case 0:
							Qbert2();
							break;
						case 1:
							Coily2_1();
							break;
						case 2:
							Coily2_2();
							break;
						case 3:
							Sam2();
							break;
					}
			}
		}
		
		for(I=0;I<4;I++)
		{
			if (Dx[I]+Dy[I]<0 && M[I]>17)
			{
				if ((M[I]==20 || M[I]==18 || M[I]==19) && Xt[I]+Yt[I]<5)
					Sp(I,-16,72,0,14+(C[P2[Xt[I]+1][Yt[I]+1]]));
				if ((M[I]==20 || M[I]==21 || M[I]==22 || M[I]==23) && Xt[I]+Yt[I]<4)
					Sp(I,-16+16*(Dx[I]-Dy[I]),96,0,14+(C[P2[Xt[I]+1-Dy[I]][Yt[I]+1-Dx[I]]]));
				if (M[I]==22 && Xt[I]+Yt[I]<3)
					Sp(I,-16+32*(Dx[I]-Dy[I]),120,0,14+(C[P2[Xt[I]+1-2*Dy[I]][Yt[I]+1-2*Dx[I]]]));
			}
		}
		
		if (Nbec>-1)
			for(I=0;I<Nbec+1;I++)
				switch(Tt[I])
				{
					case 0:
						Qbert();
						break;
					case 1:
						Red_ball();
						break;
					case 2:
						Coilyf();
						break;
					case 3:
						Sam();
						break;
					case 4:
						Green_ball();
						break;
					case 5:
						Ugg_d();
						break;
					case 6:
						Ugg_g();
						break;
				}
				
		if (Pouf>-1)
		{
			Spr(Xpf,Ypf,0,17+Pouf);
			if (Pouf==3) tv= -1; else tv=0;
			Pouf=Pouf+1+5*tv;
		}

		if (Lv==10)
		{
			Afftime();
			Tm--;
		}
	  }
	}		

	if (quit==2) quit=0;

	if (W==2)
	{
		View();
		Rd++;
		if (Rd==5)
		{
			Rd=1;
			Lv++;
		}
		W=3;
	}
	
	if ((W==3)&&(Demo)&&(Lv==3)) W=1;

	if (W==1)
	{
		if (Lv>Oldlv && Lv!=10)
			Oldlv=Lv;
		W=0;
		View();
	}
	
	Nwtab=0;
	
	if (W==0)
	{
		View();
		afftext8(25,6,"GAME OVER");
		Spr(9*8,13*8+4,1,22);
		Spr(28*8,13*8+4,0,22);
		View();
		for(i=0;i<180;i++) pb_wait_for_vbl();	
	}
	else
	{
		if (Lv!=10 || Rd!=2)
		{
			if (Demo)
			{
				Dmx=Xt[0];
				Dmy=Yt[0];
			}
    		Nwtab=1;
    	}
    }
    
  }while(Nwtab);

	    
	if (W==3)
	{
		View();
		afftext8(18,2,"   CONGRATULATIONS!   ");
		afftext8(18,3,"   YOU HAVE ESCAPED   ");
		afftext8(18,4," THE TERRIFIC PYRAMID ");
		afftext8(18,5,"       COMPLEX!       ");
		afftext8(18,6,"     YOU ARE FREE!    ");
		afftext8(18,8," SPECIAL BONUS 100000 ");
		Spr(4*8,108,1,5);
		Spr(34*8,108,0,5);
		Sc=Sc+10000; //Sc=score/10
		View();
		for(i=0;i<360;i++) pb_wait_for_vbl();
	}

	return 1;
}

