/*
    * https://joshbeam.com/articles/triangle_rasterization/
    * https://www.cs.drexel.edu/~deb39/Classes/Papers/comp175-06-pineda.pdf
    * https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
    * https://jtsorlinis.github.io/rendering-tutorial/
 */
#include "3d.h"
#include "r2_maths.h"

#define PIXEL_SIZE 1

/* Are we looking at a triangle like:
    B    C
    ----|
    \   |
     \  |
      \ |
       A
  (to prevent edge overdraw)
*/
i8 is_top_left(vec2 *s, vec2 *e)
{
    vec2 edge = { e->x - s->x, e->y - s->y};
    return (edge.y == 0 && edge.x > 0) || edge.y > 0;
}

/* Clockwise
         B
        /\
       /  \
      /    \
    A ----- C
*/
void triangle_fill(vertex A, vertex B, vertex C, const texture* tex)
{
    // Calculate the edge function for the whole triangle (ABC)
    // pointing towards the center basically
    f32 ABC = vec2_edge_cross(&A.vec, &B.vec, &C.vec);
    if(ABC < 0) {
         return;
    }

    // This will squeeze everything towards the center of the triangle a bit
    // if it has a left/top bias - which will help with overlapping overdraw
    f32 bias0 = is_top_left(&A.vec, &B.vec) ? 0 : -1;
    f32 bias1 = is_top_left(&B.vec, &C.vec) ? 0 : -1;
    f32 bias2 = is_top_left(&C.vec, &A.vec) ? 0 : -1;

    f32 minX = MIN(MIN(A.vec.x, B.vec.x), C.vec.x);
    f32 minY = MIN(MIN(A.vec.y, B.vec.y), C.vec.y);
    f32 maxX = MAX(MAX(A.vec.x, B.vec.x), C.vec.x);
    f32 maxY = MAX(MAX(A.vec.y, B.vec.y), C.vec.y);

#ifdef DEBUG_BOX_TRIANGLE
    wefx_set_psize(1);
    wefx_color(0xff, 0xff, 0xff);
    wefx_rect(minX, maxY, maxX, minY);
#endif

    wefx_set_psize(PIXEL_SIZE);
    vertex SP = (vertex){ .vec={0, 0, 0, 1}, .u=0, .v=0};
    for (SP.vec.y = minY; SP.vec.y < maxY; SP.vec.y+=PIXEL_SIZE)
    {
        for (SP.vec.x = minX; SP.vec.x < maxX; SP.vec.x+=PIXEL_SIZE)
        {
            f32 AB_P = vec2_edge_cross(&A.vec, &B.vec, &SP.vec) + bias0;
            f32 BC_P = vec2_edge_cross(&B.vec, &C.vec, &SP.vec) + bias1;
            f32 CA_P = vec2_edge_cross(&C.vec, &A.vec, &SP.vec) + bias2;

            // Our graphics system has 0,0 on the bottom left not the
            // top right
            if (AB_P > 0 && BC_P > 0 && CA_P > 0)
            {
                // Normalise the edge functions by dividing by the total
                // area to get the barycentric coordinates. The weights used
                // to interplate the points in the triangle
                f32 wA = (BC_P / ABC); // alpha
                f32 wB = (CA_P / ABC); // beta
                f32 wG = (AB_P / ABC); // gamma

#ifdef DEBUG_UV_TRIANGLE
                ui8 r = (255.0 * wA) + (000.0 * wB) + (000.0 * wG);
                ui8 g = (000.0 * wA) + (255.0 * wB) + (000.0 * wG);
                ui8 b = (000.0 * wA) + (000.0 * wB) + (255.0 * wG);
                wefx_color(r, g, b);
#else
                f32 x_img = ((A.u * wA) + (B.u * wB) + (C.u * wG));
                f32 y_img = ((A.v * wA) + (B.v * wB) + (C.v * wG));

                ui32 pixx = -(tex->w * x_img);
                ui32 pixy = (tex->h * y_img);

                i32 pix = (int) ((pixx + pixy * tex->w) * 3);
                ui8 r = tex->image[pix+0];
                ui8 g = tex->image[pix+1];
                ui8 b = tex->image[pix+2];
                wefx_color(r, g, b);
#endif
                wefx_pixel(SP.vec.x, SP.vec.y);
            }
        }
    }
}

void draw_scene(mat4* screenM, state* state)
{
    // mat4 m = make_perspective(90.0, H/W, 1, 20);
    // perspective divide
    // char *str = mat4_tos(&m);
    // printf("%s\n", str);
    // free(str);

    // f32 angle = time * .1;
    // vec2 center = {W>>1, H>>1};
    // vertex v0 = {rotate_vec(tri[0].vec, center, angle), 1, 1};
    // vertex v1 = {rotate_vec(tri[1].vec, center, angle), 0, 1};
    // vertex v2 = {rotate_vec(tri[2].vec, center, angle), 1, 0};

    wefx_clear();

    vec4 A = {0};
    vec4 B = {0};
    vec4 C = {0};

    mat4_transform(&state->entities.mesh[ID_PLAYER].tris->v[0].vec, screenM, &A);
    mat4_transform(&state->entities.mesh[ID_PLAYER].tris->v[1].vec, screenM, &B);
    mat4_transform(&state->entities.mesh[ID_PLAYER].tris->v[2].vec, screenM, &C);
    // printf("A: %f %f %f\n", A.x, A.y, A.z);
    // printf("B: %f %f %f\n", B.x, B.y, B.z);
    // printf("C: %f %f %f\n\n", C.x, C.y, C.z);

    triangle_fill(
        (vertex){ .vec=A, .u=state->entities.mesh[ID_PLAYER].tris->v[0].u, .v=state->entities.mesh[ID_PLAYER].tris->v[0].v},
        (vertex){ .vec=B, .u=state->entities.mesh[ID_PLAYER].tris->v[1].u, .v=state->entities.mesh[ID_PLAYER].tris->v[1].v},
        (vertex){ .vec=C, .u=state->entities.mesh[ID_PLAYER].tris->v[2].u, .v=state->entities.mesh[ID_PLAYER].tris->v[2].v},
        &state->entities.mesh[ID_PLAYER].material.tex
    );

    wefx_color(0xff, 0xff, 0xff);

    vec4 P = {{0, 0, 0, 1}};
    vec4 pt = {0};
    mat4_transform(&P, screenM, &pt);
    wefx_set_psize(4);
    wefx_pixel(pt.x, pt.y);
    wefx_set_psize(1);
}


// vec2 rotate_vec(vec2 v, vec2 c, f32 angle)
// {
//     vec2 rot = {0};
//     v.x -= c.x;
//     v.y -= c.y;

//     // these aren't ints
//     rot.x = v.x * cosf(angle) - v.y * sinf(angle);
//     rot.y = v.x * sinf(angle) + v.y * cosf(angle);

//     rot.x += c.x;
//     rot.y += c.y;

//     rot.x = round(rot.x);
//     rot.y = round(rot.y);

//     return rot;
// }

mat4 make_screenSpaceTransform(float hW, float hH)
{
    // If you look at the projection matrix calculations you will see that they
    // do not depend on the resolution of the framebuffer, but on the aspect ratio
    // defined by the width / height. This is because the output of the vertex
    // shader is in clip space which is an axis aligned cube with coordinates
    // ranging from (-1.0,-1.0,-1.0) to (+1.0,+1.0,+1.0). The clip space coordinates
    // are transformed to screen space coordinates using the viewport transform you
    // set with glViewport.

    mat4 out = {0};
    // clang-format off
    out.m00 = hW;  out.m10 = 0;   out.m20 = 0;  out.m30 = hW;
    out.m01 = 0;   out.m11 = -hH;  out.m21 = 0;  out.m31 = hH;
    out.m02 = 0;   out.m12 = 0;   out.m22 = 1;  out.m32 = 0;
    out.m03 = 0;   out.m13 = 0;   out.m23 = 0;  out.m33 = 1;
    // clang-format on
    return out;
}

// mat4 make_perspective(f32 fov, f32 aspect, f32 znear, f32 zfar)
// {
//     mat4 out = {0};

//     f32 Sx = aspect * (1 / tanf(fov /2));
//     f32 Sy = 1 / tanf(fov / 2);
//     f32 Sz = zfar / (zfar - znear);
//     f32 Pz = (-zfar * znear) / (zfar - znear);

//     // clang-format off
//     out.m00 = Sx; out.m10 = 0;  out.m20 = 0;  out.m30 = 0;
//     out.m01 = 0;  out.m11 = Sy; out.m21 = 0;  out.m31 = 0;
//     out.m02 = 0;  out.m12 = 0;  out.m22 = Sz; out.m32 = Pz;
//     out.m03 = 0;  out.m13 = 0;  out.m23 = 1; out.m33 = 0;
//     // clang-format on

//     return out;
// }

