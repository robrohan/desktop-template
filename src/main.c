#define R2_MATHS_IMPLEMENTATION
#include "r2_maths.h"

#include "raylib.h"

#include "wefx.h"
#include "3d.h"

#define W 320
#define H 240

void integrate(state* state, f32 t, f32 dt)
{
    // state->meshes->tris->v[2].vec.y = sin(t);
    state->entities.mesh[ID_PLAYER].tris->v[2].vec.y = sin(t);
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
    InitWindow(W, H, "Desktop Example");
    SetTargetFPS(60);

    printf("%dx%d (monitor)\n", GetMonitorWidth(0), GetMonitorHeight(0));
    printf("%dx%d (window)\n", W, H);

    /////
    int err = wefx_open(W, H, "Desktop Example");
    if (err)
        return 1;
    wefx_clear_color(0x00, 0x00, 0x00);
    wefx_clear();
    srand(9999991);
    /////

    Image img = GenImageColor(W, H, BLACK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    /////
    Image sprite = LoadImage("./assets/8x8.png");
    ImageFormat(&sprite, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    printf("%dx%d\n", sprite.width, sprite.height);
    /////

    //////////////////////////////////////////////

    texture stb_tex = {
        .w = sprite.width,
        .h = sprite.height,
        .image = (ui8*)sprite.data
    };
    material material = { .tex=stb_tex };

    triangle tris = {
        .v = {
            { .vec={ {-1,  1, 0, 1} },  .u=0,  .v=0 },
            { .vec={ { 1,  1, 0, 1} },  .u=0,  .v=1 },
            { .vec={ { 0, -1, 0, 1} },  .u=.5, .v=.5 }
        }
    };

    mesh mesh = {
        .tris={ tris },
    };

    entity player = { ID_PLAYER };

    entities entities = {
        .mesh={mesh},
        .transform={{
            .position={{0, 0, 0}},
            .scale={{1, 1, 1}},
            .rotation={{0, 0, 0, 0}}
        }},
        .material={material}
    };

    state game_state = {
        .entities=entities
    };

    //////////////////////////////////////////////

    i8 running = 1;
    f32 dt = 1 / 60.0;
    f32 t = 0.0;

    while (running && !WindowShouldClose())
    {
        // input
        if (IsKeyDown(KEY_ESCAPE))
        {
            running = 0;
            break;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            printf("mouse pressed: left\n");
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            printf("mouse released: left\n");

        if (IsKeyPressed(KEY_SPACE))
            printf("fps: %d\n", GetFPS());

        if (IsFileDropped()) {
            FilePathList files = LoadDroppedFiles();
            printf("dropped %d files:\n", files.count);
            printf("%s\n", files.paths[0]);
            UnloadDroppedFiles(files);
        }

        // time
        f32 frame_time = GetFrameTime();

        // physics - converting Time Deltas to seconds
        // https://www.gafferongames.com/post/fix_your_timestep/
        while (frame_time > 0.0)
        {
            float delta_time = MIN(frame_time, dt);
            integrate(&game_state, t, delta_time);
            frame_time -= delta_time;
            t += delta_time;
        }

        // render to wefx cpu buffer
        render(&game_state);

        // blit cpu buffer to screen via texture
        Rectangle src = {0, 0, W, H};
        Rectangle dst = {0, 0, W, H};
        Vector2 origin = {0, 0};

        UpdateTexture(tex, wefx_get_buffer());

        BeginDrawing();
            DrawTexturePro(tex, src, dst, origin, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadImage(sprite);
    CloseWindow();

    return 0;
}
