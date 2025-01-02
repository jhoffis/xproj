//port of ooPo's ps2sdk math3d library
#pragma once
#include "nums.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

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
f32_v3 vec3_normalize(f32_v3 v);

// Multiply a 3x3 matrix by a 3D vector
f32_v3 vec3_multiply_mat3x3(f32_m3x3 matrix, f32_v3 vector);

// Compute the dot product of two vectors
f32 vec3_dot_product(f32_v3 a, f32_v3 b);

// Compare the direction of two vectors
i32 vec3_is_same_direction(f32_v3 a, f32_v3 b, float threshold);

f32 vec3_distance(f32 *a, f32 *b);

f32_v3 vec3_subtract(f32 *a, f32 *b);

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

// Convert Euler angles (yaw, pitch, roll in radians) to a 3x3 rotation matrix
void mat3x3_euler_to_rotation_matrix(f32_m3x3 rotation_matrix, f32 pitch, f32 yaw, f32 roll);

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
