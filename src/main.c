// #define SOKOL_NOAPI
#define SOKOL_GLCORE
// #define SOKOL_GLES3
#define SOKOL_APP_IMPL
#include "sokol_app.h"


void frame_cb(void)
{
    // called 60fps frame render
    // printf("hi\n");
}

void event_cb(const sapp_event *event)
{
    // called when an event happens
    // printf("howdy %i\n", event);
}

void init_cb(void)
{
    // called after the 3d context is made
    printf("%ix%i\n", sapp_width(), sapp_height());
    printf("GL Version: %i.%i\n", 
        sapp_gl_get_major_version(), sapp_gl_get_minor_version());
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc) {
        .width = 640,
        .height = 480,
        .init_cb = init_cb,
        .frame_cb = frame_cb,
        // .cleanup_cb = my_cleanup_func,
        .event_cb = event_cb,
    };
}