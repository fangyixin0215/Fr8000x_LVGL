


/*********************
 *      INCLUDES
 *********************/

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "user_knob_config.h"
#else
#include "user_app/user_knob_config.h"
#include <Windows.h>
#include "resource.h"
#endif

lv_obj_t* prj_parent_cont = NULL;
lv_obj_t* prj_prev_cont = NULL;

lv_indev_t* g_indev_keypad=NULL;
lv_group_t* g_group=NULL;

static void event_handler(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_KEY)
    {
        uint32_t* key = lv_event_get_param(e);
        printf("key_code %d \n", *key);
    }
}

void knob_gui_init(void)
{
    prj_parent_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_pad_all(prj_parent_cont, 0, 0);
    lv_obj_set_style_border_width(prj_parent_cont, 0, 0);
    lv_obj_set_size(prj_parent_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_add_flag(prj_parent_cont, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scroll_snap_x(prj_parent_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_snap_y(prj_parent_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(prj_parent_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(prj_parent_cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_add_event_cb(prj_parent_cont, event_handler, LV_EVENT_ALL, NULL);

    prj_prev_cont = lv_obj_create(prj_parent_cont);
    g_group = lv_group_create();
    lv_indev_set_group(g_indev_keypad, g_group);
    knob_home_page_entry(prj_prev_cont);
    /*knob_seting_page_entry(prj_prev_cont);*/
    //knob_wash_page_entry(prj_prev_cont);
    //knob_power_page_entry(prj_prev_cont);
    /*knob_temp_page_entry(prj_prev_cont);*/
   // lv_timer_create(ui_test_timer_cb, 100, prj_prev_cont);
}



