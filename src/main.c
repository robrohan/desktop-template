#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#include "RGFW.h"

u8 icon[4 * 3 * 3] = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
                      0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
                      0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};

RGFW_area screenSize;

// fill buffer with a color, clearing anything that was on it
void clear(RGFW_window *win, u8 color[3])
{
    // loop through each pixel
    for (u32 y = 0; y < (u32)win->r.h; y++)
    {
        for (u32 x = 0; x < screenSize.w; x++)
        {
            u32 index = (y * 4 * screenSize.w) + x * 4;
            // copy the color to that pixel
            memcpy(win->buffer + index, color, 4 * sizeof(u8));
        }
    }
}

// this can also be used to convert BGR to RGB
void bitmap_rgbToBgr(u8 *bitmap, RGFW_area a)
{
    // loop through each *pixel* (not channel) of the buffer
    for (u32 y = 0; y < a.h; y++)
    {
        for (u32 x = 0; x < a.w; x++)
        {
            u32 index = (y * 4 * a.w) + x * 4;
            u8 red = bitmap[index];
            bitmap[index] = bitmap[index + 2];
            bitmap[index + 2] = red;
        }
    }
}

void drawBitmap(RGFW_window *win, u8 *bitmap, RGFW_rect rect)
{
    for (u32 y = 0; y < (u32)rect.h; y++)
    {
        u32 index = (rect.y + y) * (4 * screenSize.w) + rect.x * 4;
        memcpy(win->buffer + index, bitmap + (4 * rect.w * y), rect.w * 4 * sizeof(u8));
    }
}

void drawRect(RGFW_window *win, RGFW_rect r, u8 color[3])
{
    for (i32 x = r.x; x < (r.x + r.w); x++)
    {
        for (i32 y = r.y; y < (r.y + r.h); y++)
        {
            int index = y * (4 * screenSize.w) + x * 4;
            memcpy(win->buffer + index, color, 4 * sizeof(u8));
        }
    }
}

/////////////////////////////////////////////

void draw(RGFW_window *win, int ticks)
{
    clear(win, (u8[4]){0xc0, 0xc0, 0xc0, 0xff});
	drawRect(win, RGFW_RECT(100, 200, 200, 200), (u8[4]){0xff, 0, 0, 0xff});
    drawBitmap(win, icon, RGFW_RECT(100, 100, 3, 3));
}

/////////////////////////////////////////////

int main(void)
{
    RGFW_window *win = RGFW_createWindow(
        "Example", 
        RGFW_RECT(0, 0, 250, 300), 
        RGFW_ALLOW_DND | RGFW_NO_GPU_RENDER);
    win->fpsCap = 30;

    screenSize = RGFW_getScreenSize();
    printf("%dx%d\n", screenSize.w, screenSize.h);
    printf("%dx%d\n", win->r.w, win->r.h);

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
                printf("Mouse move %dx%d\n", 
                    win->event.point.x, win->event.point.y);
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
        draw(win, ticks);
        RGFW_window_setGPURender(win, 0);
        RGFW_window_swapBuffers(win);

        ticks++;
    }

    RGFW_window_close(win);
}
