/*
    * https://joshbeam.com/articles/triangle_rasterization/
    * https://www.cs.drexel.edu/~deb39/Classes/Papers/comp175-06-pineda.pdf
    * https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
    * https://jtsorlinis.github.io/rendering-tutorial/
 */
#include "3d.h"

#define PIXEL_SIZE 1


i32 edge_cross(vector *a, vector *b, vector *c)
{
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
};


/* Are we looking at a triangle like:
    B    C
    ----|
    \   |
     \  |
      \ |
       A
  (to prevent edge overdraw)
*/
i8 is_top_left(vector *s, vector *e)
{
    vector edge = { e->x - s->x, e->y - s->y};
    return (edge.y == 0 && edge.x > 0) || edge.y > 0;
}

/* Clockwise 
         B
        /\
       /  \
      /    \
    A ----- C
  and our drawing 0,0 is the bottom left (not top left)
*/
void triangle_fill(vertex A, vertex B, vertex C, ui8* image)
{
    // Calculate the edge function for the whole triangle (ABC)
    // pointing towards the center basically
    i32 ABC = -1 * edge_cross(&A.vec, &B.vec, &C.vec);
    if(ABC < 0) {
         return;
    }

    // This will squeeze everything towards the center of the triangle a bit 
    // if it has a left/top bias - which will help with overlapping overdraw
    i32 bias0 = is_top_left(&A.vec, &B.vec) ? 0 : -1;
    i32 bias1 = is_top_left(&B.vec, &C.vec) ? 0 : -1;
    i32 bias2 = is_top_left(&C.vec, &A.vec) ? 0 : -1;

    i32 minX = MIN(MIN(A.vec.x, B.vec.x), C.vec.x);
    i32 minY = MIN(MIN(A.vec.y, B.vec.y), C.vec.y);
    i32 maxX = MAX(MAX(A.vec.x, B.vec.x), C.vec.x);
    i32 maxY = MAX(MAX(A.vec.y, B.vec.y), C.vec.y);

#ifdef DEBUG_BOX_TRIANGLE
    wefx_color(0xff, 0xff, 0xff);
    wefx_rect(minX, maxY, maxX, minY, 1);
#endif

    vertex SP = (vertex){ {0, 0}, 0, 0};
    for (SP.vec.y = minY; SP.vec.y < maxY; SP.vec.y+=PIXEL_SIZE)
    {
        for (SP.vec.x = minX; SP.vec.x < maxX; SP.vec.x+=PIXEL_SIZE)
        {
            i32 AB_P = edge_cross(&A.vec, &B.vec, &SP.vec) + bias0;
            i32 BC_P = edge_cross(&B.vec, &C.vec, &SP.vec) + bias1;
            i32 CA_P = edge_cross(&C.vec, &A.vec, &SP.vec) + bias2;
            
            // Our graphics system has 0,0 on the bottom left not the
            // top right
            if (AB_P <= 0 && BC_P <= 0 && CA_P <= 0)
            {
                // Normalise the edge functions by dividing by the total 
                // area to get the barycentric coordinates. The weights used
                // to interplate the points in the triangle
                // printf("%d %d %d\n", AB_P, BC_P, CA_P);
                f32 wA = ((BC_P *100) / ABC); // alpha
                f32 wB = ((CA_P *100) / ABC); // beta
                f32 wG = ((AB_P *100) / ABC); // gamma
                // printf("%d %d %d\n", wA, wB, wC);

                f32 x_img = ((wA * A.u) + (wB * B.u) + (wG * C.u))/100;
                f32 y_img = ((wA * A.v) + (wB * B.v) + (wG * C.v))/100;

                // printf("%f --> %f %f\n", wA+wB+wG, x_img, y_img);

#ifdef DEBUG_UV_TRIANGLE
                ui8 r = (255.0 * wA) + (000.0 * wB) + (000.0 * wG);
                ui8 g = (000.0 * wA) + (255.0 * wB) + (000.0 * wG);
                ui8 b = (000.0 * wA) + (000.0 * wB) + (255.0 * wG);
                wefx_color(r, g, b);
#else
                // ui8 r = image[10];
                // ui8 g = image[11];
                // ui8 b = image[12];
                // printf("%x %x %x\n", r, g, b);

                // i16 pixx = (x_img * 128) -.5;
                // i16 pixy = ((1-y_img) * 128) -.5;

                i16 pixx = -(x_img * 128) -.5;
                i16 pixy = -(y_img * 128) -.5;

                // printf("%d %d\n", pixx, pixy);

                ui8 r = image[pixx+(128*pixy)+0];
                ui8 g = image[pixx+(128*pixy)+1];
                ui8 b = image[pixx+(128*pixy)+2];
                wefx_color(r, g, b);
#endif

                wefx_pixel(SP.vec.x, SP.vec.y, PIXEL_SIZE);
            }
        }
    }
}

void draw_scene(i32 time, i32 W, i32 H, ui8* image)
{
    mat4 m = make_perspective(90.0, H/W, 1, 20);
    // perspective divide
    // char *str = mat4_tos(&m);
    // printf("%s\n", str);
    // free(str);

    vertex tri[3];
    tri[0] = (vertex){ {100, 10}, 0, 0};
    tri[1] = (vertex){ {W >> 1, abs( (H) * (sin(time * .01)))}, .5, 0 };
    tri[2] = (vertex){ {W - 100, 10}, 0, .5};

    // tri[3] = (vertex){ {W - 100, H-10}, 32, 128 };
    // tri[4] = (vertex){ {W - 10, H-50}, 128, 64 };

    f32 angle = time * .01;
    vector center = {W>>1, H>>1};
    // printf("%d\n", angle);

    vertex v0 = {rotate_vec(tri[0].vec, center, angle), 0, 0};
    vertex v1 = {rotate_vec(tri[1].vec, center, angle), 1, 0};
    vertex v2 = {rotate_vec(tri[2].vec, center, angle), 0, 1};

    wefx_clear();

    triangle_fill(tri[0], tri[1], tri[2], image);
    // triangle_fill(tri[2], tri[3], tri[4], image);
    // triangle_fill(tri[3], tri[2], tri[1], image);
    triangle_fill(v0, v1, v2, image);

    wefx_color(0xff, 0xff, 0xff);
    vertex P = (vertex){ {W >> 1, H >> 1, 1} };
    wefx_pixel(P.vec.x, P.vec.y, 4);
}


vector rotate_vec(vector v, vector c, f32 angle) 
{
    vector rot;
    v.x -= c.x;
    v.y -= c.y;

    // these aren't ints
    rot.x = v.x * cosf(angle) - v.y * sinf(angle);
    rot.y = v.x * sinf(angle) + v.y * cosf(angle);

    rot.x += c.x;
    rot.y += c.y;

    rot.x = round(rot.x);
    rot.y = round(rot.y);

    return rot;
}

mat4 make_perspective(f32 fov, f32 aspect, f32 znear, f32 zfar)
{
    mat4 out = {0};

    f32 Sx = aspect * (1 / tanf(fov /2));
    f32 Sy = 1 / tanf(fov / 2);
    f32 Sz = zfar / (zfar - znear);
    f32 Pz = (-zfar * znear) / (zfar - znear);

    // clang-format off
    out.m00 = Sx; out.m10 = 0;  out.m20 = 0;  out.m30 = 0;
    out.m01 = 0;  out.m11 = Sy; out.m21 = 0;  out.m31 = 0;
    out.m02 = 0;  out.m12 = 0;  out.m22 = Sz; out.m32 = Pz;
    out.m03 = 0;  out.m13 = 0;  out.m23 = 1; out.m33 = 0;
    // clang-format on

    return out;
}

char *mat4_tos(const mat4 *m)
{
    char *out = calloc(sizeof(char), 300);
    // clang-format off
    snprintf(out, 300, "[\n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n %f, %f, %f, %f \n]\n", 
        m->m00, m->m10, m->m20, m->m30,
        m->m01, m->m11, m->m21, m->m31,
        m->m02, m->m12, m->m22, m->m32,
        m->m03, m->m13, m->m23, m->m33
    );
    // clang-format on
    return out;
}