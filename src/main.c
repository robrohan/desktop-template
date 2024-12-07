#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#include "RGFW.h"

#include "wefx.h"
#include "3d.h"

#define W 320
#define H 240

RGFW_area screenSize;

void draw_bitmap(RGFW_window *win, u8 *bitmap, RGFW_rect rect)
{
    for (u32 y = 0; y < (u32)rect.h; y++)
    {
        u32 index = (rect.y + y) * (4 * screenSize.w) + rect.x * 4;
        memcpy(win->buffer + index, bitmap + (4 * rect.w * y), rect.w * 4 * sizeof(u8));
    }
}

void draw(int time)
{
    draw_scene(time, W, H);
}

/////////////////////////////////////////////

int main(void)
{
    RGFW_window *win = RGFW_createWindow(
        "Desktop Example",
        RGFW_RECT(0, 0, W, H),
        RGFW_ALLOW_DND | RGFW_NO_GPU_RENDER); // | RGFW_NO_RESIZE);

    screenSize = RGFW_getScreenSize();
    printf("%dx%d\n", screenSize.w, screenSize.h);
    printf("%dx%d\n", win->r.w, win->r.h);

    /////
    int err = wefx_open(win->r.w, win->r.h, "Desktop Example");
    if (err)
        return 1;
    wefx_clear_color(0x00, 0x00, 0x00);
    wefx_clear();
    srand(9999991);
    /////

    i8 running = 1;
    int ticks = 0;
    while (running)
    {
        // input
        while (RGFW_window_checkEvent(win))
        {
            if (win->event.type == RGFW_quit || RGFW_isPressed(win, RGFW_Escape))
            {
                running = 0;
                break;
            }

            switch (win->event.type)
            {
            case RGFW_mouseButtonPressed:
                printf("mouse pressed: %d\n", win->event.button);
                break;
            case RGFW_mouseButtonReleased:
                printf("mouse released: %d\n", win->event.button);
                break;
            case RGFW_mousePosChanged:
                // printf("Mouse move %dx%d\n", 
                //     win->event.point.x, win->event.point.y);
                break;
            case RGFW_keyPressed:
                printf("pressed: %d\n", win->event.keyCode);
                break;
            case RGFW_keyReleased:
                printf("released: %d\n", win->event.keyCode);
                break;
            case RGFW_dnd_init:
                printf("drop started at %dx%d\n",
                    win->event.point.x, win->event.point.y);
                break;
            case RGFW_dnd:
                printf("dropped %d files: \n", win->event.droppedFilesCount);
                printf("%s\n", win->event.droppedFiles[0]);
                break;
            }
        }

        // render
        draw(ticks);
        RGFW_window_setGPURender(win, 0);
        RGFW_window_swapBuffers(win);
        
        draw_bitmap(win, (u8 *)wefx_get_buffer(), RGFW_RECT(0, 0, W, H));
        ticks++;
    }

    RGFW_window_close(win);
}
