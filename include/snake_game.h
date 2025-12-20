#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <stdint.h>
#include "gpio_hw.h"

/* Game point type */
typedef struct { 
    int x, y; 
} point_t;

/* Game speed control */
void snake_game_set_speed_ms(int speed_ms);
int snake_game_get_speed_ms(void);

/* Game color control */
void snake_game_set_head_color(uint16_t r, uint16_t g, uint16_t b);
void snake_game_set_body_color(uint16_t r, uint16_t g, uint16_t b);
void snake_game_get_head_color(uint16_t *r, uint16_t *g, uint16_t *b);
void snake_game_get_body_color(uint16_t *r, uint16_t *g, uint16_t *b);

/* Game control */
void snake_game_start_task(void);
void snake_game_init(void);

#endif // SNAKE_GAME_H

