/**
 * @file gpio_hw.c
 * @brief GPIO and SPI hardware initialization
 * @author Jozef Ondrejicka
 */

#include "../include/gpio_hw.h"
#include "../include/display.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"

/**
 * @brief Initialize all GPIO pins
 * Configures display control pins (outputs) and button pins (inputs with pull-up)
 */
void gpio_init_all(void)
{
    // Configure display control pins as outputs
    gpio_config_t out = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL<<PIN_ADR0)|(1ULL<<PIN_ADR1)|(1ULL<<PIN_ADR2)|(1ULL<<PIN_ADR3)|
            (1ULL<<PIN_EN)|(1ULL<<PIN_XLAT)|(1ULL<<PIN_BLANK)
    };
    gpio_config(&out);

    // Configure button pins as inputs with pull-up resistors
    gpio_config_t in = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =
            (1ULL<<SW1_DOWN)|(1ULL<<SW2_UP)|(1ULL<<SW3_LEFT)|(1ULL<<SW4_RIGHT),
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&in);

    // Set initial pin states
    gpio_set_level(PIN_EN, 1);
    gpio_set_level(PIN_BLANK, 1);
    gpio_set_level(PIN_XLAT, 0);
}

/**
 * @brief Initialize SPI bus for TLC5947 communication
 * Configures SPI3 at 10MHz, mode 0, with DMA support (max 30MHz per requirements)
 */
void spi_init(void)
{
    // Configure SPI bus
    spi_bus_config_t bus = {
        .mosi_io_num = PIN_SPI_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_SPI_CLK,
        .max_transfer_sz = TLC_TOTAL_BYTES
    };

    // Add SPI device
    spi_device_interface_config_t dev = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1
    };

    // Initialize SPI bus
    spi_device_handle_t spi_handle;
    esp_err_t ret = spi_bus_initialize(SPI3_HOST, &bus, SPI_DMA_CH_AUTO);

    // Check for errors during SPI bus initialization
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI3_HOST, &dev, &spi_handle);
    ESP_ERROR_CHECK(ret);
    
    // Pass SPI handle to display module
    display_set_spi_handle(spi_handle);
}

