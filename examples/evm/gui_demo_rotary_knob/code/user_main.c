


#include "stdio.h"
#include "../lvgl/lvgl.h"
#include "user_main.h"
#include "input_driver.h"
#include "os_timer.h"
#include "key_driver.h"

#if 0
//#include "../lvgl/lv_port_indev.h"

user_gui_page_t g_user_gui_page={0};
user_setpage_param_t g_setpage_param={0};

/*********************
 *      DEFINES
 *********************/
static lv_obj_t *title_main=NULL; //定义全局的titleview对象
static lv_obj_t *current_time=NULL;
static lv_obj_t *my_temp_img=NULL;
static lv_obj_t *my_humi_img=NULL;
static lv_obj_t *my_switch_img=NULL;
static lv_obj_t *humi_lab=NULL;
static lv_obj_t *temp_lab=NULL;
static lv_obj_t *g_my_arc=NULL;
static lv_anim_t g_anim_tmp;//动态更新圆弧用
static lv_anim_t g_anim_water_level;

static lv_obj_t *g_water_level_bar;
static lv_style_t g_style_indic;
static lv_obj_t *g_water_lab;
static lv_obj_t *g_worktime_lab;

static lv_obj_t *g_start_imgbtn;
static lv_obj_t *g_stop_imgbtn;
static lv_obj_t *g_settemp_img;

static uint16_t user_guitask_id=0;

static uint16_t tempval=20;
static uint16_t humival=85;

//全局的时间结构体
static Time_t My_time=
{
	.year=2022,
	.month=1,
	.day=7,
	.hour=19,
	.minute=52,
	.second=37,
};

void user_window_test1_init(lv_obj_t *p_title);
void user_window_test2_init(lv_obj_t *p_title);
void user_window_test3_init(lv_obj_t *p_title);
void user_window_test4_init(lv_obj_t *p_title);
void user_window_test5_init(lv_obj_t *p_title);
void user_master_window_init(lv_obj_t *p_title);
void user_window_test6_init(lv_obj_t *p_title);
void user_window_test7_init(lv_obj_t *p_title);
void user_window_test8_init(lv_obj_t *p_title);
void user_window_test9_init(lv_obj_t *p_title);
void user_window_test10_init(lv_obj_t *p_title);
void user_window_test11_init(lv_obj_t *p_title);
void user_window_test12_init(lv_obj_t *p_title);
void user_window_test13_init(lv_obj_t *p_title);
void user_window_test14_init(lv_obj_t *p_title);
void user_window_test15_init(lv_obj_t *p_title);


static void switch_status_btn_event_cb(lv_event_t * event);


//window_manage_t user_window_manger[]=
//{
//    [WINDOW_USER1]={.window_obj=NULL, .col_id=0,.row_id=0,.dir=LV_DIR_ALL,.Init=user_master_window_init},           							//下拉菜单
//	[WINDOW_USER2]={.window_obj=NULL, .col_id=0,.row_id=1,.dir=LV_DIR_HOR|LV_DIR_TOP,.Init=user_window_test1_init},//主界靿
//	[WINDOW_USER3]={.window_obj=NULL, .col_id=1,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test2_init},
//	[WINDOW_USER4]={.window_obj=NULL, .col_id=2,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test3_init},
//	[WINDOW_USER5]={.window_obj=NULL, .col_id=3,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test4_init},
//	[WINDOW_USER6]={.window_obj=NULL, .col_id=5,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test5_init},
//	[WINDOW_USER7]={.window_obj=NULL, .col_id=6,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test6_init},
//	[WINDOW_USER8]={.window_obj=NULL, .col_id=7,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test7_init},
//	[WINDOW_USER9]={.window_obj=NULL, .col_id=8,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test8_init},
//};
window_manage_t user_window_manger[]=
{
  [WINDOW_USER1]={.window_obj=NULL, .col_id=0,.row_id=1,.dir=LV_DIR_ALL,.Init=user_window_test1_init},           							//下拉菜单
	[WINDOW_USER2]={.window_obj=NULL, .col_id=1,.row_id=1,.dir=LV_DIR_ALL,.Init=user_window_test2_init},//主界靿
	[WINDOW_USER3]={.window_obj=NULL, .col_id=2,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test3_init},
	[WINDOW_USER4]={.window_obj=NULL, .col_id=3,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test4_init},
	[WINDOW_USER5]={.window_obj=NULL, .col_id=4,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test5_init},
	[WINDOW_USER6]={.window_obj=NULL, .col_id=5,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test6_init},
	[WINDOW_USER7]={.window_obj=NULL, .col_id=6,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test7_init},
	[WINDOW_USER8]={.window_obj=NULL, .col_id=7,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test8_init},
	[WINDOW_USER9]={.window_obj=NULL, .col_id=8,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test9_init},
	[WINDOW_USER10]={.window_obj=NULL, .col_id=9,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test10_init},
	[WINDOW_USER11]={.window_obj=NULL, .col_id=10,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test11_init},
	[WINDOW_USER12]={.window_obj=NULL, .col_id=11,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test12_init},
	[WINDOW_USER13]={.window_obj=NULL, .col_id=12,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test13_init},
	[WINDOW_USER14]={.window_obj=NULL, .col_id=13,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test14_init},
	[WINDOW_USER15]={.window_obj=NULL, .col_id=14,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_test15_init},
	
};
setting_btn_t user_setting_group[SET_BTN_MAX];
//extern lv_obj_t * current_time;


static void my_second_timer(lv_timer_t * timer)
{
        static uint16_t last_val=0;
	    //lv_event_send(current_time,LV_EVENT_REFRESH,NULL);
				lv_obj_t *current_act=lv_tileview_get_tile_act(title_main);
        if(current_act == user_window_manger[WINDOW_USER1].window_obj)
        {
            lv_label_set_text_fmt(current_time, "#ffffff %02d:%02d:%02d#",My_time.hour,My_time.minute,My_time.second);
        }
				#if 0
        if(current_act == user_window_manger[WINDOW_USER2].window_obj)
        {
                humival=lv_rand(10,99);
                tempval=lv_rand(0,99);
                lv_label_set_text_fmt(temp_lab, "#ffffff %d#",tempval);
                lv_label_set_text_fmt(humi_lab, "#ffffff %d%%#",humival);

//                lv_anim_set_values(&g_anim_tmp, last_val, humival);
//                lv_anim_start(&g_anim_tmp);
//                last_val=humival;
        }
				#endif
	  // lv_label_set_text_fmt(current_time, "#ffffff %8s#",time_temp_buff);
        My_time.second++;

	   if( My_time.second>=60)
		 {
		     My_time.second=0;
			    My_time.minute++;
			 if(My_time.minute>=60)
			 {
					My_time.minute=0;
				  My_time.hour++;
						 if(My_time.hour>=24)
						 {
								My_time.hour=0;
						 }
			 }
		 }
		 printf("timer %d:%d:%d\r\n", My_time.hour,My_time.minute,My_time.second);
}



static lv_indev_t * indev_keypad;
static lv_group_t *g_work_group;
static lv_indev_t * indev_encoder;
int32_t encoder_diff;
lv_indev_state_t encoder_state;

void lv_port_indev_init(void)
{
		encoder_init();
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;//设备类型 编码器
   // indev_drv.read_cb = encoder_read;//回调函数
    indev_encoder = lv_indev_drv_register(&indev_drv);

    g_work_group = lv_group_create();	//创建组'
	  lv_group_set_default(g_work_group);
		lv_group_remove_all_objs(g_work_group);
	
    lv_indev_set_group(indev_encoder, g_work_group);	//将组绑定到输入设备
	  
}
lv_obj_t * get_title_main(void)
{
  return title_main;
}
uint16_t get_user_guitask_id(void)
{
   return user_guitask_id;
}


char  *msg_tab[]={"btn1", "btn2"};

void iphone_status_btn_event_cb(lv_event_t * event)
{
        //printf("%s\r\n ",__FUNCTION__);

		//判断当前的对象
        lv_obj_t * obj = lv_event_get_target(event);
        lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", NULL, true);
       //  lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", msg_tab, true);
         lv_obj_center(mbox1);
       // lv_obj_t* mbox1 = lv_msgbox_create(obj, "help","hello","1243",NULL);
}
void alarm_status_btn_event_cb(lv_event_t * event)
{
        printf("alarm_status_btn_event_cb\r\n ");
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER7].col_id, user_window_manger[WINDOW_USER7].row_id,LV_ANIM_OFF);
}
void user_window_test1_init(lv_obj_t *p_title)
{
  //创建一个logo图片
	//lv_obj_t * tile0 = lv_tileview_add_tile(user_window_manger[WINDOW_USER1].window_obj);
	lv_obj_t * iphone_img = lv_img_create(p_title);
	lv_obj_set_size(iphone_img,xn_iphone.header.w,xn_iphone.header.h);
	lv_img_set_src(iphone_img,&xn_iphone);//设置图片源
	lv_obj_set_pos(iphone_img,90+0*80,100+0*42);


//	lv_obj_t * alarm_img = lv_img_create(p_title);
//	lv_obj_set_size(alarm_img,img_test2.header.w,img_test2.header.h);
//	lv_img_set_src(alarm_img,&img_test2);//设置图片源  count_img
//	lv_obj_set_pos(alarm_img,70+1*80,100+0*42);

	lv_obj_t * alarm_img = lv_img_create(p_title);
	lv_obj_set_size(alarm_img,xn_count.header.w,xn_count.header.h);
	lv_img_set_src(alarm_img,&xn_count);//设置图片源  count_img
	lv_obj_set_pos(alarm_img,90+1*80,100+0*42);
	
	lv_obj_t * rts_img = lv_img_create(p_title);
	lv_obj_set_size(rts_img,xn_music.header.w,xn_music.header.h);
	lv_img_set_src(rts_img,&xn_music);//设置图片源
	lv_obj_set_pos(rts_img,90+2*80,100+0*42);

	lv_obj_t * snow_img = lv_img_create(p_title);
	lv_obj_set_size(snow_img, xn_xueh.header.w,xn_xueh.header.h);
	lv_img_set_src(snow_img,&xn_xueh);//设置图片源
	lv_obj_set_pos(snow_img,90+0*80,100+1*142);



	lv_obj_t * hot_img = lv_img_create(p_title);
	lv_obj_set_size(hot_img,xn_huoh.header.w,xn_huoh.header.h);
	lv_img_set_src(hot_img,&xn_huoh);//设置图片源
	lv_obj_set_pos(hot_img,90+1*80,100+1*142);

	lv_obj_t * msg_img = lv_img_create(p_title);
	lv_obj_set_size(msg_img,xn_cam.header.w,xn_cam.header.h);
	lv_img_set_src(msg_img,&xn_cam);//设置图片源
	lv_obj_set_pos(msg_img,90+2*80,100+1*142);
		//显示时间
    current_time = lv_label_create(p_title); // 创建一个标签，
	if (current_time != NULL)
	{
			lv_label_set_recolor(current_time, true);                        /*Enable re-coloring by commands in the text*/
      lv_label_set_text_fmt(current_time, "#ffffff %2d:%2d:%2d#",My_time.hour,My_time.minute,My_time.second);
		  lv_obj_set_style_text_font(current_time, &time_72, 0);
      //lv_obj_set_size(current_time, 160, 50);
			lv_obj_align(current_time, LV_ALIGN_CENTER,0,0);
	}
	lv_obj_add_flag(iphone_img,LV_OBJ_FLAG_CLICKABLE); //增加可以点击的属性
    //lv_obj_add_flag(iphone_img,LV_OBJ_FLAG_CHECKABLE); //增加可以点击的属性
  lv_obj_add_state(iphone_img,LV_STATE_PRESSED);
  lv_obj_add_event_cb(iphone_img,iphone_status_btn_event_cb, LV_EVENT_CLICKED, NULL);	//增加按键处理回调函数
	
	lv_obj_add_flag(alarm_img,LV_OBJ_FLAG_CLICKABLE); //增加可以点击的属性
  lv_obj_add_state(alarm_img,LV_STATE_PRESSED);
	lv_obj_add_event_cb(alarm_img,alarm_status_btn_event_cb, LV_EVENT_CLICKED, NULL);	
//    lv_obj_t * label = lv_label_create(p_title);          /*Add a label to the button*/
//    lv_label_set_text(label, "Button");                     /*Set the labels text*/
//    lv_obj_add_flag(label,LV_OBJ_FLAG_CHECKABLE);
//    lv_obj_center(label);
		
//		lv_group_set_editing(g_work_group,false);//导航模式
}

static void my_timer1(lv_timer_t * timer)
{
    static int value=0;
    value++;
    switch(value)
    {
    case 2:
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER2].col_id, user_window_manger[WINDOW_USER2].row_id,LV_ANIM_OFF);
        break;

    case 5:
//        lv_obj_set_tile_id(title_main,1, 1,LV_ANIM_OFF);
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER3].col_id, user_window_manger[WINDOW_USER3].row_id,LV_ANIM_OFF);
        break;

    case 6:
//        lv_obj_set_tile_id(title_main,1, 2,LV_ANIM_OFF);
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER4].col_id, user_window_manger[WINDOW_USER4].row_id,LV_ANIM_OFF);
        break;

    case 7:
       lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER5].col_id, user_window_manger[WINDOW_USER5].row_id,LV_ANIM_OFF);

        break;
    case 8:
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER6].col_id, user_window_manger[WINDOW_USER6].row_id,LV_ANIM_OFF);
        
			break;

    case 9:
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER7].col_id, user_window_manger[WINDOW_USER7].row_id,LV_ANIM_OFF);
				
        break;
		case 10:
        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER1].col_id, user_window_manger[WINDOW_USER1].row_id,LV_ANIM_OFF);
				value=0;
       break;
//		case 11:
//        lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER9].col_id, user_window_manger[WINDOW_USER9].row_id,LV_ANIM_OFF);
//        value=0;
//        break;
    }
    printf("lv_arc_get_value(arc) = %d", value);
}

static void myset_arcangle_cb(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
    //printf("v:%d\r\n",v);
}


#if 0
void user_window_test2_init(lv_obj_t *p_title)
{

    my_temp_img = lv_img_create(p_title);
	  lv_img_set_src(my_temp_img,&temperature);//temp_img设置图片源
	  lv_obj_set_pos(my_temp_img,90,90);


    my_humi_img = lv_img_create(p_title);
	  lv_img_set_src(my_humi_img,&humidity);//设置图片源
	  lv_obj_set_pos(my_humi_img,90,90+80);

      lv_obj_t *wsd_lab = lv_label_create(p_title); // 创建一个标签，
      if (wsd_lab != NULL)
      {
			lv_label_set_recolor(wsd_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(wsd_lab, &language_36, 0);
		    lv_label_set_text_fmt(wsd_lab, "#ffffff Humiture#");

			lv_obj_set_pos(wsd_lab,90+20,90+80+90);
      }
      temp_lab = lv_label_create(p_title); // 创建一个标签，
      if (temp_lab != NULL)
      {
           lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(temp_lab, &language_36, 0);
		   lv_label_set_text_fmt(temp_lab, "#ffffff %d#",tempval);
           lv_obj_set_pos(temp_lab,90+100,90);
      }
      humi_lab = lv_label_create(p_title); // 创建一个标签，
      if (humi_lab != NULL)
      {
           lv_label_set_recolor(humi_lab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(humi_lab, &language_36, 0);
					 lv_label_set_text_fmt(humi_lab, "#ffffff %d#",humival);
           lv_obj_set_pos(humi_lab,90+100,90+80);
      }

    /*Create an Arc 创建圆弧*/
    lv_obj_t * g_my_arc = lv_arc_create(p_title);
    lv_obj_set_size(g_my_arc, 360, 360);
    lv_arc_set_rotation(g_my_arc, 270);
    lv_arc_set_bg_angles(g_my_arc, 0, 360);
    lv_arc_set_range(g_my_arc, 0, 100);
    lv_obj_remove_style(g_my_arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed 确保旋钮不显示*/
    lv_obj_clear_flag(g_my_arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click */
    lv_obj_center(g_my_arc);
    /*Manually update the label for the first time*/
    //lv_event_send(g_my_arc, LV_EVENT_VALUE_CHANGED, NULL);
    lv_arc_set_value(g_my_arc,50);
    //lv_arc_set_value(g_my_arc,80);
    //动态更新圆弧中的值必须使用此方法
    lv_anim_init(&g_anim_tmp);
    lv_anim_set_var(&g_anim_tmp, g_my_arc);
    lv_anim_set_exec_cb(&g_anim_tmp, myset_arcangle_cb);//设置回调
    lv_anim_set_time(&g_anim_tmp, 500);//更新时间
//    lv_anim_set_repeat_count(&g_anim_tmp, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
//    lv_anim_set_repeat_delay(&g_anim_tmp, 500);
//    lv_anim_start(&g_anim_tmp);//开始刷新


}
#endif
void user_window_test2_init(lv_obj_t *p_title)
{
		g_settemp_img = lv_img_create(p_title);
	  lv_img_set_src(g_settemp_img, &xn_temppage);
	  lv_obj_align(g_settemp_img, LV_ALIGN_CENTER,0,0);
    lv_obj_t *swtich_lab = lv_label_create(p_title); // 创建一个标签，
		if (swtich_lab != NULL)
		{
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_36, 0);
		  lv_label_set_text_fmt(swtich_lab, "#ffffff SetTemp#");
			lv_obj_set_pos(swtich_lab,140,110+80+90+20);
		}

}

void user_window_test3_init(lv_obj_t *p_title)
{
	  my_switch_img = lv_img_create(p_title);
	  lv_img_set_src(my_switch_img, &xn_pwoer);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(my_switch_img, LV_ALIGN_CENTER,0,0);

      lv_obj_t *swtich_lab = lv_label_create(p_title); // 创建一个标签，
		if (swtich_lab != NULL)
		{
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_36, 0);
		    lv_label_set_text_fmt(swtich_lab, "#ffffff Power#");

			lv_obj_set_pos(swtich_lab,140,110+80+90+20);
		}
		lv_obj_add_flag(my_switch_img,LV_OBJ_FLAG_CLICKABLE ); //增加可以点击的属性
    lv_obj_add_state(my_switch_img,LV_STATE_PRESSED);
    lv_obj_add_event_cb(my_switch_img,switch_status_btn_event_cb, LV_EVENT_CLICKED, NULL);	//增加按键处理回调函数
}

void user_window_test4_init(lv_obj_t *p_title)
{
	  lv_obj_t *rotary_sw_obj = lv_img_create(p_title);
	  lv_img_set_src(rotary_sw_obj, &xn_sw);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(rotary_sw_obj, LV_ALIGN_CENTER,0,0);
      lv_obj_t *swtich_lab = lv_label_create(p_title); // 创建一个标签，
      if (swtich_lab != NULL)
      {
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_72, 0);
		    lv_label_set_text_fmt(swtich_lab, "#ffffff 水位#");
			lv_obj_set_pos(swtich_lab,120,120+80+90);
      }
		
}

void user_window_test5_init(lv_obj_t *p_title)
{
	  lv_obj_t *rotary_xyj_obj = lv_img_create(p_title);
	  lv_img_set_src(rotary_xyj_obj, &xn_xyj);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(rotary_xyj_obj, LV_ALIGN_CENTER,0,0);
      lv_obj_t *xyj_lab = lv_label_create(p_title); // 创建一个标签，
      if (xyj_lab != NULL)
      {
			lv_label_set_recolor(xyj_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(xyj_lab, &language_72, 0);
		    lv_label_set_text_fmt(xyj_lab, "#ffffff 洗衣#");
			lv_obj_set_pos(xyj_lab,120,120+80+90);
      }
}
static void msgbox_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * msgbox = lv_event_get_current_target(e);
		lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        const char * txt = lv_msgbox_get_active_btn_text(msgbox);
        if(txt) {
            lv_msgbox_close(msgbox);
            lv_group_focus_freeze(g_work_group, false);
//            lv_group_focus_obj(lv_obj_get_child(obj, 0));
//            lv_obj_scroll_to(obj, 0, 0, LV_ANIM_OFF);

        }
    }
}
static void msgbox_create(void)
{
    static const char * btns[] = {"Ok", "Cel", ""};
    lv_obj_t * mbox = lv_msgbox_create(NULL, "Hi", "encoder demo", btns, false);
    lv_obj_add_event_cb(mbox, msgbox_event_cb, LV_EVENT_ALL, NULL);
    lv_group_focus_obj(lv_msgbox_get_btns(mbox));
    lv_obj_add_state(lv_msgbox_get_btns(mbox), LV_STATE_FOCUS_KEY);
#if LV_EX_MOUSEWHEEL
    lv_group_set_editing(g, true);
#endif
    lv_group_focus_freeze(g_work_group, true);

    lv_obj_align(mbox, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * bg = lv_obj_get_parent(mbox);
    lv_obj_set_style_bg_opa(bg, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(bg, lv_palette_main(LV_PALETTE_GREY), 0);
}

void user_window_test6_init(lv_obj_t *p_title)
{
		  lv_obj_t *rotary_led_obj = lv_img_create(p_title);
	  lv_img_set_src(rotary_led_obj, &xn_led);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(rotary_led_obj, LV_ALIGN_CENTER,0,0);

		lv_obj_t *xyj_lab = lv_label_create(p_title); // 创建一个标签，
		if (xyj_lab != NULL)
		{
		  lv_label_set_recolor(xyj_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(xyj_lab, &language_72, 0);
			lv_label_set_text_fmt(xyj_lab, "#ffffff 灯控#");
		  lv_obj_set_pos(xyj_lab,120,120+80+90);
		}
//	  lv_obj_t *label1 = lv_label_create(p_title); // 创建一个标签，
////	  msgbox_create();
//    if (label1 != NULL)
//    {
//        lv_label_set_recolor(label1, true);                        /*Enable re-coloring by commands in the text*/
//        lv_obj_set_style_text_font(label1, &language_36, 0);
//				lv_label_set_text_fmt(label1, "#ffffff test#");
//        lv_obj_set_pos(label1,90+100,90);
//    }
//		/* 创建一个 switch 部件(对象) */
//    lv_obj_t * sw = lv_switch_create(p_title);       // 创建一个 switch 部件(对象),他的父对象是活动屏幕对象
//    //lv_obj_center(sw);                                    // 方法1：让对象居中，简洁
//    lv_obj_align(sw, LV_ALIGN_TOP_MID, 10, 10);            // 方法2：让对象居中，较为灵活
//		lv_obj_add_state(sw, LV_STATE_CHECKED);                       // 开关默认处于关闭状态，这里设置为打开状态
//		lv_obj_set_size(sw,60,35);
//		
//		 /* 创建一个 checkbox 部件(对象) */
//    lv_obj_t * checkbox = lv_checkbox_create(p_title);     // 创建一个 switch 部件(对象),他的父对象是活动屏幕对象
//    lv_checkbox_set_text(checkbox, "checkbox" LV_SYMBOL_PLAY);  // 修改复选框的提示文字(覆盖)
////		lv_obj_align(checkbox,     LV_ALIGN_BOTTOM_MID, -60, -60);            // 方法2：让对象居中，较为灵活
//		lv_obj_set_pos(checkbox,100+50,90+50);
//		 /* 创建一个btn部件(对象) */
//    lv_obj_t * btn = lv_btn_create(p_title);       // 创建一个btn部件(对象),他的父对象是活动屏幕对象
//    // 打开了 LV_OBJ_FLAG_CHECKABLE ，当对象被点击时有选中切换(Toggle)状态的效果
//    // 其触发的是 LV_EVENT_VALUE_CHANGED 事件类型
//    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
//		lv_obj_set_size(btn,55,30);
//		lv_obj_align(btn,     LV_ALIGN_LEFT_MID, 60, 0);
		
//	  lv_img_set_src(jtou_img, &jt_img);
//		//lv_obj_set_pos(my_switch_img,360/2-37,60);
//	  lv_obj_align(jtou_img, LV_ALIGN_CENTER,0,0);

}
static void find_display_handler(window_manage_t *p_window,uint8_t i)  //查找当前的初始化函数
{
    if(((void *)p_window[i].Init) != NULL)
    {
        ((window_manage_t *)&p_window[i])->Init(p_window[i].window_obj);
    }
}

static lv_mem_monitor_t   mem;
static bool my_swtich_status=false;

static void event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e);
    /*The current target is always the container as the event is added to it*/
    lv_obj_t * cont = lv_event_get_current_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t * obj = lv_event_get_target(e);
    /*If container was clicked do nothing*/
//    uint32_t *p_user_data=lv_event_get_user_data(target);
    // const char * txt = lv_btnmatrix_get_btn_text(target, id);
//    printf(" %d \r\n",*p_user_data);//LV_EVENT_CLICKED
    //printf("%d \r\n",lv_obj_has_flag(target,LV_OBJ_FLAG_EVENT_BUBBLE));
//    uint32_t* active_id = lv_event_get_user_data(e); //获取用户数据
//    lv_obj_t* cont = lv_event_get_current_target(e); // 获取当前目标事件的布局对象
//    lv_obj_t* act_cb = lv_event_get_target(e);//获取产生事件的对象
//    lv_obj_t* old_cb = lv_obj_get_child(cont, *active_id); //根据id序号获取子对象
}


void user_window_test7_init(lv_obj_t *p_title)
{
	  lv_obj_t *rotary_xyj_obj = lv_img_create(p_title);
	  lv_img_set_src(rotary_xyj_obj, &xn_gd);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(rotary_xyj_obj, LV_ALIGN_CENTER,0,0);
		lv_obj_t *xyj_lab = lv_label_create(p_title); // 创建一个标签，
		if (xyj_lab != NULL)
		{
		  lv_label_set_recolor(xyj_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(xyj_lab, &language_36, 0);
			lv_label_set_text_fmt(xyj_lab, "#ffffff Setting#");
		  lv_obj_set_pos(xyj_lab,120,130+80+90);
		}
	 static lv_style_t style;                       //创建style
   lv_style_init(&style);                         //初始化style
   lv_style_set_radius(&style,10);                 //设置样式的圆角
   lv_style_set_opa(&style,LV_OPA_60);            //设置样式背景透明度
   lv_style_set_radius(&style,10);
   lv_style_set_bg_color(&style,lv_palette_lighten(LV_PALETTE_BLUE,100));  //设置样式背景颜色

   lv_style_set_border_color(&style,lv_palette_main(LV_PALETTE_RED));        //设置样式边框颜色
   lv_style_set_border_width(&style,3);                  //设置样式边框宽度
   lv_style_set_line_width(&style,4);
   lv_style_set_border_opa(&style,LV_OPA_100);            //设置样式边框透明度
   lv_style_set_border_side(&style,LV_BORDER_SIDE_FULL); //设置样式边框显示范围
}




static void water_levelset_temp(void * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);
}

void user_window_test8_init(lv_obj_t *p_title)
{
//	  bar1 = lv_bar_create(p_title);
//    lv_obj_set_size(bar1, 200, 20);
//    lv_obj_center(bar1);
//    lv_bar_set_value(bar1, 70, LV_ANIM_OFF);
		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
		if (temp_lab != NULL)
		{
		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
			lv_label_set_text_fmt(temp_lab, "#ffffff Page8#");
		  lv_obj_set_pos(temp_lab,120,130+80+90);
		}
		
		lv_obj_t *temp_img = lv_img_create(p_title);
	  lv_img_set_src(temp_img, &xn_smart);
	  lv_obj_align(temp_img, LV_ALIGN_CENTER,0,0);
		
}

void user_window_test9_init(lv_obj_t *p_title)
{
//		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
//		if (temp_lab != NULL)
//		{
//		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
//		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
//			lv_label_set_text_fmt(temp_lab, "#ffffff Page9#");
//		  lv_obj_set_pos(temp_lab,120,130+80+90);
//		}
		g_setpage_param.set_temp=30;
    my_temp_img = lv_img_create(p_title);
	  lv_img_set_src(my_temp_img,&temperature);//temp_img设置图片源
	  lv_obj_set_pos(my_temp_img,90,90);

    my_humi_img = lv_img_create(p_title);
	  lv_img_set_src(my_humi_img,&humidity);//设置图片源
	  lv_obj_set_pos(my_humi_img,90,90+80);
      lv_obj_t *wsd_lab = lv_label_create(p_title); // 创建一个标签，
      if (wsd_lab != NULL)
      {
			lv_label_set_recolor(wsd_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(wsd_lab, &language_36, 0);
		    lv_label_set_text_fmt(wsd_lab, "#ffffff Humiture#");

			lv_obj_set_pos(wsd_lab,90+20,90+80+90);
      }
      temp_lab = lv_label_create(p_title); // 创建一个标签，
      if (temp_lab != NULL)
      {
           lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(temp_lab, &language_36, 0);
		   lv_label_set_text_fmt(temp_lab, "#ffffff %d#",g_setpage_param.set_temp);
           lv_obj_set_pos(temp_lab,90+100,90);
      }
      humi_lab = lv_label_create(p_title); // 创建一个标签，
      if (humi_lab != NULL)
      {
           lv_label_set_recolor(humi_lab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(humi_lab, &language_36, 0);
					 lv_label_set_text_fmt(humi_lab, "#ffffff %d#",g_setpage_param.set_temp);
           lv_obj_set_pos(humi_lab,90+100,90+80);
      }

    /*Create an Arc 创建圆弧*/
    lv_obj_t * g_my_arc = lv_arc_create(p_title);
    lv_obj_set_size(g_my_arc, 360, 360);
    lv_arc_set_rotation(g_my_arc, 270);
    lv_arc_set_bg_angles(g_my_arc, 0, 360);
    lv_arc_set_range(g_my_arc, 0, 100);
    lv_obj_remove_style(g_my_arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed 确保旋钮不显示*/
    lv_obj_clear_flag(g_my_arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click */
    lv_obj_center(g_my_arc);
    /*Manually update the label for the first time*/
    //lv_event_send(g_my_arc, LV_EVENT_VALUE_CHANGED, NULL);
			
    lv_arc_set_value(g_my_arc,g_setpage_param.set_temp);
    //lv_arc_set_value(g_my_arc,80);
    //动态更新圆弧中的值必须使用此方法
    lv_anim_init(&g_anim_tmp);
    lv_anim_set_var(&g_anim_tmp, g_my_arc);
    lv_anim_set_exec_cb(&g_anim_tmp, myset_arcangle_cb);//设置回调
    lv_anim_set_time(&g_anim_tmp, 500);//更新时间

}
void user_window_test10_init(lv_obj_t *p_title)
{

//		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
//		if (temp_lab != NULL)
//		{
//		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
//		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
//			lv_label_set_text_fmt(temp_lab, "#ffffff Page10#");
//		  lv_obj_set_pos(temp_lab,120,130+80+90);
//		}
		my_switch_img = lv_img_create(p_title);
	  lv_img_set_src(my_switch_img, &switch_on);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(my_switch_img, LV_ALIGN_CENTER,0,0);
    lv_obj_t *swtich_lab = lv_label_create(p_title); // 创建一个标签，
		if (swtich_lab != NULL)
		{
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_36, 0);
		    lv_label_set_text_fmt(swtich_lab, "#ffffff Switch#");

			lv_obj_set_pos(swtich_lab,140,110+80+90+20);
		}
		lv_obj_add_flag(my_switch_img,LV_OBJ_FLAG_CLICKABLE ); //增加可以点击的属性
		
}

void user_window_test11_init(lv_obj_t *p_title)
{
		lv_style_init(&g_style_indic);//初始化样式
    lv_style_set_bg_opa(&g_style_indic, LV_OPA_COVER);//不透明度百分比。
    lv_style_set_bg_color(&g_style_indic, lv_palette_main(LV_PALETTE_RED));//设置颜色
    lv_style_set_bg_grad_color(&g_style_indic, lv_palette_main(LV_PALETTE_BLUE));//设置背景色
		lv_style_set_bg_grad_dir(&g_style_indic, LV_GRAD_DIR_VER);//垂直 从上到下
		//lv_style_set_border_width(&g_style_indic,2);//设置边界
	
    g_water_level_bar = lv_bar_create(p_title);//创建一个滑杆对象
		lv_obj_add_style(g_water_level_bar, &g_style_indic, LV_PART_INDICATOR);//添加样式  LV_PART_INDICATOR为指示
    lv_obj_set_size(g_water_level_bar, 20, 200);//设置大小
    //lv_obj_center(g_water_level_bar);
		lv_obj_set_pos(g_water_level_bar,150,50);//设置坐标
    lv_bar_set_range(g_water_level_bar, 0, 100);//设置bar值范围
		
		lv_anim_init(&g_anim_water_level);//初始化一个动画对象
    lv_anim_set_exec_cb(&g_anim_water_level, water_levelset_temp);//设置刷新值回调
    lv_anim_set_time(&g_anim_water_level, 500);//设置刷新时间
    lv_anim_set_var(&g_anim_water_level, g_water_level_bar);//设置值
	  
		g_water_lab = lv_label_create(p_title); // 创建一个标签，
		if (g_water_lab != NULL)
		{
		  lv_label_set_recolor(g_water_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_water_lab, &language_36, 0);
			g_setpage_param.water_level=40;
			lv_label_set_text_fmt(g_water_lab, "#ffffff %dL#",g_setpage_param.water_level);
		  lv_obj_set_pos(g_water_lab,190,120);
		}
		lv_anim_set_values(&g_anim_water_level, 0, g_setpage_param.water_level);
    lv_anim_start(&g_anim_water_level);
		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
		if (temp_lab != NULL)
		{
		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
			lv_label_set_text_fmt(temp_lab, "#ffffff Water Level");
		  lv_obj_set_pos(temp_lab,120,130+80+90);
		}

}
void user_window_test12_init(lv_obj_t *p_title)
{
		g_worktime_lab = lv_label_create(p_title); // 创建一个标签，
		if (g_worktime_lab != NULL)
		{
		  lv_label_set_recolor(g_worktime_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_worktime_lab, &language_36, 0);
			g_setpage_param.work_time=50;
			lv_label_set_text_fmt(g_worktime_lab, "#ffffff %dmin#",g_setpage_param.work_time);
		  lv_obj_set_pos(g_worktime_lab,200,100);
		}
		lv_obj_t *temp_img = lv_img_create(p_title);
	  lv_img_set_src(temp_img, &xn_time);
		lv_obj_set_pos(temp_img,150,100);
		
	  //lv_obj_align(temp_img, LV_ALIGN_CENTER,0,0);
    lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
		if (temp_lab != NULL)
		{
				lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
				lv_obj_set_style_text_font(temp_lab, &language_36, 0);
		    lv_label_set_text_fmt(temp_lab, "#ffffff SetTime#");
				lv_obj_set_pos(temp_lab,140,120+80+90+20);
		}
		
		g_start_imgbtn = lv_img_create(p_title);
	  lv_img_set_src(g_start_imgbtn, &xn_smart);
		lv_obj_align(g_start_imgbtn, LV_ALIGN_CENTER,0,0);
		
//		g_stop_imgbtn = lv_img_create(p_title);
//	  lv_img_set_src(g_stop_imgbtn, &xn_stop);
//		lv_obj_set_pos(g_start_imgbtn,150,100);
//		lv_obj_align(g_stop_imgbtn, LV_ALIGN_CENTER,-110,0);
//		lv_obj_add_flag(my_switch_img,LV_OBJ_FLAG_CLICKABLE ); //增加可以点击的属性

}
lv_obj_t * g_colorwheel_cw;//LV_COLOR_DEPTH

void user_window_test13_init(lv_obj_t *p_title)
{
		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
	
		if (temp_lab != NULL)
		{
		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
			lv_label_set_text_fmt(temp_lab, "#ffffff LED Adjust#");
		  lv_obj_set_pos(temp_lab,120,140+80+90);
		}
		g_colorwheel_cw = lv_colorwheel_create(p_title, true);
    lv_obj_set_size(g_colorwheel_cw, 200, 200);
    lv_obj_center(g_colorwheel_cw);
    g_setpage_param.led_color.full=0x00f8;
    lv_colorwheel_set_rgb(g_colorwheel_cw,g_setpage_param.led_color);
}

void user_window_test14_init(lv_obj_t *p_title)
{	

		lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
		if (temp_lab != NULL)
		{
		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
			lv_label_set_text_fmt(temp_lab, "#ffffff Page14#");
		  lv_obj_set_pos(temp_lab,120,130+80+90);
		}
	

}
void user_window_test15_init(lv_obj_t *p_title)
{

		lv_obj_t *my_img = lv_img_create(p_title);
	  lv_img_set_src(my_img, &xn_tuos);
		//lv_obj_set_pos(my_switch_img,360/2-37,60);
	  lv_obj_align(my_img, LV_ALIGN_CENTER,0,0);
  /*Create a spinner*/
//    lv_obj_t * spinner = lv_spinner_create(p_title, 2000, 60);
//    lv_obj_set_size(spinner, 30, 30);
//    lv_obj_center(spinner);
	 	lv_obj_t *temp_lab = lv_label_create(p_title); // 创建一个标签，
		if (temp_lab != NULL)
		{
		  lv_label_set_recolor(temp_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(temp_lab, &language_36, 0);
			lv_label_set_text_fmt(temp_lab, "#ffffff Washing...#");
		  lv_obj_set_pos(temp_lab,120,140+80+90);
		}
}

static int user_guitask_func(os_event_t *param)
{
		static int8_t page_index_value=0;
	
    switch(param->event_id)
    {
        case USER_GUI_EVT_PAGE_SWITCH:		
						break;
//        case USER_GUI_EVT_BUTTON:
					  case USER_GUI_EVT_ENCODER_LEFT:
					  case USER_GUI_EVT_ENCODER_RIGHT:
					  case USER_GUI_EVT_ENCODER_SHORT_PRESS:
					  case USER_GUI_EVT_ENCODER_LONG_PRESS:
						menu_switch_item(g_menu_param.current_page_id,param->event_id);//菜单项切换
						break;
    }
    return EVT_CONSUMED;
}


 void user_guitask_init(void)
{
    title_main = lv_tileview_create(lv_scr_act());                 //创建titlewindow
    lv_obj_set_style_bg_color(title_main, lv_color_make(0x00,0x00,0x00), 0);//设置背景颜色为黑
    lv_obj_set_scrollbar_mode(title_main, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
	      lv_obj_t *swtich_lab = lv_label_create(title_main); // 创建一个标签，
      if (swtich_lab != NULL)
      {
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_72, 0);
		    lv_label_set_text_fmt(swtich_lab, "#ffffff 水位首页#");
			lv_obj_set_pos(swtich_lab,120,120+80+90);
      }
//    for(int i=WINDOW_USER1;i<WINDOW_MAX ;i++)   //创建界面
//    {
//        user_window_manger[i].window_obj = lv_tileview_add_tile(title_main, user_window_manger[i].col_id, user_window_manger[i].row_id,user_window_manger[i].dir);
//        find_display_handler(user_window_manger,i);
//    }
//    lv_obj_set_tile_id(title_main,user_window_manger[WINDOW_USER4].col_id, user_window_manger[WINDOW_USER4].row_id,LV_ANIM_OFF);
    //lv_obj_set_tile_id(title_main,window_manger[WINDOW_SCROLL_MENU].col_id, window_manger[WINDOW_SCROLL_MENU].row_id,LV_ANIM_OFF);
    lv_mem_monitor(&mem);
    printf("--------------------mem heap size=%d, free_size=%d,used=%d\r\n",mem.total_size,mem.free_size,mem.total_size - mem.free_size);

//		lv_timer_t * second_timer;
//		lv_timer_t * timer;
//    second_timer = lv_timer_create(my_second_timer, 1000,  NULL);
//    //timer = lv_timer_create(my_timer1,1000,  NULL);
//	  //lv_timer_ready(timer);
//	  lv_timer_ready(second_timer);
//		encoder_init();
//    user_guitask_id = os_task_create(user_guitask_func);
}


static void switch_status_btn_event_cb(lv_event_t * event)
{
        printf("%s\r\n status:%d",__FUNCTION__,my_swtich_status);
		//判断当前的对象
        lv_obj_t * obj = lv_event_get_target(event);
        if(my_swtich_status==false)
				{
								lv_img_set_src(obj ,&switch_on);						//设置背景图片
								my_swtich_status=true;
				}else
				{
								lv_img_set_src(obj ,&switch_off);						//设置背景图片
								my_swtich_status=false;
				}
}
#endif