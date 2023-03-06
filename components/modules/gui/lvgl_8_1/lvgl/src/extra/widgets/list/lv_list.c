/**
 * @file lv_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_list.h"
#include "../../../core/lv_disp.h"
#include "../../../widgets/lv_label.h"
#include "../../../widgets/lv_img.h"
#include "../../../widgets/lv_btn.h"

#if LV_USE_LIST

/*********************
 *      DEFINES
 *********************/
#define MV_CLASS &lv_list

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

const lv_obj_class_t lv_list_class = {
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2
};

const lv_obj_class_t lv_list_btn_class = {
    .base_class = &lv_btn_class,
};

const lv_obj_class_t lv_list_text_class = {
    .base_class = &lv_label_class,
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_list_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_class, parent);
    lv_obj_class_init_obj(obj);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    return obj;
}

lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_text_class, list);
    lv_obj_class_init_obj(obj);
    lv_label_set_text(obj, txt);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(obj, LV_PCT(100));
    return obj;
}

lv_obj_t * lv_list_add_btn(lv_obj_t * list, const char * icon, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
	
	  static lv_style_t lv_style;              //外部样式
    lv_style_init(&lv_style);           // 初始化按钮样式
	  lv_style_set_radius(&lv_style, 0); // 设置样式圆角弧度
	  //lv_obj_set_style_bg_color(list1, lv_color_make(0,0,0), 0);
	  //lv_obj_set_scrollbar_mode(list1, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显示
    //lv_obj_set_style_bg_opa(list1,LV_OPA_60,0);               //设置透明度
		lv_style_set_bg_color(&lv_style,lv_color_make(0,0,0));//设置背景颜色为黑色
	  lv_style_set_text_align(&lv_style,LV_TEXT_ALIGN_LEFT);
	  lv_style_set_pad_all(&lv_style,0); //铺满整个布局
	
	
	 lv_style_set_outline_width(&lv_style,1);          //设置样式轮廓的宽度   
   lv_style_set_outline_color(&lv_style,lv_palette_main(LV_PALETTE_NONE));    //设置样式轮廓颜色
   lv_style_set_outline_pad(&lv_style,0);            //设置样式轮廓的边距
	 lv_obj_add_style(obj, &lv_style, 0);    //增加样式
	
	
	 static lv_style_t txt_style;              //外部样式
   lv_style_init(&txt_style);           // 初始化按钮样式
	 lv_style_set_text_color(&txt_style,lv_color_make(0xff,0xff,0xff));

	
	

    if(icon) {
        lv_obj_t * img = lv_img_create(obj);
        lv_img_set_src(img, icon);
			  //lv_obj_set_x(img,50);
    }

    if(txt) {
        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
			  //lv_obj_set_x(label,150);
			  //lv_obj_center(label);
			  lv_style_set_align(&txt_style, LV_ALIGN_CENTER);
        //lv_obj_set_flex_grow(label, 1);
				lv_obj_add_style(label, &txt_style, 0);    //增加样式
    }

    return obj;
}

const char * lv_list_get_btn_text(lv_obj_t * list, lv_obj_t * btn)
{
    LV_UNUSED(list);
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return lv_label_get_text(child);
        }

    }

    return "";
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LIST*/
