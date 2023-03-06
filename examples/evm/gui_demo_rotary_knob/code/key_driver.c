/************************************************************
  * @brief   按键驱动程序 支持多个按键长按 短按 双击事件检测
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  * @note    drive_key.c
  ***********************************************************/

#include "key_driver.h"
#include "input_driver.h"
//#include "pic.h"



#define KEY1_IN   gpio_read_pin(EC11_KEY_PORT, EC11_KEY_GPIO_NUM)   // 按键输入口  切换
#define KEY2_IN   1   // 按键输入口 设置 //不用设置为1
#define KEY3_IN   1               //加
#define KEY4_IN   1               //减


void key_gpio_init(void);
static void put_buttons_fifo(uint8_t key_data);//按键值送入FIFO
uint8_t read_key_fifo(void);//取FIFO中的数据 


static key_struct_t g_btn_state[KEY_NUM]={0};

static key_fifo_t g_key_fifo_stu={0};		/* 按键FIFO变量*/


/************************************************************
  * @brief   底层按键IO状态读取
  * @param   NULL
  * @return  KEY1_IN 引脚状态值 高低电平
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
static uint8_t read_key1_states(void)
{
    return KEY1_IN;
}
static uint8_t read_key2_states(void)
{
    return KEY2_IN;
}
static uint8_t read_key3_states(void)
{
    return KEY3_IN;
}
static uint8_t read_key4_states(void)
{
    return KEY4_IN;
}

/************************************************************
  * @brief   底层按键初始化  
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
void key_init(void)
{
     uint8_t idx=0;
     key_gpio_init();
			for(idx=0;idx<KEY_NUM;idx++)
			{
				 g_btn_state[idx].click_cnt=0;
				 g_btn_state[idx].down_cnt=0;
				 g_btn_state[idx].key_event=0;
				 g_btn_state[idx].long_cnt=0;
				 g_btn_state[idx].long_time=0;
				 g_btn_state[idx].state=KEY_STA0;  
			}
    g_btn_state[0].key_read_cb = read_key1_states;//按键IO状态读取 
    g_btn_state[1].key_read_cb = read_key2_states;
    g_btn_state[2].key_read_cb = read_key3_states;//按键IO状态读取 
    g_btn_state[3].key_read_cb = read_key4_states;
   
}

/************************************************************
  * @brief   底层按键检测 消抖动 返回按键事件状态
  * @param   index 按键序号
  * @return  pButton->key_event 按键状态
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
uint8_t key_check(uint8_t idx) 
{ 
     key_struct_t *pButton;
     pButton = &g_btn_state[idx];
     switch(pButton->state) //
    {
        case KEY_STA0:
            if(!pButton->key_read_cb())
            {
                 pButton->state= KEY_STA1;      // 按下
            }
            break;
        case KEY_STA1:                       
              if(!pButton->key_read_cb())   
              {
                   pButton->down_cnt=0 ;     
                   pButton->state= KEY_STA2;   
              }else
              {
                      pButton->state= KEY_STA0; 
              }
            break;
        case KEY_STA2: 
						if( pButton->key_read_cb()) 
						{ 
									 pButton->key_event = KEY_SHORT;   
									 pButton->state= KEY_STA0;   
						} 
						else if (++pButton->down_cnt >= LONG_TIME)     
						{ 
									 pButton->key_event = KEY_LONG;        // 按下时间>x*10ms，返回长键事件 
									 pButton->state = KEY_STA3;   
						} 
        break; 
      case KEY_STA3:                 // 返回无按键事件 
						if (pButton->key_read_cb()) 
						{
								 pButton->state = KEY_STA0; 
						}
						else  //可以加入长按 连续处理 长按事件  
						{
						#if(IS_LONG_FUSILLADE==1)
								if(++pButton->long_cnt>15)//定时支持长按后连发
								{
										 pButton->long_cnt=0;
										 pButton->key_event = KEY_LONG;        // 长键事件
								}
						#endif
						}
        break; 
    }
   return  pButton->key_event; 
} 
/************************************************************
  * @brief   读取和确定每个按键的事件值 然后压入FIFO
  * @param   index 按键序号
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
void key_read_state(uint8_t index)
{
    key_struct_t *pButton;
    pButton = &g_btn_state[index];
    key_check(index); 
    switch(pButton->make_sta) 
    { 
        case KEY_STA0: 
            if (pButton->key_event == KEY_SHORT )  
            { 
                 pButton->click_cnt = 0;               // 单击
                 pButton->make_sta = KEY_STA1; 
            } 
            else if(pButton->key_event == KEY_LONG)
            {
                     put_buttons_fifo((3 * index) + 3);//按键 入FIFO
					// pFunc=pBtton->LongPress_Event;
            }else   //无按键  按下 
            {
                
            }
            break; 
        case KEY_STA1: 
            if(pButton->key_event  == KEY_SHORT)  // 双击
            {        
                   put_buttons_fifo((3 * index) + 2);//双击按键 压入FIFO
                   g_btn_state[index].make_sta = KEY_STA0; 
					// pFunc=pBtton->KEY_SHORT_Event;
            } 
            else                                
            {                           
                 if(++pButton->click_cnt >= SHORT_TIME)       
                 {
                     //  (*pFunc)();//执行事件处理函数 
                       put_buttons_fifo((3 * index) + 1);//Short_Time*10ms内没有再次出现单按事件  短按按键 压入FIFO  
                       pButton->make_sta = KEY_STA0;     // 返回初始状态 
                 } 
             } 
             break; 
    }
     pButton->key_event = 0;
}

/************************************************************
  * @brief   按键值压入FIFO
  * @param   key_data 需要压入的键值
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
static void put_buttons_fifo(uint8_t key_data)//按键值送入FIFO
{
	g_key_fifo_stu.Buff[g_key_fifo_stu.rear] = key_data;
	if (++g_key_fifo_stu.rear  >= KEY_FIFO_NUM)
	{
		g_key_fifo_stu.rear = 0;
	}
}

/************************************************************
  * @brief   从按键值FIFO中取数据
  * @param   NULL
  * @return  返回FIFO中数据
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
uint8_t read_key_fifo(void)//取FIFO中的数据 
{
	uint8_t key_data;
	if (g_key_fifo_stu.front == g_key_fifo_stu.rear)
	{
		return KEY_NONE;
	}
	else
	{
		key_data = g_key_fifo_stu.Buff[g_key_fifo_stu.front];
		if (++g_key_fifo_stu.front >= KEY_FIFO_NUM)
		{
			g_key_fifo_stu.front = 0;
		}
		return key_data;
	}
}
/************************************************************
  * @brief   扫描按键   10ms调用一次即可 可用定时器产生10ms时基
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
void key_scanf(void)
{ 
    uint8_t i=0;
    for(i=0;i<KEY_NUM;i++)
    {
        key_read_state(i);
    }
}
/************************************************************
  * @brief   按键输入引脚初始化
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2019-10-21
  * @version v1.0
  ***********************************************************/
void key_gpio_init(void)
{
//   	TRISC=0x1F;
//		PORTC=0xDF; 
}


/*
 应用实例：
 void loopscankey_Task(void)
 {
         if(flags.10ms)
        {
            key_scanf();
            flags.10ms=0;
        }
 }
 void treating_keydode(void)
  {
      uint8_t key_Code=0;
      key_Code =  read_key_fifo();
      switch(key_Code)
      {
          case KEY1_SHORT_EVENT:
              break;                     
          case KEY1_DUAL_EVENT:
              break;
          case KEY1_LONG_EVENT:
              break;
          case KEY2_SHORT_EVENT:
              break;
          case KEY2_DUAL_EVENT:
              break; 
          case KEY2_LONG_EVENT:
              break;     
          case KEY3_SHORT_EVENT:
              break;
          case KEY3_DUAL_EVENT:
              break;
          case KEY3_LONG_EVENT:
              break;    
          case KEY4_SHORT_EVENT:
              break;
          case KEY4_DUAL_EVENT:
              break;
          case KEY4_LONG_EVENT:
              break; 
      }   
   } 
 */
