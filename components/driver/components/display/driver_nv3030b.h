
#ifndef __DRIVER_NV3030B_H__
#define __DRIVER_NV3030B_H__

#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

/************************************************************************************
 * @fn      gc9c01_init
 *
 * @brief   Initial SPI, GPIO and DMA used by gc9c01, and init gc9c01 registers.
 */
void nv3030b_init(void);

/************************************************************************************
 * @fn      gc9c01_set_window
 *
 * @brief   used to define area of frame memory where MCU can access. 
 *
 * @param   x_s: SC.
 *          x_e: EC.
 *          y_s: SP.
 *          y_e: EP.
 */
void nv3030b_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e);

/************************************************************************************
 * @fn      gc9c01_display_wait_transfer_done
 *
 * @brief   Used to wait for all data have been sent to gc9c01 when DMA is used.
 */
void nv3030b_display_wait_transfer_done(void);

/************************************************************************************
 * @fn      gc9c01_display
 *
 * @brief   Receive an amount of data in blocking mode.(Dual¡¢Quad mode)
 *
 * @param   pixel_count: total pixels count to be sent.
 *          data: pointer to data buffer
 *          callback: used to notify caller after DMA transfer is done.
 */
void nv3030b_display(uint32_t pixel_count, uint16_t *data, void (*callback)(void));

/************************************************************************************
 * @fn      gc9c01_display_gather
 *
 * @brief   used to send left or right part of frame with DMA gather mode
 *
 * @param   pixel_count: total pixels count to be sent.
 *          data: address of first pixel to be sent
 *          interval: lcd width
 *          count: data to be sent in a single line
 */
void nv3030b_display_gather(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count);

/************************************************************************************
 * @fn      gc9c01_dma_isr
 *
 * @brief   DMA isr handler for gc9c01.
 */
void nv3030b_dma_isr(void);

void nv3030b_init(void);
 void nv3030b_backlight_open(void);
 void nv3030b_backlight_close(void);
 
 uint8_t nv3030b_read_te(void);
 void nv3030b_sleep_in(void);
void nv3030b_sleep_out(void);

 void nv3030b_display_ex(uint32_t pixel_count, uint16_t *data, uint32_t w, uint32_t h, bool right_half, void (*callback)(void));
 void nv3030b_display_pro(uint32_t w, uint32_t h, uint16_t *data, bool right_half, void (*callback)(void));

#ifdef __cplusplus
}
#endif

#endif	/* __DRIVER_GC9C01_H__ */

