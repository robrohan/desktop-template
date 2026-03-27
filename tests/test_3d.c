#include <stdio.h>

#ifndef WEFX_NO_WALLOC
#define WEFX_NO_WALLOC
#endif
#ifndef WEFX_NO_EXPORT
#define WEFX_NO_EXPORT
#endif
#ifndef WEFX_NO_MATH
#define WEFX_NO_MATH
#endif
#ifndef WEFX_ORIGIN_TOP_LEFT
#define WEFX_ORIGIN_TOP_LEFT
#endif

#include "../vendor/r2_unit.h"

/* Include the CPU-side sources directly — no raylib needed */
#include "../src/wefx.c"
#include "../src/3d.c"

int r2_tests_run = 0;

/* ---------------------------------------------------------------
 * make_screenSpaceTransform
 * --------------------------------------------------------------- */

static const char *test_sst_center(void)
{
    /* clip-space origin (0,0,0,1) should map to screen center */
    mat4 m = make_screenSpaceTransform(160.0f, 120.0f);
    vec4 clip = {{0, 0, 0, 1}};
    vec4 screen = {0};
    mat4_transform(&clip, &m, &screen);
    r2_assert("clip (0,0) -> screen x == 160", screen.x == 160.0f);
    r2_assert("clip (0,0) -> screen y == 120", screen.y == 120.0f);
    return 0;
}

static const char *test_sst_top_right(void)
{
    /* clip-space (1,1) maps to top-right corner; Y is flipped */
    mat4 m = make_screenSpaceTransform(160.0f, 120.0f);
    vec4 clip = {{1, 1, 0, 1}};
    vec4 screen = {0};
    mat4_transform(&clip, &m, &screen);
    r2_assert("clip (1,1) -> screen x == 320", screen.x == 320.0f);
    r2_assert("clip (1,1) -> screen y == 0",   screen.y == 0.0f);
    return 0;
}

static const char *test_sst_bottom_left(void)
{
    /* clip-space (-1,-1) maps to bottom-left corner */
    mat4 m = make_screenSpaceTransform(160.0f, 120.0f);
    vec4 clip = {{-1, -1, 0, 1}};
    vec4 screen = {0};
    mat4_transform(&clip, &m, &screen);
    r2_assert("clip (-1,-1) -> screen x == 0",   screen.x == 0.0f);
    r2_assert("clip (-1,-1) -> screen y == 240", screen.y == 240.0f);
    return 0;
}

/* ---------------------------------------------------------------
 * is_top_left
 * --------------------------------------------------------------- */

static const char *test_tl_horizontal_right(void)
{
    /* horizontal edge going right → top edge → top-left bias */
    vec4 s = {{0, 0, 0, 1}};
    vec4 e = {{1, 0, 0, 1}};
    r2_assert("horizontal-right edge is top-left", is_top_left(&s, &e));
    return 0;
}

static const char *test_tl_horizontal_left(void)
{
    /* horizontal edge going left → NOT a top-left edge */
    vec4 s = {{1, 0, 0, 1}};
    vec4 e = {{0, 0, 0, 1}};
    r2_assert("horizontal-left edge is NOT top-left", !is_top_left(&s, &e));
    return 0;
}

static const char *test_tl_downward(void)
{
    /* edge going down (y increases) → left edge → top-left bias */
    vec4 s = {{0, 0, 0, 1}};
    vec4 e = {{0, 1, 0, 1}};
    r2_assert("downward edge is top-left", is_top_left(&s, &e));
    return 0;
}

static const char *test_tl_upward(void)
{
    /* edge going up (y decreases) → NOT a top-left edge */
    vec4 s = {{0, 1, 0, 1}};
    vec4 e = {{0, 0, 0, 1}};
    r2_assert("upward edge is NOT top-left", !is_top_left(&s, &e));
    return 0;
}

/* ---------------------------------------------------------------
 * triangle_fill
 * --------------------------------------------------------------- */

static const char *test_fill_draws_interior(void)
{
    /* Set up a 32×32 wefx buffer */
    wefx_open(32, 32, "test");
    wefx_clear_color(0, 0, 0);
    wefx_clear();

    /*
     * Solid 1×1 RGB texture: r=128 g=64 b=32
     * Using all-zero UVs so every sample hits image[0..2].
     */
    ui8 img[3] = {128, 64, 32};
    texture tex = {.w = 1, .h = 1, .image = img};

    /*
     * CW triangle in screen-space that covers pixel (5,5):
     *   A=(0,0)  B=(20,0)  C=(0,20)
     * ABC cross product = 400 > 0 (not culled).
     */
    vertex A = {.vec = {{ 0,  0, 0, 1}}, .u = 0, .v = 0};
    vertex B = {.vec = {{20,  0, 0, 1}}, .u = 0, .v = 0};
    vertex C = {.vec = {{ 0, 20, 0, 1}}, .u = 0, .v = 0};

    triangle_fill(A, B, C, &tex);

    /*
     * Expected colour for rgb(128,64,32):
     *   rgb_to_int = (0xFF<<24)|(32<<16)|(64<<8)|128 = 0xFF204080
     * wefx_pixel(5,5) with psize=1 writes to (5,5), so offset = 5+5*32 = 165.
     */
    unsigned int expected = (0xFF << 24) | (32 << 16) | (64 << 8) | 128;
    unsigned int *buf = wefx_get_buffer();
    r2_assert("interior pixel (5,5) should be filled", buf[5 + 5 * 32] == expected);
    return 0;
}

static const char *test_fill_backface_culled(void)
{
    /* CCW winding → ABC < 0 → early return, nothing drawn */
    wefx_open(32, 32, "test");
    wefx_clear_color(0, 0, 0);
    wefx_clear();

    ui8 img[3] = {255, 255, 255};
    texture tex = {.w = 1, .h = 1, .image = img};

    /* Swap B and C to flip winding to CCW */
    vertex A = {.vec = {{ 0,  0, 0, 1}}, .u = 0, .v = 0};
    vertex B = {.vec = {{ 0, 20, 0, 1}}, .u = 0, .v = 0};
    vertex C = {.vec = {{20,  0, 0, 1}}, .u = 0, .v = 0};

    triangle_fill(A, B, C, &tex);

    unsigned int *buf = wefx_get_buffer();
    /* background is rgb_to_int(0,0,0) = (0xFF<<24) = 0xFF000000 */
    unsigned int black_bg = (0xFF << 24);
    r2_assert("backface triangle draws nothing at (5,5)", buf[5 + 5 * 32] == black_bg);
    return 0;
}

/* ---------------------------------------------------------------
 * Suite
 * --------------------------------------------------------------- */

static const char *all_tests(void)
{
    r2_run_test(test_sst_center);
    r2_run_test(test_sst_top_right);
    r2_run_test(test_sst_bottom_left);
    r2_run_test(test_tl_horizontal_right);
    r2_run_test(test_tl_horizontal_left);
    r2_run_test(test_tl_downward);
    r2_run_test(test_tl_upward);
    r2_run_test(test_fill_draws_interior);
    r2_run_test(test_fill_backface_culled);
    return 0;
}

int main(void)
{
    const char *result = all_tests();
    if (result != 0) {
        printf("FAILED: %s\n", result);
    } else {
        printf("ALL TESTS PASSED (%d)\n", r2_tests_run);
    }
    return result != 0;
}
