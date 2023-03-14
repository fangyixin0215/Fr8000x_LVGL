


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
 
static lv_mem_monitor_t   mem;//监视lvgl内存使用情况
static int8_t curr_set_item = 0;

void seting_page_backdrop_recolor(lv_obj_t* obj, uint8_t index);


static void seting_event_handler(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    //lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t* obj = e->user_data;
    if (!lv_obj_is_valid(obj))
        return;
    if (event == LV_EVENT_DELETE)
    {
        printf("home LV_EVENT_DELETE\r\n");

    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t* key = lv_event_get_param(e);
        switch (*key)
        {
        case LEFT_CODE:
            if (++curr_set_item > 3)curr_set_item = 0;
            seting_page_backdrop_recolor(obj, curr_set_item);
            break;
        case RIGHT_CODE:
            if (--curr_set_item < 0)curr_set_item=3;
            seting_page_backdrop_recolor(obj,curr_set_item);
            break;
        case ENTER_CODE:
            
            break;

        case PARAM_UPDATE_CODE:

            break;
        }
        printf("key_code %d \n", *key);
    }

}


void seting_page_backdrop_recolor(lv_obj_t* obj, uint8_t index)
{
#define bg_make_val  0x1a,0x6b,0xe6
    switch (index)
    {
    case 0:
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,0), lv_color_make(bg_make_val), 0);//设置背景颜色
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,1), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,2), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,3), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 1:
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,0), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,1), lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,2), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj,3), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 2:
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 0), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 1), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 2), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 3), lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        break;
    case 3:

        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 0), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 1), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 2), lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(lv_obj_get_child(obj, 3), lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 4:

        break;
    }
}


void knob_seting_page_entry(lv_obj_t* obj)
{
    static uint8_t init_flag = 0;
    static lv_obj_t* g_set_block_obj1;
    static lv_style_t mainpage_style_base;

    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    if (init_flag == 0)
    {
        init_flag = 1;
        //初始化样式
        lv_style_init(&mainpage_style_base);
        //样式背景颜色：红色
        lv_style_set_bg_color(&mainpage_style_base, lv_color_make(0x00, 0x00, 0x00));//背景黑色
        //样式外边距：6pix
        lv_style_set_border_width(&mainpage_style_base, 2);
        //样式外边框颜色：蓝色
        lv_style_set_border_color(&mainpage_style_base, lv_color_make(0xff, 0xff, 0xff));//边框白色色
    }

    lv_obj_t *main_block_obj1 = lv_obj_create(obj);
    lv_obj_set_scrollbar_mode(main_block_obj1, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    //lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
    //将style_def设置为默认样式
    lv_obj_add_style(main_block_obj1, &mainpage_style_base, 0);
    lv_obj_set_size(main_block_obj1, 110, 110);
    lv_obj_set_pos(main_block_obj1, 10 + 60, 70);

    lv_obj_t* main_block_led_img = lv_img_create(main_block_obj1);
    lv_img_set_src(main_block_led_img, &xn_led_main);//设置图片源
    lv_obj_align(main_block_led_img, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t*  main_block_obj2 = lv_obj_create(obj);
    lv_obj_set_scrollbar_mode(main_block_obj2, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(main_block_obj2, &mainpage_style_base, 0);
    lv_obj_set_size(main_block_obj2, 110, 110);
    lv_obj_set_pos(main_block_obj2, 10 + 60 + 110 + 20, 70);
    lv_obj_t* main_block_curtain_img = lv_img_create(main_block_obj2);
    lv_img_set_src(main_block_curtain_img, &xn_curtain_main);//设置图片源
    lv_obj_align(main_block_curtain_img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t*  main_block_obj3 = lv_obj_create(obj);
    lv_obj_set_scrollbar_mode(main_block_obj3, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(main_block_obj3, &mainpage_style_base, 0);
    lv_obj_set_size(main_block_obj3, 110, 110);
    lv_obj_set_pos(main_block_obj3, 10 + 60, 70 + 110 + 20);
    lv_obj_t *main_block_control_img = lv_img_create(main_block_obj3);
    lv_img_set_src(main_block_control_img, &xn_temp_contrl);//设置图片源 
    lv_obj_align(main_block_control_img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *main_block_obj4 = lv_obj_create(obj);
    lv_obj_set_scrollbar_mode(main_block_obj4, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(main_block_obj4, &mainpage_style_base, 0);
    lv_obj_set_size(main_block_obj4, 110, 110);
    lv_obj_set_pos(main_block_obj4, 10 + 60 + 110 + 20, 70 + 110 + 20);

    lv_obj_t *main_block_ledadj_img = lv_img_create(main_block_obj4);
    lv_img_set_src(main_block_ledadj_img, &xn_ledadj_main);//设置图片源
    lv_obj_align(main_block_ledadj_img, LV_ALIGN_TOP_MID, 0, 0);
    seting_page_backdrop_recolor(obj,0);
    lv_obj_t *main_swtich_lab = lv_label_create(main_block_obj1); // 创建一个标签，
    if (main_swtich_lab != NULL)
    {
        lv_label_set_recolor(main_swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(main_swtich_lab, &language_16, 0);
        lv_label_set_text_fmt(main_swtich_lab, "#ffffff 开关#");
        lv_obj_align(main_swtich_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    lv_obj_t *main_curtain_lab = lv_label_create(main_block_obj2); // 创建一个标签，
    if (main_curtain_lab != NULL)
    {
        lv_label_set_recolor(main_curtain_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(main_curtain_lab, &language_16, 0);
        lv_label_set_text_fmt(main_curtain_lab, "#ffffff 窗帘    10%%#");
        lv_obj_align(main_curtain_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    lv_obj_t *main_control_lab = lv_label_create(main_block_obj3); // 创建一个标签，
    if (main_control_lab != NULL)
    {
        lv_label_set_recolor(main_control_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(main_control_lab, &language_16, 0);
        lv_label_set_text_fmt(main_control_lab, "#ffffff 温控    25℃#");
        lv_obj_align(main_control_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    lv_obj_t* led_temp_lab = lv_label_create(main_block_obj4); // 创建一个标签，
    if (led_temp_lab != NULL)
    {
        lv_label_set_recolor(led_temp_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(led_temp_lab, &language_16, 0);
        lv_label_set_text_fmt(led_temp_lab, "#ffffff 调色灯#");
        lv_obj_align(led_temp_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    
#if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, seting_event_handler, LV_EVENT_ALL, obj);
    lv_group_add_obj(g_group, obj);
#endif
}
