#include "../include/display.h"
#include "../include/gpio_hw.h"

#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* TLC channel mapping */
static const uint8_t MAP_R[ROWS] = {1,4,7,10,13,16,19,22};
static const uint8_t MAP_G[ROWS] = {0,3,6,9,12,15,18,21};
static const uint8_t MAP_B[ROWS] = {2,5,8,11,14,17,20,23};

/* Framebuffer */
static uint16_t framebuffer[COLS][ROWS][3];
static uint16_t tlc[TLC_CHANNELS];
static spi_device_handle_t spi;
static uint32_t column_delay_us = 250;

/* Forward declarations */
static void select_column(uint8_t col);
static void send_column(uint8_t col);

/* Framebuffer API */
void clear_screen(void)
{
    memset(framebuffer, 0, sizeof(framebuffer));
}

void set_pixel(int x, int y, uint16_t r, uint16_t g, uint16_t b)
{
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    framebuffer[x][y][0] = r;
    framebuffer[x][y][1] = g;
    framebuffer[x][y][2] = b;
}

/* Column selection */
static inline void select_column(uint8_t col)
{
    gpio_set_level(PIN_ADR0, (col >> 0) & 1);
    gpio_set_level(PIN_ADR1, (col >> 1) & 1);
    gpio_set_level(PIN_ADR2, (col >> 2) & 1);
    gpio_set_level(PIN_ADR3, (col >> 3) & 1);
}

/* Send column data to TLC5940 */
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
    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret != ESP_OK) {
    }

    gpio_set_level(PIN_XLAT, 1);
    esp_rom_delay_us(1);
    gpio_set_level(PIN_XLAT, 0);
}

/* Display task */
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
            esp_rom_delay_us(column_delay_us);
        }
    }
}

/* Public API */
void display_start_task(void)
{
    xTaskCreate(display_task, "display", 4096, NULL, 5, NULL);
}

void display_set_column_delay_us(uint32_t delay_us)
{
    column_delay_us = delay_us;
}

/* Initialize SPI handle - called from gpio_hw.c */
void display_set_spi_handle(spi_device_handle_t handle)
{
    spi = handle;
}

