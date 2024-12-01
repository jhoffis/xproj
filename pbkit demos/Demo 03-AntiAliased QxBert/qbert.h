#ifndef _QBERT_H_
#define _QBERT_H_


extern unsigned int pal[16];

struct s_bloc //where sprite is stored in extra buffer #1
{
	int f; //flag=0 means no sprite
	int x;
	int y;
	int w;
	int h;
};

//bitmaps
extern char *blocaa[];
extern char *blocab[];
extern char *blocac[];
extern char *blocad[];
extern char *blocba[];
extern char *blocbb[];
extern char *blocbc[];
extern char *blocbd[];
extern char *blocbe[];
extern char *blocbf[];
extern char *blocca[];
extern char *bloccb[];
extern char *bloccc[];
extern char *bloccd[];
extern char *blocda[];
extern char *blocdb[];
extern char *blocdc[];
extern char *blocea[];
extern char *bloceb[];
extern char *blocec[];
extern char *bloced[];
extern char *blocfa[];
extern char *blocfb[];
extern char *blocga[];
extern char *blokaa[];
extern char *blokab[];
extern char *blokba[];
extern char *blokca[];
extern char *blokcb[];
extern char *blokda[];
extern char *blokea[];
extern char *blokeb[];
extern char *blokfa[];
extern char *blokfb[];
extern char *blokga[];
extern char *blokgb[];


void prepare_sprites(void);	//in graphics.c
void prepare_texture_mapping(void);
void antialiased_texture_copy(void);
void image_bitblt(void);
void set_source_extra_buffer(int n);
void draw_sprite(struct s_bloc *pbloc,int dst_x,int dst_y);
void draw_pyramid(void);
void Spr(int x,int y,int mirrorflag,int n);
void set_filters_on(void);
void set_filters_off(void);
void set_transparency_on(void);
void set_transparency_off(void);
void updatecubecolor(void);
void updateliftcolor(void);


void View(void);		//in qbert.c
void afftext8(int x, int y, char *s);
void Afftime(void);
void Flip(void);
void Killed(int Nk);
void Qbert(void);
void Coilyf(void);
void Green_ball(void);
void Sam(void);
void Red_ball(void);
void Ugg_d(void);
void Ugg_g(void);
void Guide(int No,int Nk);
void Pos(int No);
void Coily2_1(void);
void Coily2_2(void);
void Sam2(void);
void Qbert2(void);
void Score(int Xo);
int randint(int n);
void Life(void);
void Sp(int No,int Xo,int Yo,int i,int n);


int game(void);		//in game.c

#endif

