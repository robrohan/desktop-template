#define RGFW_IMPLEMENTATION
#define RGFW_BUFFER
#include "RGFW.h"

#include "wefx.h"
#include "3d.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

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

void integrate(state* state, f32 t, f32 dt)
{
    state->ts->v[2].vec.y = (H * sin(t));
    // state->ts->v[2].vec.x += 1 * dt;
    // printf("%f %f %f %f\n", t, dt, sin(t), state->ts->v[2].vec.y);
}

void render(state* state)
{
    draw_scene(W, H, state);
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


    /////
    int w = 0;
    int h = 0;
    int channels = 0;
    ui8* image = stbi_load("./assets/8x8.png", &w, &h, &channels, STBI_rgb);
    printf("%dx%d %d\n", w, h, channels);
    /////

    //////////////////////////////////////////////

    triangle tris = {
        .v = {
            { .vec={100, 10}, 0, 0 },
            { .vec={W-100, 10}, 0, 1},
            { .vec={W>>1, H}, .5, .5}
        },
        .tex = {
            .w = 8,
            .h = 8,
            .c = 3,
            .image = image
        }
    };
    state game_state = { .ts = &tris };

    //////////////////////////////////////////////

    i8 running = 1;
    // will overflow after about 49 days
    unsigned int ticks = 0;
    ui32 current_time = 0;
    f32 dt = 1 / 60.0;
    f32 t = 0.0;
    while (running)
    {
        RGFW_window_makeCurrent(win);

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
                if (RGFW_isPressed(win, RGFW_Space))
                    printf("fps : %i\n", RGFW_window_checkFPS(win, 0));
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

        // time
        ticks++;
        ui32 new_time = ticks;
        ui32 frame_time = new_time - current_time;
        current_time = new_time;

        // physics - converting Time Deltas to seconds
        // receiving the time delta in terms of seconds will be more
        // intuitive than working with milliseconds
        // δ or Δ time
        while(frame_time > 0.0)
        {
            float delta_time = MIN(frame_time, dt);
            integrate(&game_state, t, delta_time);
            frame_time -= delta_time;
            t += delta_time;
        }

        // render
        render(&game_state);

        // draw our framebuffer bitmap to the screen
        RGFW_window_setGPURender(win, 0);
        RGFW_window_swapBuffers(win);
        draw_bitmap(win, (u8 *)wefx_get_buffer(), RGFW_RECT(0, 0, W, H));
    }

    stbi_image_free(image);
    RGFW_window_close(win);
}
