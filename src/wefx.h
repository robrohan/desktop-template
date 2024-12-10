#ifndef WEFX__H
#define WEFX__H

#include <stdlib.h>

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

// Open a new graphics window.
int wefx_open(unsigned int width, unsigned int height, const char *title);

// Draw a point at (x,y)
void wefx_point(int x, int y);

void wefx_pixel(int left, int top, int psize);

void wefx_circle(int x0, int y0, int r0, int psize);

// Draw a line from (x1,y1) to (x2,y2)
void wefx_line(int x1, int y1, int x2, int y2, int psize);

void wefx_rect(int x0, int y0, int x1, int y1, int psize);

// Change the current drawing color.
void wefx_color(unsigned int red, unsigned int green, unsigned int blue);

void wefx_color_i(int color);

// Clear the graphics window to the background color.
void wefx_clear(void);

// Change the current background color.
void wefx_clear_color(unsigned int red, unsigned int green, unsigned int blue);

// Wait for the user to press a key or mouse button.
// char wefx_wait();

// Return the X and Y coordinates of the last event.
// int wefx_xpos();
// int wefx_ypos();

// Return the X and Y dimensions of the window.
int wefx_xsize(void);
int wefx_ysize(void);

extern void print(const char *);

void wefx_draw(unsigned int *screen);

unsigned int * wefx_get_buffer(void);

#endif // WEFX__H
