#ifndef WEFX_3D__H
#define WEFX_3D__H

#include <stdint.h>
#include <math.h>
#include "wefx.h"

#define R2_MATHS_IMPLEMENTATION
#include "r2_maths.h"

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

typedef struct vertex
{
    vec2 vec;
    f32 u,v;
} vertex;


vec2 rotate_vec(vec2 v, vec2 c, f32 angle);

mat4 make_perspective(f32 fov, f32 aspect, f32 znear, f32 zfar);

void draw_scene(i32 time, i32 W, i32 H, ui8* image);

#endif // WEFX_3D__H
