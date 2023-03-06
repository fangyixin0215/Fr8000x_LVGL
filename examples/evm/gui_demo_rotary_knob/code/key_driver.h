/* 
 * File:   drive_key.h
 * Author: LH
 *
 * Created on Ten 21, 2019, 15:06 PM
 */

#ifndef _KEY_DRIVER_H__
#define	_KEY_DRIVER_H__

#include <stdint.h>
#include "driver_gpio.h"
#include "driver_system.h"
#ifdef	__cplusplus
extern "C" {
#endif
 
#define LONG_TIME  50  //长按的时间  *30ms 可以根据实际需求进行相应更改
#define SHORT_TIME 2  //短按时间   *30ms  
#define KEY_NUM 4  //按键数量      

#define IS_LONG_FUSILLADE		0 //是否支持长按连发     
typedef enum
{
	KEY_NONE = 0,			/* 无按键 */  
	KEY_SHORT,				/* 短按键 单击 */
  KEY_DUAL,              /* 双击 */
	KEY_LONG,				/* 长按键 */
}KEY_EM;

//按键状态枚举
typedef enum
{
   KEY_STA0 = 0,
   KEY_STA1,
   KEY_STA2,
   KEY_STA3,
}KEY_STA_EM;
 
//按键触发事件枚举
typedef enum
{
   KEY_NONE_EVENT = 0,
   KEY1_SHORT_EVENT,
   KEY1_DUAL_EVENT,
   KEY1_LONG_EVENT,
   
   KEY2_SHORT_EVENT,
   KEY2_DUAL_EVENT,
   KEY2_LONG_EVENT,
   
   KEY3_SHORT_EVENT,
   KEY3_DUAL_EVENT,
   KEY3_LONG_EVENT,
           
   KEY4_SHORT_EVENT,
   KEY4_DUAL_EVENT,
   KEY4_LONG_EVENT,
}KEY_Val_EM;

typedef uint8_t (*pFunction)(void); 
typedef void (*pvFunction)(void); 

//按键全局结构体变量。
typedef struct
{
  pFunction key_read_cb;
	uint16_t  down_cnt;			//  按下时间计数器 */
	uint16_t  long_cnt;		// 长按计数器 */
	uint16_t  long_time;		//按键按下持续时间 */
	uint8_t  state;			//按键当前状态    初始态0  
	uint8_t  key_event;    
	uint16_t  click_cnt;//单击时间 
	uint8_t  make_sta;  //按键确定状态 
}key_struct_t;

/* 按键FIFO结构体 */
#define KEY_FIFO_NUM	10
typedef struct
{
	uint8_t Buff[KEY_FIFO_NUM];		
	uint8_t front;					
	uint8_t rear;					
			
}key_fifo_t;


   

void key_init(void);
void key_scanf(void);

uint8_t read_key_fifo(void);//外部调用 读取FiFO的按键值 

#ifdef	__cplusplus
}
#endif

#endif	/* DRIVE_KEY_H */

