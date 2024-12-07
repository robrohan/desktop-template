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
void triangle_fill(vertex A, vertex B, vertex C)
{
    // Calculate the edge function for the whole triangle (ABC)
    // pointing towards the center basically
    i32 AB_C = -1 * edge_cross(&A.vec, &B.vec, &C.vec);
    if(AB_C < 0) {
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
        // wefx_color(rand() % 0xff, rand() % 0xff, rand() & 0xff);
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
                // area to get the barycentric coordinates
                // printf("%d %d %d\n", AB_P, BC_P, CA_P);
                i32 wA = (BC_P *100) / AB_C;
                i32 wB = (CA_P *100) / AB_C;
                i32 wC = (AB_P *100) / AB_C;
                // printf("%d %d %d\n", wA, wB, wC);

                ui8 r = (255.0 * wA) + (000.0 * wB) + (000.0 * wC);
                ui8 g = (000.0 * wA) + (255.0 * wB) + (000.0 * wC);
                ui8 b = (000.0 * wA) + (000.0 * wB) + (255.0 * wC);
                wefx_color(r, g, b);
                wefx_pixel(SP.vec.x, SP.vec.y, PIXEL_SIZE);
            }
        }
    }
}

void draw_scene(i32 time, i32 W, i32 H)
{
    vertex tri[7];
    tri[0] = (vertex){ {100, 10} };
    tri[1] = (vertex){ {W >> 1, abs( (H) * (sin(time * .01)))} };
    tri[2] = (vertex){ {W - 100, 10} };

    tri[3] = (vertex){ {W - 100, H-10} };
    tri[4] = (vertex){ {W - 10, H-50} };

    wefx_clear();

    wefx_color(0xff, 0xff, 0xff);
    vertex P = (vertex){ {W >> 1, H >> 1, 1} };
    wefx_pixel(P.vec.x, P.vec.y, 4);


    triangle_fill(tri[0], tri[1], tri[2]);
    triangle_fill(tri[2], tri[3], tri[4]);
    triangle_fill(tri[3], tri[2], tri[1]);
}
