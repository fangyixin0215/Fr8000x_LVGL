#ifndef __DRIVER_CST816D_H
#define __DRIVER_CST816D_H

/*********************
 *      INCLUDES
 *********************/
#include "driver_gpio.h"
#include "driver_i2c.h"
#include "driver_system.h"

#include "co_printf.h"
#include "co_log.h"

/*********************
 *      DEFINES
 *********************/
#define CST816D_RST_SET			gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define CST816D_RST_CLR			gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)
#define CST816D_TOUCHPAD 		0


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void cst816_init(void);
void cst816_read_bytes(uint8_t regaddr,uint8_t* pdata,uint16_t len);
void cst816_isr_handle(void);
/**********************
 *      MACROS
 **********************/
 





#endif
