#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>
#include "gpio_hw.h"

/* Font array type */
extern uint8_t font_3[ROWS][5];
extern uint8_t font_2[ROWS][5];
extern uint8_t font_1[ROWS][5];

/* Transform fonts (90° CW + flip up-down) */
void transform_fonts(void);

/* Draw a big digit on screen */
void draw_big_digit(const uint8_t digit[ROWS][5], uint16_t r, uint16_t g, uint16_t b);

#endif // FONTS_H

