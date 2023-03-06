#include <stdio.h>

#include "plf.h"
#include "driver_system.h"
#include "co_printf.h"

#include "driver_pmu.h"
#include "driver_exti.h"
#include "driver_gpio.h"

struct exti_t *exti_regs = (struct exti_t *)EXTI_BASE;
uint32_t *ext_int0en_reg = (uint32_t *)EXT_INT0_EN;

/*********************************************************************
 * @fn     exti_enable
 *
 * @brief   enable gpio exter interrupt 
 *
 * @param   gpio_pin    - @ref driver_system  ex:GPIO_PA0
 *          
 * @return  None.
 */
void exti_enable(uint32_t gpio_pin)
{
    exti_regs->ext_int0_control |= gpio_pin;
}

/*********************************************************************
 * @fn     exti_disable
 *
 * @brief   unenable gpio exter interrupt 
 *
 *@param   gpio_pin    - @ref driver_system  ex:GPIO_PA0
 *
 * @return  None.
 */
void exti_disable(uint32_t gpio_pin)
{
   exti_regs->ext_int0_control &= (~gpio_pin);
}

/*********************************************************************
 * @fn     exti_get_status
 *
 * @brief   return exter interrupt status
 *
 * @param   void
 * @return  ext_int_status.
 */
uint32_t exti_get_status(void)
{
    return exti_regs->ext_int0_status ;
}

/*********************************************************************
 * @fn     exti_clear
 *
 * @brief   clear gpio exter interrupt 
 *
 * @param   bit        - @ref driver_system  ex:GPIO_PA0.
 *
 * @return  None.
 */
void exti_clear(uint32_t bit)
{
    exti_regs->ext_int0_status  = bit;
}

/*********************************************************************
 * @fn     exti_set_type
 *
 * @brief   set gpio exter interrup trigger type
 *
 * @param   gpio_pin    - @ref driver_system   
 *          type        - @ref ext_int_type_t
 *
 * @return  None.
 */
void exti_set_type(enum exti_gpio_pin gpio_pin, enum exti_type_t type)
{
    uint8_t offset, index;
    uint32_t value;
    index = gpio_pin /16;
    offset = (gpio_pin %16) << 1;

    value = exti_regs->ext_int_type[index];
    value &= (~(EXT_INT_TYPE_MSK<<offset));
    value |= (type << offset);
    exti_regs->ext_int_type[index] = value;
}

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
void exti_set_control(enum exti_gpio_pin gpio_pin, uint32_t clk, uint8_t counter)
{
    uint32_t pclk;
    system_regs->misc.gpio_clk_div = system_get_pclk_config();
    pclk = 1000000;
    exti_regs->ext_int_cnt[gpio_pin] =((pclk/clk-1)<<8) | (counter-1) ;
}

/*********************************************************************
 * @fn     exti_isr
 *
 * @brief   rewrite interrupt service function
 *
 * @param   void
 *
 * @return  None.
 */
//#include "SEGGER_RTT.h"
//__weak void exti_isr(void)
//{
//    uint32_t status;
//    status = exti_get_status();
//	
//			if (exti_get_LineStatus(24))
//	{
////		SEGGER_RTT_printf(0,"button click\r\n");						 
//		exti_clear_LineStatus(24);
//	}
//	
//    exti_clear(status);
//    co_printf("enter the interrupt !!!\r\n");

//}



__attribute__((section("ram_code"))) void HardFault_Handler_C(unsigned int* hardfault_args)
{
    co_delay_100us(500);
    co_printf("Crash, dump regs:\r\n");
    co_printf("PC    = 0x%08X\r\n",hardfault_args[6]);
    co_printf("LR    = 0x%08X\r\n",hardfault_args[5]);
#if 1
    co_printf("R0    = 0x%08X\r\n",hardfault_args[0]);
    co_printf("R1    = 0x%08X\r\n",hardfault_args[1]);
    co_printf("R2    = 0x%08X\r\n",hardfault_args[2]);
    co_printf("R3    = 0x%08X\r\n",hardfault_args[3]);
    co_printf("R12   = 0x%08X\r\n",hardfault_args[4]);
    co_printf("PSR   = 0x%08X\r\n",hardfault_args[7]);
    co_printf("BFAR  = 0x%08X\r\n",*(unsigned int*)0xE000ED38);
    co_printf("CFSR  = 0x%08X\r\n",*(unsigned int*)0xE000ED28);
    co_printf("HFSR  = 0x%08X\r\n",*(unsigned int*)0xE000ED2C);
    co_printf("DFSR  = 0x%08X\r\n",*(unsigned int*)0xE000ED30);
    co_printf("AFSR  = 0x%08X\r\n",*(unsigned int*)0xE000ED3C);
    co_printf("SHCSR = 0x%08X\r\n",SCB->SHCSR);
    co_printf("dump sp stack[sp sp-512]:\r\n");
    uint16_t i = 0;
    do
    {
        co_printf("0x%08X,",*(hardfault_args++));
        i++;
        if(i%4 == 0)
            co_printf("\r\n");
    }
    while(i<128);
#endif

#ifdef USER_MEM_API_ENABLE
    show_ke_malloc();
#endif
#if 0

    #include "flash.h"
    uint8_t tmp[12] = {0x71,0x72,63,14,15,16,17,18,19,0x1A,0x1B,0x7C};
    flash_erase(USER_FLASH_BASE_ADDR, 0);
    flash_write(USER_FLASH_BASE_ADDR,12, &tmp[0]);
#endif

    //store_reset_info(RST_CAUSE_CRASH);
    //platform_reset_patch(0);
    while(1);
}

