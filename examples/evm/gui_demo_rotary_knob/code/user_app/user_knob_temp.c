


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

static int8_t set_temp = 10;

static void temp_page_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    //lv_obj_t *obj = lv_event_get_target(e);
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
            set_temp += 5;
            if (set_temp > 99)set_temp = 0;
            lv_arc_set_value(lv_obj_get_child(obj, 5), set_temp);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 3), "#ffffff %d#", set_temp);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 4), "#ffffff %d%%#", set_temp);
            break;
        case RIGHT_CODE:
            set_temp -= 5;
            if (set_temp <0)set_temp = 100;
            lv_arc_set_value(lv_obj_get_child(obj, 5), set_temp);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 3), "#ffffff %d#", set_temp);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 4), "#ffffff %d%%#", set_temp);
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

void knob_temp_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_t *temp_img1 = lv_img_create(obj);//0
    lv_img_set_src(temp_img1, &temperature);//temp_img设置图片源
    lv_obj_set_pos(temp_img1, 90, 90);

    lv_obj_t* humi_img1 = lv_img_create(obj);//1
    lv_img_set_src(humi_img1, &humidity);//设置图片源
    lv_obj_set_pos(humi_img1, 90, 90 + 80);

    lv_obj_t* wsd_lab = lv_label_create(obj); // 创建一个标签，2
    if (wsd_lab != NULL)
    {
        lv_label_set_recolor(wsd_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(wsd_lab, &language_36, 0);
        lv_label_set_text_fmt(wsd_lab, "#ffffff Humiture#");
        lv_obj_set_pos(wsd_lab, 90 + 20, 90 + 80 + 90);
    }

    lv_obj_t* temp_lab = lv_label_create(obj); // 创建一个标签，3
    if (temp_lab != NULL)
    {
        lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(temp_lab, &language_36, 0);
        lv_label_set_text_fmt(temp_lab, "#ffffff %d#",set_temp);
        lv_obj_set_pos(temp_lab, 90 + 100, 90);
    }
    lv_obj_t* humi_lab = lv_label_create(obj); // 创建一个标签，4
    if (humi_lab != NULL)
    {
        lv_label_set_recolor(humi_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(humi_lab, &language_36, 0);
        lv_label_set_text_fmt(humi_lab, "#ffffff %d%%#",set_temp);
        lv_obj_set_pos(humi_lab, 90 + 100, 90 + 80);
    }
    /*Create an Arc 创建圆弧*/
    lv_obj_t* g_arc_handle = lv_arc_create(obj);//5
    lv_obj_set_size(g_arc_handle, 400, 400);
    lv_arc_set_rotation(g_arc_handle, 270);
    lv_arc_set_bg_angles(g_arc_handle, 0, 360);
    lv_arc_set_range(g_arc_handle, 0, 100);
    lv_obj_remove_style(g_arc_handle, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed 确保旋钮不显示*/
    lv_obj_clear_flag(g_arc_handle, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click */
    lv_obj_center(g_arc_handle);
    /*Manually update the label for the first time*/
    //lv_event_send(g_arc_handle, LV_EVENT_VALUE_CHANGED, NULL);

    lv_arc_set_value(g_arc_handle,set_temp);

#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, temp_page_event_cb, LV_EVENT_KEY, obj);
    lv_group_add_obj(g_group, obj);
#endif
}
