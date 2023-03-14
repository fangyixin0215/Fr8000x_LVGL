


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

static uint8_t set_power_sta = 0;

static void power_page_event_cb(lv_event_t* e)
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
        case RIGHT_CODE:
            set_power_sta ^= 1;
            if (set_power_sta)
            {
                lv_img_set_src(lv_obj_get_child(obj, 0), &switch_on);
            }
            else
            {
                lv_img_set_src(lv_obj_get_child(obj, 0), &switch_off);
            }
 
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

void knob_power_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t * sw_img = lv_img_create(obj);
    if (set_power_sta)
    {
        lv_img_set_src(lv_obj_get_child(obj, 0), &switch_on);
    }
    else
    {
        lv_img_set_src(lv_obj_get_child(obj, 0), &switch_off);
    }
    lv_obj_align(sw_img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* swtich_lab = lv_label_create(obj); // 创建一个标签，
    if (swtich_lab != NULL)
    {
        lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(swtich_lab, &language_72, 0);
        lv_label_set_text_fmt(swtich_lab, "#ffffff 开关#");
        lv_obj_set_pos(swtich_lab, 140, 110 + 80 + 90 + 20);
    }

#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, power_page_event_cb, LV_EVENT_KEY, obj);
    lv_group_add_obj(g_group, obj);
#endif
}
