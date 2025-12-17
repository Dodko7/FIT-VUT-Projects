#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_rom_sys.h"


/* ===================== PIN MAPOVANIE PODĽA SCHÉMY ===================== */

/* 74HCT154 – výber stĺpca */
#define PIN_ADR0 25
#define PIN_ADR1 16
#define PIN_ADR2 17
#define PIN_ADR3 27
#define PIN_EN   14   // aktívny LOW

/* TLC5947 – SPI */
#define PIN_SPI_CLK   18
#define PIN_SPI_MOSI  23
#define PIN_XLAT      26
#define PIN_BLANK     12

/* ===================== NASTAVENIE FARBY ===================== */
/* 12-bit PWM: 0 – 4095 */
#define COLOR_R 1024
#define COLOR_G 1024
#define COLOR_B 1024

/* ===================== KONŠTANTY ===================== */

#define MATRIX_COLS 16
#define MATRIX_ROWS 8
#define TLC_CHANNELS 24
#define TLC_BITS_PER_CH 12
#define TLC_TOTAL_BITS (TLC_CHANNELS * TLC_BITS_PER_CH)
#define TLC_TOTAL_BYTES (TLC_TOTAL_BITS / 8)

/* ===================== FRAMEBUFFER ===================== */
/* [col][row][RGB] */
static uint16_t framebuffer[MATRIX_COLS][MATRIX_ROWS][3];

/* SPI handle */
static spi_device_handle_t spi_handle;

/* ===================== POMOCNÉ FUNKCIE ===================== */

static void gpio_init_all(void)
{
    gpio_config_t io = {0};

    /* Výstupy */
    io.mode = GPIO_MODE_OUTPUT;
    io.pin_bit_mask =
        (1ULL << PIN_ADR0) |
        (1ULL << PIN_ADR1) |
        (1ULL << PIN_ADR2) |
        (1ULL << PIN_ADR3) |
        (1ULL << PIN_EN)   |
        (1ULL << PIN_XLAT) |
        (1ULL << PIN_BLANK);
    gpio_config(&io);

    /* Definované počiatočné stavy */
    gpio_set_level(PIN_EN, 1);      // vypni všetky stĺpce
    gpio_set_level(PIN_BLANK, 1);   // zhasni LED
    gpio_set_level(PIN_XLAT, 0);
}

static void spi_init_tlc5947(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_SPI_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_SPI_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = TLC_TOTAL_BYTES
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
        .mode = 0,
        .spics_io_num = -1, // CS nepoužívame
        .queue_size = 1
    };

    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
}

/* nastaví adresu stĺpca (0–15) */
static void select_column(uint8_t col)
{
    gpio_set_level(PIN_ADR0, (col >> 0) & 1);
    gpio_set_level(PIN_ADR1, (col >> 1) & 1);
    gpio_set_level(PIN_ADR2, (col >> 2) & 1);
    gpio_set_level(PIN_ADR3, (col >> 3) & 1);
}

/* odošle PWM dáta pre jeden stĺpec */
static void send_column_pwm(uint8_t col)
{
    uint8_t tx_buf[TLC_TOTAL_BYTES];
    memset(tx_buf, 0, sizeof(tx_buf));

    int bit_pos = TLC_TOTAL_BITS - 1;

    for (int row = 0; row < MATRIX_ROWS; row++) {
        /* poradie: B, G, R (podľa schémy riadkov) */
        uint16_t values[3] = {
            framebuffer[col][row][2], // B
            framebuffer[col][row][1], // G
            framebuffer[col][row][0]  // R
        };

        for (int c = 0; c < 3; c++) {
            for (int b = 11; b >= 0; b--) {
                if (values[c] & (1 << b)) {
                    tx_buf[bit_pos / 8] |= (1 << (bit_pos % 8));
                }
                bit_pos--;
            }
        }
    }

    spi_transaction_t t = {
        .length = TLC_TOTAL_BITS,
        .tx_buffer = tx_buf
    };

    spi_device_transmit(spi_handle, &t);

    /* XLAT pulz */
    gpio_set_level(PIN_XLAT, 1);
    esp_rom_delay_us(1);
    gpio_set_level(PIN_XLAT, 0);
}

/* ===================== MULTIPLEX TASK ===================== */

static void display_task(void *arg)
{
    while (1) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {

            /* 1. deaktivuj všetko */
            gpio_set_level(PIN_EN, 1);
            gpio_set_level(PIN_BLANK, 1);

            /* 2. pošli PWM dáta */
            send_column_pwm(col);

            /* 3. povoľ výstupy TLC */
            gpio_set_level(PIN_BLANK, 0);

            /* 4. nastav adresu stĺpca */
            select_column(col);

            /* 5. aktivuj stĺpec */
            gpio_set_level(PIN_EN, 0);

            /* 6. čas svitu */
            esp_rom_delay_us(500);
        }
    }
}


/* ===================== HLAVNÁ FUNKCIA ===================== */

void app_main(void)
{
    gpio_set_level(PIN_EN, 1);
    gpio_set_level(PIN_BLANK, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    gpio_init_all();
    spi_init_tlc5947();

    /* vymaž framebuffer */
    memset(framebuffer, 0, sizeof(framebuffer));

    /* nastav celú maticu na jednu farbu */
    for (int c = 0; c < MATRIX_COLS; c++) {
        for (int r = 0; r < MATRIX_ROWS; r++) {
            framebuffer[c][r][0] = COLOR_R;
            framebuffer[c][r][1] = COLOR_G;
            framebuffer[c][r][2] = COLOR_B;
        }
    }

    xTaskCreate(display_task, "display_task", 4096, NULL, 5, NULL);
}
