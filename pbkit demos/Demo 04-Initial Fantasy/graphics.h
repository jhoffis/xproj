#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

void prepare_shaders(void);
void antialiased_texture_copy(void);
void image_bitblt(void);
void set_source_extra_buffer(int n);
void set_filters_on(void);
void set_filters_off(void);
void set_transparency_on(void);
void set_transparency_off(void);

#endif

