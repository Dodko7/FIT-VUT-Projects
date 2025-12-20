#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

/* Maximum brightness value for 12-bit TLC5940 */
#define COLOR_BRIGHTNESS_MAX 4095

/* Color enumeration */
typedef enum {
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_CYAN,
    COLOR_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE,
    COLOR_ORANGE,
    COLOR_INDIGO,
    COLOR_VIOLET,
    COLOR_BLACK
} color_enum_t;

/* RGB triple structure */
typedef struct {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} rgb_triple_t;

/* Convert color enum to RGB triple with brightness control */
rgb_triple_t color_to_rgb_triple(color_enum_t color, uint8_t brightness_percent);

#endif // COLORS_H

