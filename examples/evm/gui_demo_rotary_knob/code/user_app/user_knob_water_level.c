


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

static int8_t set_water_level = 10;

static void water_page_event_cb(lv_event_t* e)
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
            set_water_level += 5;
            if (set_water_level > 99)set_water_level = 0;
            lv_bar_set_value(lv_obj_get_child(obj, 0), set_water_level,LV_ANIM_OFF);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 1), "#ffffff %dL#", set_water_level);

            break;
        case RIGHT_CODE:
            set_water_level -= 5;
            if (set_water_level <0)set_water_level = 100;
            lv_bar_set_value(lv_obj_get_child(obj, 0), set_water_level, LV_ANIM_OFF);
            lv_label_set_text_fmt(lv_obj_get_child(obj, 1), "#ffffff %dL#", set_water_level);
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

void knob_water_page_entry(lv_obj_t* obj)
{
    static uint8_t init_flag = 0;
    lv_obj_t* water_level_bar;
    static lv_style_t  g_style_indic;

    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    if (init_flag == 0)
    {
        init_flag = 1;
        lv_style_init(&g_style_indic);//初始化样式 
        lv_style_set_bg_opa(&g_style_indic, LV_OPA_COVER);//不透明度百分比。
        lv_style_set_bg_color(&g_style_indic, lv_palette_main(LV_PALETTE_RED));//设置颜色
        lv_style_set_bg_grad_color(&g_style_indic, lv_palette_main(LV_PALETTE_BLUE));//设置背景色
        lv_style_set_bg_grad_dir(&g_style_indic, LV_GRAD_DIR_VER);//垂直 从上到下
        //lv_style_set_border_width(&g_style_indic,2);//设置边界
    }

    water_level_bar = lv_bar_create(obj);//创建一个滑杆对象0
    lv_obj_add_style(water_level_bar, &g_style_indic, LV_PART_INDICATOR);//添加样式  LV_PART_INDICATOR为指示
    lv_obj_set_size(water_level_bar, 20, 200);//设置大小
    //lv_obj_center(g_water_level_bar);
    lv_obj_set_pos(water_level_bar, 150, 50);//设置坐标
    lv_bar_set_range(water_level_bar, 0, 100);//设置bar值范围
    lv_bar_set_value(water_level_bar, set_water_level,LV_ANIM_OFF);

    lv_obj_t *water_lab = lv_label_create(obj); // 创建一个标签，1
    if (water_lab != NULL)
    {
        lv_label_set_recolor(water_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(water_lab, &language_36, 0);
        lv_label_set_text_fmt(water_lab, "#ffffff %dL#", set_water_level);
        lv_obj_set_pos(water_lab, 190, 120);
    }
    
    lv_obj_t* g_temp_textlab = lv_label_create(obj); // 创建一个标签，2
    if (g_temp_textlab != NULL)
    {
        lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
        lv_label_set_text_fmt(g_temp_textlab, "#ffffff Water Level");
        lv_obj_set_pos(g_temp_textlab, 120, 130 + 80 + 90);
    }

#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, water_page_event_cb, LV_EVENT_KEY, obj);
    lv_group_add_obj(g_group, obj);
#endif
}
