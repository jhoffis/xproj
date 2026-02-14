#ifndef MY_NUMS
#define MY_NUMS

#include <math.h>
#include <string.h>
#include <stdbool.h>
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /**< pi */
#endif

// 0, 0x3ffb000 means anywhere in the memory that is less than 64 mb.
// https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
#define MAX_MEM_64 0x3ffb000

// Unsigned integers
#define U16_MAX 65535
#define U32_MAX 4294967295
#define U8_MAX 255               // 2^8 - 1
#define U64_MAX 18446744073709551615ULL  // 2^64 - 1

// Signed integers
#define S8_MIN -128              // -2^7
#define S8_MAX 127               // 2^7 - 1
#define S16_MIN -32768           // -2^15
#define S16_MAX 32767            // 2^15 - 1
#define S32_MIN -2147483648      // -2^31
#define S32_MAX 2147483647       // 2^31 - 1
#define S64_MIN -9223372036854775808LL   // -2^63
#define S64_MAX 9223372036854775807LL    // 2^63 - 1

// Floating point
#define F32_MIN 1.175494e-38F    // Minimum positive normalized value
#define F32_MAX 3.402823e+38F    // Maximum value
#define F64_MIN 2.225074e-308    // Minimum positive normalized value
#define F64_MAX 1.797693e+308    // Maximum value

// Epsilon values (smallest positive increment)
#define F32_EPSILON 1.192093e-07F
#define F64_EPSILON 2.220446e-16

// Infinity values
#define F32_INFINITY __builtin_inff()
#define F64_INFINITY __builtin_inf()


#define MY_BIT_MASK(n)  (1U << (n)) - 1

typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long long u64;
typedef long long s64;
#pragma pack(1)
typedef struct {
    u64 high;
    u64 low;
} u128;
#pragma pack()

typedef struct {
    u32 x;
    u32 y;
} u32_v2;

typedef struct {
    s32 x;
    s32 y;
} s32_v2;

typedef struct {
    s16 x;
    s16 y;
} s16_v2;

typedef union {
    u16 u;
    struct {
        u16 mantissa : 10; // 10 bits for the mantissa (fraction), includes implicit leading 1
        u16 exponent : 5;  // 5 bits for the exponent
        u16 sign : 1;      // 1 bit for the sign
    } parts;
} f16;

_Static_assert(sizeof(f16)  == 2,  "Must be 2 bytes");
_Static_assert(sizeof(f32)  == 4,  "Must be 4 bytes");
_Static_assert(sizeof(f64)  == 8,  "Must be 8 bytes");
_Static_assert(sizeof(u8)   == 1,  "Must be 1 byte");
_Static_assert(sizeof(s8)   == 1,  "Must be 1 byte");
_Static_assert(sizeof(u16)  == 2,  "Must be 2 bytes");
_Static_assert(sizeof(s16)  == 2,  "Must be 2 bytes");
_Static_assert(sizeof(u32)  == 4,  "Must be 4 bytes");
_Static_assert(sizeof(s32)  == 4,  "Must be 4 bytes");
_Static_assert(sizeof(u64)  == 8,  "Must be 8 bytes");
_Static_assert(sizeof(s64)  == 8,  "Must be 8 bytes");
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

_Static_assert(sizeof(f32_v2) == 8, "Must be 8 bytes");
_Static_assert(sizeof(f32_v3) == 12, "Must be 12 bytes");
_Static_assert(sizeof(f32_v4) == 16, "Must be 16 bytes");
_Static_assert(sizeof(f32_m3x3) == 4*9, "Must be 36 bytes");
_Static_assert(sizeof(f32_m4x4) == 4*16, "Must be 64 bytes");

void mat4x4_copy(f32_m4x4 output, f32_m4x4 input0);
void mat4x4_multiply(f32_m4x4 output, f32_m4x4 input0, f32_m4x4 input1);
void mat4x4_unit(f32_m4x4 output);
void mat4x4_translate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1);
void mat4x4_rotate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1);
void create_world_view(f32_m4x4 world_view, f32_v4 translation, f32_v4 rotation);
void mat4x4_perspective(f32_m4x4 mat, f32 fov_degrees, f32 aspect, f32 near, f32 far);
void mat4x4_ortho(f32_m4x4 output, f32 left, f32 right, f32 bottom, f32 top);

static inline u8 fast_log2(u16 x) {
    if (x == 0) {
        return 0; // log2(0) is undefined, return 0 or handle as an error
    }
    return 15 - __builtin_clz(x); // 16-bit width: 15 - leading zeros
}

// https://www.shadertoy.com/view/M3ycWd
// Function to snap a vector to the nearest 45-degree angle or axis
#define SQRT2 1.41421356f
#define SQRT0_5 0.70710678f

static inline f32_v2 snap45(f32 x, f32 y) {
    // Scale vector to align with 45-degree grid using pre-calculated square root
    
    x = roundf(x * SQRT2);
    y = roundf(y * SQRT2);

    float sum_abs = (x >= 0 ? x : -x) + (y >= 0 ? y : -y);
    float scale = (sum_abs > 1.5f) ? SQRT0_5 : 1.0f;

    return (f32_v2){x * scale, y * scale};
}

static inline s32_v2 f32_v2_to_s32_v2(f32_v2 val) {
    return (s32_v2){ (s32)roundf(val.x), (s32)roundf(val.y) };
}

static inline s16_v2 f32_v2_to_s16_v2(f32_v2 val) {
    return (s16_v2){ (s16)roundf(val.x), (s16)roundf(val.y) };
}

static inline f32_v2 f32_v2_neg(f32_v2 v) {
    return (f32_v2) {-v.x, -v.y};
}

static inline bool f32_equals(f32 a, f32 b, f32 tolerance) {
    return fabsf(a - b) < tolerance;
}

static inline bool f32_v2_equals(f32_v2 a, f32_v2 b, f32 tolerance) {
    return f32_equals(a.x, b.x, tolerance) && f32_equals(a.y, b.y, tolerance);
}

static inline f32_v2 f32_v2_diff(f32_v2 a, f32_v2 b) {
    return (f32_v2) {a.x - b.x, a.y - b.y};
}

static inline f32 absf(f32 val) {
    return val < 0 ? -val : val;
}

static inline f32 f32_v2_distance(f32_v2 a, f32_v2 b) {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

static inline bool s16_v2_equals(s16_v2 a, s16_v2 b) {
    return a.x == b.x && a.y == b.y;
}

static inline f32 clampf(f32 x, f32 lo, f32 hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

bool move_towards(f32 movespd, f32_v2 dest, f32_v2 *pos);
bool is_close_to_point(const f32 range, const f32_v2 point, const f32_v2 input);


static inline s32 compare_s32s(const void *a, const void *b) {
    s32 ia = *(const s32 *)a;
    s32 ib = *(const s32 *)b;
    return (ia > ib) - (ia < ib);
}

static inline s32 find_s32(const void *a, const void *b) {
    return *(const s32 *)a - *(const s32 *)b;
}

// Fast DJB2 hash function for name lookup optimization
static inline u32 fast_hash(const char *str) {
    u32 hash = 5381;
    u32 c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

static inline u32 simple_hash(const char *str) {
    size_t len = strlen(str);

    if (len < 16) return fast_hash(str);

    u32 hash = 5381;
    size_t chunk_end = len & ~15;
    for (size_t i = 0; i < chunk_end; i += 16) {
        hash = ((hash << 5) + hash) + (u32)str[i];
        hash = ((hash << 5) + hash) + (u32)str[i + 1];
        hash = ((hash << 5) + hash) + (u32)str[i + 2];
        hash = ((hash << 5) + hash) + (u32)str[i + 3];
        hash = ((hash << 5) + hash) + (u32)str[i + 4];
        hash = ((hash << 5) + hash) + (u32)str[i + 5];
        hash = ((hash << 5) + hash) + (u32)str[i + 6];
        hash = ((hash << 5) + hash) + (u32)str[i + 7];
        hash = ((hash << 5) + hash) + (u32)str[i + 8];
        hash = ((hash << 5) + hash) + (u32)str[i + 9];
        hash = ((hash << 5) + hash) + (u32)str[i + 10];
        hash = ((hash << 5) + hash) + (u32)str[i + 11];
        hash = ((hash << 5) + hash) + (u32)str[i + 12];
        hash = ((hash << 5) + hash) + (u32)str[i + 13];
        hash = ((hash << 5) + hash) + (u32)str[i + 14];
        hash = ((hash << 5) + hash) + (u32)str[i + 15];
    }
    for (size_t i = chunk_end; i < len; i++) {
        hash = ((hash << 5) + hash) + (u32)str[i];
    }
    return hash;
}

#define GET_SHIFT(mask) \
    _Generic((mask), \
        u64: __builtin_ctzll, \
        s64: __builtin_ctzll, \
        u32: __builtin_ctz, \
        s32: __builtin_ctz, \
        u16: __builtin_ctz, \
        s16: __builtin_ctz, \
        u8: __builtin_ctz, \
        s8: __builtin_ctz, \
        default: __builtin_ctz \
    )(mask)

// Set bits in value according to mask, with min/max clamping
// Returns true if no clamping occurred, false if value was clamped
#define SET_INT_VALUE(value, input, mask, min, max) ({ \
    const auto shift = GET_SHIFT(mask); \
    const auto mask_width = (mask) >> shift; \
    const auto original_input = (typeof(value))(input); \
    auto clamped_input = original_input; \
    bool was_clamped = false; \
    if (clamped_input < (min)) { clamped_input = (min); was_clamped = true; } \
    if (clamped_input > (max)) { clamped_input = (max); was_clamped = true; } \
    const auto input_masked = clamped_input & mask_width; \
    (value) = ((value) & ~(mask)) | (input_masked << shift); \
    !was_clamped; \
})

// Get bits from value according to mask
#define GET_INT_VALUE(value, mask) \
    (((value) & (mask)) >> GET_SHIFT(mask))

// Set a boolean bit (convenience macro for SET_INT_VALUE with 0/1 bounds)
#define SET_BOOL_VALUE(value, input, mask) SET_INT_VALUE((value), (input), (mask), 0, 1)
#define ZERO_VALUE(value, mask) ((value) &= ~(mask))

// Add to bits in value according to mask, with maximum clamp
#define ADD_INT_VALUE(value, input, mask, max) do { \
    const auto shift = GET_SHIFT(mask); \
    const auto mask_width = (mask) >> shift; \
    const auto current = GET_INT_VALUE(value, mask); \
    if (current >= (max)) break; \
    auto new_val = current + (input); \
    if (new_val > (max)) new_val = (max); \
    (value) = ((value) & ~(mask)) | ((new_val & mask_width) << shift); \
} while(0)

// Subtract from bits in value according to mask, with minimum clamp
#define SUB_INT_VALUE(value, input, mask, min) do { \
    const auto shift = GET_SHIFT(mask); \
    const auto mask_width = (mask) >> shift; \
    const auto current = GET_INT_VALUE(value, mask); \
    auto new_val = current; \
    if ((input) > current) { \
        new_val = (min); \
    } else { \
        new_val = current - (input); \
        if (new_val < (min)) new_val = (min); \
    } \
    (value) = ((value) & ~(mask)) | ((new_val & mask_width) << shift); \
} while(0)

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
