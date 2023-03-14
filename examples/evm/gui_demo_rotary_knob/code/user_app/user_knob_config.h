#ifndef _USER_KNOB_CONFIG_H__
#define _USER_KNOB_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

 
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "picture_define.h"
#include "stdio.h"
#else
#include "picture/picture_define.h"
#endif

enum keypad_setting_id
{
        HOME_PAGE_ID = 0,
        SUB_PAGE1,
        SUB_PAGE2,
        SUB_PAGE3,
        SUB_PAGE4,
        SUB_PAGE5,
        SUB_PAGE6,
        SUB_PAGE7,
 };

static inline lv_color_t knob_font_white_color(void) { return lv_color_make(0xff, 0xff, 0xff); }

LV_FONT_DECLARE(language_16);
LV_FONT_DECLARE(language_20);
LV_FONT_DECLARE(language_72);
LV_FONT_DECLARE(language_24);
LV_FONT_DECLARE(language_36);

#define PAGE_HCI_EN 1
extern lv_obj_t* prj_parent_cont ;
extern lv_obj_t* prj_prev_cont ;

extern lv_indev_t* g_indev_keypad ;
extern lv_group_t* g_group ;

#define LEFT_CODE       150
#define RIGHT_CODE      151
#define ENTER_CODE      152
#define RELEASE_CODE    LV_KEY_UP
#define DBLCLICK_CODE   153
#define PARAM_UPDATE_CODE 154


void knob_gui_init(void);
void knob_home_page_entry(lv_obj_t* obj);
void knob_temp_page_entry(lv_obj_t* obj);
void knob_power_page_entry(lv_obj_t* obj);
void knob_water_page_entry(lv_obj_t* obj);
void knob_wash_page_entry(lv_obj_t* obj);
void knob_led_page_entry(lv_obj_t* obj);
void knob_seting_page_entry(lv_obj_t* obj);



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* user_demo_1_H */





