#define FPL_IMPLEMENTATION
#include "final_platform_layer.h"

#define R2_MATHS_IMPLEMENTATION
#include "r2_maths.h"

#include "wefx.h"

struct InputState
{
    vec2 mousePos;
    fplButtonState mouseStates[fplMouseButtonType_MaxCount];
    float mouseWheelDelta;
    uint64_t lastMouseWheelUpdateTime;
};

void draw(int time, int W, int H)
{
    wefx_clear();

    int x = time % W;
    wefx_point(x, (H / 2) + cos(time) * 2);

    for (int i = 0; i < 130; i++)
    {
        wefx_color(0xff, 0, 0);
        wefx_point(x - i, ((H / 2) + sin(time - i) * 3) - 20);

        wefx_color(0, 0xff, 0);
        wefx_point(x - i, (H / 2) + cos(time - i) * 2);

        wefx_color(0, 0, 0xff);
        wefx_point(x - i, ((H / 2) + sin(time - i) * 3) + 20);
    }

    wefx_color(0xff, 0xff, 0xff);
    wefx_line(0, 0, W, H);
    wefx_line(0, H, W, 0);

    wefx_color(rand() % 0xff, rand() % 0xff, rand() & 0xff);
    wefx_line(W / 2, H / 2, abs(rand() % W), abs(rand() % H));

    wefx_circle(W >> 1, H >> 1, (H >> 1) - 5);
}

int main(int argc, char **args)
{
    // Create default settings
    fplSettings settings = fplMakeDefaultSettings();

    settings.video.backend = fplVideoBackendType_Software;
    settings.video.isAutoSize = true;
    settings.video.isVSync = true;

    settings.window.isResizable = false;
    settings.window.windowSize.height = 300;
    settings.window.windowSize.width = 300;

    int err = wefx_open(300, 300, "Example");
    if (err)
        return 1;

    if (fplPlatformInit(fplInitFlags_Video, &settings))
    {
        struct InputState input = {};
        int ticks = 0;
        while (fplWindowUpdate())
        {
            // Input
            fplEvent ev;
            while (fplPollEvent(&ev))
            {
                switch (ev.type)
                {
                case fplEventType_Mouse: {
                    if (ev.mouse.type == fplMouseEventType_Move)
                    {
                        input.mousePos.x = (float)ev.mouse.mouseX;
                        input.mousePos.y = (float)ev.mouse.mouseY;
                    }
                    else if (ev.mouse.type == fplMouseEventType_Button)
                    {
                        input.mouseStates[(int)ev.mouse.mouseButton] = ev.mouse.buttonState;
                    }
                    else if (ev.mouse.type == fplMouseEventType_Wheel)
                    {
                        input.mouseWheelDelta = ev.mouse.wheelDelta;
                        input.lastMouseWheelUpdateTime = fplMillisecondsQuery();
                    }
                    printf("%d, %d :: %d %d %d\n", (int)input.mousePos.x, (int)input.mousePos.y, input.mouseStates[0],
                           input.mouseStates[1], input.mouseStates[2]);
                }
                break;

                case fplEventType_Keyboard: {
                    if (ev.keyboard.keyCode >= 32 && ev.keyboard.keyCode < 255)
                    {
                        printf("%li\n", ev.keyboard.keyCode);
                    }
                }
                break;

                case fplEventType_Window:
                case fplEventType_Gamepad:
                case fplEventType_None:
                break;
                }
            }

            // own low level library
            wefx_clear_color(0x00, 0x00, 0x00);
            wefx_color(0xff, 0xff, 0xff);
            wefx_clear();
            srand(ticks);
            draw(ticks, 300, 300);
            ///////////////////////

            // Render
            fplVideoBackBuffer *buff = fplGetVideoBackBuffer();
            wefx_draw(buff->pixels);

            // More of what you'd want to do for your own library
            // for (uint32_t y = 0; y < buff->height; ++y)
            // {
            //     uint32_t *p = (uint32_t *)((uint8_t *)buff->pixels + y * buff->lineWidth);
            //     for (uint32_t x = 0; x < buff->width; ++x)
            //     {
            //         uint32_t color = 0xFFFFFF00;
            //         *p++ = color;
            //     }
            // }

            fplVideoFlip();

            ticks++;
        }
        fplPlatformRelease();
        return 0;
    }
    else
    {
        return -1;
    }
}