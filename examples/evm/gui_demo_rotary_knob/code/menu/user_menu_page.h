#ifndef __USER_MENU_PAGE_H
#define __USER_MENU_PAGE_H
#include "user_menu_handle.h"
#include "user_menu_config.h"
#include "user_menu_setting.h"
#include "stdio.h"
#include "../lvgl/lvgl.h"

typedef struct MENU_CONFIG_PARAM
{
	menu_u8 main_current_index ;//主界面当前索引
	menu_u8 switch_state ; 
	menu_u8 select_item;
	menu_u8 work_status ;
	menu_u8 set_temp ;
	menu_u8 water_level ;
	menu_u8 work_time;
	lv_color_t led_color;
	menu_u8 	current_page_id ;//当前界面 索引
	menu_u8 	last_page_index;//保存上一次的界面索引
}menu_config_param_t;
extern menu_config_param_t g_menu_param ;

typedef void (*event_menu_funcb)(menu_u8);
typedef struct EVENT_MENU_STRUCT
{
	int 	event_menu_id ; 				//操作菜单
	event_menu_funcb eventfuncb ;	//操作方法
}EVENT_MENU_PAGE_t;



typedef enum  //界面id值
{
	MAIN_PAGE = 0,
	USER_PAGE0,
	USER_PAGE1,
	USER_PAGE2,
	USER_PAGE3,
	USER_PAGE4,
	USER_PAGE5,
	USER_PAGE6,
	USER_PAGE7,
	WORK_PAGE
}event_page_enumt;


void enter_page_handle(menu_i32 index,menu_u8 KeyCode);
void menu_switch_item(menu_i32 current_index, menu_u8 KeyCode);

void main_page_process(menu_u8 KeyCode);
void user_page0_process(menu_u8 KeyCode);
void user_page1_process(menu_u8 KeyCode);
void user_page2_process(menu_u8 KeyCode);
void user_page3_process(menu_u8 KeyCode);
void user_page4_process(menu_u8 KeyCode);
void user_page5_process(menu_u8 KeyCode);
void user_page6_process(menu_u8 KeyCode);
void user_page7_process(menu_u8 KeyCode);
void work_page_process(menu_u8 KeyCode);

#endif //__MENU_H
