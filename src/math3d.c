//port of ooPo's ps2sdk math3d library

#include "math3d.h"

unsigned long times(void *);
#define cpu_ticks() times(0)



//vector functions


// Normalize a 3D vector
f32_v3 vec3_normalize(f32_v3 v) {
    f32 magnitude = 1.0f / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (f32_v3){v.x * magnitude, v.y * magnitude, v.z * magnitude};
}

// Multiply a 3x3 matrix by a 3D vector
f32_v3 vec3_multiply_mat3x3(f32_m3x3 matrix, f32_v3 vector) {
    return (f32_v3){
        matrix[0] * vector.x + matrix[1] * vector.y + matrix[2] * vector.z,
        matrix[3] * vector.x + matrix[4] * vector.y + matrix[5] * vector.z,
        matrix[6] * vector.x + matrix[7] * vector.y + matrix[8] * vector.z
    };
}

// Compute the dot product of two vectors
f32 vec3_dot_product(f32_v3 a, f32_v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Compare the direction of two vectors
i32 vec3_is_same_direction(f32_v3 a, f32_v3 b, float threshold) {
    // Normalize both vectors
    a = vec3_normalize(a);
    b = vec3_normalize(b);

    // Compute the dot product
    float dot = vec3_dot_product(a, b);

    // Check if the angle between them is within the threshold
    return dot >= threshold;  // Cosine similarity threshold (e.g., 0.9 for ~25°)
}

f32 vec3_distance(f32 *a, f32 *b) {
    f32 x = b[0] - a[0];
    f32 y = b[1] - a[1];
    f32 z = b[2] - a[2];
    return sqrtf(x*x + y*y + z*z);
}

f32_v3 vec3_subtract(f32 *a, f32 *b) {
    return (f32_v3){a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}


void vec4_mul_left_matrix(f32_v4 vec, f32_m4x4 mat) {    
    // Cache the input vector since we'll overwrite it
    const f32 x = vec.x, y = vec.y, z = vec.z, w = vec.w;
    
    // Direct calculation avoiding loops and temporary array
    vec.x = x * mat[0] + y * mat[4] + z * mat[8]  + w * mat[12];
    vec.y = x * mat[1] + y * mat[5] + z * mat[9]  + w * mat[13];
    vec.z = x * mat[2] + y * mat[6] + z * mat[10] + w * mat[14];
    vec.w = x * mat[3] + y * mat[7] + z * mat[11] + w * mat[15];
}

void vec4_mul_right_matrix(f32_v4 vec, f32_m4x4 mat) {    
    // Cache the input vector since we'll overwrite it
    const f32 x = vec.x, y = vec.y, z = vec.z, w = vec.w;

    vec.x = x * mat[0]  + y * mat[1]  + z * mat[2]  + w * mat[3];
    vec.y = x * mat[4]  + y * mat[5]  + z * mat[6]  + w * mat[7];
    vec.z = x * mat[8]  + y * mat[9]  + z * mat[10] + w * mat[11];
    vec.w = x * mat[12] + y * mat[13] + z * mat[14] + w * mat[15];
}

void vec4_copy(f32_v4 *output, f32_v4 *input0) {
    memcpy(output, input0, sizeof(f32_v4));
}

f32_v4 vec4_copy_clamp(f32_v4 input0, f32 min, f32 max) {
    f32_v4 work;

    // Copy the vector.
    vec4_copy(&work, &input0);

    // Clamp the minimum values.
    if (work.x < min) { work.x = min; }
    if (work.y < min) { work.y = min; }
    if (work.z < min) { work.z = min; }
    if (work.w < min) { work.w = min; }

    // Clamp the maximum values.
    if (work.x > max) { work.x = max; }
    if (work.y > max) { work.y = max; }
    if (work.z > max) { work.z = max; }
    if (work.w > max) { work.w = max; }

    // Output the result.
    return work;
}

f32 vec4_copy_innerproduct(f32_v4 input0, f32_v4 input1) {
    f32_v4 work0, work1;

    // Normalize the first vector.
    work0.x = (input0.x / input0.w);
    work0.y = (input0.y / input0.w);
    work0.z = (input0.z / input0.w);
    work0.w = 1.00f;

    // Normalize the second vector.
    work1.x = (input1.x / input1.w);
    work1.y = (input1.y / input1.w);
    work1.z = (input1.z / input1.w);
    work1.w = 1.00f;

    // Return the inner product.
    return (work0.x * work1.x) + (work0.y * work1.y) + (work0.z * work1.z);
}

f32_v4 vec4_copy_multiply(f32_v4 input0, f32_v4 input1) {
    f32_v4 work;

    // Multiply the vectors together.
    work.x = input0.x * input1.x;
    work.y = input0.y * input1.y;
    work.z = input0.z * input1.z;
    work.w = input0.w * input1.w;

    // Output the result.
    return work;
}

f32_v4 vec4_copy_normalize(f32_v4 v)
{
    f32 magnitude = 1.0f / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (f32_v4){v.x * magnitude, v.y * magnitude, v.z * magnitude, v.w};
}

f32_v4 vec4_copy_outerproduct(f32_v4 input0, f32_v4 input1)
{
    f32_v4 work;

    work.x = input0.y * input1.z - input0.z * input1.y;
    work.y = input0.z * input1.x - input0.x * input1.z;
    work.z = input0.x * input1.y - input0.y * input1.x;

    // Output the result.
    return work;
}

//matrices function

// Convert Euler angles (yaw, pitch, roll in radians) to a 3x3 rotation matrix
void mat3x3_euler_to_rotation_matrix(f32_m3x3 rotation_matrix, f32 pitch, f32 yaw, f32 roll) {
    f32 cy = cos(yaw), sy = sin(yaw);
    f32 cp = cos(pitch), sp = sin(pitch);
    f32 cr = cos(roll), sr = sin(roll);

    memcpy(rotation_matrix, (f32_m3x3) {
        cy * cr + sy * sp * sr, cr * sy * sp - cy * sr, cp * sy,
        cp * sr,                cp * cr,               -sp     ,
        cy * sp * sr - cr * sy, cy * cr * sp + sr * sy, cy * cp
    }, sizeof(f32_m3x3));
}


void matrix_copy(f32_m4x4 output, f32_m4x4 input0) 
{
    memcpy(output, input0, sizeof(f32_m4x4));
}

void matrix_inverse(f32_m4x4 output, f32_m4x4 input0) 
{
    f32_m4x4 work;

    // Calculate the inverse of the matrix.
    matrix_transpose(work, input0);
    work[_14] = 0.00f;
    work[_24] = 0.00f;
    work[_34] = 0.00f;
    work[_41] = -(input0[_41] * work[_11] + input0[_42] * work[_21] + input0[_43] * work[_31]);
    work[_42] = -(input0[_41] * work[_12] + input0[_42] * work[_22] + input0[_43] * work[_32]);
    work[_43] = -(input0[_41] * work[_13] + input0[_42] * work[_23] + input0[_43] * work[_33]);
    work[_44] = 1.00f;

    // Output the result.
    matrix_copy(output, work);
}

void matrix_multiply(f32_m4x4 output, f32_m4x4 input0, f32_m4x4 input1)
{
    f32_m4x4 work;
    
    work[_11] = input0[_11]*input1[_11] + input0[_12]*input1[_21] + input0[_13]*input1[_31] + input0[_14]*input1[_41];
    work[_12] = input0[_11]*input1[_12] + input0[_12]*input1[_22] + input0[_13]*input1[_32] + input0[_14]*input1[_42];
    work[_13] = input0[_11]*input1[_13] + input0[_12]*input1[_23] + input0[_13]*input1[_33] + input0[_14]*input1[_43];
    work[_14] = input0[_11]*input1[_14] + input0[_12]*input1[_24] + input0[_13]*input1[_34] + input0[_14]*input1[_44];
    work[_21] = input0[_21]*input1[_11] + input0[_22]*input1[_21] + input0[_23]*input1[_31] + input0[_24]*input1[_41];
    work[_22] = input0[_21]*input1[_12] + input0[_22]*input1[_22] + input0[_23]*input1[_32] + input0[_24]*input1[_42];
    work[_23] = input0[_21]*input1[_13] + input0[_22]*input1[_23] + input0[_23]*input1[_33] + input0[_24]*input1[_43];
    work[_24] = input0[_21]*input1[_14] + input0[_22]*input1[_24] + input0[_23]*input1[_34] + input0[_24]*input1[_44];
    work[_31] = input0[_31]*input1[_11] + input0[_32]*input1[_21] + input0[_33]*input1[_31] + input0[_34]*input1[_41];
    work[_32] = input0[_31]*input1[_12] + input0[_32]*input1[_22] + input0[_33]*input1[_32] + input0[_34]*input1[_42];
    work[_33] = input0[_31]*input1[_13] + input0[_32]*input1[_23] + input0[_33]*input1[_33] + input0[_34]*input1[_43];
    work[_34] = input0[_31]*input1[_14] + input0[_32]*input1[_24] + input0[_33]*input1[_34] + input0[_34]*input1[_44];
    work[_41] = input0[_41]*input1[_11] + input0[_42]*input1[_21] + input0[_43]*input1[_31] + input0[_44]*input1[_41];
    work[_42] = input0[_41]*input1[_12] + input0[_42]*input1[_22] + input0[_43]*input1[_32] + input0[_44]*input1[_42];
    work[_43] = input0[_41]*input1[_13] + input0[_42]*input1[_23] + input0[_43]*input1[_33] + input0[_44]*input1[_43];
    work[_44] = input0[_41]*input1[_14] + input0[_42]*input1[_24] + input0[_43]*input1[_34] + input0[_44]*input1[_44];

    // Output the result.
    matrix_copy(output, work);
}

void matrix_rotate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1) 
{
    f32_m4x4 work;

    // Apply the z-axis rotation.
    matrix_unit(work);
    work[_11] =  cosf(input1.z);
    work[_12] =  sinf(input1.z);
    work[_21] = -sinf(input1.z);
    work[_22] =  cosf(input1.z);
    matrix_multiply(output, input0, work);
    
    // Apply the y-axis rotation.
    matrix_unit(work);
    work[_11] =  cosf(input1.y);
    work[_13] = -sinf(input1.y);
    work[_31] =  sinf(input1.y);
    work[_33] =  cosf(input1.y);
    matrix_multiply(output, output, work);

    // Apply the x-axis rotation.
    matrix_unit(work);
    work[_22] =  cosf(input1.x);
    work[_23] =  sinf(input1.x);
    work[_32] = -sinf(input1.x);
    work[_33] =  cosf(input1.x);
    matrix_multiply(output, output, work);
}

void matrix_scale(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1) 
{
    f32_m4x4 work;

    // Apply the scaling.
    matrix_unit(work);
    work[_11] = input1.x;
    work[_22] = input1.y;
    work[_33] = input1.z;
    matrix_multiply(output, input0, work);
}

void matrix_translate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1) 
{
    f32_m4x4 work;

    // Apply the translation.
    matrix_unit(work);
    work[_41] = input1.x;
    work[_42] = input1.y;
    work[_43] = input1.z;
    matrix_multiply(output, input0, work);
}

void matrix_transpose(f32_m4x4 output, f32_m4x4 input0) 
{
    f32_m4x4 work;

    // Transpose the matrix.
    work[_11] = input0[_11];
    work[_12] = input0[_21];
    work[_13] = input0[_31];
    work[_14] = input0[_41];
    work[_21] = input0[_12];
    work[_22] = input0[_22];
    work[_23] = input0[_32];
    work[_24] = input0[_42];
    work[_31] = input0[_13];
    work[_32] = input0[_23];
    work[_33] = input0[_33];
    work[_34] = input0[_43];
    work[_41] = input0[_14];
    work[_42] = input0[_24];
    work[_43] = input0[_34];
    work[_44] = input0[_44];

    // Output the result.
    matrix_copy(output, work);
}

void matrix_unit(f32_m4x4 output) 
{
    // Create a unit matrix.
    memset(output, 0, sizeof(f32_m4x4));
    output[_11] = 1.00f;
    output[_22] = 1.00f;
    output[_33] = 1.00f;
    output[_44] = 1.00f;
}

//creation functions

void create_local_world(f32_m4x4 local_world, f32_v4 translation, f32_v4 rotation)
{
    // Create the local_world matrix.
    matrix_unit(local_world);
    matrix_rotate(local_world, local_world, rotation);
    matrix_translate(local_world, local_world, translation);
}

void create_local_light(f32_m4x4 local_light, f32_v4 rotation) 
{
    // Create the local_light matrix.
    matrix_unit(local_light);
    matrix_rotate(local_light, local_light, rotation);
}


void create_world_view(f32_m4x4 world_view, f32_v4 translation, f32_v4 rotation) 
{
    f32_v4 work0, work1;

    // Reverse the translation.
    work0.x = -translation.x;
    work0.y = -translation.y;
    work0.z = -translation.z;
    work0.w =  translation.w;

    // Reverse the rotation.
    work1.x = -rotation.x;
    work1.y = -rotation.y;
    work1.z = -rotation.z;
    work1.w =  rotation.w;

    // Create the world_view matrix.
    matrix_unit(world_view);
    matrix_translate(world_view, world_view, work0);
    matrix_rotate(world_view, world_view, work1);
}

void create_view_screen(f32_m4x4 view_screen, f32 aspect, f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) 
{
/* We want to create a matrix that transforms 
   field of view frustum (a truncated pyramid)
   into a normalized cuboid (for fast hardware clipping):
    w,  0,  0,          0,
    0,  -h, 0,          0,
    0,  0,  (f+n) / (f-n),      -1,
    0,  0,  (2*f*n) / (f-n),    0
   (w:width,h:height,n:z near,f:z far)
*/

    // Apply the aspect ratio adjustment.
    left = (left * aspect); right = (right * aspect);

    // Create the view_screen matrix.
/*  matrix_unit(view_screen);
    view_screen[_11] = (2 * near) / (right - left);
    view_screen[_22] = (2 * near) / (top - bottom);
    view_screen[_31] = (right + left) / (right - left);
    view_screen[_32] = (top + bottom) / (top - bottom);
    view_screen[_33] = (far + near) / (far - near);
    view_screen[_34] = -1.00f;
    view_screen[_43] = (2 * far * near) / (far - near);
    view_screen[_44] = 0.00f;

    //This is good for ps2 clipping, where pixel is considered visible if:
    //-w < x < w
    //-w < y < w
    //-w < z < w
    //It's not automatic, it's done by using 'clipw' and testing flags in vu1 code
    //Result of the test allows to exclude entire triangle
*/
    
    //For xbox1 clipping, pixel is considered visible if:
    //-w < x < w
    //-w < y < w
    // 0 < z < w
    //It's automatic and verified for each pixel before pixel shader is called

    //so we need this :
    matrix_unit(view_screen);
    // view_screen[_11] = (2 * near) / (right - left);
    // view_screen[_22] = (2 * near) / (top - bottom);
    // view_screen[_31] = - (right + left) / (right - left);
    // view_screen[_32] = - (top + bottom) / (top - bottom);
    // view_screen[_33] = - far / (far - near);
    // view_screen[_34] = - 1.00f;
    // view_screen[_43] = near * far / (far - near);
    // view_screen[_44] = 0.00f;
    view_screen[_11] = (2 * near) / (right - left);
    view_screen[_22] = (2 * near) / (top - bottom);
    view_screen[_31] = (right + left) / (right - left);
    view_screen[_32] = (top + bottom) / (top - bottom);
    view_screen[_33] = (far + near) / (far - near);
    view_screen[_34] = -1.00f;
    view_screen[_43] = (2 * far * near) / (far - near);
    view_screen[_44] = 0.00f;
}

void create_local_screen(f32_m4x4 local_screen, f32_m4x4 local_world, f32_m4x4 world_view, f32_m4x4 view_screen) 
{
    // Create the local_screen matrix.
    matrix_unit(local_screen);
    matrix_multiply(local_screen, local_screen, local_world);
    matrix_multiply(local_screen, local_screen, world_view);
    matrix_multiply(local_screen, local_screen, view_screen);
}

