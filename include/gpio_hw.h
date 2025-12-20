#ifndef GPIO_HW_H
#define GPIO_HW_H

#include "esp_err.h"

/* Display pins */
#define PIN_ADR0 25
#define PIN_ADR1 17
#define PIN_ADR2 16
#define PIN_ADR3 27
#define PIN_EN   14

#define PIN_SPI_MOSI 23
#define PIN_SPI_CLK  18
#define PIN_XLAT     13
#define PIN_BLANK    12

/* Button pins */
#define SW1_DOWN   22
#define SW2_UP     21
#define SW3_LEFT   26
#define SW4_RIGHT  4

/* Display dimensions */
#define COLS 16
#define ROWS 8

/* Initialize GPIO pins */
void gpio_init_all(void);

/* Initialize SPI bus */
void spi_init(void);

#endif // GPIO_HW_H

