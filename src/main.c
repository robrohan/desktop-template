#define FPL_IMPLEMENTATION
#include "final_platform_layer.h"

#define R2_MATHS_IMPLEMENTATION
#include "r2_maths.h"

struct InputState
{
    vec2 mousePos;
    fplButtonState mouseStates[fplMouseButtonType_MaxCount];
    float mouseWheelDelta;
    uint64_t lastMouseWheelUpdateTime;
};

int main(int argc, char **args)
{
    // Create default settings
    fplSettings settings = fplMakeDefaultSettings();

    settings.video.backend = fplVideoBackendType_Software;
    settings.video.isAutoSize = true;

    if (fplPlatformInit(fplInitFlags_Video, &settings))
    {

        struct InputState input = {};
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
                    printf("%d, %d :: %d %d %d\n", 
                        (int)input.mousePos.x, (int)input.mousePos.y,
                        input.mouseStates[0], input.mouseStates[1], input.mouseStates[2]
                    );
                }
                break;

                case fplEventType_Keyboard: {
                    if (ev.keyboard.keyCode >= 32 && ev.keyboard.keyCode < 255)
                    {
                        printf("%li\n", ev.keyboard.keyCode);
                    }
                }
                break;
                }
            }

            // Render
            fplVideoBackBuffer *buff = fplGetVideoBackBuffer();
            for (uint32_t y = 0; y < buff->height; ++y)
            {
                uint32_t *p = (uint32_t *)((uint8_t *)buff->pixels + y * buff->lineWidth);
                for (uint32_t x = 0; x < buff->width; ++x)
                {
                    uint32_t color = 0xFFFFFF00;
                    *p++ = color;
                }
            }

            fplVideoFlip();
        }
        fplPlatformRelease();
        return 0;
    }
    else
    {
        return -1;
    }
}