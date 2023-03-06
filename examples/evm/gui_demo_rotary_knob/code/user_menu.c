#include "user_menu.h"
#include "lvgl.h"
#include "os_timer.h"
#include "lv_color.h"


 LV_IMG_DECLARE(bg);
 LV_IMG_DECLARE(bg1);
 
 LV_IMG_DECLARE(second);
 LV_IMG_DECLARE(minute);
 LV_IMG_DECLARE(hour);
 

 
 

LV_IMG_DECLARE(menu_slide_Big_BP);
LV_IMG_DECLARE(menu_slide_Big_Breathe);
LV_IMG_DECLARE(menu_slide_Big_Calculator);
LV_IMG_DECLARE(menu_slide_Big_camera);
LV_IMG_DECLARE(menu_slide_Big_hr);
LV_IMG_DECLARE(menu_slide_Big_music);
LV_IMG_DECLARE(menu_slide_Big_Phone);
 
 
 
 
static  lv_obj_t * hour_img ;
static	lv_obj_t * minute_img;
static   lv_obj_t * second_img;


static  lv_obj_t *  BP_img ;
static	lv_obj_t *  Breathe_img;
static  lv_obj_t *  Calculator_img;
static  lv_obj_t *  camera_img ;
static  lv_obj_t *  phone_img ;



 
os_timer_t timer_count;
 
lv_timer_t * timer;
uint32_t angle=0,

minute_angle=0,
hour_angle=0
;
 
void  handler()
{

	lv_img_set_angle(second_img,angle);

	
	if(angle>=3600)
	{
		angle=0;
		minute_angle+=60;
		lv_img_set_angle(minute_img,minute_angle);  
	}

	angle+=60;
}
 
 


 


void my_timer(lv_timer_t * timer)
{
	
	lv_img_set_angle(second_img,angle);
	angle+=36;
	
	if(angle>=3600)
	{
		angle=0;
		minute_angle+=50;
		lv_img_set_angle(minute_img,minute_angle);  
	}
	
	
  /*Use the user_data*/
  //printf("my_timer called with user data\n");

}













//C:\windows\System32\cmd.exe

//C:\Program Files\Git\git-bash.exe






#if 0
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected month: %s\n", buf);
    }
}
#endif
/**
 * An infinite roller with the name of the months
 */





static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
}

void lv_example_win_1(void)
{
	
#if 1
    lv_obj_t * win = lv_win_create(lv_scr_act(), 40);
    lv_obj_t * btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_win_add_title(win, "A title");

    btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 60);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be aded here*/
    lv_obj_t * label =  lv_label_create(cont);
    lv_label_set_text(label, "This is\n"
                             "a pretty\n"
                             "long text\n"
                             "to see how\n"
                             "the window\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "Some more\n"
                             "text to be\n"
                             "sure it\n"
                             "overflows. :)");

#endif



    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 0);
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    /*Add content to the tabs*/


    lv_obj_t * img1 = lv_img_create(tab1);
    lv_img_set_src(img1, &bg);
	   
	  hour_img = lv_img_create(tab1);
    lv_img_set_src(hour_img, &hour);
		lv_obj_set_pos(hour_img,360/2-13,100);
	
	
	
	
		minute_img = lv_img_create(tab1);
    lv_img_set_src(minute_img, &minute);
		lv_obj_set_pos(minute_img,360/2-10,60);

    second_img = lv_img_create(tab1);
    lv_img_set_src(second_img, &second);
	 	lv_obj_set_pos(second_img,360/2-10,60);

    //lv_obj_align(second_img, LV_ALIGN_CENTER, 0, 30);


lv_img_set_pivot(second_img, 9, 110);

lv_img_set_pivot(minute_img, 10, 110);

lv_img_set_pivot(hour_img, 10, 67);
	
	
	lv_tabview_set_act(tab1,0,LV_ANIM_OFF);
	
os_timer_init(&timer_count,handler,NULL);
os_timer_start(&timer_count,500,true);


























		#if 0
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "This the first tab\n\n"
                             "If the content\n"
                             "of a tab\n"
                             "becomes too\n"
                             "longer\n"
                             "than the\n"
                             "container\n"
                             "then it\n"
                             "automatically\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "\n"
                             "Can you see it?");

    label = lv_label_create(tab2);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3);
    lv_label_set_text(label, "Third tab");



    lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
#endif
}




















void lv_example_roller_1(void)
{

#if 1
	  //timer=lv_timer_create(my_timer, 5,NULL);
	
	
	
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &bg);
	   
	  hour_img = lv_img_create(lv_scr_act());
    lv_img_set_src(hour_img, &hour);
		lv_obj_set_pos(hour_img,360/2-13,100);
	
	
	
	
		minute_img = lv_img_create(lv_scr_act());
    lv_img_set_src(minute_img, &minute);
		lv_obj_set_pos(minute_img,360/2-10,60);

    second_img = lv_img_create(lv_scr_act());
    lv_img_set_src(second_img, &second);
	 	lv_obj_set_pos(second_img,360/2-10,60);

    //lv_obj_align(second_img, LV_ALIGN_CENTER, 0, 30);


lv_img_set_pivot(second_img, 9, 110);

lv_img_set_pivot(minute_img, 10, 110);

lv_img_set_pivot(hour_img, 10, 67);
	
	
	
	
os_timer_init(&timer_count,handler,NULL);
os_timer_start(&timer_count,1000,true);

#endif


}



void lv_example_tileview_1(void)
{
	
     lv_obj_t *tv = lv_tileview_create(lv_scr_act());       //创建titlewindow
     lv_obj_set_style_bg_color(tv, lv_color_make(0,0,0), 0);//设置背景颜色为黑色
	   lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显示
	
	
    /*Tile1: just a label*/
     lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM|LV_DIR_TOP);
    //lv_obj_t * label = lv_label_create(tile1);
	
	
	   lv_obj_t * img1 = lv_img_create(tile1);
    //lv_img_set_src(img1, &bg);
	   
	
	
	

	
	
	
		BP_img = lv_img_create(img1);
    lv_img_set_src(BP_img, &menu_slide_Big_BP);
		lv_obj_set_pos(BP_img,20,100);
		
		
		Breathe_img = lv_img_create(img1);
    lv_img_set_src(Breathe_img, &menu_slide_Big_hr);
		lv_obj_set_pos(Breathe_img,100,180);
		
		
		
		Calculator_img = lv_img_create(img1);
    lv_img_set_src(Calculator_img, &menu_slide_Big_Calculator);
		lv_obj_set_pos(Calculator_img,180,100);
		
		
	
	  camera_img = lv_img_create(img1);
    lv_img_set_src(camera_img, &menu_slide_Big_music);
		lv_obj_set_pos(camera_img,260,180);
	
	
	phone_img		=	lv_img_create(img1);
  lv_img_set_src(phone_img, &menu_slide_Big_Phone);
	lv_obj_set_pos(phone_img,260,100);


	
	 //lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_PINK), 0);
	
	
	
	
//	  hour_img = lv_img_create(img1);
//    lv_img_set_src(hour_img, &hour);
//		lv_obj_set_pos(hour_img,360/2-13,100);
//	
//	
//	
//	
//		minute_img = lv_img_create(img1);
//    lv_img_set_src(minute_img, &minute);
//		lv_obj_set_pos(minute_img,360/2-10,60);

//    second_img = lv_img_create(img1);
//    lv_img_set_src(second_img, &second);
//	 	lv_obj_set_pos(second_img,360/2-10,60);

//    //lv_obj_align(second_img, LV_ALIGN_CENTER, 0, 30);


//lv_img_set_pivot(second_img, 9, 110);

//lv_img_set_pivot(minute_img, 10, 110);

//lv_img_set_pivot(hour_img, 10, 67);
	
	
	
	
os_timer_init(&timer_count,handler,NULL);
os_timer_start(&timer_count,1000,true);
	
	
	
    //lv_label_set_text(label, "Scroll down");
    //lv_obj_center(label);  //设置中心区域


    /*Tile2: a button*/
		
		lv_obj_t * label = lv_label_create(tile1);
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);


    //lv_disp_set_bg_image(,bg1);

	  lv_obj_t * img2 = lv_img_create(tile2);
    lv_img_set_src(img2, &bg1);
	   
	  hour_img = lv_img_create(img2);
    lv_img_set_src(hour_img, &hour);
		lv_obj_set_pos(hour_img,360/2-13,100);
	
	
	
	
		minute_img = lv_img_create(img2);
    lv_img_set_src(minute_img, &minute);
		lv_obj_set_pos(minute_img,360/2-10,60);

    second_img = lv_img_create(img2);
    lv_img_set_src(second_img, &second);
	 	lv_obj_set_pos(second_img,360/2-10,60);

    //lv_obj_align(second_img, LV_ALIGN_CENTER, 0, 30);


lv_img_set_pivot(second_img, 9, 110);

lv_img_set_pivot(minute_img, 10, 110);

lv_img_set_pivot(hour_img, 10, 67);









//    lv_obj_t * btn = lv_btn_create(tile2);

//    label = lv_label_create(btn);
//    lv_label_set_text(label, "Scroll up or right");

//    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
//    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 =  lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
    lv_obj_t * list = lv_list_create(tile3);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

    lv_list_add_btn(list, NULL, "One");
    lv_list_add_btn(list, NULL, "Two");
    lv_list_add_btn(list, NULL, "Three");
    lv_list_add_btn(list, NULL, "Four");
    lv_list_add_btn(list, NULL, "Five");
    lv_list_add_btn(list, NULL, "Six");
    lv_list_add_btn(list, NULL, "Seven");
    lv_list_add_btn(list, NULL, "Eight");
    lv_list_add_btn(list, NULL, "Nine");
    lv_list_add_btn(list, NULL, "Ten");

}

void  main_init()
{
     lv_obj_t *tv = lv_tileview_create(lv_scr_act());      //创建
	   lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF); //去掉滚动条
	
	
    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0,   LV_DIR_BOTTOM  );  //配置可以下拉
    lv_obj_t * label = lv_label_create(tile1);
    lv_label_set_text(label, "Scroll down");
    lv_obj_center(label);  //设置中心区域

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

    /*Tile2: a button*/
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);

    lv_obj_t * btn = lv_btn_create(tile2);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Scroll up or right");

    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 =  lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
    lv_obj_t * list = lv_list_create(tile3);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

    lv_list_add_btn(list, NULL, "One");
    lv_list_add_btn(list, NULL, "Two");
    lv_list_add_btn(list, NULL, "Three");
    lv_list_add_btn(list, NULL, "Four");
    lv_list_add_btn(list, NULL, "Five");
    lv_list_add_btn(list, NULL, "Six");
    lv_list_add_btn(list, NULL, "Seven");
    lv_list_add_btn(list, NULL, "Eight");
    lv_list_add_btn(list, NULL, "Nine");
    lv_list_add_btn(list, NULL, "Ten");



}


