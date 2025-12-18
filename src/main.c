#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_system.h"
#include "esp_random.h"

/* ============================================================ */
static const char *TAG = "SNAKE";
/* ============================================================ */
/* PINY – DISPLEJ */
#define PIN_ADR0 25
#define PIN_ADR1 17
#define PIN_ADR2 16
#define PIN_ADR3 27
#define PIN_EN   14

#define PIN_SPI_MOSI 23
#define PIN_SPI_CLK  18
#define PIN_XLAT     13
#define PIN_BLANK    12

#define SNAKE_SPEED_MS 150

/* PINY – TLAČIDLÁ */
#define SW1_DOWN   22
#define SW2_UP     21
#define SW3_LEFT   26
#define SW4_RIGHT  4

/* ROZMERY */
#define COLS 16
#define ROWS 8

/* TLC */
#define TLC_CHANNELS     24
#define TLC_BITS_PER_CH  12
#define TLC_TOTAL_BITS   (TLC_CHANNELS * TLC_BITS_PER_CH)
#define TLC_TOTAL_BYTES (TLC_TOTAL_BITS / 8)

/* MAPOVANIE TLC */
static const uint8_t MAP_R[ROWS] = {1,4,7,10,13,16,19,22};
static const uint8_t MAP_G[ROWS] = {0,3,6,9,12,15,18,21};
static const uint8_t MAP_B[ROWS] = {2,5,8,11,14,17,20,23};

/* FRAMEBUFFER */
static uint16_t framebuffer[COLS][ROWS][3];
static uint16_t tlc[TLC_CHANNELS];
static spi_device_handle_t spi;

/* ============================================================ */
/* FRAMEBUFFER API */
static inline void clear_screen(void)
{
    memset(framebuffer, 0, sizeof(framebuffer));
}

static inline void set_pixel(int x, int y, uint16_t r, uint16_t g, uint16_t b)
{
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    framebuffer[x][y][0] = r;
    framebuffer[x][y][1] = g;
    framebuffer[x][y][2] = b;
}

/* ============================================================ */
/* GPIO + SPI INIT */
static void gpio_init_all(void)
{
    gpio_config_t out = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL<<PIN_ADR0)|(1ULL<<PIN_ADR1)|(1ULL<<PIN_ADR2)|(1ULL<<PIN_ADR3)|
            (1ULL<<PIN_EN)|(1ULL<<PIN_XLAT)|(1ULL<<PIN_BLANK)
    };
    gpio_config(&out);

    gpio_config_t in = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =
            (1ULL<<SW1_DOWN)|(1ULL<<SW2_UP)|(1ULL<<SW3_LEFT)|(1ULL<<SW4_RIGHT),
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&in);

    gpio_set_level(PIN_EN, 1);
    gpio_set_level(PIN_BLANK, 1);
    gpio_set_level(PIN_XLAT, 0);
}

static void spi_init(void)
{
    spi_bus_config_t bus = {
        .mosi_io_num = PIN_SPI_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_SPI_CLK,
        .max_transfer_sz = TLC_TOTAL_BYTES
    };
    spi_device_interface_config_t dev = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1
    };
    spi_bus_initialize(SPI3_HOST, &bus, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI3_HOST, &dev, &spi);
}

/* ============================================================ */
/* MULTIPLEX */
static inline void select_column(uint8_t col)
{
    gpio_set_level(PIN_ADR0, (col >> 0) & 1);
    gpio_set_level(PIN_ADR1, (col >> 1) & 1);
    gpio_set_level(PIN_ADR2, (col >> 2) & 1);
    gpio_set_level(PIN_ADR3, (col >> 3) & 1);
}

static void send_column(uint8_t col)
{
    memset(tlc, 0, sizeof(tlc));

    for (int r = 0; r < ROWS; r++) {
        tlc[MAP_R[r]] = framebuffer[col][r][0];
        tlc[MAP_G[r]] = framebuffer[col][r][1];
        tlc[MAP_B[r]] = framebuffer[col][r][2];
    }

    uint8_t tx[TLC_TOTAL_BYTES] = {0};
    int bit = 0;

    for (int ch = TLC_CHANNELS - 1; ch >= 0; ch--) {
        uint16_t v = tlc[ch] & 0x0FFF;
        for (int b = 11; b >= 0; b--) {
            if (v & (1 << b))
                tx[bit >> 3] |= (1 << (7 - (bit & 7)));
            bit++;
        }
    }

    spi_transaction_t t = {
        .length = TLC_TOTAL_BITS,
        .tx_buffer = tx
    };
    spi_device_transmit(spi, &t);

    gpio_set_level(PIN_XLAT, 1);
    esp_rom_delay_us(1);
    gpio_set_level(PIN_XLAT, 0);
}

static void display_task(void *arg)
{
    while (1) {
        for (uint8_t c = 0; c < COLS; c++) {
            gpio_set_level(PIN_BLANK, 1);
            gpio_set_level(PIN_EN, 1);
            select_column(c);
            send_column(c);
            gpio_set_level(PIN_EN, 0);
            gpio_set_level(PIN_BLANK, 0);
            esp_rom_delay_us(250);
        }
    }
}

/* ============================================================ */
/* SNAKE GAME */
typedef struct { int x, y; } point_t;

#define SNAKE_MAX 32
static point_t snake[SNAKE_MAX];
static int snake_len;
static int dx, dy;

static point_t food_grow;
static point_t food_poison;

/* ------------------------------------------------------------ */
static int snake_contains(int x, int y)
{
    for (int i = 0; i < snake_len; i++)
        if (snake[i].x == x && snake[i].y == y)
            return 1;
    return 0;
}

static void spawn_food(point_t *f)
{
    do {
        f->x = esp_random() % COLS;
        f->y = esp_random() % ROWS;
    } while (snake_contains(f->x, f->y) ||
             (f != &food_grow && f->x == food_grow.x && f->y == food_grow.y) ||
             (f != &food_poison && f->x == food_poison.x && f->y == food_poison.y));
}

static void snake_init(void)
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

static void read_buttons(void)
{
    if (!gpio_get_level(SW1_DOWN) && dy == 0) { dx = 0; dy = 1; }
    if (!gpio_get_level(SW2_UP)   && dy == 0) { dx = 0; dy = -1; }
    if (!gpio_get_level(SW3_LEFT) && dx == 0) { dx = 1; dy = 0; }
    if (!gpio_get_level(SW4_RIGHT)&& dx == 0) { dx = -1; dy = 0; }
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

    if (snake_len < 5)
        return -1;

    return 0;
}

static void snake_render(void)
{
    clear_screen();

    set_pixel(food_grow.x, food_grow.y, 0, 0, 4095);
    set_pixel(food_poison.x, food_poison.y, 4095, 0, 0);

    for (int i = 0; i < snake_len; i++) {
        if (i == 0)
            set_pixel(snake[i].x, snake[i].y, 1024, 1024, 0);
        else
            set_pixel(snake[i].x, snake[i].y, 0, 512, 0);
    }
}

static void game_over(void)
{
    clear_screen();
    for (int x = 0; x < COLS; x++)
        for (int y = 0; y < ROWS; y++)
            set_pixel(x, y, 4095, 0, 0);

    vTaskDelay(pdMS_TO_TICKS(1000));
    snake_init();
}

static void game_task(void *arg)
{
    snake_init();

    while (1) {
        read_buttons();

        if (snake_step() < 0)
            game_over();

        snake_render();
        vTaskDelay(pdMS_TO_TICKS(SNAKE_SPEED_MS));
    }
}

/* ============================================================ */
void app_main(void)
{
    ESP_LOGI(TAG, "Snake – two apples logic");

    gpio_init_all();
    spi_init();
    clear_screen();

    xTaskCreate(display_task, "display", 4096, NULL, 5, NULL);
    xTaskCreate(game_task, "game", 4096, NULL, 4, NULL);
}
