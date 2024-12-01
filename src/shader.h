#ifndef MY_SHADERS
#define MY_SHADERS
#include "nums.h"

void init_shader(i32 which);
void set_attrib_pointer(u32 index, u32 format, u32 size, u32 stride, const void* data);
void draw_arrays(u32 mode, i32 start, i32 count);


#endif // MY_SHADERS
