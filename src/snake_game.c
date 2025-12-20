#include "../include/snake_game.h"
#include "../include/display.h"
#include "../include/fonts.h"
#include "../include/gpio_hw.h"
#include "../include/colors.h"

#include "driver/gpio.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SNAKE_MAX 32
#define DEBOUNCE_TIME_US 50000  // 50ms debounce time
#define MIN_SPEED_MS 40
#define MAX_SPEED_MS 400
#define SPEED_STEP_MS 20
#define GAME_OVER_SNAKE_DELAY_MS 1200
#define GAME_OVER_PAUSE_MS 600
#define GAME_OVER_WAVE_WIDTH 3
#define GAME_OVER_COL_DELAY_MS 20
#define COUNTDOWN_DELAY_MS 1200
#define MIN_SNAKE_LEN 5

/* Game state */
static point_t snake[SNAKE_MAX];
static int snake_len;
static int dx, dy;

static point_t food_grow;
static point_t food_poison;

/* Game settings */
static int game_speed_ms = 150;

/* Color settings using unified color system */
static color_enum_t game_color_head = COLOR_YELLOW;
static color_enum_t game_color_body = COLOR_GREEN;
static uint8_t game_brightness = 100;

/* Helper functions */
static int snake_contains(int x, int y)
{
    for (int i = 0; i < snake_len; i++)
        if (snake[i].x == x && snake[i].y == y)
            return 1;
    return 0;
}

static void spawn_food(point_t *f)
{
    int attempts = 0;
    const int max_attempts = 100;

    do {
        f->x = esp_random() % COLS;
        f->y = esp_random() % ROWS;
        attempts++;
        if (attempts > max_attempts) {
            break;
        }
    } while (snake_contains(f->x, f->y) ||
             (f != &food_grow && f->x == food_grow.x && f->y == food_grow.y) ||
             (f != &food_poison && f->x == food_poison.x && f->y == food_poison.y));
}

static void read_buttons(void)
{
    static int64_t last_down_time = 0;
    static int64_t last_up_time = 0;
    static int64_t last_left_time = 0;
    static int64_t last_right_time = 0;

    int64_t now = esp_timer_get_time();

    if (!gpio_get_level(SW1_DOWN) && (now - last_down_time > DEBOUNCE_TIME_US) && dy == 0) {
        dx = 0; dy = 1;
        last_down_time = now;
    }
    if (!gpio_get_level(SW2_UP) && (now - last_up_time > DEBOUNCE_TIME_US) && dy == 0) {
        dx = 0; dy = -1;
        last_up_time = now;
    }
    if (!gpio_get_level(SW3_LEFT) && (now - last_left_time > DEBOUNCE_TIME_US) && dx == 0) {
        dx = 1; dy = 0;
        last_left_time = now;
    }
    if (!gpio_get_level(SW4_RIGHT) && (now - last_right_time > DEBOUNCE_TIME_US) && dx == 0) {
        dx = -1; dy = 0;
        last_right_time = now;
    }
}

static int snake_step(void)
{
    for (int i = snake_len - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0].x = (snake[0].x - dy + COLS) % COLS;
    snake[0].y = (snake[0].y + dx + ROWS) % ROWS;

    for (int i = 1; i < snake_len; i++)
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            return -1;

    if (snake[0].x == food_grow.x && snake[0].y == food_grow.y) {
        if (snake_len < SNAKE_MAX) snake_len++;
        spawn_food(&food_grow);
    }

    if (snake[0].x == food_poison.x && snake[0].y == food_poison.y) {
        snake_len--;
        spawn_food(&food_poison);
    }

    if (snake_len < MIN_SNAKE_LEN)
        return -1;

    return 0;
}

static void snake_render(void)
{
    rgb_triple_t head_rgb = color_to_rgb_triple(game_color_head, game_brightness);
    // Body uses same color but at half brightness
    rgb_triple_t body_rgb = color_to_rgb_triple(game_color_body, game_brightness / 2);
    rgb_triple_t food_grow_rgb = color_to_rgb_triple(COLOR_BLUE, game_brightness);
    rgb_triple_t food_poison_rgb = color_to_rgb_triple(COLOR_RED, game_brightness);

    clear_screen();
    set_pixel(food_grow.x, food_grow.y, food_grow_rgb.r, food_grow_rgb.g, food_grow_rgb.b);
    set_pixel(food_poison.x, food_poison.y, food_poison_rgb.r, food_poison_rgb.g, food_poison_rgb.b);

    for (int i = 0; i < snake_len; i++) {
        if (i == 0)
            set_pixel(snake[i].x, snake[i].y, head_rgb.r, head_rgb.g, head_rgb.b);
        else
            set_pixel(snake[i].x, snake[i].y, body_rgb.r, body_rgb.g, body_rgb.b);
    }
}

static void screensaver_loop(void)
{
    point_t ss_snake[SNAKE_MAX];
    int ss_len = 6;
    int ss_dx = 0, ss_dy = -1;

    point_t tl={6,2}, tr={9,2}, br={9,5}, bl={6,5};

    for(int i=0;i<ss_len;i++){
        ss_snake[i].x = tl.x;
        ss_snake[i].y = bl.y - i;
    }

    while (1) {

        if (!gpio_get_level(SW2_UP)) {
            if (game_speed_ms > MIN_SPEED_MS) game_speed_ms -= SPEED_STEP_MS;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!gpio_get_level(SW1_DOWN)) {
            if (game_speed_ms < MAX_SPEED_MS) game_speed_ms += SPEED_STEP_MS;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!gpio_get_level(SW4_RIGHT)) {
            // Cycle through colors (skip COLOR_BLACK)
            game_color_head = (game_color_head + 1) % COLOR_BLACK;
            game_color_body = game_color_head;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!gpio_get_level(SW3_LEFT)) {
            return;
        }

        for (int i = ss_len - 1; i > 0; i--)
            ss_snake[i] = ss_snake[i - 1];

        ss_snake[0].x += ss_dx;
        ss_snake[0].y += ss_dy;

        if (ss_snake[0].x == tl.x && ss_snake[0].y == tl.y) {
            ss_dx = 1; ss_dy = 0;
        } else if (ss_snake[0].x == tr.x && ss_snake[0].y == tr.y) {
            ss_dx = 0; ss_dy = 1;
        } else if (ss_snake[0].x == br.x && ss_snake[0].y == br.y) {
            ss_dx = -1; ss_dy = 0;
        } else if (ss_snake[0].x == bl.x && ss_snake[0].y == bl.y) {
            ss_dx = 0; ss_dy = -1;
        }

        rgb_triple_t head_rgb = color_to_rgb_triple(game_color_head, game_brightness);
        // Body uses same color but at half brightness
        rgb_triple_t body_rgb = color_to_rgb_triple(game_color_body, game_brightness / 2);
        
        clear_screen();
        for (int i=0;i<ss_len;i++) {
            if (i==0)
                set_pixel(ss_snake[i].x, ss_snake[i].y, head_rgb.r, head_rgb.g, head_rgb.b);
            else
                set_pixel(ss_snake[i].x, ss_snake[i].y, body_rgb.r, body_rgb.g, body_rgb.b);
        }

        vTaskDelay(pdMS_TO_TICKS(game_speed_ms));
    }
}

static void game_over(void)
{
    rgb_triple_t red_rgb = color_to_rgb_triple(COLOR_RED, game_brightness);
    rgb_triple_t yellow_rgb = color_to_rgb_triple(COLOR_YELLOW, game_brightness);
    rgb_triple_t green_rgb = color_to_rgb_triple(COLOR_GREEN, game_brightness);

    // 1. Zčervenaj celý had
    clear_screen();
    for (int i = 0; i < snake_len; i++) {
        set_pixel(snake[i].x, snake[i].y, red_rgb.r, red_rgb.g, red_rgb.b);
    }
    vTaskDelay(pdMS_TO_TICKS(GAME_OVER_SNAKE_DELAY_MS));

    // 2. Pauza pred pásom
    vTaskDelay(pdMS_TO_TICKS(GAME_OVER_PAUSE_MS));

    // 3. Červený pás ide cez columns – obrátený smer (od COL15 po COL0)
    clear_screen();
    for (int start_col = COLS - 1; start_col >= -GAME_OVER_WAVE_WIDTH; start_col--) {
        clear_screen();
        for (int col = start_col; col > start_col - GAME_OVER_WAVE_WIDTH && col >= 0; col--) {
            if (col < COLS) {
                for (int row = 0; row < ROWS; row++) {
                    set_pixel(col, row, red_rgb.r, red_rgb.g, red_rgb.b);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(GAME_OVER_COL_DELAY_MS));
    }

    // Screensaver – had v smere hodinových ručičiek so žltou hlavou
    screensaver_loop();

    // Countdown 3-2-1
    draw_big_digit(font_3, red_rgb.r, red_rgb.g, red_rgb.b);
    vTaskDelay(pdMS_TO_TICKS(COUNTDOWN_DELAY_MS));
    draw_big_digit(font_2, yellow_rgb.r, yellow_rgb.g, yellow_rgb.b);
    vTaskDelay(pdMS_TO_TICKS(COUNTDOWN_DELAY_MS));
    draw_big_digit(font_1, green_rgb.r, green_rgb.g, green_rgb.b);
    vTaskDelay(pdMS_TO_TICKS(COUNTDOWN_DELAY_MS));

    snake_game_init();
}

static void game_task(void *arg)
{
    snake_game_init();
    while (1) {
        read_buttons();
        if (snake_step() < 0)
            game_over();
        snake_render();
        vTaskDelay(pdMS_TO_TICKS(game_speed_ms));
    }
}

/* Public API */
void snake_game_init(void)
{
    snake_len = 6;
    dx = 1; dy = 0;

    for (int i = 0; i < snake_len; i++) {
        snake[i].x = 6 - i;
        snake[i].y = 4;
    }

    spawn_food(&food_grow);
    spawn_food(&food_poison);
}

void snake_game_start_task(void)
{
    xTaskCreate(game_task, "game", 4096, NULL, 4, NULL);
}

void snake_game_set_speed_ms(int speed_ms)
{
    game_speed_ms = speed_ms;
}

int snake_game_get_speed_ms(void)
{
    return game_speed_ms;
}

void snake_game_set_head_color(uint16_t r, uint16_t g, uint16_t b)
{
    // Note: This function is kept for API compatibility but does nothing
    // as the game now uses color enums. Consider removing from public API.
    (void)r; (void)g; (void)b;
}

void snake_game_set_body_color(uint16_t r, uint16_t g, uint16_t b)
{
    // Note: This function is kept for API compatibility but does nothing
    // as the game now uses color enums. Consider removing from public API.
    (void)r; (void)g; (void)b;
}

void snake_game_get_head_color(uint16_t *r, uint16_t *g, uint16_t *b)
{
    rgb_triple_t rgb = color_to_rgb_triple(game_color_head, game_brightness);
    if (r) *r = rgb.r;
    if (g) *g = rgb.g;
    if (b) *b = rgb.b;
}

void snake_game_get_body_color(uint16_t *r, uint16_t *g, uint16_t *b)
{
    rgb_triple_t rgb = color_to_rgb_triple(game_color_body, game_brightness);
    if (r) *r = rgb.r;
    if (g) *g = rgb.g;
    if (b) *b = rgb.b;
}

