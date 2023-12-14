#include "system_dwt.h"

#define  DWT_CR      *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC
#define  DEM_CR_TRCENA_EN                   (1 << 24)
#define  DWT_CR_CYCCNTENA                		(1 <<  0)

/************************************************************
  * @brief   系统dwt周期性计时器初始化
  * @param   NULL
  * @return  时钟周期计数器计数值
  * @author  LH
  * @date    2020-05-19
  ***********************************************************/
uint8_t	system_dwt_init(void)
{
    DEM_CR |= (uint32_t)DEM_CR_TRCENA_EN; /* 使能DWT外设 */               
    DWT_CYCCNT = (uint32_t)0u;
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
	  __ASM volatile ("NOP");     
    __ASM volatile ("NOP");
    __ASM volatile ("NOP"); //执行3条空指令 
	 return (DWT_CYCCNT)?0:1; //检查时钟周期计数器是否已经启动
}

/************************************************************
  * @brief   获取时钟周期计数器计数值
  * @param   NULL
  * @return  计数值
  * @author  LH
  * @date    2020-05-19
  ***********************************************************/
uint32_t get_system_dwt_value(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}


/************************************************************
  * @brief   通过dwt周期计数器延时us 主频96MHz的情况下，32位计数器计满是2^32/96000000 = 44.739秒 建议使用本函数做延迟的话，延迟在1秒以下。  
  * @param   delay_time/us 延迟长度，单位1us
  * @return  
  * @author  LH
  * @date    2020-05-19
  ***********************************************************/
void dwt_delay_us(uint32_t delay_time)
{
  uint32_t temp_cnt, delay_cnt;
	uint32_t start_time;
	start_time = DWT_CYCCNT;                                     /* 刚进入时的计数器值 */
	temp_cnt = 0;
	delay_cnt = delay_time * (system_get_clock() / 1000000);	 /* 需要的节拍数 */ 		      
	while(temp_cnt < delay_cnt)
	{
		temp_cnt = DWT_CYCCNT - start_time; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}

}

/************************************************************
  * @brief   通过dwt周期计数器延时计数节拍
  * @param   delay_time
  * @return  
  * @author  LH
  * @date    2020-05-19
  ***********************************************************/
void dwt_delay_tick(uint32_t tick)
{
  uint32_t temp_cnt, delay_cnt;
	uint32_t start_time;
	temp_cnt = 0;
	delay_cnt = tick;	 						/* 需要的节拍数 */ 		      
	start_time = DWT_CYCCNT;         /* 刚进入时的计数器值 */
	while(temp_cnt < delay_cnt)
	{
		temp_cnt = DWT_CYCCNT - start_time; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}
}


