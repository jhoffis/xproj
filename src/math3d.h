//port of ooPo's ps2sdk math3d library
#pragma once
#include "nums.h"
#include <math.h>

//vector indices
#define	_X					0
#define	_Y					1
#define	_Z					2
#define	_W					3

//4x4 matrices indices
#define	_11					0
#define	_12					1
#define	_13					2
#define	_14					3
#define	_21					4
#define	_22					5
#define	_23					6
#define	_24					7
#define	_31					8
#define	_32					9
#define	_33					10
#define	_34					11
#define	_41					12
#define	_42					13
#define	_43					14
#define	_44					15


//vector functions


// Normalize a 3D vector
static f32_v3 normalize(f32_v3 v) {
    f32 magnitude = 1.0f / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (f32_v3){v.x * magnitude, v.y * magnitude, v.z * magnitude};
}

// Convert Euler angles (yaw, pitch, roll in radians) to a 3x3 rotation matrix
static void euler_to_rotation_matrix(f32_m3x3 rotation_matrix, f32 pitch, f32 yaw, f32 roll) {
    f32 cy = cos(yaw), sy = sin(yaw);
    f32 cp = cos(pitch), sp = sin(pitch);
    f32 cr = cos(roll), sr = sin(roll);

    memcpy(rotation_matrix, (f32_m3x3) {
        cy * cr + sy * sp * sr, cr * sy * sp - cy * sr, cp * sy,
        cp * sr,                cp * cr,               -sp     ,
        cy * sp * sr - cr * sy, cy * cr * sp + sr * sy, cy * cp
    }, sizeof(f32_m3x3));
}

// Multiply a 3x3 matrix by a 3D vector
static f32_v3 multiply_matrix_vector(f32_m3x3 matrix, f32_v3 vector) {
    return (f32_v3){
        matrix[0] * vector.x + matrix[1] * vector.y + matrix[2] * vector.z,
        matrix[3] * vector.x + matrix[4] * vector.y + matrix[5] * vector.z,
        matrix[6] * vector.x + matrix[7] * vector.y + matrix[8] * vector.z
    };
}

// Compute the dot product of two vectors
static f32 dot_product(f32_v3 a, f32_v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Compare the direction of two vectors
static i32 is_same_direction(f32_v3 a, f32_v3 b, float threshold) {
    // Normalize both vectors
    a = normalize(a);
    b = normalize(b);

    // Compute the dot product
    float dot = dot_product(a, b);

    // Check if the angle between them is within the threshold
    return dot >= threshold;  // Cosine similarity threshold (e.g., 0.9 for ~25°)
}

static f32 distance_vec3(f32 *a, f32 *b) {
    f32 x = b[0] - a[0];
    f32 y = b[1] - a[1];
    f32 z = b[2] - a[2];
    return sqrtf(x*x + y*y + z*z);
}

static f32_v3 subtract(f32 *a, f32 *b) {
    return (f32_v3){a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

void vec4_mul_left_matrix(f32_v4 vec, f32_m4x4 mat);

void vec4_mul_right_matrix(f32_v4 vec, f32_m4x4 mat);

// Copy a vector.
void vec4_copy(f32_v4 *output, f32_v4 *input0);

// Clamp a vector's values by cutting them off at a minimum and maximum value.
f32_v4 vec4_copy_clamp(f32_v4 input0, f32 min, f32 max);

// Calculate the inner product of two vectors. Returns a scalar value.
f32 vec4_copy_innerproduct(f32_v4 input0, f32_v4 input1);

// Multiply two vectors together.
f32_v4 vec4_copy_multiply(f32_v4 input0, f32_v4 input1);

// Normalize a vector by determining its length and dividing its values by this value.
f32_v4 vec4_copy_normalize(f32_v4 input0);

// Calculate the outer product of two vectors.
f32_v4 vec4_copy_outerproduct(f32_v4 input0, f32_v4 input1);

//matrices functions

// Copy a matrix.
void matrix_copy(f32_m4x4 output, f32_m4x4 input0);

// Calculate the inverse of a matrix.
void matrix_inverse(f32_m4x4 output, f32_m4x4 input0);

// Multiply two matrices together.
void matrix_multiply(f32_m4x4 output, f32_m4x4 input0, f32_m4x4 input1);

// Create a rotation matrix and apply it to the specified input matrix.
void matrix_rotate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1);

// Create a scaling matrix and apply it to the specified input matrix.
void matrix_scale(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1);

// Create a translation matrix and apply it to the specified input matrix.
void matrix_translate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1);

// Transpose a matrix.
void matrix_transpose(f32_m4x4 output, f32_m4x4 input0);

// Create a unit matrix.
void matrix_unit(f32_m4x4 output);

//creation functions

// Create a local_world matrix given a translation and rotation.
// Commonly used to describe an object's position and orientation.
void create_local_world(f32_m4x4 local_world, f32_v4 translation, f32_v4 rotation);

// Create a local_light matrix given a rotation.
// Commonly used to transform an object's normals for lighting calculations.
void create_local_light(f32_m4x4 local_light, f32_v4 rotation);

// Create a world_view matrix given a translation and rotation.
// Commonly used to describe a camera's position and rotation.
void create_world_view(f32_m4x4 world_view, f32_v4 translation, f32_v4 rotation);

// Create a view_screen matrix given an aspect and clipping plane values.
// Functionally similar to the opengl function: glFrustum()
void create_view_screen(f32_m4x4 view_screen, f32 aspect, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

// Create a local_screen matrix given a local_world, world_view and view_screen matrix.
// Commonly used with vector_apply() to transform vertices for rendering.
void create_local_screen(f32_m4x4 local_screen, f32_m4x4 local_world, f32_m4x4 world_view, f32_m4x4 view_screen);
