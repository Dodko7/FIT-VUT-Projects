/**
 * @file main.c
 * @brief Main application entry point for ESP32 Snake game
 * @author Jozef Ondrejicka
 */

#include "../include/gpio_hw.h"
#include "../include/display.h"
#include "../include/fonts.h"
#include "../include/snake_game.h"

/**
 * @brief Main application entry point
 * Initializes hardware, transforms fonts, and starts game/display tasks
 */
void app_main(void)
{
    gpio_init_all();
    spi_init();
    clear_screen();

    transform_fonts();

    display_start_task();
    snake_game_start_task();
}
