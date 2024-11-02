


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

#define SETTING_NUM 7

void page_change_timer(void);

void knob_group_page_entry(lv_obj_t* obj);

 //绑定主界面的图片资源 
const void *home_page_image[] ={
[0] = (void*)&xn_shouye,
[1] =  (void*)&xn_temppage,
[2] =  (void*)&xn_pwoer,
[3] =  (void*)&xn_sw,
[4] =  (void*)&xn_xyj,
[5] =  (void*)&xn_led,
[6] =  (void*)&xn_gd,
[7] =  (void*)&xn_xyj,
};

const char  home_setting_char[SETTING_NUM][15] =
{
    "首页",
    "温控",
    "电源",
    "水位",
    "洗衣",
    "灯光",
    "设置",
};

static int8_t curr_img_id = 0;
static uint8_t curr_page_id = 0;


void knob_return_home_page(void)
{

    if (lv_group_get_obj_count(g_group) > 0)
    lv_group_remove_all_objs(g_group);
    lv_obj_clean(prj_parent_cont);
    prj_prev_cont = lv_obj_create(prj_parent_cont);
    knob_home_page_entry(prj_prev_cont);

   
}
static void home_event_handler(lv_event_t* e)
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
            if (++curr_img_id >6)curr_img_id = 0;
            lv_img_set_src(lv_obj_get_child(obj, 0), home_page_image[curr_img_id]);
            //lv_label_set_text(lv_obj_get_child(obj, 1), home_setting_char[curr_img_id]);
            break;
        case RIGHT_CODE:
            --curr_img_id;
            if (curr_img_id <0)curr_img_id = 6;
            lv_img_set_src(lv_obj_get_child(obj, 0), home_page_image[curr_img_id]);
            lv_label_set_text(lv_obj_get_child(obj, 1), home_setting_char[curr_img_id]);
            break;
        case ENTER_CODE:
            if (lv_group_get_obj_count(g_group) > 0)
            lv_group_remove_all_objs(g_group);
            lv_obj_clean(prj_parent_cont);
            prj_prev_cont = lv_obj_create(prj_parent_cont);
            switch (curr_img_id+1)
            {
            case SUB_PAGE1:
                knob_group_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE2:
                knob_temp_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE3:
                knob_power_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE4:
                knob_water_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE5:
                knob_wash_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE6:
                knob_led_page_entry(prj_prev_cont);
                break;

            case SUB_PAGE7:
                knob_seting_page_entry(prj_prev_cont);
                break;
            }
            //fr_keypad_system_setting(prj_prev_cont);
            break;

        case PARAM_UPDATE_CODE:
           
            break;
        }
        printf("key_code %d \n", *key);
    }

}

void knob_home_page_entry(lv_obj_t* obj)
{

    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t* m_img = lv_img_create(obj);
    //lv_img_set_src(m_img, home_page_image[2]);//设置图片源
	lv_img_set_src(m_img, home_page_image[curr_img_id]);//设置图片源
    lv_obj_align(m_img, LV_ALIGN_CENTER, 0, 0);
	lv_obj_center(m_img);
    lv_obj_t* m_lab = lv_label_create(obj); // 创建一个标签，
    if (m_lab != NULL)
    {
        lv_label_set_recolor(m_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(m_lab, &language_72, 0);
        lv_obj_set_style_text_color(m_lab, knob_font_white_color(), 0);
//        lv_label_set_text(m_lab, home_setting_char[curr_img_id]);
//        lv_obj_set_pos(m_lab, 120, 120 + 80 + 90 + 10);
    }
    #if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(obj, home_event_handler, LV_EVENT_ALL, obj);
    lv_group_add_obj(g_group, obj);
    #endif
}

static void group_page_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_KEY)
    {
        uint32_t* key = lv_event_get_param(e);
        switch (*key)
        {
        //case LEFT_CODE:
        //case RIGHT_CODE:
        case ENTER_CODE:
            lv_group_remove_all_objs(g_group);
            lv_obj_clean(prj_parent_cont);
            prj_prev_cont = lv_obj_create(prj_parent_cont);
            knob_home_page_entry(prj_prev_cont);
            break;
        }
        printf("key_code %d \n", *key);


    }
}
void knob_group_page_entry(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);

    //xn_group_map
    lv_obj_t* group_img = lv_img_create(obj);
    lv_img_set_src(group_img, &xn_group);//设置图片源
    lv_obj_align(group_img, LV_ALIGN_CENTER, 0, 0);
    #if (PAGE_HCI_EN==1)
    lv_obj_add_event_cb(group_img, group_page_event_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(g_group, group_img);
    #endif
}



void test_page_switch_func(lv_timer_t * t)
{
	LV_UNUSED(t);
	printf("%s...\r\n",__func__);
	--curr_img_id;
	if (curr_img_id <0)curr_img_id = 6;
//	lv_img_set_src(lv_obj_get_child(obj, 0), home_page_image[curr_img_id]);
//	lv_label_set_text(lv_obj_get_child(obj, 1), home_setting_char[curr_img_id]);

	if (lv_group_get_obj_count(g_group) > 0)
	lv_group_remove_all_objs(g_group);
	lv_obj_clean(prj_parent_cont);
	prj_prev_cont = lv_obj_create(prj_parent_cont);
	switch (curr_img_id+1)
	{
		case SUB_PAGE1:
		knob_group_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE2:
		knob_temp_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE3:
		knob_power_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE4:
		knob_water_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE5:
		knob_wash_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE6:
		knob_led_page_entry(prj_prev_cont);
		break;

		case SUB_PAGE7:
		knob_seting_page_entry(prj_prev_cont);
		break;
	}
	page_change_timer();

}

void page_change_timer(void)
{
	lv_timer_t * t = lv_timer_create(test_page_switch_func, 1000, NULL);
   lv_timer_set_repeat_count(t, 1);
}
