#ifndef WEFX_3D__H
#define WEFX_3D__H

#include <stdint.h>
#include <math.h>
#include "wefx.h"

#ifndef EPSILON
#define EPSILON 0.000000954
#endif

#ifndef M_PI
// clang-format off
#define M_PI     3.141592653589
#define M_PI2    6.283185307179
#define M_PI_SQR 9.869604401089
#define M_PID2   1.570796326794
#define M_3PID2  4.712388980384
// clang-format on
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef int32_t i32;
typedef uint32_t ui32;
typedef int16_t i16;
typedef uint16_t ui16;
typedef int8_t i8;
typedef uint8_t ui8;
typedef float f32;
// typedef unsigned char ui8

typedef struct vector
{
    i32 x, y;
} vector;

typedef struct vertex
{
    vector vec;
    f32 u,v;
} vertex;


/**
 * 4x4 Matrix backed by a flat array.
 * Access the array with ->a_mat4 or use the format:
 *   m<row><col>
 * where row and column are zero based
 *
 * e.g. m4->m22 for the 3rd row, 3rd column value
 * m4->a_mat4 for the array with values in order
 */
typedef union u_mat4 {
    f32 a_mat4[16];
    struct
    {
        // clang-format off
        f32 m00; f32 m10; f32 m20; f32 m30; // 16
        f32 m01; f32 m11; f32 m21; f32 m31; // 16
        f32 m02; f32 m12; f32 m22; f32 m32; // 16
        f32 m03; f32 m13; f32 m23; f32 m33; // 16 -- 64
        // clang-format on
    };
} mat4;


vector rotate_vec(vector v, vector c, f32 angle);

mat4 make_perspective(f32 fov, f32 aspect, f32 znear, f32 zfar);

char *mat4_tos(const mat4 *m);

void draw_scene(i32 time, i32 W, i32 H, ui8* image);

#endif // WEFX_3D__H
