#ifndef _DRIVER_ST7796S_H__
#define _DRIVER_ST7796S_H__

#include "driver_if8080.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "sys_utils.h"

#define COLORBIT16


#define ST7796S_BACKLIGHT_OPEN() gpio_write_pin(GPIO_A, GPIO_PIN_0, GPIO_PIN_SET)
#define ST7796S_BACKLIGHT_CLOSE() gpio_write_pin(GPIO_A, GPIO_PIN_0, GPIO_PIN_CLEAR)

void st7796s_display_wait_transfer_done(void);
void st7796s_init(void);
void st7796s_display(uint32_t pixel_count, void *ptr, void (*callback)(void));
void st7796s_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e);
 
void st7796s_dma_isr(void);
 
#endif