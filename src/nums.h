#ifndef MY_NUMS
#define MY_NUMS

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /**< pi */
#endif

// 0, 0x3ffb000 means anywhere in the memory that is less than 64 mb.
// https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
#define MAX_MEM_64 0x3ffb000

#include <stdbool.h>

#define MY_BIT_MASK(n)  (1U << (n)) - 1

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

typedef union {
    u16 u;
    struct {
        u16 mantissa : 10; // 10 bits for the mantissa (fraction), includes implicit leading 1
        u16 exponent : 5;  // 5 bits for the exponent
        u16 sign : 1;      // 1 bit for the sign
    } parts;
} f16;

_Static_assert(sizeof(f16) == 2, "Must be 2 bytes");
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

typedef struct {
    f32 x;
    f32 y;
} f32_v2;
typedef struct {
    f32 x;
    f32 y;
    f32 z;
} f32_v3;
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} f32_v4;
typedef f32 f32_m3x3[9];
typedef f32 f32_m4x4[16];

// // Define the structure for a 16-bit float
// // Convert float to half float
// f16 float_to_half(float f) {
//     f16 hf;
//     u32 f_bits = *(u32*)&f;
//     u32 sign = (f_bits >> 31) & 0x1;
//     u32 exp = (f_bits >> 23) & 0xFF;
//     u32 mant = (f_bits & 0x7FFFFF);
//     
//     // Handle simple cases first
//     if (exp == 0xFF) { // Infinity or NaN
//         hf.parts.sign = sign;
//         hf.parts.exponent = 0x1F;
//         hf.parts.mantissa = (mant == 0) ? 0 : 1;
//         return hf;
//     }
//     
//     if (exp == 0 && mant == 0) { // Zero
//         hf.u = 0;
//         return hf;
//     }
//
//     // Normalize the mantissa for non-zero numbers
//     int new_exp = exp - 127 + 15; // Bias adjustment
//     if (new_exp >= 0x1F) { // Overflow to infinity
//         hf.parts.sign = sign;
//         hf.parts.exponent = 0x1F;
//         hf.parts.mantissa = 0;
//         return hf;
//     }
//
//     if (new_exp <= 0) { // Underflow to zero or denormals (not fully implemented)
//         if (new_exp <= -10) { // Underflow to zero
//             hf.u = 0;
//             return hf;
//         }
//         // Here you would handle denormals if needed, but for simplicity:
//         hf.parts.sign = sign;
//         hf.parts.exponent = 0;
//         hf.parts.mantissa = (mant | 0x800000) >> (1 - new_exp); // Simplified, not precise
//         return hf;
//     }
//
//     // Normal case
//     hf.parts.sign = sign;
//     hf.parts.exponent = new_exp;
//     hf.parts.mantissa = (mant >> 13) & 0x3FF; // Keep only 10 bits of mantissa
//
//     return hf;
// }
//
// // Convert half float back to float (simplified for normal numbers)
// f32 half_to_float(f16 hf) {
//     u32 sign = hf.parts.sign << 31;
//     u32 exp = ((hf.parts.exponent == 0) ? 0 : (hf.parts.exponent - 15 + 127)) << 23;
//     u32 mant = hf.parts.mantissa << 13; // Add implicit leading 1
//
//     u32 f_bits = sign | exp | mant;
//     return *(f32*)&f_bits;
// }

#endif // MY_NUMS
