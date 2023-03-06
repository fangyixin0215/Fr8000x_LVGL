#ifndef _DRIVER_ST77903_H
#define _DRIVER_ST77903_H

#include "stdint.h"
#include "stdbool.h"

void display(uint16_t *data);
void st77903_init(void);

void st77903_display(uint16_t *data,uint8_t step);
void st77903_dma_isr(void);
uint8_t get_st77903_send_state(void);

void st77903_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e);
void LCD_Draw_ColorLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color,uint16_t *data);
void st77903_update_handle(void);

#endif  // _USER_TASK_H

