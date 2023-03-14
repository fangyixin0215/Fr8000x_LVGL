#include "driver_ktm57xx.h"
#include "stdio.h"
#include "os_timer.h"
#include "os_mem.h"
#include "os_msg_q.h"
#include "os_task.h"
#include "sys_utils.h"



//uint8_t RMData_1[9];    //存放测量结束读回的数据
//uint8_t RRData_1[3]={0};//存放读取寄存器返回的值 0. status 1.Msb 2.Lsb

//uint8_t RRData_1[3];//存放读取寄存器返回的值 0. status 1.Msb 2.Lsb
//uint8_t RMData_1[9];//存放测量结束读回的数据

//uint8_t flagInt = 1; //1 : 当前为WOC模式 0 : 当前模式是burst mode
//uint16_t x;
//uint16_t y;

//uint8_t flag1 = 0;
//uint16_t Density_X_1;
//uint16_t Density_Y_1;
//uint16_t Density_Z_1;
//uint16_t max_x_1 = 0;
//uint16_t max_y_1 = 0;

//uint16_t min_x_1 = 0;
//uint16_t min_y_1 = 0;

//uint16_t angleInt; //把当前角度值转化为整数型与之前的角度值进行比较
//uint8_t  oledFlag = 0; //判断是否需要点亮屏幕，0：灭屏 1:亮屏
//uint16_t angleCounter = 0; //判断两次角度值相差的次数，如果有超过500次没有转动就熄灭屏幕
//uint16_t angleOld = 0;//记录上一次测试出的角度值
//uint8_t  KeyNumber;
//uint8_t  KeyUpdateFlag;
//uint8_t  KeyState = 0;        // =0 按键值减小  = 1 按键值增大
//uint8_t  Key_Fall_Flag ;
//float   Angle;

//uint8_t flagInt;
//uint8_t Submenu;
//uint8_t Screen_Interface_Flag;
//uint8_t Menu_Falg;
 
ktm57xx_struct_t g_ktm57xx_param={0};

os_timer_t ktm57xx_time;
void ktm57xx_timetask_funcb(void *arg);
/*****************************************************************************
**  文件名称：ktm57xx_init
**  功能概要：IO口初始化
**  参数说明：无
**  函数返回：无
******************************************************************************/
void ktm57xx_init(void)
{
		static GPIO_InitTypeDef   GPIO_Handle;
	 	__SYSTEM_GPIO_CLK_ENABLE();
	/*  IO Init */
    GPIO_Handle.Pin       =  KTM57XX_MISO_GPIO;
	  GPIO_Handle.Mode      = GPIO_MODE_INPUT;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    //GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(KTM57XX_MISO_PORT, &GPIO_Handle);

		GPIO_Handle.Pin       = KTM57XX_CS_GPIO;
	  GPIO_Handle.Pull      = GPIO_PULLUP;
		GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
		gpio_init(KTM57XX_CS_PORT, &GPIO_Handle);
		
	  GPIO_Handle.Pin       = KTM57XX_MOSI_GPIO;
		GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_Handle.Pull      = GPIO_PULLUP;
		gpio_init(KTM57XX_MOSI_PORT, &GPIO_Handle);
		
	  GPIO_Handle.Pin       = KTM57XX_SCLK_GPIO;
		GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_Handle.Pull      = GPIO_PULLUP;
		gpio_init(KTM57XX_SCLK_PORT, &GPIO_Handle);
		
    #if(KTM57XX_EXTI_EN==1)
		GPIO_Handle.Pin       = KTM57XX_EXTI_GPIO;
		GPIO_Handle.Mode      = GPIO_MODE_EXTI_IT_FALLING;//GPIO_MODE_EXTI_IT_RISING;GPIO_MODE_EXTI_IT_FALLING
		GPIO_Handle.Pull      = GPIO_PULLUP;
		gpio_init(KTM57XX_EXTI_PORT, &GPIO_Handle);
		exti_clear_LineStatus(KTM57XX_EXTI_LINE);
		NVIC_EnableIRQ(GPIO_IRQn);
	  #endif
		SCL_1_Set();
		MOSI_1_Set();
		nCS_1_Set();
		co_delay_10us(1000);
		ktm57xx_register_init();
		co_delay_10us(1000);
		ktm57xx_continuous_mode(0x0f);
		g_ktm57xx_param.last_trigger_time=system_get_curr_time();
//CS --> PA6、SCLK --> PA4、MOSI --> PA3、MISO --> PA7 INT PA1
//		os_timer_init(&ktm57xx_time,ktm57xx_timetask_funcb,NULL);
//		os_timer_start(&ktm57xx_time,50,true);
}


/*****************************************************************************
**  文件名称：Write_byte
**  功能概要：写入8位地址或数据
**  参数说明：byte ：8位地址或数据
**  函数返回：无
******************************************************************************/
void ktm57xx_write_byte(uint8_t byte)
{ 
	uint8_t i;
	for(i = 0;i < 8;i++)  
	{
		
		SCL_1_Clr();
		if(byte & 0x80) 
			 MOSI_1_Set();
		else 
			 MOSI_1_Clr();
		SCL_1_Set();
		byte <<= 1;  	
	}	
}


/*****************************************************************************
**  文件名称：Read_byte
**  功能概要：读取8位数据
**  参数说明：无
**  函数返回：读取的8位数据
******************************************************************************/
uint8_t ktm57xx_read_byte(void)
{
	uint8_t i;
	uint8_t byte;
	uint8_t dataStore;
	for(i = 0;i < 8;i++)  
	{
		SCL_1_Clr();
		
		byte   = MISO_1_Read();
		dataStore = (dataStore << 1) + byte;
		SCL_1_Set();
		
	}	
	return dataStore;
}


/*****************************************************************************
**  文件名称：ktm57xx_continuous_mode
**  功能概要：设置连续测量模式
**  参数说明：axis：ZYXT 设置对哪个参数进行测量
**  函数返回：返回STATUS状态寄存器值
******************************************************************************/
uint8_t ktm57xx_continuous_mode(uint8_t axis) 
{
	uint8_t dataStore;	
	uint8_t com;

	com = CMD_BURST_MODE + axis;
	nCS_1_Clr();
	ktm57xx_write_byte(com); 
	dataStore = ktm57xx_read_byte();	
	nCS_1_Set();
	return dataStore; 

}



/*****************************************************************************
**  文件名称：ktm57xx_wakeup_mode
**  功能概要：设置唤醒模式
**  参数说明：axis：ZYXT 设置对哪个参数进行测量
**  函数返回：返回STATUS状态寄存器值
******************************************************************************/
uint8_t ktm57xx_wakeup_mode(uint8_t axis) 
{
	uint8_t dataStore;	
	uint8_t com;

	com = CMD_WAKEUP_ON_CHANGE + axis;
	nCS_1_Clr();
	ktm57xx_write_byte(com); 
	dataStore = ktm57xx_read_byte();	
	nCS_1_Set();
	return 	dataStore;  

}



/*****************************************************************************
**  文件名称：ktm57xx_single_mode
**  功能概要：设置单次测量模式
**  参数说明：axis：ZYXT 设置对哪个参数进行测量
**  函数返回：返回STATUS状态寄存器值
******************************************************************************/
uint8_t ktm57xx_single_mode(uint8_t axis) 
{
	uint8_t dataStore;	
	uint8_t com;

	com = CMD_SINGLE_MEASUREMENT + axis;
	nCS_1_Clr();
	ktm57xx_write_byte(com); 
	dataStore = ktm57xx_read_byte();	
	nCS_1_Set();
	return 	dataStore; 
	
}


/*****************************************************************************
**  文件名称：ktm57xx_reset
**  功能概要：复位
**  参数说明：无
**  函数返回：无
******************************************************************************/
void ktm57xx_reset(void) 
{
	uint8_t com;

	com = CMD_RESET;
	nCS_1_Clr();
	
	ktm57xx_write_byte(com); 
	
	nCS_1_Set();

}


/*****************************************************************************
**  文件名称：ktm57xx_read_measurement
**  功能概要：读测量输出
**  参数说明：axis：ZYXT 设置对哪个参数进行输出
**  函数返回：返回的字节数
******************************************************************************/
uint8_t ktm57xx_read_measurement(uint8_t axis) 
{
	uint8_t i,j;	
	uint8_t com;
	uint8_t counter = 1;//counter计算read回来多少位数
	
	com = CMD_READ_MEASUREMENT + axis;	

	for(i = 0;i < 4;i++)
	{
		counter = counter + ( axis & 0x01 )*2;
		axis = axis >> 1;
	}
	if(counter>10)return 0;
	nCS_1_Clr();
	ktm57xx_write_byte(com);
	
	for(j = 0;j < counter;j++) 
	{	
		g_ktm57xx_param.read_data[j] = ktm57xx_read_byte();
	}
	nCS_1_Set();	
	return counter;

}


/*****************************************************************************
**  文件名称：ktm57xx_read_register
**  功能概要：读寄存器
**  参数说明：Register：寄存器
**  函数返回：无
******************************************************************************/
void ktm57xx_read_register(uint8_t Register)
{
	uint8_t j;	
	uint8_t registerData;

	uint8_t comRR[2]; //存放需要写入的寄存器和命令
	
	registerData = Register << 2 ;
	comRR[0] = CMD_READ_REGISTER;
	comRR[1] = registerData; //把要写入的命令都放入数组
	
	nCS_1_Clr();
	for(j=0;j<2;j++)
	{
		ktm57xx_write_byte(comRR[j]);		
  }
	for(j=0;j<3;j++)
	{
		g_ktm57xx_param.read_data[j]=ktm57xx_read_byte();
	}
	nCS_1_Set();
	
}



/*****************************************************************************
**  文件名称：ktm57xx_write_register
**  功能概要：写寄存器
**  参数说明：writeData：写入寄存器的数据
              Register ：需写入的寄存器
**  函数返回：状态寄存器
******************************************************************************/
uint8_t ktm57xx_write_register(uint16_t writeData,uint8_t Register)
{
	uint8_t j;	
	uint8_t registerData;
	uint8_t dataStore = 0;
	uint8_t comWR[4]; //存放需要写入的寄存器和命令
	
	registerData = Register << 2 ;
	comWR[0] = CMD_WRITE_REGISTER;
	comWR[1] = writeData >> 8; //把要写入的命令都放入数组
	comWR[2] = writeData & 0xff;
	comWR[3] = registerData;
	
	//printf("write:%x %x %x %x",comWR[0],comWR[1],comWR[2],comWR[3]);
	nCS_1_Clr();	
	for(j = 0;j < 4;j++)
	{
		ktm57xx_write_byte(comWR[j]);		
  }
	dataStore = ktm57xx_read_byte();
	nCS_1_Set();
	return dataStore;

	
}

/*****************************************************************************
**  文件名称：ktm57xx_exti_current_mode
**  功能概要：退出当前模式
**  参数说明：无
**  函数返回：状态寄存器
******************************************************************************/
uint8_t ktm57xx_exti_current_mode(void)
{	
	uint8_t dataStore = 0;
	uint8_t com = CMD_EXIT_MODE;
	nCS_1_Clr();
	ktm57xx_write_byte(com);	
	dataStore = ktm57xx_read_byte();
	nCS_1_Set();
  return dataStore;
}



void ktm57xx_unlock_otp (void)
{
	uint8_t j;	

	uint8_t comWR[3]; //存放需要写入的寄存器和命令

	comWR[0] = 0xb0;
	comWR[1] = 0xba; //把要写入的命令都放入数组
	comWR[2] = 0xbb;

	nCS_1_Clr();
	for(j = 0;j < 3;j++)
	{
		ktm57xx_write_byte(comWR[j]);   	
  }
	nCS_1_Set();	
}




/*****************************************************************************
**  文件名称：ktm57xx_register_init
**  功能概要：初始化寄存器
**  参数说明：无 
**  函数返回：状态寄存器
******************************************************************************/
void ktm57xx_register_init(void)  
{	
	uint8_t temp_val=0;
	
	temp_val = ktm57xx_write_register(0x0433,0x1C);
	ktm57xx_read_register(0x1C);
	//printf("temp:%x 1c reg:%x %x %x \r\n",temp_val,g_ktm57xx_param.read_data[0],g_ktm57xx_param.read_data[1],g_ktm57xx_param.read_data[2]);
	temp_val = ktm57xx_write_register(0x0002,0x1D); //设置int引脚中断时间0~3F可调 0x01=20ms
//	temp_val = ktm57xx_write_register(0x0003,0x1D); //设置int引脚中断时间0~3F可调 0x01=20ms
	ktm57xx_read_register(0x1D);
	//printf("temp:%x 1d reg:%x %x %x \r\n",temp_val,g_ktm57xx_param.read_data[0],g_ktm57xx_param.read_data[1],g_ktm57xx_param.read_data[2]);
	temp_val=ktm57xx_write_register(0x8000,0x1E);
	ktm57xx_read_register(0x1e);
	//printf("temp:%x 1e reg:%x %x %x \r\n",temp_val,g_ktm57xx_param.read_data[0],g_ktm57xx_param.read_data[1],g_ktm57xx_param.read_data[2]);
	ktm57xx_write_register(0x0000,0x1F);
	
	
}

uint16_t ktm57xx_read_angle(void)
{
	uint32_t angle_temp=0;
	ktm57xx_read_measurement(0X0f);
	//printf("0x%x %x %x %x \r\n",g_ktm57xx_param.read_data[0],g_ktm57xx_param.read_data[1],g_ktm57xx_param.read_data[2],g_ktm57xx_param.read_data[3]);
	angle_temp = (( g_ktm57xx_param.read_data[3] << 8) + g_ktm57xx_param.read_data[4]);
	angle_temp = (angle_temp *360 * 1) / 65536; //角度360°对应65536个LSB
	return angle_temp;
}

/************************************************************
  * @brief   磁编码器数据处理 解析转动方向
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-31
  ***********************************************************/
void ktm57xx_readdata_handle(void)
{
//		os_event_t gui_page_event; 
	  uint8_t event_val=0;
		g_ktm57xx_param.angle = ktm57xx_read_angle();
	  //printf("angle %d  %d\r\n",g_ktm57xx_param.angle,system_get_curr_time());
		if((g_ktm57xx_param.last_angle != 0) && (g_ktm57xx_param.angle!=g_ktm57xx_param.last_angle))
		{
				int32_t temp_val = g_ktm57xx_param.angle-g_ktm57xx_param.last_angle;
				if(abs(temp_val)>300) //临界点处理
				{
					 if(g_ktm57xx_param.last_angle>g_ktm57xx_param.angle) 	
					 {
							printf("left %d  %d \r\n",g_ktm57xx_param.angle,g_ktm57xx_param.last_angle);
//						  event_val = USER_GUI_EVT_ENCODER_LEFT;
					 }
					 else
					 {
					   printf("right %d  %d \r\n",g_ktm57xx_param.angle,g_ktm57xx_param.last_angle);
//						 event_val= USER_GUI_EVT_ENCODER_RIGHT; 
					 }
				}
			  else if((temp_val>JUDGE_ANGLE_LEFT))
				{
//					 event_val= USER_GUI_EVT_ENCODER_LEFT; 
					 printf("left %d  %d \r\n",g_ktm57xx_param.angle,g_ktm57xx_param.last_angle);
				}
				else if(temp_val<JUDGE_ANGLE_RIGHT)
				{
//					  event_val = USER_GUI_EVT_ENCODER_RIGHT; 
						printf("right %d  %d \r\n",g_ktm57xx_param.angle,g_ktm57xx_param.last_angle);
				}
			if((event_val != 0) && ((system_get_curr_time()-g_ktm57xx_param.last_trigger_time) > KTM57XX_SHAKE_TIME))
			{
//					g_ktm57xx_param.last_trigger_time=system_get_curr_time();
//					gui_page_event.event_id = event_val; 
//					gui_page_event.param = &event_val; 
//					gui_page_event.param_len = 1; 
//					os_msg_post(get_user_menu_task_id(), &gui_page_event);
			}
		}
		g_ktm57xx_param.Density_Y = ( g_ktm57xx_param.read_data[5] << 8) + g_ktm57xx_param.read_data[6];
		g_ktm57xx_param.Density_Z = ( g_ktm57xx_param.read_data[7] << 8) + g_ktm57xx_param.read_data[8];
		g_ktm57xx_param.last_angle=g_ktm57xx_param.angle;
}

/************************************************************
  * @brief   磁编码器中断处理 读取传感器角度数据
  * @param   name : NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-30
  ***********************************************************/
void ktm57xx_exti_isr_handle(void)
{
	//exti_interrupt_disable(KTM57XX_EXTI_LINE);
	if (exti_get_LineStatus(KTM57XX_EXTI_LINE))
	{
		ktm57xx_readdata_handle();
		exti_clear_LineStatus(KTM57XX_EXTI_LINE); 
	}
	//exti_interrupt_enable(KTM57XX_EXTI_LINE);
}
void ktm57xx_timetask_funcb(void *arg)
{
	 
	ktm57xx_readdata_handle();
}



