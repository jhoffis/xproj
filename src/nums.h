#ifndef MY_NUMS
#define MY_NUMS

typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef char i8;
typedef unsigned short u16;
typedef short i16;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long long u64;
typedef long long i64;
_Static_assert(sizeof(f32) == 4, "Must be 4 bytes");
_Static_assert(sizeof(f64) == 8, "Must be 8 bytes");
_Static_assert(sizeof(u8) == 1, "Must be 1 byte");
_Static_assert(sizeof(i8) == 1, "Must be 1 byte");
_Static_assert(sizeof(u16) == 2, "Must be 2 bytes");
_Static_assert(sizeof(i16) == 2, "Must be 2 bytes");
_Static_assert(sizeof(u32) == 4, "Must be 4 bytes");
_Static_assert(sizeof(i32) == 4, "Must be 4 bytes");
_Static_assert(sizeof(u64) == 8, "Must be 8 bytes");
_Static_assert(sizeof(i64) == 8, "Must be 8 bytes");
#define MAX_MEM_64 0x3ffb000

#endif // MY_NUMS
