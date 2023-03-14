
/************************************************************
  * @brief   ec11编码器检测子程序，通过外部中断实现，检测旋转方向
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-19
  * @note    input_driver.c
	* @version v1.0
  ***********************************************************/
	
#include "input_driver.h"
#include "driver_exti.h"
#include "co_printf.h"
#include "os_timer.h"

#include "key_driver.h"
#include "user_knob_config.h"
#include "driver_ktm57xx.h"


static os_timer_t check_handle_timer;
static os_timer_t encoder_handle_timer;
static char g_key_lock_flag=0;

static void encoder_handle_cb(void *parm);
static void check_handle_cb(void *parm);

static GPIO_InitTypeDef   GPIO_Handle;

/************************************************************
  * @brief   编码器输入引脚初始化
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-19
  ***********************************************************/
void input_encoder_init(void)
{
	
		GPIO_InitTypeDef GPIO_Handle; 
	__SYSTEM_GPIO_CLK_ENABLE();  
  GPIO_Handle.Pin  = 	EC11_KEY_GPIO_NUM;  
	GPIO_Handle.Mode =  GPIO_MODE_INPUT;
	GPIO_Handle.Pull =  GPIO_PULLUP;
	gpio_init(EC11_KEY_PORT, &GPIO_Handle);	
	#if (DEVELOPMENT_BOARD_8000GP==1)
 	GPIO_Handle.Pin = EC11_EXTI_GPIO_NUM;   
	GPIO_Handle.Mode = /*GPIO_MODE_INPUT*/GPIO_MODE_EXTI_IT_FALLING;
	GPIO_Handle.Pull = GPIO_PULLUP;
	gpio_init(EC11_EXTI_PORT, &GPIO_Handle);
	exti_interrupt_enable(EC11_EXTI_LINE);
  exti_clear_LineStatus(EC11_EXTI_LINE);	
	NVIC_EnableIRQ(GPIO_IRQn);   
	
	GPIO_Handle.Pin  = 	EC11_DIR_GPIO_NUM;  
	GPIO_Handle.Mode =  GPIO_MODE_INPUT;
	GPIO_Handle.Pull =  GPIO_PULLUP;
	gpio_init(EC11_DIR_PORT, &GPIO_Handle);
	
  os_timer_init(&check_handle_timer, check_handle_cb, NULL);
  os_timer_start(&check_handle_timer, 80, true);
	#endif
	
	#if (KNOB_DEMO_BOARD==1)
 	GPIO_Handle.Pin = EC11_EXTI_GPIO_NUM;   
	GPIO_Handle.Mode = /*GPIO_MODE_INPUT*/GPIO_MODE_EXTI_IT_FALLING;
	GPIO_Handle.Pull = GPIO_PULLUP;
	gpio_init(EC11_EXTI_PORT, &GPIO_Handle);
	exti_clear_LineStatus(EC11_EXTI_LINE);	
	NVIC_EnableIRQ(GPIO_IRQn);   
	
	GPIO_Handle.Pin  = 	EC11_DIR_GPIO_NUM;  
	GPIO_Handle.Mode =  GPIO_MODE_INPUT;
	GPIO_Handle.Pull =  GPIO_PULLUP;
	gpio_init(EC11_DIR_PORT, &GPIO_Handle);
	
  os_timer_init(&check_handle_timer, check_handle_cb, NULL);
  os_timer_start(&check_handle_timer, 80, true);
	#endif
  key_init();
  #if ((KNOB_DEMO_BOARD==1) ||(DEVELOPMENT_BOARD_8000GP==1))
  os_timer_init(&encoder_handle_timer, encoder_handle_cb, NULL);
  os_timer_start(&encoder_handle_timer, 10, true);
	#endif
} 

/************************************************************
  * @brief   编码器方向检测状态回调
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-19
  ***********************************************************/
static void check_handle_cb(void *parm)
{
		if(g_key_lock_flag)
		{
				g_key_lock_flag=0;
			  exti_interrupt_enable(EC11_EXTI_LINE);		 
		}
}

extern uint8_t g_key_code;

/************************************************************
  * @brief   编码器按键检测状态回调
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-19
  ***********************************************************/
static void encoder_handle_cb(void *parm)
{
		key_scanf();//扫描
	  uint8_t keycode=read_key_fifo();//读取数据
	  if(keycode!=0)
		{
			switch(keycode)
			{
				case KEY1_SHORT_EVENT:
						 g_key_code=ENTER_CODE;
						break;                     
				case KEY1_DUAL_EVENT:
						break;
				case KEY1_LONG_EVENT:
						 g_key_code=DBLCLICK_CODE;
						break;
			}  
			
		}
}

/************************************************************
  * @brief   编码器触发外部中断处理
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-19
  ***********************************************************/
__attribute__((section("ram_code"))) void ec11_exti_handle(void)
{
	uint8_t keycode=0;
	static uint32_t last_trigger_time=0;
	if (exti_get_LineStatus(EC11_EXTI_LINE))
	{
				if(g_key_lock_flag==0)
				{
						if(gpio_read_pin(EC11_DIR_PORT, EC11_DIR_GPIO_NUM)==0)
						{
							  g_key_code = RIGHT_CODE;
						}else
						{
							  g_key_code = LEFT_CODE;
						}
						g_key_lock_flag=1;
			 }
//			if((system_get_curr_time()-last_trigger_time) >180)
//			{
//							if(gpio_read_pin(EC11_DIR_PORT, EC11_DIR_GPIO_NUM)==0)
//							{
//									 gui_page_event.event_id = USER_GUI_EVT_ENCODER_LEFT; 
//							}else
//							{
//									gui_page_event.event_id = USER_GUI_EVT_ENCODER_RIGHT;  
//							}
//							gui_page_event.param = &keycode; 
//							gui_page_event.param_len = 1; 
//							os_msg_post(get_user_menu_task_id(), &gui_page_event);
//							last_trigger_time=system_get_curr_time();
//			 }
				exti_clear_LineStatus(EC11_EXTI_LINE);

	} 
	
}

__attribute__((section("ram_code"))) void exti_isr(void)
{
	 uint32_t status;
   status = exti_get_status();
	#if(CST816D_TOUCHPAD==1)
	cst816_isr_handle();
	#endif
	#if(KTM57XX_EXTI_EN==1&&KTX_PCB_BOARD==1)
	ktm57xx_exti_isr_handle();
	#endif
	#if (DEVELOPMENT_BOARD_8000GP==1 || KNOB_DEMO_BOARD==1)
	ec11_exti_handle();
	#endif
  exti_clear(status);  
}
