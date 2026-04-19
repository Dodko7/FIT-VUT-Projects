/**
 * @file fonts.c
 * @brief Font definitions and rendering for countdown digits
 * @author Jozef Ondrejicka
 */

#include "../include/fonts.h"
#include "../include/display.h"

/* Original horizontal font definitions (8 rows × 5 columns) */
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

/* Transformed fonts (vertical orientation for display) */
uint8_t font_3[ROWS][5];
uint8_t font_2[ROWS][5];
uint8_t font_1[ROWS][5];

/**
 * @brief Transform fonts from horizontal to vertical orientation
 * Performs 90° clockwise rotation + vertical flip
 */
void transform_fonts(void)
{
    const uint8_t (*orig[3])[ROWS][5] = { &original_3, &original_2, &original_1 };
    uint8_t (*transformed[3])[ROWS][5] = { &font_3, &font_2, &font_1 };

    for (int num = 0; num < 3; num++) {
        // Step 1: 90° clockwise rotation
        uint8_t temp[ROWS][5] = {0};
        for (int old_y = 0; old_y < ROWS; old_y++) {
            for (int old_x = 0; old_x < 5; old_x++) {
                int new_x = old_y;
                int new_y = 4 - old_x;
                temp[new_x][new_y] = (*orig[num])[old_y][old_x];
            }
        }
        // Step 2: Vertical flip
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < 5; x++) {
                (*transformed[num])[ROWS - 1 - y][x] = temp[y][x];
            }
        }
    }
}

/**
 * @brief Draw large digit on display (centered)
 * @param digit Font array (font_1, font_2, or font_3)
 * @param r Red component (0-4095)
 * @param g Green component (0-4095)
 * @param b Blue component (0-4095)
 */
void draw_big_digit(const uint8_t digit[ROWS][5], uint16_t r, uint16_t g, uint16_t b)
{
    clear_screen();

    // Center digit on display
    int offset_x = (COLS - 8) / 2;
    int offset_y = (ROWS - 5) / 2;

    // Draw digit pixels
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < ROWS; x++) {
            if (digit[x][y]) {
                set_pixel(offset_x + x, offset_y + y, r, g, b);
            }
        }
    }
}

