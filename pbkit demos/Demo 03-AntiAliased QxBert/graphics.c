#include "pbKit.h"
#include "qbert.h"

extern int Lv,Rd;
extern int P2[7][7];
extern int C[4];

DWORD 	pal32bpp[16];


struct s_bloc	sprites[36][2];

int slx[36]=
{
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,32,32,32,
	16,16,16,16,16,16,96,
	32,32,32,32,32,32,16,16,16,16,32,32
};

int sly[36]=
{
	21,21,21,21,24,24,24,24,24,24,10,10,10,10,32,32,32,
	16,16,16,16,34,34,46,
	23,23,26,22,22,24,34,34,31,31,20,20
};

unsigned int Levcl[50]=
{
	0x2f00,0x50f0,0x6ff0,0x8fff,0x4ccc,
	0x1a06,0x30a6,0x5aa6,0x8fff,0x4ccc,
	0x3f84,0x58f4,0x7ff4,0x8fff,0x4ccc,
	0x30ff,0x500f,0x8fff,0x6ccc,0x4888,
	0x1f80,0x38f0,0x5ff8,0x6ccc,0x4888,
	0x208f,0x68ff,0x488f,0x50ff,0x3088,
	0x6f88,0x4f08,0x8ff8,0x5f0f,0x3808,
	0x2800,0x4840,0x3620,0x8cc0,0x6880,
	0x3246,0x4624,0x5482,0x1222,0x0000,
	0x4ff8,0x6f8f,0x88ff,0x3444,0x1222
};

DWORD 	*g_buffer;
DWORD	g_pitch;



//pixels always drawn (bit 2=0) in texture operations
void set_transparency_off(void)
{
	DWORD		*p;

//main interest of alphakill method is that it's a texture stage dependent method
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); p+=2; //set stage 0 texture enable flags
	pb_end(p);


//alternate method : alphafunc, i.e set alphafunc='>=' and alpharef=1 (pixel drawn if alpha>=1)
/*	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_ALPHA_FUNC_ENABLE,0); p+=2; //Disables Alpha test
	pb_end(p);
*/

//alternate method: blend source and destination colors according to alpha color component
//(allows 256 levels of transparency, 255=opaque)
/*	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BLEND_FUNC_ENABLE,0); p+=2; //Disables Blending
	pb_end(p);
*/
}

//pixels drawn only if alpha!=0 (bit 2=1) in texture operations
void set_transparency_on(void)
{
	DWORD		*p;
	
//main interest of alphakill method is that it's a texture stage dependent method
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc4); p+=2; //set stage 0 texture enable flags
	pb_end(p);

//alternate method : alphafunc, i.e set alphafunc='>=' and alpharef=1 (pixel drawn if alpha>=1)
/*	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_ALPHA_FUNC_ENABLE,1); p+=2; //Enables Alpha test
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_ALPHA_FUNC_REF,1); p+=2; //Alpha reference value for condition below
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_ALPHA_FUNC_FUNC,0x206); p+=2; //Pixel draw condition: 0x20n (0:Never 1:< 2:= 3:<= 4:> 5:!= 6:>= 7:Always)
	pb_end(p);
*/

//alternate method: blend source and destination colors according to alpha color component
//(allows 256 levels of transparency, 255=opaque)
/*	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BLEND_FUNC_ENABLE,1); p+=2; //Enables Blending
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BLEND_FUNC_SRC,0x302); p+=2;//SrcAlpha
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BLEND_FUNC_DST,0x303); p+=2;//1-SrcAlpha
	pb_end(p);
*/
}

//no anti-aliasing filter for texture operations
void set_filters_off(void)
{
	DWORD		*p;
	
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x02022000); p+=2; //set stage 0 texture filters (No AA!)
	pb_end(p);
}

//anti-aliasing filter (gaussian cubic) for texture operations
void set_filters_on(void)
{
	DWORD		*p;
	
	p=pb_begin();
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x04074000); p+=2; //set stage 0 texture filters (AA!)
	pb_end(p);
}

void draw_sprite(struct s_bloc *pbloc,int dst_x,int dst_y)
{
	float xScale  = 2.0f;
	float yScale  = 2.0f;
	float fLeft   = (float)(dst_x*2);
	float fTop    = (float)(dst_y*2);
	float fRight  = (float)((dst_x+pbloc->w)*2);
	float fBottom = (float)((dst_y+pbloc->h)*2);
	float uAdjust = (float)(pbloc->x*2);
	float vAdjust = (float)(pbloc->y*2);

	DWORD *p;
	
	int i;

	if (pbloc==NULL) return;
	if (pbloc->f==0) return;

	//we assume set_transparency_on() as been called previously if transparency is needed

	p=pb_begin();

	pb_push1(p,NV20_TCL_PRIMITIVE_3D_BEGIN_END,QUADS); p+=2; //(beginning of list) quad used here, but 2 triangles work too

	pb_push(p++,0x40000000|NV20_TCL_PRIMITIVE_3D_VERTEX_DATA,16); //bit 30 means all params go to same register 0x1818

	// Vertex 0
	*((float *)(p++))=fLeft;
	*((float *)(p++))=fTop;
	*((float *)(p++))=uAdjust;
	*((float *)(p++))=vAdjust;
	// Vertex 1
	*((float *)(p++))=fRight;
	*((float *)(p++))=fTop;
	*((float *)(p++))=uAdjust + pbloc->w*xScale;
	*((float *)(p++))=vAdjust;
	// Vertex 2
	*((float *)(p++))=fRight;
	*((float *)(p++))=fBottom;
	*((float *)(p++))=uAdjust + pbloc->w*xScale;
	*((float *)(p++))=vAdjust + pbloc->h*yScale;
	// Vertex 3
	*((float *)(p++))=fLeft;
	*((float *)(p++))=fBottom;
	*((float *)(p++))=uAdjust;
	*((float *)(p++))=vAdjust + pbloc->h*yScale;

	pb_push(p++,NV20_TCL_PRIMITIVE_3D_BEGIN_END,1);
	*(p++)=STOP; //triggers the drawing (end of list)

	pb_end(p);
}

void Spr(int x,int y,int i,int n)
{
	draw_sprite(&sprites[n][i],x,y-sly[n]);
}

void draw_pyramid(void)
{	
	int x,y;
	for(y=0;y<7;y++)
		for(x=0;x<7;x++)
			if (x+y<7)
				Spr(160+x*16-y*16-16,24+x*24+y*24+24,0,14+C[P2[x][y]]);
}


void set_source_extra_buffer(int n)
{
	DWORD		*p;
	
	p=pb_begin();
	pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),((DWORD)pb_extra_buffer(n))&0x03FFFFFF,0x0001122a); p+=3; //set stage 0 texture address & format
	pb_end(p);
}

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
	DstAddr=0; //dest_addr-base_dma_addr (base_dma_addr=last targetted buffer addr)
	
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

void prepare_texture_mapping(void)
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
	//		((2+VolumeFlag)<<4)|	//0x20=2D surface, 0x30=3D volume
	//		((2+CubeMapFlag)<<2)|	//meaning of bit 3 is "use border color"
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
	pb_push1(p,NV20_TCL_PRIMITIVE_3D_LIGHT_CONTROL,0x00020001); p+=2; //(specular flags=0x00020000)
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
	//take a look at intermediate assembler file vs.psh

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
	p=pb_push_mcode(p,pb_pcode2mcode(g_ps11_main)); //incomplete (just sets 1 stage 'r0<-?' with 'tex t0' supported)
	pb_end(p);
}



void plot(int x, int y, DWORD color)
{
	DWORD		*p;
	
	if ((x>=0)&&(x<320)&&(y>=0)&&(y<240))
	{
		p=g_buffer+2*y*g_pitch+2*x;
		*p=color;
		*(p+1)=color;
		p+=g_pitch;
		*p=color;
		*(p+1)=color;
	}
}

void pbox(int x, int y, int w, int h, DWORD color)
{
	int i,j;
	for(i=0;i<h;i++) for(j=0;j<w;j++) plot(x+j,y+i,color);
}


//draws a sprite in extra buffer #1 and remembers its coordinates
void loadsprite(int n,char *s[])
{
	static int next_x=0,next_y=4,max_ly=0;
	int i,j,c,lx,ly,v;
	int mirror;

	lx=slx[n];
	ly=sly[n];
	
	sprites[n][0].f=0; //sprite
	sprites[n][1].f=0; //mirrored copy
	
	//do we need the mirrored copy?
	if ((n<10)||(n>20)) mirror=1; else mirror=0;
	
	if (next_x+lx>320) { next_x=0; next_y+=max_ly; max_ly=0; }
	if (next_y+ly*(1+mirror)>240) return; //no more room in extra buffer #1

	if (ly*(1+mirror)+8>max_ly) max_ly=ly*(1+mirror)+8;

	//draws sprite and its mirrored image
	for(i=0;i<ly;i++)
		for(j=0;j<lx;j++)
		{
			v=lx-j-1; //mirrored coordinate
			c=(int)s[i][j];
			if (c!=' ')
			{
				if (c<'A') c-='0';
				else
					if (c<'a') c-='A'-10;
					else
						c-='a'-10;
						
				plot(next_x+j,next_y+i,pal32bpp[c]);
				if (mirror) plot(next_x+v,next_y+i+ly+4,pal32bpp[c]);
			}
		}

	sprites[n][0].f=1; //sprite exists
	sprites[n][0].x=next_x;
	sprites[n][0].y=next_y;
	sprites[n][0].w=lx;
	sprites[n][0].h=ly;

	if (mirror)
	{
		sprites[n][1].f=1; //sprite exists
		sprites[n][1].x=next_x;
		sprites[n][1].y=next_y+ly+4;
		sprites[n][1].w=lx;
		sprites[n][1].h=ly;
	}	

	next_x+=lx;	
}

//redraws a sprite in extra buffer #1 (palette changed)
void reloadsprite(int n,char *s[])
{
	int i,j,c,lx,ly,v;
	int mirror;
	int next_x,next_y;

	if (sprites[n][0].f==0) return; //doesn't exist
	
	next_x=sprites[n][0].x;
	next_y=sprites[n][0].y;
	
	lx=slx[n];
	ly=sly[n];
	
	sprites[n][0].f=0; //sprite
	sprites[n][1].f=0; //mirrored copy
	
	//do we need the mirrored copy?
	if ((n<10)||(n>20)) mirror=1; else mirror=0;
	
	//draws sprite and its mirrored image
	for(i=0;i<ly;i++)
		for(j=0;j<lx;j++)
		{
			v=lx-j-1; //mirrored coordinate
			c=(int)s[i][j];
			if (c!=' ')
			{
				if (c<'A') c-='0';
				else
					if (c<'a') c-='A'-10;
					else
						c-='a'-10;
						
				plot(next_x+j,next_y+i,pal32bpp[c]);
				if (mirror) plot(next_x+v,next_y+i+ly+4,pal32bpp[c]);
			}
		}

	sprites[n][0].f=1; //sprite exists
	sprites[n][0].x=next_x;
	sprites[n][0].y=next_y;
	sprites[n][0].w=lx;
	sprites[n][0].h=ly;

	if (mirror)
	{
		sprites[n][1].f=1; //sprite exists
		sprites[n][1].x=next_x;
		sprites[n][1].y=next_y+ly+4;
		sprites[n][1].w=lx;
		sprites[n][1].h=ly;
	}	
}


void prepare_sprites(void)
{
	DWORD		*p;
	DWORD		pitch;
	int		i,j,n;

	//converts M4R4G4B4 palette into A8R8G8B8 palette
	for(i=0;i<16;i++)
		pal32bpp[i]=	(((pal[i]&0x0f00)*0x11)<<8)|
				(((pal[i]&0x00f0)*0x11)<<4)|
				(((pal[i]&0x000f)*0x11)<<0)|
				0xff000000; //opaque
				
	g_buffer=pb_extra_buffer(1);
	g_pitch=pb_back_buffer_pitch()/4; //gap between a pixel line and the next one

	pbox(0,0,320,240,0);
	
	//Draws some broad ugly diagonals in texture in order to detect the anti-aliasing effect and transparency effect
	//for(i=0;i<256;i++) for(j=0;j<256;j++) *(g_buffer+i*g_pitch+j)=((((i&~1)+(j&~1))&16)*15)*256+((((j&~1)-(i&~1))&16)*15)*65536+255;
	//for(i=0;i<64;i++) for(j=0;j<64;j++) *(g_buffer+i*g_pitch+j)|=0xff000000;

	

	n=0;
	
	loadsprite(n++,blocaa);
	loadsprite(n++,blocab);
	loadsprite(n++,blocac);
	loadsprite(n++,blocad);
	loadsprite(n++,blocba);
	loadsprite(n++,blocbb);
	loadsprite(n++,blocbc);
	loadsprite(n++,blocbd);
	loadsprite(n++,blocbe);
	loadsprite(n++,blocbf);
	loadsprite(n++,blocca);
	loadsprite(n++,bloccb);
	loadsprite(n++,bloccc);
	loadsprite(n++,bloccd);
	loadsprite(n++,blocda);
	loadsprite(n++,blocdb);
	loadsprite(n++,blocdc);
	loadsprite(n++,blocea);
	loadsprite(n++,bloceb);
	loadsprite(n++,blocec);
	loadsprite(n++,bloced);
	loadsprite(n++,blocfa);
	loadsprite(n++,blocfb);
	loadsprite(n++,blocga);
	loadsprite(n++,blokaa);
	loadsprite(n++,blokab);
	loadsprite(n++,blokba);
	loadsprite(n++,blokca);
	loadsprite(n++,blokcb);
	loadsprite(n++,blokda);
	loadsprite(n++,blokea);
	loadsprite(n++,blokeb);
	loadsprite(n++,blokfa);
	loadsprite(n++,blokfb);
	loadsprite(n++,blokga);
	loadsprite(n++,blokgb);
}

void updatecubecolor()
{
	int i;
	
	pal[1]=Levcl[(Lv-1)*5];
	pal[2]=Levcl[(Lv-1)*5+1];
	pal[3]=Levcl[(Lv-1)*5+2];
	pal[7]=Levcl[(Lv-1)*5+3];
	pal[8]=Levcl[(Lv-1)*5+4];
	if (Rd==4)
	{
		pal[3]=0;
		pal[7]=0;
		pal[8]=0;
	}

	//converts M4R4G4B4 palette into A8R8G8B8 palette
	for(i=0;i<16;i++)
		pal32bpp[i]=	(((pal[i]&0x0f00)*0x11)<<8)|
				(((pal[i]&0x00f0)*0x11)<<4)|
				(((pal[i]&0x000f)*0x11)<<0)|
				0xff000000; //opaque

	reloadsprite(14,blocda);
	reloadsprite(15,blocdb);
	reloadsprite(16,blocdc);
}

void updateliftcolor(void)
{
	unsigned int t;
	int i;
	static int toggle;
	
	toggle^=1;
	
	if (toggle&1) return;

	t=pal[12];
	pal[12]=pal[11];
	pal[11]=pal[10];
	pal[10]=pal[9];
	pal[9]=t;

	//converts M4R4G4B4 palette into A8R8G8B8 palette
	for(i=9;i<13;i++)
		pal32bpp[i]=	(((pal[i]&0x0f00)*0x11)<<8)|
				(((pal[i]&0x00f0)*0x11)<<4)|
				(((pal[i]&0x000f)*0x11)<<0)|
				0xff000000; //opaque

	reloadsprite(13,bloccd);
}

