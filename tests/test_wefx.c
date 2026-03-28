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
#include "../src/wefx.c"

int r2_tests_run = 0;

static const char *test_open(void)
{
    int result = wefx_open(10, 10, "test");
    r2_assert("wefx_open should return 0 on success", result == 0);
    r2_assert("wefx_xsize should return the given width", wefx_xsize() == 10);
    r2_assert("wefx_ysize should return the given height", wefx_ysize() == 10);
    return 0;
}

static const char *test_get_buffer(void)
{
    wefx_open(4, 4, "test");
    unsigned int *buf = wefx_get_buffer();
    r2_assert("wefx_get_buffer should not return NULL", buf != NULL);
    return 0;
}

static const char *test_clear(void)
{
    wefx_open(4, 4, "test");
    wefx_clear_color(0xFF, 0x00, 0x00); /* red background */
    wefx_clear();
    unsigned int *buf = wefx_get_buffer();
    /* rgb_to_int(0xFF, 0, 0) = (0xFF<<24) | (0<<16) | (0<<8) | 0xFF */
    unsigned int expected = (0xFF << 24) | (0 << 16) | (0 << 8) | 0xFF;
    r2_assert("buffer[0] should equal background color after clear", buf[0] == expected);
    r2_assert("buffer[15] should equal background color after clear", buf[15] == expected);
    return 0;
}

static const char *test_color_and_point(void)
{
    wefx_open(10, 10, "test");
    wefx_clear_color(0, 0, 0);
    wefx_clear();
    wefx_color(255, 0, 0); /* red foreground */
    wefx_point(2, 3);
    unsigned int *buf = wefx_get_buffer();
    /* rgb_to_int(255, 0, 0) = (0xFF<<24) | (0<<16) | (0<<8) | 255 */
    unsigned int expected = (0xFF << 24) | (0 << 16) | (0 << 8) | 255;
    /* with WEFX_ORIGIN_TOP_LEFT: offset = x + y*w = 2 + 3*10 = 32 */
    r2_assert("point at (2,3) should be red", buf[2 + 3 * 10] == expected);
    return 0;
}

static const char *test_clear_resets_pixels(void)
{
    wefx_open(8, 8, "test");
    wefx_color(255, 255, 255);
    wefx_point(0, 0);
    unsigned int *buf = wefx_get_buffer();
    /* white: (0xFF<<24)|(255<<16)|(255<<8)|255 = 0xFFFFFFFF */
    unsigned int white = (0xFF << 24) | (255 << 16) | (255 << 8) | 255;
    r2_assert("point drawn at (0,0) should be white", buf[0] == white);

    wefx_clear_color(0, 0, 0);
    wefx_clear();
    /* black bg: (0xFF<<24)|(0<<16)|(0<<8)|0 = 0xFF000000 */
    unsigned int black = (0xFF << 24);
    r2_assert("clear resets (0,0) to black bg color", buf[0] == black);
    return 0;
}

static const char *all_tests(void)
{
    r2_run_test(test_open);
    r2_run_test(test_get_buffer);
    r2_run_test(test_clear);
    r2_run_test(test_color_and_point);
    r2_run_test(test_clear_resets_pixels);
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
