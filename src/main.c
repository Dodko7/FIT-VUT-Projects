#include "../include/gpio_hw.h"
#include "../include/display.h"
#include "../include/fonts.h"
#include "../include/snake_game.h"

void app_main(void)
{
    gpio_init_all();
    spi_init();
    clear_screen();

    transform_fonts();

    display_start_task();
    snake_game_start_task();
}
