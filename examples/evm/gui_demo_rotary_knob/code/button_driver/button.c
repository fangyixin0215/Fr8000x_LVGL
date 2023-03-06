#include "button.h"
#include "driver_gpio.h"
#include "SEGGER_RTT.h"
#include "window_manager.h"
#include "driver_exti.h"
#include "driver_cst816d.h"
#include "input_driver.h"
//_SEGGER_RTT


typedef void (*btn_callback)(void);


	void user_button_init()
{

//1.使能GPIO时钟 	
	GPIO_InitTypeDef GPIO_Handle; 
	__SYSTEM_GPIO_CLK_ENABLE();     
//2.修改gpio相关配置
 	GPIO_Handle.Pin = GPIO_PIN_0;    //使用PD0按键
	GPIO_Handle.Mode = /*GPIO_MODE_INPUT*/GPIO_MODE_EXTI_IT_FALLING;
	GPIO_Handle.Pull = GPIO_PULLUP;
//3.初始化gpio
	gpio_init(GPIO_D, &GPIO_Handle);
	//read_status=gpio_read_pin(GPIO_D, GPIO_PIN_0);
	exti_clear_LineStatus(24);			//清除中断线状态 PD0
	NVIC_EnableIRQ(GPIO_IRQn);         //使能外部中断
}



//4.配置外部中断处理函数
//__attribute__((section("ram_code"))) void exti_isr_ram(void)
//{
//	 uint32_t status;
//   status = exti_get_status();
//	if (exti_get_LineStatus(24))
//	{
//		Return_Main_Display();
//		SEGGER_RTT_printf(0,"button click\r\n");						 
//		exti_clear_LineStatus(24);
//	} 
//	#if(CST816D_TOUCHPAD==1)
//	cst816_isr_handle();
//	#endif
////	ec11_exti_handle();
//  exti_clear(status);  
//}
