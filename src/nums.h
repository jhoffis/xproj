#ifndef MY_NUMS
#define MY_NUMS

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /**< pi */
#endif

// 0, 0x3ffb000 means anywhere in the memory that is less than 64 mb.
// https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
#define MAX_MEM_64 0x3ffb000

#include <stdbool.h>

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
#pragma pack(1)
typedef struct {
    u64 high;
    u64 low;
} u128;
#pragma pack()
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
_Static_assert(sizeof(u128) == 16, "Must be 16 bytes");

#endif // MY_NUMS
