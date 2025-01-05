#define RGFW_IMPLEMENTATION
#ifndef RENDER_OPENGL
#define RGFW_BUFFER
#endif
#include "RGFW.h"

#define R2_MATHS_IMPLEMENTATION
#include "r2_maths.h"

#include "wefx.h"
#include "3d.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#define W 320
#define H 240

RGFW_area screenSize;

#ifndef RENDER_OPENGL
void draw_bitmap(RGFW_window *win, u8 *bitmap, RGFW_rect rect)
{
    // we can use this to draw the raw pixels on a the raw window.
    // RENDER_OPENGL needs to be off for this
    for (u32 y = 0; y < (u32)rect.h; y++)
    {
        u32 index = (rect.y + y) * (4 * screenSize.w) + rect.x * 4;
        memcpy(win->buffer + index, bitmap + (4 * rect.w * y), rect.w * 4 * sizeof(u8));
    }
}
#endif

void integrate(state* state, f32 t, f32 dt)
{
    // state->meshes->tris->v[2].vec.y = sin(t);
    state->entities.mesh[ID_PLAYER].tris->v[2].vec.y = sin(t);

    // state->meshes->tris->v[2].vec.x += 1 * dt;
}

void render(state* state)
{
    // TODO: on each loop?
    mat4 screenSpaceM = make_screenSpaceTransform(W/2, H/2);
    draw_scene(&screenSpaceM, state);
}

/////////////////////////////////////////////

int main(void)
{
#ifdef RENDER_OPENGL
    // MacOS only supports really old gl from C
    RGFW_setGLVersion(RGFW_glCore, 2, 1);
#endif

    RGFW_window *win = RGFW_createWindow(
        "Desktop Example",
        RGFW_RECT(0, 0, W, H),
        RGFW_ALLOW_DND); // | RGFW_NO_GPU_RENDER); // | RGFW_NO_RESIZE);

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

#ifdef RENDER_OPENGL
    glEnable(GL_TEXTURE_2D);
    GLuint _tex;
    glGenTextures(1, &_tex);
#endif

    /////
    int w = 0;
    int h = 0;
    int channels = 0;
    ui8* image = stbi_load("./assets/8x8.png", &w, &h, &channels, STBI_rgb);
    printf("%dx%d %d\n", w, h, channels);
    /////

    //////////////////////////////////////////////

    texture tex = {
        .w = 8,
        .h = 8,
        .image = image
    };
    material material = { .tex=tex };

    triangle tris = {
        .v = {
            { .vec={ {-1,  1, 0, 1} },  .u=0,  .v=0 },
            { .vec={ { 1,  1, 0, 1} },  .u=0,  .v=1 },
            { .vec={ { 0, -1, 0, 1} },  .u=.5, .v=.5 }
        }
    };

    mesh mesh = {
        .tris={ tris },
        .material=material
    };

    entity player = { ID_PLAYER };

    entities entities = {
        .mesh={mesh},
        .transform={{
            .position={0, 0, 0},
            .scale={1, 1, 1},
            .rotation={0, 0, 0, 0}
        }}
    };

    state game_state = {
        .entities=entities
    };

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
                printf("pressed: %d\n", win->event.key);
                if (RGFW_isPressed(win, RGFW_Space))
                    printf("fps : %i\n", RGFW_window_checkFPS(win, 0));
                break;
            case RGFW_keyReleased:
                printf("released: %d\n", win->event.key);
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
        // https://www.gafferongames.com/post/fix_your_timestep/
        while(frame_time > 0.0)
        {
            float delta_time = MIN(frame_time, dt);
            integrate(&game_state, t, delta_time);
            frame_time -= delta_time;
            t += delta_time;
        }

        // render
        render(&game_state);

        double scale = 1;

#ifdef __APPLE__
    RGFW_monitor monitor = RGFW_window_getMonitor(win);
    scale = monitor.pixelRatio;
#endif

#ifdef RENDER_OPENGL
        // https://bdptech.blogspot.com/2016/05/using-opengl-to-blit-array-of-pixels.html
        int scale_width = win->r.w * scale;
        int scale_height = win->r.h * scale;
        glEnable(GL_BLEND);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport((GLint)0, (GLint)0, (GLint)scale_width, (GLint)scale_height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1.0, 1.0, 0.0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, (u8 *)wefx_get_buffer()
        );

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0, 0, 0);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1, 0, 0);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1, 1, 0);

        glTexCoord2f(0.0f,1.0f);
        glVertex3f(0, 1, 0);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
#else
        draw_bitmap(win, (u8 *)wefx_get_buffer(), RGFW_RECT(0, 0, W, H));
#endif

        // draw our framebuffer bitmap to the screen
        RGFW_window_setGPURender(win, 1);
        RGFW_window_swapBuffers(win);
        RGFW_window_checkFPS(win, 60);
    }

    stbi_image_free(image);
    RGFW_window_close(win);
}
