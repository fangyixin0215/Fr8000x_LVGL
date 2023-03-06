#ifndef __USER_MENU_SETTING_H
#define __USER_MENU_SETTING_H
#include "user_menu_config.h"

//菜单选项值 根据具体情况设置
typedef struct CURRENT_PARAM_SET
{
		menu_u8 page0_index_val;
		menu_u8 page1_index_val;
	  menu_u8 page2_index_val;
	  menu_u8 page3_index_val;
	  menu_u8 page4_index_val;
		menu_u8 page5_index_val;
		menu_u8 page6_index_val;
		menu_u8 page7_index_val;
}current_seting_param_t ;
extern current_seting_param_t current_seting_param ;



#endif


