/**
 * @file display.c
 * @brief Display driver for 16x8 RGB LED matrix using TLC5947
 * @author Jozef Ondrejicka
 */

#include "../include/display.h"
#include "../include/gpio_hw.h"

#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* TLC5947 channel mapping for RGB rows */
static const uint8_t MAP_R[ROWS] = {1,4,7,10,13,16,19,22};
static const uint8_t MAP_G[ROWS] = {0,3,6,9,12,15,18,21};
static const uint8_t MAP_B[ROWS] = {2,5,8,11,14,17,20,23};

/* Framebuffer storage [column][row][RGB] */
static uint16_t framebuffer[COLS][ROWS][3];
static uint16_t tlc[TLC_CHANNELS];
static spi_device_handle_t spi;
static uint32_t column_delay_us = 250;

/* Forward declarations */
static void select_column(uint8_t col);
static void send_column(uint8_t col);

/**
 * @brief Clear entire framebuffer (set all pixels to black)
 */
void clear_screen(void)
{
    memset(framebuffer, 0, sizeof(framebuffer));
}

/**
 * @brief Set pixel color in framebuffer
 * @param x Column (0 to COLS-1)
 * @param y Row (0 to ROWS-1)
 * @param r Red component (0-4095)
 * @param g Green component (0-4095)
 * @param b Blue component (0-4095)
 */
void set_pixel(int x, int y, uint16_t r, uint16_t g, uint16_t b)
{
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    framebuffer[x][y][0] = r;
    framebuffer[x][y][1] = g;
    framebuffer[x][y][2] = b;
}

/**
 * @brief Select column using 74HC154 decoder
 * @param col Column number (0-15)
 */
static inline void select_column(uint8_t col)
{
    gpio_set_level(PIN_ADR0, col & 1);
    gpio_set_level(PIN_ADR1, (col >> 1) & 1);
    gpio_set_level(PIN_ADR2, (col >> 2) & 1);
    gpio_set_level(PIN_ADR3, (col >> 3) & 1);
}

/**
 * @brief Send column data to TLC5947 via SPI
 * Maps framebuffer to TLC channels, serializes to bitstream, transmits via SPI
 * @param col Column number to send
 */
static void send_column(uint8_t col)
{
    memset(tlc, 0, sizeof(tlc));

    // Map framebuffer RGB to TLC5947 channels
    for (int r = 0; r < ROWS; r++) {
        tlc[MAP_R[r]] = framebuffer[col][r][0];
        tlc[MAP_G[r]] = framebuffer[col][r][1];
        tlc[MAP_B[r]] = framebuffer[col][r][2];
    }

    // Serialize 12-bit values to byte array (MSB-first)
    uint8_t tx[TLC_TOTAL_BYTES] = {0};
    int bit = 0;

    // Serialize channels in reverse order
    for (int ch = TLC_CHANNELS - 1; ch >= 0; ch--) {
        uint16_t v = tlc[ch] & 0x0FFF;
        for (int b = 11; b >= 0; b--) {
            if (v & (1 << b))
                tx[bit >> 3] |= (1 << (7 - (bit & 7)));
            bit++;
        }
    }

    // Transmit via SPI
    spi_transaction_t t = {
        .length = TLC_TOTAL_BITS,
        .tx_buffer = tx
    };
    esp_err_t ret = spi_device_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);

    // Latch data into TLC5947
    gpio_set_level(PIN_XLAT, 1);
    esp_rom_delay_us(1);
    gpio_set_level(PIN_XLAT, 0);
}

/**
 * @brief Display refresh task (FreeRTOS)
 * Continuously cycles through columns for persistence of vision effect
 */
static void display_task()
{
    // Main refresh loop
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

/**
 * @brief Start display refresh task
 */
void display_start_task(void)
{
    xTaskCreate(display_task, "display", 4096, NULL, 5, NULL);
}

/**
 * @brief Set column display delay (affects refresh rate)
 * @param delay_us Delay in microseconds per column
 */
void display_set_column_delay_us(uint32_t delay_us)
{
    column_delay_us = delay_us;
}

/**
 * @brief Set SPI device handle (called from gpio_hw.c after SPI init)
 * @param handle SPI device handle
 */
void display_set_spi_handle(spi_device_handle_t handle)
{
    spi = handle;
}

