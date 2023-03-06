#ifndef _DRIVER_EXTI_H_
#define _DRIVER_EXTI_H_

/*
 * INCLUDE
 */
#include <stdint.h>

#include "plf.h"

#include "driver_gpio.h"

#define EXT_INT0_EN             (GPIOAB_BASE + 0x40)
#define EXT_INT_TYPE_MSK        0x03     // set interrupt type trigger   0x0000 0011 
#define EXT_INT_TYPE_LEN        2

#define BIT(x) (1<<(x))

enum exti_type_t
{
    EXTI_TYPE_LOW,
    EXTI_TYPE_HIGH,
    EXTI_TYPE_NEG,
    EXTI_TYPE_POS,
};

enum exti_gpio_pin
{
    EXTI_GPIOA_0,
    EXTI_GPIOA_1,
    EXTI_GPIOA_2,
    EXTI_GPIOA_3,
    EXTI_GPIOA_4,
    EXTI_GPIOA_5,
    EXTI_GPIOA_6,
    EXTI_GPIOA_7,
    
    EXTI_GPIOB_0,
    EXTI_GPIOB_1,
    EXTI_GPIOB_2,
    EXTI_GPIOB_3,
    EXTI_GPIOB_4,
    EXTI_GPIOB_5,
    EXTI_GPIOB_6,
    EXTI_GPIOB_7,
    
    EXTI_GPIOC_0,
    EXTI_GPIOC_1,
    EXTI_GPIOC_2,
    EXTI_GPIOC_3,
    EXTI_GPIOC_4,
    EXTI_GPIOC_5,
    EXTI_GPIOC_6,
    EXTI_GPIOC_7,
    
    EXTI_GPIOD_0,
    EXTI_GPIOD_1,
    EXTI_GPIOD_2,
    EXTI_GPIOD_3,
    EXTI_GPIOD_4,
    EXTI_GPIOD_5,
    EXTI_GPIOD_6,
    EXTI_GPIOD_7,
};

//extern interrupt reg x50060000  +  x8000
struct exti_t
{   
    uint32_t ext_int0_control;              //0x8000
    uint32_t reserved1;
    uint32_t ext_int0_status;               //0x8008
    uint32_t reserved2;
    uint32_t ext_int_type[2];               //0x8010 pa0~pa7,pb0~pb7   0x8014  pc0~pc7,pd0~pd7
    uint32_t reserved3;
    uint32_t reserved4;    
    uint32_t ext_int_cnt[32];   //0x8020
};

 /*********************************************************************
  * @fn     exti_enable
  *
  * @brief   enable gpio exter interrupt 
  *
  * @param   gpio_pin    - @ref driver_system  ex:GPIO_PA0
  *          
  * @return  None.
  */
void exti_enable(uint32_t gpio_pin);
 
/*********************************************************************
 * @fn     exti_disable
 *
 * @brief   unenable gpio exter interrupt 
 *
 *@param   gpio_pin    - @ref driver_system  ex:GPIO_PA0
 *
 * @return  None.
 */
void exti_disable(uint32_t gpio_pin);

/*********************************************************************
 * @fn     exti_get_src
 *
 * @brief   return exter interrupt status
 *
 * @param   void
 * @return  ext_int_status.
 */
uint32_t exti_get_status(void);

/*********************************************************************
 * @fn     ext_int_clear
 *
 * @brief   clear gpio exter interrupt 
 *
 * @param   bit        - @ref driver_system  ex:GPIO_PA0.
 *
 * @return  None.
 */
void exti_clear(uint32_t exti_src);

/*********************************************************************
 * @fn     ext_int_set_type
 *
 * @brief   set gpio exter interrup trigger type
 *
 * @param   gpio_pin    - @ref driver_system   
 *          type        - @ref ext_int_type_t
 *
 * @return  None.
 */
void exti_set_type(enum exti_gpio_pin gpio_pin, enum exti_type_t type);

/*********************************************************************
 * @fn     exti_set_control
 *
 * @brief   set gpio extern fractional frequency and  simpling frequency
 *
 * @param   gpio_pin    - @ref exti_gpio_pin.
 *          clk         - set self frequency 
 *          counter     - simoling frequency
 *
 * @return  None.
 */
void exti_set_control(enum exti_gpio_pin gpio_pin, uint32_t clk, uint8_t counter);

#endif /* _DRIVER_EXTI_H_ */
