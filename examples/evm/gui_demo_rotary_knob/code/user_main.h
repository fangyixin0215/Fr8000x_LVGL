
#ifndef __USER_MAIN_H__
#define __USER_MAIN_H__
#include "lvgl.h"

//#include "window_music.h"
//#include "window_heart.h"
//#include "window_down_menu.h"
//#include "window_zoom.h"
//#include "window_manager.h"
//#include "window_scroll_menu.h"
//#include "window_main.h"
//#include "window_sport.h"
//#include "window_add.h"
#include "lvgl.h"
#include "language.h"
#include "os_task.h"
#include "os_msg_q.h"

////时间结构体
//typedef struct
//{
//	unsigned int  year;
//	unsigned char month;
//    unsigned char day;
//	unsigned char hour;
//	unsigned char minute;
//	unsigned char second;

//}Time_t;
enum
{
  WINDOW_USER1=0,
	WINDOW_USER2,
	WINDOW_USER3,
	WINDOW_USER4,
	WINDOW_USER5,
	WINDOW_USER6,
	WINDOW_USER7,
	WINDOW_USER8,
	WINDOW_USER9,
	WINDOW_USER10,
	WINDOW_USER11,
	WINDOW_USER12,
	WINDOW_USER13,
	WINDOW_USER14,
	WINDOW_USER15,
	WINDOW_MAX,
};

typedef struct
{
    lv_obj_t  *btn_obj;
    void (*btn_event_cb)(lv_event_t *event);

}setting_btn_t;

enum
{
    SET_BTN0=0,
    SET_BTN1,
    SET_BTN2,
    SET_BTN3,
    SET_BTN4,
    SET_BTN5,
    SET_BTN6,
    SET_BTN7,
    SET_BTN8,
    SET_BTN9,
    SET_BTN_MAX,
};

enum user_guitask_event_t {
    USER_GUI_EVT_PAGE_SWITCH=0,
		USER_GUI_EVT_ENCODER_LEFT,
	  USER_GUI_EVT_ENCODER_RIGHT,
		USER_GUI_EVT_ENCODER_SHORT_PRESS,
	  USER_GUI_EVT_ENCODER_LONG_PRESS,
};

typedef struct
{
    lv_obj_t  *window_obj; //返回的界面
    uint8_t  col_id;
	uint8_t  row_id;
	lv_dir_t dir;         //滑动的方吿
	void (*Init)(lv_obj_t *p_title);  			//初始化函敿
}window_manage_t;

typedef struct 
{
	uint8_t mainpage_state;//主页面状态 
	uint8_t subpage_state;
	
}user_gui_page_t;

typedef struct 
{
	uint8_t water_level;//设置水位
	lv_color_t led_color;//设置颜色参数
	uint8_t work_time;
	uint8_t set_temp;
}user_setpage_param_t;

extern window_manage_t user_window_manger[];

void user_guitask_init(void);
void lv_port_indev_init(void);
lv_obj_t * get_title_main(void);
uint16_t get_user_guitask_id(void);

#endif // __USER_MAIN_H__
