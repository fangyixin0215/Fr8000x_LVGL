/*
  ******************************************************************************
  * @file    driver_display.c
  * @author  FreqChip Firmware Team
  * @version V1.0.0
  * @date    2021
  * @brief   display abstract interfase.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/

#include <stdint.h>

#include "driver_display.h"


void display_init(void)
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_init();
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_init();
#endif
#ifdef DISPLAY_TYPE_GC9A01
    gc9a01_init();
#endif
#ifdef DISPLAY_TYPE_NV3041A
    nv3041a_init();
#endif	
#ifdef DISPLAY_TYPE_ST7789V
    st7789v_init();
#endif	

#ifdef DISPLAY_TYPE_ST7796S
    st7796s_init();
#endif	
	
#ifdef DISPLAY_TYPE_ST77903
    st77903_init();
#endif

#ifdef DISPLAY_TYPE_ST7365
    st7365_init();
#endif

#ifdef DISPLAY_TYPE_NV3030B
    nv3030b_init();
#endif
}

void display_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_set_window(x_s, x_e, y_s, y_e);
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_set_window(x_s, x_e, y_s, y_e);
#endif
#ifdef DISPLAY_TYPE_ST77903
#endif
#ifdef DISPLAY_TYPE_GC9A01
    gc9a01_set_window(x_s, x_e, y_s, y_e);
#endif
#ifdef DISPLAY_TYPE_NV3041A
    nv3041a_set_window(x_s, x_e, y_s, y_e);
#endif	
#ifdef DISPLAY_TYPE_ST7789V
    st7789v_set_window(x_s, x_e, y_s, y_e);
#endif	

#ifdef DISPLAY_TYPE_ST7796S
    st7796s_set_window(x_s, x_e, y_s, y_e);
#endif	

#ifdef DISPLAY_TYPE_ST7365
    st7365_set_window(x_s, x_e, y_s, y_e);
#endif	
#ifdef DISPLAY_TYPE_NV3030B
    nv3030b_set_window(x_s, x_e, y_s, y_e);
#endif	
}

void display_wait_transfer_done(void)
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_display_wait_transfer_done();
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_display_wait_transfer_done();
#endif
#ifdef DISPLAY_TYPE_ST77903
     
#endif

#ifdef DISPLAY_TYPE_GC9A01
    gc9a01_display_wait_transfer_done();
#endif
#ifdef DISPLAY_TYPE_NV3041A
    nv3041a_display_wait_transfer_done();
#endif	
#ifdef DISPLAY_TYPE_ST7789V
    st7789v_display_wait_transfer_done();
#endif	

#ifdef DISPLAY_TYPE_ST7796S
    st7796s_display_wait_transfer_done();
#endif	
#ifdef DISPLAY_TYPE_ST7365
    st7365_display_wait_transfer_done();
#endif	

#ifdef DISPLAY_TYPE_NV3030B
    nv3030b_display_wait_transfer_done();
#endif	
}

void display_update(uint32_t pixel_count, uint16_t *data, void(*callback)(void))
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_display(pixel_count, data, callback);
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_display(pixel_count, data, callback);
#endif
#ifdef DISPLAY_TYPE_GC9A01
    gc9a01_display(pixel_count, data, callback);
#endif
#ifdef DISPLAY_TYPE_NV3041A
    nv3041a_display(pixel_count, data, callback);
#endif	
#ifdef DISPLAY_TYPE_ST7789V
    st7789v_display(pixel_count, data, callback);
#endif	

#ifdef DISPLAY_TYPE_ST7796S
    st7796s_display(pixel_count, data, callback);
#endif	
	
#ifdef DISPLAY_TYPE_ST7365
    st7365_display(pixel_count, data, callback);
#endif		

#ifdef DISPLAY_TYPE_NV3030B
    nv3030b_display(pixel_count, data, callback);
#endif	
}

void display_gather_update(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count, void(*callback)(void))
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_display_gather(pixel_count, data, interval, count);
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_display_gather(pixel_count, data, interval, count);
#endif
	
}

__attribute__((section("ram_code"))) void dma_isr(void)
{
#ifdef DISPLAY_TYPE_GC9C01
    gc9c01_dma_isr();
#endif
#ifdef DISPLAY_TYPE_JD9854
    jd9854_dma_isr();
#endif
#ifdef DISPLAY_TYPE_ST77903
     st77903_dma_isr();
#endif
#ifdef DISPLAY_TYPE_GC9A01
    gc9a01_dma_isr();
#endif
#ifdef DISPLAY_TYPE_NV3041A
    nv3041a_dma_isr();
#endif	
#ifdef DISPLAY_TYPE_ST7789V
    st7789v_dma_isr();
#endif	

#ifdef DISPLAY_TYPE_ST7796S
    st7796s_dma_isr();
#endif	

#ifdef DISPLAY_TYPE_ST7365
   st7365_dma_isr();
#endif	
#ifdef DISPLAY_TYPE_NV3030B
   nv3030b_dma_isr();
#endif	
}
