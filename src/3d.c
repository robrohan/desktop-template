#include "3d.h"

#define PIXEL_SIZE 1


i32 edge_cross(vector *a, vector *b, vector *c)
{
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
};


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

    i32 minX = MIN(MIN(A.vec.x, B.vec.x), C.vec.x);
    i32 minY = MIN(MIN(A.vec.y, B.vec.y), C.vec.y);
    i32 maxX = MAX(MAX(A.vec.x, B.vec.x), C.vec.x);
    i32 maxY = MAX(MAX(A.vec.y, B.vec.y), C.vec.y);

    // wefx_rect(minX, maxY, maxX, minY, 1);

    vertex SP = (vertex){ {0, 0}, 0, 0};
    for (SP.vec.y = minY; SP.vec.y < maxY; SP.vec.y+=PIXEL_SIZE)
    {
        // wefx_color(rand() % 0xff, rand() % 0xff, rand() & 0xff);
        for (SP.vec.x = minX; SP.vec.x < maxX; SP.vec.x+=PIXEL_SIZE)
        {
            i32 AB_P = edge_cross(&A.vec, &B.vec, &SP.vec);
            i32 BC_P = edge_cross(&B.vec, &C.vec, &SP.vec);
            i32 CA_P = edge_cross(&C.vec, &A.vec, &SP.vec);
            
            // printf("%d %d %d\n", wA, wB, wC);
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

                ui8 r = (255.0 * wA) + (000.0 * wB) + (000.0 * wC);
                ui8 g = (000.0 * wA) + (255.0 * wB) + (000.0 * wC);
                ui8 b = (000.0 * wA) + (000.0 * wB) + (255.0 * wC);
                wefx_color(r, g, b);
                wefx_pixel(SP.vec.x, SP.vec.y, PIXEL_SIZE);
            }
        }
    }
}


vertex tri[7];

void draw_scene(i32 time, i32 W, i32 H)
{

    tri[0] = (vertex){ {100, 10} };
    tri[1] = (vertex){ {W >> 1, abs( (H) * (sin(time * .01)))} };
    tri[2] = (vertex){ {W - 100, 10} };

    tri[3] = (vertex){ {W - 100, H-10} };
    tri[4] = (vertex){ {W - 10, H-50} };



    // wefx_color(0xff, 0xff, 0xff);
    // // only draw clockwise triangles
    // i32 abc = edge_function(tri[0], tri[1], tri[2]);
    // if (abc < 0)
    // {
    //     draw_triangle(tri);
    // }

    // https://jtsorlinis.github.io/rendering-tutorial/
    // Check if the point is inside the edge AB
    // if (ABP >= 0) {
    // Point is inside the edge AB
    // }

    wefx_clear();

    wefx_color(0xff, 0xff, 0xff);
    vertex P = (vertex){ {W >> 1, H >> 1, 1} };
    // wefx_point(P.vec.x, P.vec.y);
    wefx_pixel(P.vec.x, P.vec.y, 4);

    // Calculate our edge function for all three edges of the triangle ABC
    // i32 AB_P = edge_function(A, B, P);
    // i32 BC_P = edge_function(B, C, P);
    // i32 CA_P = edge_function(C, A, P);
    // if (AB_P < 0 && BC_P < 0 && CA_P < 0)
    // {
    //     printf("%d %d %d \n", AB_P, BC_P, CA_P);
    // }
    triangle_fill(tri[0], tri[1], tri[2]);
    triangle_fill(tri[2], tri[3], tri[4]);
    triangle_fill(tri[3], tri[2], tri[1]);
}
