#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "driver/spi_master.h"

/* TLC5940 configuration */
#define TLC_CHANNELS     24
#define TLC_BITS_PER_CH  12
#define TLC_TOTAL_BITS   (TLC_CHANNELS * TLC_BITS_PER_CH)
#define TLC_TOTAL_BYTES (TLC_TOTAL_BITS / 8)

/* Framebuffer API */
void clear_screen(void);
void set_pixel(int x, int y, uint16_t r, uint16_t g, uint16_t b);

/* Display control */
void display_start_task(void);
void display_set_column_delay_us(uint32_t delay_us);
void display_set_spi_handle(spi_device_handle_t handle);

#endif // DISPLAY_H

