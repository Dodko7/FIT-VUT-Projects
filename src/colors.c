#include "../include/colors.h"

static uint8_t global_brightness_percent = 100;

void color_set_brightness(uint8_t brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    global_brightness_percent = brightness_percent;
}

uint8_t color_get_brightness(void)
{
    return global_brightness_percent;
}

rgb_triple_t color_to_rgb_triple(color_enum_t color, uint8_t brightness_percent)
{
    // Clamp brightness to valid range
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    
    // Calculate coefficient and max value
    float coeff = brightness_percent / 100.0f;
    uint16_t max = (uint16_t)(COLOR_BRIGHTNESS_MAX * coeff);

    switch (color)
    {
    case COLOR_RED:
        return (rgb_triple_t){max, 0, 0};
    case COLOR_GREEN:
        return (rgb_triple_t){0, max, 0};
    case COLOR_BLUE:
        return (rgb_triple_t){0, 0, max};
    case COLOR_CYAN:
        return (rgb_triple_t){0, max, max};
    case COLOR_MAGENTA:
        return (rgb_triple_t){max, 0, max};
    case COLOR_YELLOW:
        return (rgb_triple_t){max, max, 0};
    case COLOR_WHITE:
        return (rgb_triple_t){max, max, max};
    case COLOR_ORANGE:
        return (rgb_triple_t){max, max / 2, 0};
    case COLOR_INDIGO:
        return (rgb_triple_t){max / 2, 0, max};
    case COLOR_VIOLET:
        return (rgb_triple_t){max * 3 / 4, 0, max};
    case COLOR_BLACK:
    default:
        return (rgb_triple_t){0, 0, 0};
    }
}

