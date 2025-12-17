#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "esp_rom_sys.h"

/* ============================================================
 *  LOG
 * ============================================================ */
static const char *TAG = "LED_MATRIX";

/* ============================================================
 *  PIN MAPOVANIE (PODĽA SCHÉMY SHIELDU)
 * ============================================================ */

/* 74HCT154 – výber stĺpca (aktívny LOW) */
#define PIN_ADR0 25
#define PIN_ADR1 16
#define PIN_ADR2 17
#define PIN_ADR3 27
#define PIN_EN   14

/* TLC5947 */
#define PIN_SPI_CLK   18
#define PIN_SPI_MOSI  23
#define PIN_XLAT      26
#define PIN_BLANK     12

/* ============================================================
 *  ROZMERY DISPLEJA
 * ============================================================ */
#define COLS 16
#define ROWS 8

/* ============================================================
 *  FARBA (12-bit PWM)
 * ============================================================ */
#define COLOR_R 2048
#define COLOR_G 2048
#define COLOR_B 2048

/* ============================================================
 *  TLC5947 PARAMETRE
 * ============================================================ */
#define TLC_CHANNELS 24
#define TLC_BITS_PER_CH 12
#define TLC_TOTAL_BITS  (TLC_CHANNELS * TLC_BITS_PER_CH)
#define TLC_TOTAL_BYTES (TLC_TOTAL_BITS / 8)

/* ============================================================
 *  MAPOVANIE TLC5947 KANÁLOV (PODĽA SCHÉMY)
 * ============================================================ */
static const uint8_t MAP_R[ROWS] = {1, 4, 7, 10, 13, 16, 19, 22};
static const uint8_t MAP_G[ROWS] = {0, 3, 6, 9, 12, 15, 18, 21};
static const uint8_t MAP_B[ROWS] = {2, 5, 8, 11, 14, 17, 20, 23};

/* ============================================================
 *  FRAMEBUFFER
 *  framebuffer[col][row][0]=R, [1]=G, [2]=B
 * ============================================================ */
static uint16_t framebuffer[COLS][ROWS][3];
static uint16_t tlc_channels[TLC_CHANNELS];

static spi_device_handle_t spi;

/* ============================================================
 *  GPIO INIT
 * ============================================================ */
static void gpio_init_all(void)
{
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL << PIN_ADR0) |
            (1ULL << PIN_ADR1) |
            (1ULL << PIN_ADR2) |
            (1ULL << PIN_ADR3) |
            (1ULL << PIN_EN)   |
            (1ULL << PIN_XLAT) |
            (1ULL << PIN_BLANK)
    };
    gpio_config(&io);

    gpio_set_level(PIN_EN, 1);     // vypni všetky stĺpce
    gpio_set_level(PIN_BLANK, 1);  // LED off
    gpio_set_level(PIN_XLAT, 0);
}

/* ============================================================
 *  SPI INIT
 * ============================================================ */
static void spi_init(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_SPI_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_SPI_CLK,
        .max_transfer_sz = TLC_TOTAL_BYTES
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1
    };

    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

/* ============================================================
 *  VÝBER STĹPCA
 * ============================================================ */
static inline void select_column(uint8_t col)
{
    gpio_set_level(PIN_ADR0, (col >> 0) & 1);
    gpio_set_level(PIN_ADR1, (col >> 1) & 1);
    gpio_set_level(PIN_ADR2, (col >> 2) & 1);
    gpio_set_level(PIN_ADR3, (col >> 3) & 1);
}

/* ============================================================
 *  ODOSLANIE PWM PRE JEDEN STĹPEC
 * ============================================================ */
static void send_column_pwm(uint8_t col)
{
    memset(tlc_channels, 0, sizeof(tlc_channels));

    for (int r = 0; r < ROWS; r++) {
        tlc_channels[MAP_R[r]] = framebuffer[col][r][0];
        tlc_channels[MAP_G[r]] = framebuffer[col][r][1];
        tlc_channels[MAP_B[r]] = framebuffer[col][r][2];
    }

    uint8_t tx[TLC_TOTAL_BYTES];
    memset(tx, 0, sizeof(tx));

    int bit = 0;
    for (int ch = TLC_CHANNELS - 1; ch >= 0; ch--) {
        uint16_t v = tlc_channels[ch] & 0x0FFF;
        for (int b = 11; b >= 0; b--) {
            if (v & (1 << b)) {
                tx[bit >> 3] |= (1 << (7 - (bit & 7)));
            }
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

/* ============================================================
 *  MULTIPLEX TASK
 * ============================================================ */
static void display_task(void *arg)
{
    while (1) {
        for (uint8_t col = 0; col < COLS; col++) {

            gpio_set_level(PIN_BLANK, 1);
            gpio_set_level(PIN_EN, 1);

            send_column_pwm(col);
            select_column(col);

            gpio_set_level(PIN_EN, 0);
            gpio_set_level(PIN_BLANK, 0);

            esp_rom_delay_us(250);
        }
    }
}

/* ============================================================
 *  MAIN
 * ============================================================ */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting LED matrix");

    gpio_init_all();
    spi_init();

    memset(framebuffer, 0, sizeof(framebuffer));

    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS; r++) {
            framebuffer[c][r][0] = COLOR_R;
            framebuffer[c][r][1] = COLOR_G;
            framebuffer[c][r][2] = COLOR_B;
        }
    }

    xTaskCreate(display_task, "display", 4096, NULL, 5, NULL);
}
