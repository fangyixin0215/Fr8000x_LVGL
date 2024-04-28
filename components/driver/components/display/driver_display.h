
#ifndef __DRIVER_DISPLAY_H__
#define __DRIVER_DISPLAY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
//#define DISPLAY_TYPE_GC9A01
//#define DISPLAY_TYPE_NV3041A
//#define DISPLAY_TYPE_ST7789V
//#define DISPLAY_TYPE_ST7796S
//#define DISPLAY_TYPE_GC9C01
//#define DISPLAY_TYPE_JD9854
#define DISPLAY_TYPE_ST77903
//#define DISPLAY_TYPE_ST7365

/*根据不同屏幕开启相关的宏，注意改了这里需要将lv_conf.h里面的屏幕分辨率LV_HOR_RES_MAX，LV_VER_RES_MAX改成自己驱动芯片所对应的。*/
#ifdef DISPLAY_TYPE_ST77903
#include "driver_st77903.h"
#endif

#ifdef DISPLAY_TYPE_GC9A01
#include "driver_gc9c01.h"
#endif

#ifdef DISPLAY_TYPE_JD9854
#include "driver_jd9854.h"
#endif

#ifdef DISPLAY_TYPE_ST7796S
#include "driver_st7796s.h"
#endif

#ifdef DISPLAY_TYPE_ST7789V
#include "driver_st7789v.h"
#endif

#ifdef DISPLAY_TYPE_NV3041A
#include "driver_nv3041a.h"
#endif

#ifdef DISPLAY_TYPE_ST7365
#include "driver_st7365.h"
#endif
 


//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

/************************************************************************************
 * @fn      display_init
 *
 * @brief   Initial display drivers.
 */
void display_init(void);

/************************************************************************************
 * @fn      display_set_window
 *
 * @brief   used to define area of frame memory where MCU can access. 
 *
 * @param   x_s: SC.
 *          x_e: EC.
 *          y_s: SP.
 *          y_e: EP.
 */
void display_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e);

/************************************************************************************
 * @fn      display_wait_transfer_done
 *
 * @brief   Used to wait for all data have been sent to display controller when DMA is used.
 */
void display_wait_transfer_done(void);

/************************************************************************************
 * @fn      display_update
 *
 * @brief   transfer data to framebuffer of display controller with DMA
 *
 * @param   pixel_count: total pixels count to be sent.
 *          data: pointer to data buffer
 *          callback: used to notify caller after DMA transfer is done.
 */
void display_update(uint32_t pixel_count, uint16_t *data, void(*callback)(void));

/************************************************************************************
 * @fn      display_gather_update
 *
 * @brief   used to send left or right part of frame with DMA gather mode
 *
 * @param   pixel_count: total pixels count to be sent.
 *          data: address of first pixel to be sent
 *          interval: lcd width
 *          count: data to be sent in a single line
 *          callback: used to notify caller after DMA transfer is done.
 */
void display_gather_update(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count, void(*callback)(void));

#ifdef __cplusplus
}
#endif

#endif	/* __DRIVER_DISPLAY_H__ */

