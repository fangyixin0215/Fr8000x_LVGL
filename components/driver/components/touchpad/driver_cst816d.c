/************************************************************
  * @brief   cst816d触摸ic底层驱动文件
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-16
  * @note    driver_cst816d.c
	* @version v1.0
  ***********************************************************/
#include "driver_cst816d.h"
#include "driver_system.h"
#include "jump_table.h"
#include "co_log.h"
#include "app_config.h"
#include "driver_system.h"
#include "sys_utils.h"
#include "os_timer.h"
#include "os_mem.h"
#include "os_msg_q.h"
#include "os_task.h"
#include "driver_exti.h"

/*********************
 *      INCLUDES
 *********************/
 
/*********************
 *      DEFINES
 *********************/
#define CST816_DEVICE_WRITE_ADDRESS	 	0x2A//0x2A<<1
#define CST816_DEVICE_READ_ADDRESS 		0x2B//0x2B<<1

/**********************
 *  STATIC VARIABLES
 **********************/
uint8_t g_cst816_is_pressed=0;

//os_timer_t user_time;

static GPIO_InitTypeDef   GPIO_Handle;
static I2C_HandleTypeDef  I2C1_Handle;
void cst816d_gpio_init(void)
{
	  __SYSTEM_I2C1_MASTER_CLK_SELECT_48M();
    __SYSTEM_GPIO_CLK_ENABLE();
		__SYSTEM_I2C1_CLK_ENABLE();
	
    GPIO_Handle.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_1;
    gpio_init(GPIO_D, &GPIO_Handle);
	
    I2C1_Handle.I2Cx = I2C1;
    I2C1_Handle.Init.I2C_Mode = I2C_MODE_MASTER_7BIT;
    I2C1_Handle.Init.SCL_HCNT = 60;
    I2C1_Handle.Init.SCL_LCNT = 65;
	  I2C1_Handle.Init.Slave_Address = 0x15<<1;
    i2c_init(&I2C1_Handle);
		#if(CST816D_TOUCHPAD==1)
		GPIO_Handle.Pin       = GPIO_PIN_4;
		GPIO_Handle.Mode      = GPIO_MODE_EXTI_IT_FALLING;
		GPIO_Handle.Pull      = GPIO_PULLUP;
		gpio_init(GPIO_A, &GPIO_Handle);
		exti_clear_LineStatus(EXTI_GPIOA_4);
		NVIC_EnableIRQ(GPIO_IRQn);
		#endif
		// Output 
		GPIO_Handle.Pin       = GPIO_PIN_5;
		GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
		gpio_init(GPIO_A, &GPIO_Handle);

}

void cst816_read_bytes(uint8_t regaddr,uint8_t* pdata,uint16_t len)
{
 i2c_memory_read(&I2C1_Handle,CST816_DEVICE_READ_ADDRESS,regaddr,pdata, len);
}


static uint8_t tp_temp_id =0;
void user_timetask_funcb(void *arg)
{
	uint8_t TP_type;
	uint16_t pdwSampleX, pdwSampleY;
	uint8_t ucQueryResponse = 0;
	uint8_t tp_temp[10];
//	if(is_pressed)
//	{
//		is_pressed=0;
		cst816_read_bytes(0x01,&tp_temp[0],1);  //手势
		cst816_read_bytes(0x02,&tp_temp[1],1);  // 触摸状态   1按下  0 抬起
		cst816_read_bytes(0x04,&tp_temp[2],1);  //x
		cst816_read_bytes(0x06,&tp_temp[3],1);  //y
//		cst816_read_bytes(0x01,tp_temp,8);
		co_printf("tp_temp=%x,%x,%x,%x\n",tp_temp[0],tp_temp[1],tp_temp[2],tp_temp[3]);
//	}

}
void cst816_init(void)
{
	cst816d_gpio_init();
	co_delay_10us(10000);
	CST816D_RST_CLR;
	co_delay_10us(10000);
	CST816D_RST_SET;
	co_delay_10us(10000);//100ms
	cst816_read_bytes(0xA7,&tp_temp_id,1);
	co_printf("TP----- id=%x\n",tp_temp_id);
}
#if(CST816D_TOUCHPAD==1)
/************************************************************************************
 * @fn      exti_isr
 *
 * @brief   exti interrupt handler
 */
void cst816_isr_handle(void)
{
	if (exti_get_LineStatus(EXTI_GPIOA_4))
	{
			exti_clear_LineStatus(EXTI_GPIOA_4);
			g_cst816_is_pressed=1;
		  co_printf("cst816_isr_handle\r\n");
	}

}
#endif
