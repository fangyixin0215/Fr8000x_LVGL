


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

static int8_t work_time = 10;
void knob_work_page_entry(lv_obj_t* obj);

static void wash_page_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    //lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t* obj = e->user_data;
    if (!lv_obj_is_valid(obj))
        return;
    if (event == LV_EVENT_DELETE)
    {
        printf("wash_page_event_cb LV_EVENT_DELETE\r\n");

    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t* key = lv_event_get_param(e);
        switch (*key)
        {
        case LEFT_CODE:
            work_time += 5;
            if (work_time > 99)work_time = 0;

            lv_label_set_text_fmt(lv_obj_get_child(obj, 0), "#ffffff %dmin#", work_time);
           
            break;
        case RIGHT_CODE:
            work_time -= 5;
            if (work_time <0)work_time = 100;
            lv_label_set_text_fmt(lv_obj_get_child(obj, 0), "#ffffff %dmin#", work_time);

            break;
        case ENTER_CODE:
            if (lv_group_get_obj_count(g_group) > 0)
            lv_group_remove_all_objs(g_group);
            lv_obj_clean(prj_parent_cont);
            prj_prev_cont = lv_obj_create(prj_parent_cont);
            knob_work_page_entry(prj_prev_cont);
            break;

        case PARAM_UPDATE_CODE:

            break;
        }
        printf("key_code %d \n", *key);
    }

}

void knob_wash_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);


    lv_obj_t * worktime_lab = lv_label_create(obj); // 创建一个标签， 0
    if (worktime_lab != NULL)
    {
        lv_label_set_recolor(worktime_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(worktime_lab, &language_36, 0);
        lv_label_set_text_fmt(worktime_lab, "#ffffff %dmin#", work_time);
        lv_obj_set_pos(worktime_lab, 200, 100);
    }
    lv_obj_t* temp_img = lv_img_create(obj); //1
    lv_img_set_src(temp_img, &xn_time);
    lv_obj_set_pos(temp_img, 150, 100);

    //lv_obj_align(temp_img, LV_ALIGN_CENTER,0,0);
    lv_obj_t* temp_textlab = lv_label_create(obj); // 创建一个标签，2
    if (temp_textlab != NULL)
    {
        lv_label_set_recolor(temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(temp_textlab, &language_72, 0);
        lv_label_set_text_fmt(temp_textlab, "#ffffff 时间#");
        lv_obj_set_pos(temp_textlab, 140, 120 + 80 + 90 + 20);
    }
    lv_obj_t *start_image = lv_img_create(obj);//3
    lv_img_set_src(start_image, &xn_play);
    lv_obj_align(start_image, LV_ALIGN_CENTER, 0, 0);

#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, wash_page_event_cb, LV_EVENT_KEY, obj);
    lv_group_add_obj(g_group, obj);
#endif
}


void knob_work_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t* my_img = lv_img_create(obj);
    lv_img_set_src(my_img, &xn_tuos);
    lv_obj_align(my_img, LV_ALIGN_CENTER, 0, 0);
    /*Create a spinner*/

    lv_obj_t* g_temp_textlab = lv_label_create(obj); // 创建一个标签，
    if (g_temp_textlab != NULL)
    {
        lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
        lv_label_set_text_fmt(g_temp_textlab, "#ffffff Washing...#");
        lv_obj_set_pos(g_temp_textlab, 120, 140 + 80 + 90);
    }
 
}
