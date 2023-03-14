


/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "user_knob_config.h"
#else
#include <Windows.h>
#include "resource.h"
#include "user_app/user_knob_config.h"
#endif

static lv_color_t led_color = {.full=0x00f8,};

static void led_page_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = e->user_data;
    if (!lv_obj_is_valid(obj))
        return;
    if (event == LV_EVENT_DELETE)
    {
        printf("temp_page_event_cb LV_EVENT_DELETE\r\n");

    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t* key = lv_event_get_param(e);
        switch (*key)
        {
        case LEFT_CODE:
            led_color.full += 100;
            if (led_color.full > 0xfff0)led_color.full = 0;
            lv_colorwheel_set_rgb(lv_obj_get_child(obj, 1), led_color);
            break;
        case RIGHT_CODE:
            led_color.full -= 100;
            lv_colorwheel_set_rgb(lv_obj_get_child(obj, 1), led_color);
            break;
        case ENTER_CODE:
            //fr_keypad_system_setting(prj_prev_cont);
            break;

        case PARAM_UPDATE_CODE:

            break;
        }
        printf("key_code %d \n", *key);
    }

}
void knob_led_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);


    lv_obj_t* temp_textlab = lv_label_create(obj); // 创建一个标签，
    if (temp_textlab != NULL)
    {
        lv_label_set_recolor(temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(temp_textlab, &language_36, 0);
        lv_label_set_text_fmt(temp_textlab, "#ffffff LED Adjust#");
        lv_obj_set_pos(temp_textlab, 120, 140 + 80 + 90);
    }
    lv_obj_t*  colorwheel_ledadj = lv_colorwheel_create(obj, true);
    lv_obj_set_size(colorwheel_ledadj, 200, 200);
    lv_obj_center(colorwheel_ledadj);
    //led_color.full = 0x00f8;
    lv_colorwheel_set_rgb(colorwheel_ledadj, led_color);
#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, led_page_event_cb, LV_EVENT_KEY, obj);
    lv_group_add_obj(g_group, obj);
#endif
}
