#include "raylib.h"
#include "rlgl.h"

#include <stdio.h>
#include <string.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#include "nuklear.h"

// lets nuklear measure text the same way we're about to draw it,
// so layout (button/label sizing) matches what actually renders
static float nk_raylib_text_width(nk_handle handle, float height, const char *text, int len)
{
    const Font *font = (const Font *)handle.ptr;
    char buf[256];
    if (len < 0) len = 0;
    if (len >= (int)sizeof(buf)) len = (int)sizeof(buf) - 1;
    memcpy(buf, text, (size_t)len);
    buf[len] = '\0';
    return MeasureTextEx(*font, buf, height, 0.0f).x;
}

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

static inline Color nk_raylib_color(struct nk_color c)
{
    return (Color){ c.r, c.g, c.b, c.a };
}

static inline Vector2 nk_raylib_vec2i(struct nk_vec2i v)
{
    return (Vector2){ (float)v.x, (float)v.y };
}

// walks nuklear's queued draw commands and dispatches each one straight to
// raylib's own (well tested) drawing functions, instead of hand rolling a
// textured-triangle renderer: raylib's text/shape pipeline is what draws
// correctly, so let it do the actual work
static void nk_raylib_render(struct nk_context *ctx)
{
    const struct nk_command *cmd;
    nk_foreach(cmd, ctx)
    {
        switch (cmd->type)
        {
            case NK_COMMAND_NOP: break;

            case NK_COMMAND_SCISSOR:
            {
                const struct nk_command_scissor *s = (const struct nk_command_scissor *)cmd;
                EndScissorMode();
                BeginScissorMode(s->x, s->y, (int)s->w, (int)s->h);
                break;
            }

            case NK_COMMAND_LINE:
            {
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                DrawLineEx(nk_raylib_vec2i(l->begin), nk_raylib_vec2i(l->end),
                    (float)l->line_thickness, nk_raylib_color(l->color));
                break;
            }

            case NK_COMMAND_CURVE:
            {
                const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                Vector2 points[4] = {
                    nk_raylib_vec2i(q->begin), nk_raylib_vec2i(q->ctrl[0]),
                    nk_raylib_vec2i(q->ctrl[1]), nk_raylib_vec2i(q->end)
                };
                DrawSplineBezierCubic(points, 4, (float)q->line_thickness, nk_raylib_color(q->color));
                break;
            }

            case NK_COMMAND_RECT:
            {
                const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                Rectangle rec = { (float)r->x, (float)r->y, (float)r->w, (float)r->h };
                if (r->rounding > 0)
                {
                    float roundness = (2.0f * r->rounding) / (float)(rec.width < rec.height ? rec.width : rec.height);
                    DrawRectangleRoundedLinesEx(rec, roundness, 8, (float)r->line_thickness, nk_raylib_color(r->color));
                }
                else
                {
                    DrawRectangleLinesEx(rec, (float)r->line_thickness, nk_raylib_color(r->color));
                }
                break;
            }

            case NK_COMMAND_RECT_FILLED:
            {
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                Rectangle rec = { (float)r->x, (float)r->y, (float)r->w, (float)r->h };
                if (r->rounding > 0)
                {
                    float roundness = (2.0f * r->rounding) / (float)(rec.width < rec.height ? rec.width : rec.height);
                    DrawRectangleRounded(rec, roundness, 8, nk_raylib_color(r->color));
                }
                else
                {
                    DrawRectangleRec(rec, nk_raylib_color(r->color));
                }
                break;
            }

            case NK_COMMAND_RECT_MULTI_COLOR:
            {
                const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color *)cmd;
                Rectangle rec = { (float)r->x, (float)r->y, (float)r->w, (float)r->h };
                DrawRectangleGradientEx(rec, nk_raylib_color(r->left), nk_raylib_color(r->bottom),
                    nk_raylib_color(r->right), nk_raylib_color(r->top));
                break;
            }

            case NK_COMMAND_CIRCLE:
            {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                DrawEllipseLines(c->x + c->w / 2, c->y + c->h / 2, c->w / 2.0f, c->h / 2.0f, nk_raylib_color(c->color));
                break;
            }

            case NK_COMMAND_CIRCLE_FILLED:
            {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                DrawEllipse(c->x + c->w / 2, c->y + c->h / 2, c->w / 2.0f, c->h / 2.0f, nk_raylib_color(c->color));
                break;
            }

            case NK_COMMAND_ARC:
            {
                const struct nk_command_arc *a = (const struct nk_command_arc *)cmd;
                DrawRingLines((Vector2){ (float)a->cx, (float)a->cy }, (float)a->r, (float)a->r,
                    a->a[0] * RAD2DEG, a->a[1] * RAD2DEG, 32, nk_raylib_color(a->color));
                break;
            }

            case NK_COMMAND_ARC_FILLED:
            {
                const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled *)cmd;
                DrawRing((Vector2){ (float)a->cx, (float)a->cy }, 0.0f, (float)a->r,
                    a->a[0] * RAD2DEG, a->a[1] * RAD2DEG, 32, nk_raylib_color(a->color));
                break;
            }

            case NK_COMMAND_TRIANGLE:
            {
                const struct nk_command_triangle *t = (const struct nk_command_triangle *)cmd;
                DrawTriangleLines(nk_raylib_vec2i(t->a), nk_raylib_vec2i(t->b), nk_raylib_vec2i(t->c), nk_raylib_color(t->color));
                break;
            }

            case NK_COMMAND_TRIANGLE_FILLED:
            {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
                DrawTriangle(nk_raylib_vec2i(t->a), nk_raylib_vec2i(t->b), nk_raylib_vec2i(t->c), nk_raylib_color(t->color));
                break;
            }

            case NK_COMMAND_POLYGON:
            case NK_COMMAND_POLYLINE:
            {
                const struct nk_command_polygon *p = (const struct nk_command_polygon *)cmd;
                for (int i = 0; i < p->point_count - 1; i++)
                {
                    DrawLineEx(nk_raylib_vec2i(p->points[i]), nk_raylib_vec2i(p->points[i + 1]),
                        (float)p->line_thickness, nk_raylib_color(p->color));
                }
                if (cmd->type == NK_COMMAND_POLYGON && p->point_count > 1)
                {
                    DrawLineEx(nk_raylib_vec2i(p->points[p->point_count - 1]), nk_raylib_vec2i(p->points[0]),
                        (float)p->line_thickness, nk_raylib_color(p->color));
                }
                break;
            }

            case NK_COMMAND_POLYGON_FILLED:
            {
                const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled *)cmd;
                Vector2 fan[64];
                int count = p->point_count < 64 ? p->point_count : 64;
                for (int i = 0; i < count; i++) fan[i] = nk_raylib_vec2i(p->points[i]);
                DrawTriangleFan(fan, count, nk_raylib_color(p->color));
                break;
            }

            case NK_COMMAND_TEXT:
            {
                const struct nk_command_text *t = (const struct nk_command_text *)cmd;
                const Font *font = (const Font *)t->font->userdata.ptr;
                DrawRectangle(t->x, t->y, t->w, t->h, nk_raylib_color(t->background));
                DrawTextEx(*font, t->string, (Vector2){ (float)t->x, (float)t->y }, t->height, 0.0f, nk_raylib_color(t->foreground));
                break;
            }

            case NK_COMMAND_IMAGE:
            {
                const struct nk_command_image *img = (const struct nk_command_image *)cmd;
                Texture2D tex = { .id = (unsigned int)img->img.handle.id, .width = img->img.w, .height = img->img.h, .mipmaps = 1, .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
                DrawTexturePro(tex, (Rectangle){ 0, 0, (float)img->img.w, (float)img->img.h },
                    (Rectangle){ (float)img->x, (float)img->y, (float)img->w, (float)img->h },
                    (Vector2){ 0, 0 }, 0.0f, nk_raylib_color(img->col));
                break;
            }

            default: break;
        }
    }
    EndScissorMode();
    nk_clear(ctx);
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "example");

    const int monitor = GetCurrentMonitor();
    SetWindowPosition((GetMonitorWidth(monitor) - screenWidth) / 2, (GetMonitorHeight(monitor) - screenHeight) / 2);

    SetTargetFPS(60);

    //////////////////////////////////////
    /* init gui state */
    bool customFont = FileExists("assets/Charcoal.ttf");
    Font uiFont = customFont ? LoadFontEx("assets/Charcoal.ttf", 13, NULL, 0) : GetFontDefault();
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_POINT);

    struct nk_user_font nkFont;
    nk_zero_struct(nkFont);
    nkFont.userdata = nk_handle_ptr(&uiFont);
    nkFont.height = (float)uiFont.baseSize;
    nkFont.width = nk_raylib_text_width;

    struct nk_context ctx;
    nk_init_default(&ctx, &nkFont);

    enum {EASY, HARD};
    int op = EASY;
    float value = 0.6f;

    struct nk_rect triangleRegion = nk_rect(320, 50, 320, 280);
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

        struct nk_rect triangleContent = triangleRegion;
        if (nk_begin(&ctx, "Triangle", triangleRegion, NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_TITLE)) {
            triangleContent = nk_window_get_content_region(&ctx);
        }
        nk_end(&ctx);

        BeginDrawing();
        ClearBackground(WHITE);

        // window chrome first, so the triangle draws on top of it
        nk_raylib_render(&ctx);

        // a simple gradient triangle, drawn through raylib's rlgl layer
        // so we get per-vertex colors like the old raw OpenGL version did,
        // clipped and fitted inside the "Triangle" window's content area
        BeginScissorMode((int)triangleContent.x, (int)triangleContent.y,
            (int)triangleContent.w, (int)triangleContent.h);
        rlBegin(RL_TRIANGLES);
            rlColor3f(1.0f, 0.0f, 0.0f);
            rlVertex2f(triangleContent.x + triangleContent.w * 0.1f, triangleContent.y + triangleContent.h * 0.9f);
            rlColor3f(0.0f, 1.0f, 0.0f);
            rlVertex2f(triangleContent.x + triangleContent.w * 0.9f, triangleContent.y + triangleContent.h * 0.9f);
            rlColor3f(0.0f, 0.0f, 1.0f);
            rlVertex2f(triangleContent.x + triangleContent.w * 0.5f, triangleContent.y + triangleContent.h * 0.1f);
        rlEnd();
        EndScissorMode();

        EndDrawing();
    }

    nk_free(&ctx);
    if (customFont) UnloadFont(uiFont);

    CloseWindow();
    return 0;
}
