#ifndef __USER_MENU_HANDLE_H
#define __USER_MENU_HANDLE_H


typedef struct
{
	void *image_src;
	int  image_x;
	int  image_y;
	
}main_page_image_t;

typedef enum
{
  WINDOW_MAIN_PAGE=0,
	WINDOW_PAGE0,
	WINDOW_PAGE1,
	WINDOW_PAGE2,
	WINDOW_PAGE3,
	WINDOW_PAGE4,
	WINDOW_PAGE5,
	WINDOW_PAGE6,
	WINDOW_PAGE7,
	WINDOW_PAGE8,
	WINDOW_PAGE9,
	
//	WINDOW_PAGE10,
//	WINDOW_PAGE11,
//	WINDOW_PAGE12,
//	WINDOW_PAGE13,
//	WINDOW_PAGE14,
	WINDOW_WORK,
//	WINDOW_START,
	WINDOW_PAGE_MAX,
}PAGE_NUM_t;
void user_window_main_init(lv_obj_t *p_title);
void user_window_page0_init(lv_obj_t *p_title);
void user_window_page1_init(lv_obj_t *p_title);
void user_window_page2_init(lv_obj_t *p_title);
void user_window_page3_init(lv_obj_t *p_title);
void user_window_page4_init(lv_obj_t *p_title);
void user_window_page5_init(lv_obj_t *p_title);
void user_window_page6_init(lv_obj_t *p_title);
void user_window_page7_init(lv_obj_t *p_title);
void user_window_page8_init(lv_obj_t *p_title);
void user_window_page9_init(lv_obj_t *p_title);
void user_window_page10_init(lv_obj_t *p_title);
void user_window_page11_init(lv_obj_t *p_title);
void user_window_page12_init(lv_obj_t *p_title);
void user_window_page13_init(lv_obj_t *p_title);
void user_window_page14_init(lv_obj_t *p_title);
void user_window_work_init(lv_obj_t *p_title);
void startingup_animing(void); //开机界面

void main_menupage_init(void);
void main_page_ui_process(menu_u8 index);
void user_page0_ui_process(menu_u8 index);
void user_page1_ui_process(menu_u8 index);
void user_page2_ui_process(menu_u8 index);
void user_page3_ui_process(menu_u8 index);
void user_page4_ui_process(menu_u8 index);
void user_page5_ui_process(menu_u8 index);
void user_page6_ui_process(menu_u8 index);
void user_page7_ui_process(menu_u8 index);
void user_work_ui_process(menu_u8 index);

uint8_t judge_current_tileview(lv_obj_t *tile,lv_obj_t *page_act);
void seting_page_backdrop_recolor(uint8_t index);
void gui_menu_task_init(void);
uint16_t get_user_menu_task_id(void);


LV_FONT_DECLARE(language_72)
LV_FONT_DECLARE(language_36)
LV_FONT_DECLARE(language_16)
#endif //__MENU_UI_H

