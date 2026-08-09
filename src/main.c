// #define RGFW_IMPLEMENTATION
// #define RGFW_OPENGL
// #include "RGFW.h"

#include <stdio.h>

#ifdef RGFW_MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define NK_IMPLEMENTATION
#include "nuklear.h"

void keyfunc(RGFW_window *win, RGFW_key key, RGFW_keymod keyMod, RGFW_bool repeat, RGFW_bool pressed)
{
    RGFW_UNUSED(repeat);
    if (key == RGFW_escape && pressed)
    {
        RGFW_window_setShouldClose(win, 1);
    }
}

int main()
{
    /* the RGFW_windowOpenGL flag tells it to create an OpenGL context,
    but you can also create your own with RGFW_window_createContext_OpenGL */
    RGFW_window *win =
        RGFW_createWindow("example", 0, 0, 800, 600, RGFW_windowCenter | RGFW_windowNoResize | RGFW_windowOpenGL);

    // you can use callbacks like this if you want
    RGFW_setKeyCallback(keyfunc);

    //////////////////////////////////////
    /* init gui state */
    struct nk_context ctx;
    nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);

    enum {EASY, HARD};
    static int op = EASY;
    static float value = 0.6f;
    static int i =  20;
    //////////////////////////////////////

    while (RGFW_window_shouldClose(win) == RGFW_FALSE)
    {
        RGFW_event event;
        while (RGFW_window_checkEvent(win, &event))
        {
            // or RGFW_pollEvents(); if you only want callbacks
            // you can either check the current event yourself
            if (event.type == RGFW_quit)
                break;

            i32 mouseX, mouseY;
            RGFW_window_getMouse(win, &mouseX, &mouseY);

            if (event.type == RGFW_mouseButtonPressed && event.button.value == RGFW_mouseLeft)
            {
                printf("You clicked at x: %d, y: %d\n", mouseX, mouseY);
            }

            // or use the existing functions
            if (RGFW_isMousePressed(RGFW_mouseRight))
            {
                printf("The right mouse button was clicked at x: %d, y: %d\n", mouseX, mouseY);
            }
        }

        if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
            /* fixed widget pixel width */
            nk_layout_row_static(&ctx, 30, 80, 1);
            if (nk_button_label(&ctx, "button")) {
                /* event handling */
            }

            /* fixed widget window ratio width */
            nk_layout_row_dynamic(&ctx, 30, 2);
            if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

            /* custom widget pixel width */
            nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
            {
                nk_layout_row_push(&ctx, 50);
                nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
                nk_layout_row_push(&ctx, 110);
                nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
            }
            nk_layout_row_end(&ctx);
        }
        nk_end(&ctx);


        // OpenGL 1.1 is used here for a simple example, but you can use any version you want (if you request it first
        // (see gl33/gl33.c))
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.6f, -0.75f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.6f, -0.75f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 0.75f);
        glEnd();

        RGFW_window_swapBuffers_OpenGL(win);
        glFlush();
    }

    RGFW_window_close(win);
    return 0;
}
