#include "raylib.h"
#include "rlgl.h"

#include <stdio.h>
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
// raylib's rtext.c already links in a non-static stb_rect_pack implementation
// (its stb_truetype copy is static, so nuklear still needs to bring its own)
#define NK_NO_STB_RECT_PACK_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include "nuklear.h"

// vertex format nuklear will pack its draw output into
struct nk_raylib_vertex
{
    float position[2];
    float uv[2];
    unsigned char color[4];
};

static void nk_raylib_input(struct nk_context *ctx)
{
    nk_input_begin(ctx);

    const int mouseX = GetMouseX();
    const int mouseY = GetMouseY();

    nk_input_motion(ctx, mouseX, mouseY);
    nk_input_button(ctx, NK_BUTTON_LEFT, mouseX, mouseY, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    nk_input_button(ctx, NK_BUTTON_RIGHT, mouseX, mouseY, IsMouseButtonDown(MOUSE_BUTTON_RIGHT));
    nk_input_button(ctx, NK_BUTTON_MIDDLE, mouseX, mouseY, IsMouseButtonDown(MOUSE_BUTTON_MIDDLE));
    nk_input_scroll(ctx, nk_vec2(0.0f, GetMouseWheelMove()));

    for (int c = GetCharPressed(); c > 0; c = GetCharPressed())
    {
        nk_input_unicode(ctx, (nk_rune)c);
    }

    nk_input_key(ctx, NK_KEY_DEL, IsKeyDown(KEY_DELETE));
    nk_input_key(ctx, NK_KEY_ENTER, IsKeyDown(KEY_ENTER));
    nk_input_key(ctx, NK_KEY_TAB, IsKeyDown(KEY_TAB));
    nk_input_key(ctx, NK_KEY_BACKSPACE, IsKeyDown(KEY_BACKSPACE));
    nk_input_key(ctx, NK_KEY_LEFT, IsKeyDown(KEY_LEFT));
    nk_input_key(ctx, NK_KEY_RIGHT, IsKeyDown(KEY_RIGHT));
    nk_input_key(ctx, NK_KEY_UP, IsKeyDown(KEY_UP));
    nk_input_key(ctx, NK_KEY_DOWN, IsKeyDown(KEY_DOWN));

    nk_input_end(ctx);
}

// converts the queued nuklear draw commands into vertices and
// submits them through raylib's low level rlgl drawing layer
static void nk_raylib_render(struct nk_context *ctx, const struct nk_draw_null_texture *nullTexture)
{
    static const struct nk_draw_vertex_layout_element vertexLayout[] = {
        {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_raylib_vertex, position)},
        {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_raylib_vertex, uv)},
        {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_raylib_vertex, color)},
        {NK_VERTEX_LAYOUT_END}
    };

    struct nk_convert_config config;
    memset(&config, 0, sizeof(config));
    config.vertex_layout = vertexLayout;
    config.vertex_size = sizeof(struct nk_raylib_vertex);
    config.vertex_alignment = NK_ALIGNOF(struct nk_raylib_vertex);
    config.tex_null = *nullTexture;
    config.circle_segment_count = 22;
    config.curve_segment_count = 22;
    config.arc_segment_count = 22;
    config.global_alpha = 1.0f;
    config.shape_AA = NK_ANTI_ALIASING_ON;
    config.line_AA = NK_ANTI_ALIASING_ON;

    struct nk_buffer cmds, verts, idx;
    nk_buffer_init_default(&cmds);
    nk_buffer_init_default(&verts);
    nk_buffer_init_default(&idx);
    nk_convert(ctx, &cmds, &verts, &idx, &config);

    const struct nk_raylib_vertex *vertices = (const struct nk_raylib_vertex *)nk_buffer_memory_const(&verts);
    const nk_draw_index *offset = (const nk_draw_index *)nk_buffer_memory_const(&idx);

    const struct nk_draw_command *cmd;
    nk_draw_foreach(cmd, ctx, &cmds)
    {
        if (!cmd->elem_count)
        {
            continue;
        }

        BeginScissorMode((int)cmd->clip_rect.x, (int)cmd->clip_rect.y,
            (int)cmd->clip_rect.w, (int)cmd->clip_rect.h);

        rlSetTexture((unsigned int)cmd->texture.id);
        rlBegin(RL_TRIANGLES);
        for (unsigned int i = 0; i < cmd->elem_count; i++)
        {
            const struct nk_raylib_vertex *v = &vertices[offset[i]];
            rlColor4ub(v->color[0], v->color[1], v->color[2], v->color[3]);
            rlTexCoord2f(v->uv[0], v->uv[1]);
            rlVertex2f(v->position[0], v->position[1]);
        }
        rlEnd();
        rlSetTexture(0);

        EndScissorMode();

        offset += cmd->elem_count;
    }

    nk_buffer_free(&cmds);
    nk_buffer_free(&verts);
    nk_buffer_free(&idx);
    nk_clear(ctx);
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "example");

    // raylib enables backface culling by default; our top-down ortho
    // projection flips triangle winding, and nuklear doesn't wind its
    // geometry to match, so half of every shape would get culled
    rlDisableBackfaceCulling();

    const int monitor = GetCurrentMonitor();
    SetWindowPosition((GetMonitorWidth(monitor) - screenWidth) / 2, (GetMonitorHeight(monitor) - screenHeight) / 2);

    SetTargetFPS(60);

    //////////////////////////////////////
    /* init gui state */
    struct nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_default(&atlas, 16.0f, NULL);

    int fontWidth = 0;
    int fontHeight = 0;
    const void *fontPixels = nk_font_atlas_bake(&atlas, &fontWidth, &fontHeight, NK_FONT_ATLAS_RGBA32);

    Image fontImage = {
        .data = (void *)fontPixels,
        .width = fontWidth,
        .height = fontHeight,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D fontTexture = LoadTextureFromImage(fontImage);

    struct nk_draw_null_texture nullTexture;
    nk_font_atlas_end(&atlas, nk_handle_id((int)fontTexture.id), &nullTexture);

    struct nk_context ctx;
    nk_init_default(&ctx, &font->handle);

    enum {EASY, HARD};
    int op = EASY;
    float value = 0.6f;
    //////////////////////////////////////

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            printf("You clicked at x: %d, y: %d\n", GetMouseX(), GetMouseY());
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            printf("The right mouse button was clicked at x: %d, y: %d\n", GetMouseX(), GetMouseY());
        }

        nk_raylib_input(&ctx);

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

        BeginDrawing();
        ClearBackground(WHITE);

        // a simple gradient triangle, drawn through raylib's rlgl layer
        // so we get per-vertex colors like the old raw OpenGL version did
        rlBegin(RL_TRIANGLES);
            rlColor3f(1.0f, 0.0f, 0.0f);
            rlVertex2f(160.0f, 450.0f);
            rlColor3f(0.0f, 1.0f, 0.0f);
            rlVertex2f(640.0f, 450.0f);
            rlColor3f(0.0f, 0.0f, 1.0f);
            rlVertex2f(400.0f, 150.0f);
        rlEnd();

        nk_raylib_render(&ctx, &nullTexture);

        EndDrawing();
    }

    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    UnloadTexture(fontTexture);

    CloseWindow();
    return 0;
}
