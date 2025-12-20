#include "../include/fonts.h"
#include "../include/display.h"

/* Original fonts */
static const uint8_t original_3[ROWS][5] = {
    {1,1,1,1,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {1,1,1,1,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {1,1,1,1,1}
};

static const uint8_t original_2[ROWS][5] = {
    {1,1,1,1,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {1,1,1,1,1},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,1,1,1,1}
};

static const uint8_t original_1[ROWS][5] = {
    {0,0,1,0,0},
    {0,1,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
    {1,1,1,1,1}
};

/* Transformed fonts */
uint8_t font_3[ROWS][5];
uint8_t font_2[ROWS][5];
uint8_t font_1[ROWS][5];

void transform_fonts(void)
{
    // 90° clockwise + flip up-down
    const uint8_t (*orig[3])[ROWS][5] = { &original_3, &original_2, &original_1 };
    uint8_t (*transformed[3])[ROWS][5] = { &font_3, &font_2, &font_1 };

    for (int num = 0; num < 3; num++) {
        // Najprv 90° CW
        uint8_t temp[ROWS][5] = {0};
        for (int old_y = 0; old_y < ROWS; old_y++) {
            for (int old_x = 0; old_x < 5; old_x++) {
                int new_x = old_y;
                int new_y = 4 - old_x;
                temp[new_x][new_y] = (*orig[num])[old_y][old_x];
            }
        }
        // Potom flip up-down
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < 5; x++) {
                (*transformed[num])[ROWS - 1 - y][x] = temp[y][x];
            }
        }
    }
}

void draw_big_digit(const uint8_t digit[ROWS][5], uint16_t r, uint16_t g, uint16_t b)
{
    clear_screen();

    // Centrovanie
    int offset_x = (COLS - 8) / 2;
    int offset_y = (ROWS - 5) / 2;

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < ROWS; x++) {
            if (digit[x][y]) {
                set_pixel(offset_x + x, offset_y + y, r, g, b);
            }
        }
    }
}

