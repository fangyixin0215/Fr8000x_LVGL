/************************************************************
  * @brief   自定义菜单的基础框架以及操作方法的实现
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-25
  * @note    user_menu_page.c
	* @version v1.0
  ***********************************************************/

#include "user_menu_page.h"

menu_config_param_t g_menu_param ;
//菜单操作表定义,包含每个界面，与对应的回调函数
static EVENT_MENU_PAGE_t g_menu_table[] = 
{
	{MAIN_PAGE        , main_page_process},
	{USER_PAGE0    		, user_page0_process},
	{USER_PAGE1    		, user_page1_process},
	{USER_PAGE2       , user_page2_process},
	{USER_PAGE3    		, user_page3_process},
	{USER_PAGE4    		, user_page4_process},
	{USER_PAGE5       , user_page5_process},
	{USER_PAGE6  			, user_page6_process},
	{USER_PAGE7 			, user_page7_process},
	{WORK_PAGE 				, work_page_process},
};

/************************************************************
  * @brief   跳转到菜单表所对应的页面
  * @param   id_index 菜单表中的索引值  KeyCode 具体操作按键码
  * @return  NULL
  * @author  LH
  * @date    2022-08-24
  * @version v1.0
  ***********************************************************/
static int jump_menuevent_table(menu_i32 id_index , menu_u8 KeyCode)
{
	if(id_index >= sizeof(g_menu_table)/sizeof(g_menu_table[0]) || id_index < 0)//判断范围
	{
		printf("key_code error!\n");
		return -1;
	}
	g_menu_table[id_index].eventfuncb(KeyCode);
	return 0 ;
}

/************************************************************
  * @brief   菜单项选择 通过输入编码器触发后执行
  * @param   id_index 菜单表中的索引值  KeyCode 具体操作按键码
  * @return  NULL
  * @author  LH
  * @date    2022-08-24
  * @version v1.0
  ***********************************************************/ 
void menu_switch_item(menu_i32 current_index, menu_u8 KeyCode)
{
	jump_menuevent_table(current_index , KeyCode);
}

/************************************************************
  * @brief   主页面处理 选择相关的二级界面 
  * @param   KeyCode 具体操作按键码
  * @return  NULL
  * @author  LH
  * @date    2022-08-24
  * @version v1.0
  ***********************************************************/ 
void main_page_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("main_page_process\r\n");
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
				 (g_menu_param.main_current_index > 0) ? (g_menu_param.main_current_index--) : (g_menu_param.main_current_index = 6)  ;
				 main_page_ui_process(g_menu_param.main_current_index);
				 break ;
		case ENCODER_RIGHT:
				 (g_menu_param.main_current_index < 6) ? (g_menu_param.main_current_index++) : (g_menu_param.main_current_index = 0)  ;
				 main_page_ui_process(g_menu_param.main_current_index);
				 break ;
				 
		//在主页面的时候，短按ENTER按键进入对应的子页面
		case ENTER_SHORT:
				 g_menu_param.last_page_index=g_menu_param.main_current_index;
				 enter_page_handle(g_menu_param.main_current_index+1,KeyCode);
			break ;
		
		//在主页面的时候，只要触发了ENTER按键长按即回到开机对应的页面
		case ENTER_LONG:
				 enter_page_handle(MAIN_PAGE,KeyCode);
				 //main_menupage_init();
				 break ;
		default:
			break ;
	}
}

/************************************************************
  * @brief   页面0的相关逻辑处理  
  * @param   KeyCode 具体操作按键码
  * @return  NULL
  * @author  LH
  * @date    2022-08-24
  * @version v1.0
  ***********************************************************/ 
void user_page0_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page1_process\r\n");
	#endif
	//检测实现
	switch(KeyCode)
	{
		case ENCODER_LEFT:
				 break ;
		case ENCODER_RIGHT:

				 break ;
				 
		//在主页面的时候，短按ENTER按键进入对应的子页面
		case ENTER_SHORT:
			break ;
		
		//在主页面的时候，只要触发了ENTER按键长按即回到开机对应的页面
		case ENTER_LONG:
				 enter_page_handle(MAIN_PAGE,KeyCode);
				 //main_menupage_init();
				 break ;
		default:
			break ;
	}
	
}
//温度调节
void user_page1_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page1_process\r\n");
	#endif
	//检测实现
	switch(KeyCode)
	{
		case ENCODER_LEFT:
					(g_menu_param.set_temp > 0) ? (g_menu_param.set_temp--) : (g_menu_param.set_temp = 0)  ;
				  user_page1_ui_process(g_menu_param.set_temp);
				 break ;
		case ENCODER_RIGHT:
				  (g_menu_param.set_temp  < 100) ? (g_menu_param.set_temp ++) : (g_menu_param.set_temp  = 100)  ;
			  	user_page1_ui_process(g_menu_param.set_temp);
				 break ;
				 
		//在主页面的时候，短按ENTER按键进入对应的子页面
		case ENTER_SHORT:
			break ;
		
		//在主页面的时候，只要触发了ENTER按键长按即回到开机对应的页面
		case ENTER_LONG:
				 enter_page_handle(MAIN_PAGE,KeyCode);
				 //main_menupage_init();
				 break ;
		default:
			break ;
	}
	
}
//开关电源
void user_page2_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page2_process\r\n");
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
					(g_menu_param.switch_state > 0) ? (g_menu_param.switch_state--) : (g_menu_param.switch_state = 0)  ;
					user_page2_ui_process(g_menu_param.switch_state );	
				 break ;
		case ENCODER_RIGHT:
					(g_menu_param.switch_state < 1) ? (g_menu_param.switch_state++) : (g_menu_param.switch_state = 1)  ;
					user_page2_ui_process(g_menu_param.switch_state);
				 break ;
				 
		//在主页面的时候，短按ENTER按键进入对应的子页面
		case ENTER_SHORT:
			break ;
		
		//在主页面的时候，只要触发了ENTER按键长按即回到开机对应的页面
		case ENTER_LONG:
				 enter_page_handle(MAIN_PAGE,KeyCode);
				 //main_menupage_init();
				 break ;
		default:
			break ;
	}
	
}



//水位处理
void user_page3_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page3_process val:%d\r\n",KeyCode);
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
				(g_menu_param.water_level > 0) ? (g_menu_param.water_level--) : (g_menu_param.water_level = 0)  ;
				user_page3_ui_process(g_menu_param.water_level);
				//seting_page0_states(g_menu_param.volume_level);
			break ;
			 
		case ENCODER_RIGHT:
				(g_menu_param.water_level < 100) ? (g_menu_param.water_level++) : (g_menu_param.water_level = 100)  ;
				user_page3_ui_process(g_menu_param.water_level);
				//seting_page0_states(g_menu_param.volume_level);
			break ;
		case ENTER_SHORT:
				 	enter_page_handle(WORK_PAGE,KeyCode);	
		break;	
		case ENTER_LONG:
					enter_page_handle(MAIN_PAGE,KeyCode);
			//main_menupage_init();
			break ;
		default:
			break ;
	}
}



//工作时间处理
void user_page4_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page4_process val:%d\r\n",KeyCode);
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
			(g_menu_param.work_time > 0) ? (g_menu_param.work_time--) : (g_menu_param.work_time = 0)  ;
			user_page4_ui_process(g_menu_param.work_time);
			///seting_page3_states(g_menu_param.work_time);
			break ;
			 
		case ENCODER_RIGHT:
			(g_menu_param.work_time < 100) ? (g_menu_param.work_time++) : (g_menu_param.work_time = 100)  ;
			user_page4_ui_process(g_menu_param.work_time);
			//seting_page3_states(g_menu_param.backlight_level);
			break ;
		case ENTER_SHORT:
				enter_page_handle(WORK_PAGE,KeyCode);	
		break;
		case ENTER_LONG:
			enter_page_handle(MAIN_PAGE,KeyCode);
		//		main_menupage_init();
		//enter_page_handle(g_menu_param.last_page_index,KeyCode);
			break ;
		default:
			break ;
	}
}


//颜色调节处理
void user_page5_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page5_process val:%d\r\n",g_menu_param.led_color.full);
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
			 g_menu_param.led_color.full=0xf800+lv_rand(10,0xff);
			 user_page5_ui_process(g_menu_param.led_color.full);
//			seting_page4_states(g_menu_param.wlan_on_off);
			break ;
			 
		case ENCODER_RIGHT:
			 g_menu_param.led_color.full=0x07e0+lv_rand(10,0xaf);
			user_page5_ui_process(g_menu_param.led_color.full);
//			seting_page4_states(g_menu_param.wlan_on_off);
			break ;
			
		case ENTER_LONG:
			enter_page_handle(MAIN_PAGE,KeyCode);
			//main_menupage_init();
			break ;
		
		default:
			break ;
	}
}


//执行相关处理
void user_page6_process(menu_u8 KeyCode)
{	
	#ifdef MENU_DEBUG_EN
	printf("user_page6_process\r\n");
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
			(g_menu_param.select_item > 0) ? (g_menu_param.select_item--) : (g_menu_param.select_item = 3)  ;
			user_page6_ui_process(g_menu_param.select_item);
			break ;
			 
		case ENCODER_RIGHT:
			(g_menu_param.select_item < 3) ? (g_menu_param.select_item++) : (g_menu_param.select_item = 0)  ;
			user_page6_ui_process(g_menu_param.select_item);
			break ;
			
		case ENTER_LONG:
			enter_page_handle(MAIN_PAGE,KeyCode);//长按回到主界面
			//main_menupage_init();
			break ;
		
		default:
			break ;
	}
}


void user_page7_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("user_page7_process val:%d\r\n",KeyCode);
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:

			break ;
			 
		case ENCODER_RIGHT:
			 
			break ;
			
		case ENTER_LONG:
			enter_page_handle(MAIN_PAGE,KeyCode);
		   
			//main_menupage_init();
			break ;
		default:
			break ;
	}
}

void work_page_process(menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("work_page_process val:%d\r\n",g_menu_param.work_status);
	#endif
	switch(KeyCode)
	{
		case ENCODER_LEFT:
			(g_menu_param.work_status > 0) ? (g_menu_param.work_status--) : (g_menu_param.work_status = 0)  ;
 
			break ;
		case ENCODER_RIGHT:
			(g_menu_param.work_status < 2) ? (g_menu_param.work_status++) : (g_menu_param.work_status = 2)  ;
  
			break ;
		case ENTER_SHORT:
					
		break;
		case ENTER_LONG:
			//enter_page_handle(MAIN_PAGE,KeyCode);
		  enter_page_handle(g_menu_param.last_page_index+1,KeyCode);
			//main_menupage_init();
			break ;
		default:
			break ;
	}
}

