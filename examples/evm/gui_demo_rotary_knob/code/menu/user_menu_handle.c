/************************************************************
  * @brief   自定义菜单的ui界面处理
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @note    user_menu_handle.c
	* @version v1.0
  ***********************************************************/
#include "user_menu_handle.h"
#include "stdio.h"
#include "../lvgl/lvgl.h"
#include "user_main.h"
#include "picture_define.h"
#include "os_timer.h"

 
//绑定主界面的图片资源 
const main_page_image_t main_page_image[]={
[0]={.image_src=(void *)&xn_shouye},
[1]={.image_src=(void *)&xn_temppage},
[2]={.image_src=(void *)&xn_pwoer},
[3]={.image_src=(void *)&xn_sw},
[4]={.image_src=(void *)&xn_xyj},
[5]={.image_src=(void *)&xn_led},
[6]={.image_src=(void *)&xn_gd},
[7]={.image_src=(void *)&xn_xyj},
};
/*********************
 *      DEFINES
 *********************/

/*以下是lvgl相关界面控件*/
static lv_obj_t *g_title_main=NULL; //定义全局的titleview对象
static lv_obj_t *g_temp_image=NULL;
static lv_obj_t *g_humi_image=NULL;
static lv_obj_t *g_switch_image=NULL;
static lv_obj_t *g_humi_textlab=NULL;
static lv_obj_t *g_temp_textlab=NULL;
static lv_obj_t *g_arc_handle=NULL;
static lv_anim_t g_temp_anim;//动态更新圆弧用
static lv_anim_t g_water_level_anim;

static lv_obj_t 	*g_water_level_bar;
static lv_style_t  g_style_indic;
static lv_obj_t 	*g_water_lab;
static lv_obj_t 	*g_worktime_lab;

static lv_obj_t *g_start_image;
//static lv_obj_t *g_stop_image;

static uint16_t 	user_menu_task_id=0;
static lv_obj_t * g_colorwheel_ledadj;//LV_COLOR_DEPTH

static lv_obj_t * main_page_img;
static lv_obj_t * main_page_lab;

static lv_mem_monitor_t   mem;//监视lvgl内存使用情况

/*以下实现设置界面相关控件对象*/
static lv_obj_t* g_set_title_main; // 
static lv_obj_t* g_set_block_obj1;

static lv_obj_t* g_main_block_obj1;
static lv_obj_t* g_main_block_obj2;
static lv_obj_t* g_main_block_obj3;
static lv_obj_t* g_main_block_obj4;
static lv_obj_t* g_main_block_led_img;
static lv_obj_t* g_main_block_curtain_img;
static lv_obj_t* g_main_block_control_img;
static lv_obj_t* g_main_block_ledadj_img;
static lv_obj_t* g_main_swtich_lab;
static lv_obj_t* g_main_curtain_lab;
static lv_obj_t* g_main_control_lab;
static lv_style_t g_mainpage_style_base;
static lv_obj_t* g_img_btn[20];
static lv_obj_t* g_scroll_cont=NULL;
static lv_obj_t* g_work_page_obj;
static lv_obj_t* g_animing_work;
static lv_obj_t* g_start_page_obj ;
static lv_obj_t* g_animing_start;

////全局的时间结构体 
//static Time_t My_time=
//{
//	.year=2022,
//	.month=1,
//	.day=7,
//	.hour=19,
//	.minute=52,
//	.second=37,
//};

//界面管理表 
window_manage_t user_menu_window_manage[]=
{
  [WINDOW_MAIN_PAGE]={.window_obj=NULL, .col_id=0,.row_id=1,.dir=LV_DIR_ALL,.Init=user_window_main_init},           							//下拉菜单
	[WINDOW_PAGE0]={.window_obj=NULL, .col_id=1,.row_id=1,.dir=LV_DIR_ALL,.Init=user_window_page0_init},
	[WINDOW_PAGE1]={.window_obj=NULL, .col_id=2,.row_id=1,.dir=LV_DIR_ALL,.Init=user_window_page1_init},//主界靿
	[WINDOW_PAGE2]={.window_obj=NULL, .col_id=3,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page2_init},
	[WINDOW_PAGE3]={.window_obj=NULL, .col_id=4,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page3_init},
	[WINDOW_PAGE4]={.window_obj=NULL, .col_id=5,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page4_init},
	[WINDOW_PAGE5]={.window_obj=NULL, .col_id=6,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page5_init},
	[WINDOW_PAGE6]={.window_obj=NULL, .col_id=7,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page6_init},
	[WINDOW_PAGE7]={.window_obj=NULL, .col_id=8,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page7_init},
	[WINDOW_PAGE8]={.window_obj=NULL, .col_id=9,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page8_init},
	[WINDOW_PAGE9]={.window_obj=NULL, .col_id=10,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page9_init},
	
//	[WINDOW_PAGE10]={.window_obj=NULL, .col_id=10,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page10_init},
//	[WINDOW_PAGE11]={.window_obj=NULL, .col_id=11,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page11_init},
//	[WINDOW_PAGE12]={.window_obj=NULL, .col_id=12,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page12_init},
//	[WINDOW_PAGE13]={.window_obj=NULL, .col_id=13,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page13_init},
//	[WINDOW_PAGE14]={.window_obj=NULL, .col_id=14,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_page14_init},
	[WINDOW_WORK]={.window_obj=NULL, .col_id=11,.row_id=1,.dir=LV_DIR_HOR,.Init=user_window_work_init},
	
};

/************************************************************
  * @brief   进入相应的页面 二级界面进行切图
  * @param   id_index 界面的索引值  KeyCode 具体操作按键码
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void enter_page_handle(menu_i32 index,menu_u8 KeyCode)
{
	#ifdef MENU_DEBUG_EN
	printf("current_page_id:%d\r\n",index);
	#endif
	g_menu_param.current_page_id = index ;
	switch(g_menu_param.current_page_id)
	{
		case	MAIN_PAGE 	  ://主界面
					//main_menupage_init();
					lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_MAIN_PAGE].col_id, user_menu_window_manage[WINDOW_MAIN_PAGE].row_id,LV_ANIM_OFF);
					main_page_ui_process(g_menu_param.last_page_index); //返回主页面进行切图
			    break ;
		//进入子界面的相关ui处理函数
		case 	USER_PAGE0		:
					user_page0_ui_process(current_seting_param.page0_index_val);
				  break;
		case	USER_PAGE1	  :
					user_page1_ui_process(current_seting_param.page1_index_val);
			    break ;
			    
		case  USER_PAGE2	  :
					user_page2_ui_process(current_seting_param.page2_index_val);
			    break ;
		
		case	USER_PAGE3   :
					user_page3_ui_process(current_seting_param.page3_index_val);
			    break ;
			    

		case	USER_PAGE4	  :
					user_page4_ui_process(current_seting_param.page4_index_val);
			    break ;
			    
		case	USER_PAGE5	  :
					 user_page5_ui_process(current_seting_param.page5_index_val);
			    break ;
			    
		case	USER_PAGE6 :
					user_page6_ui_process(current_seting_param.page6_index_val);
					break ;
			
		case	USER_PAGE7:	
					user_page7_ui_process(current_seting_param.page7_index_val);
					break ;
		
		case	WORK_PAGE:	
					user_work_ui_process(current_seting_param.page7_index_val);
					break ;
		default:
				  break ;
	}
}


/************************************************************
  * @brief   主菜单初始化
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void main_menupage_init(void)
{
	g_menu_param.main_current_index = 0 ;
	g_menu_param.current_page_id = MAIN_PAGE ;
	//显示主页面的UI和状态
}


/************************************************************
	* @brief    主界面UI显示处理
  * @param   index 索引值具体选择哪一项
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void main_page_ui_process(menu_u8 index)
{
	#ifdef MENU_DEBUG_EN
	printf("main_page:%d\r\n",index);
	#endif
	switch(index) //进行切图
	{
		case 0:
				  
					lv_img_set_src(main_page_img,main_page_image[0].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 首页#");
					lv_obj_set_pos(main_page_lab,120,120+80+90+10);
					 
				break ;
		case 1:
					lv_img_set_src(main_page_img,main_page_image[1].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 温控#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		case 2:
					lv_img_set_src(main_page_img,main_page_image[2].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 电源#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		case 3:
					lv_img_set_src(main_page_img,main_page_image[3].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 水位#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		case 4:
					lv_img_set_src(main_page_img,main_page_image[4].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 洗衣#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		case 5:
					lv_img_set_src(main_page_img,main_page_image[5].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 灯光#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		case 6:
					lv_img_set_src(main_page_img,main_page_image[6].image_src);//设置图片源
					lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
					lv_label_set_text_fmt(main_page_lab, "#ffffff 设置#");
					lv_obj_set_pos(main_page_lab,120,120+80+90);
				break ;
		default :
				break ;
	}
}
/************************************************************
	* @brief   子界面UI显示处理 目前只是切图 没有实现参数调整
  * @param   index 索引值具体选择哪一项
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0 
  ***********************************************************/
void user_page0_ui_process(menu_u8 index)
{
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE0].window_obj)) //判断是否为当前界面
		{
					#ifdef MENU_DEBUG_EN
					printf("switch_page:%d\r\n",index);
					#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE0].col_id, user_menu_window_manage[WINDOW_PAGE0].row_id,LV_ANIM_OFF);
		}
		switch(index)
		{
			case 0:
					
					break ;
			
			case 1:
					
					break ;
			
			case 2:
					
					break ;
			
			case 3:
					
					break ;
			
			case 4:
					
					break ;
			default:
					break ;
		}
}

/************************************************************
 * @brief    子界面UI显示处理
  * @param   index 索引值具体选择哪一项
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void user_page1_ui_process(menu_u8 index)
{
		static uint8_t last_set_temp = 30;
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE1].window_obj)) //判断是否为当前界面
		{
				#ifdef MENU_DEBUG_EN
				printf("switch_page:%d\r\n",index);
				#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE1].col_id, user_menu_window_manage[WINDOW_PAGE1].row_id,LV_ANIM_OFF);
		}
		lv_label_set_text_fmt(g_temp_textlab, "#ffffff %d#",g_menu_param.set_temp);
		lv_label_set_text_fmt(g_humi_textlab, "#ffffff %d%%#",g_menu_param.set_temp);
		lv_anim_set_values(&g_temp_anim, last_set_temp, g_menu_param.set_temp);
		lv_anim_start(&g_temp_anim);
		last_set_temp=g_menu_param.set_temp;
}

//user_page1_ui_process 处理
void user_page2_ui_process(menu_u8 index)
{
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE2].window_obj)) //判断是否为当前界面
		{
				#ifdef MENU_DEBUG_EN
				printf("switch_page:%d\r\n",index);
				#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE2].col_id, user_menu_window_manage[WINDOW_PAGE2].row_id,LV_ANIM_OFF);
				//return ;
		}
		if(index)
		{
			lv_img_set_src(g_switch_image, &switch_off);
		}
		else
		{
				lv_img_set_src(g_switch_image, &switch_on);
		}
		switch(index)
		{
			case 0:
					
					break ;
			
			case 1:
					
					break ;
			
			case 2:
					
					break ;
			
			case 3:
					
					break ;
			
			case 4:
					
					break ;
			default:
					break ;
		}
}

void user_page3_ui_process(menu_u8 index)
{
		static uint8_t last_water_level = 40;
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE3].window_obj)) //判断是否为当前界面
		{
				#ifdef MENU_DEBUG_EN
				printf("switch_page:%d\r\n",index);
				#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE3].col_id, user_menu_window_manage[WINDOW_PAGE3].row_id,LV_ANIM_OFF);
		}
		lv_label_set_text_fmt(g_water_lab, "#ffffff %dL#",g_menu_param.water_level);
		lv_obj_set_pos(g_water_lab,190,120);
		lv_anim_set_values(&g_water_level_anim, last_water_level, g_menu_param.water_level);
		lv_anim_start(&g_water_level_anim);
		last_water_level=g_menu_param.water_level;
}

//设置ui处理
void user_page4_ui_process(menu_u8 index)
{
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE4].window_obj)) //判断是否为当前界面
		{
				#ifdef MENU_DEBUG_EN
				printf("switch_page:%d\r\n",index);
				#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE4].col_id, user_menu_window_manage[WINDOW_PAGE4].row_id,LV_ANIM_OFF);
		}
		lv_label_set_text_fmt(g_worktime_lab, "#ffffff %dmin#",g_menu_param.work_time);
		switch(index)
		{
			case 0:
					
					break ;
			
			case 1:
					
					break ;
			
			case 2:
					
					break ;
			
			case 3:
					
					break ;
			
			case 4:
					
					break ;
			default:
					break ;
		}
}

//上传页面ui处理
void user_page5_ui_process(menu_u8 index)
{
		
		if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE5].window_obj)) //判断是否为当前界面
		{
				#ifdef MENU_DEBUG_EN
				printf("switch_page:%d\r\n",index);
				#endif
				lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE5].col_id, user_menu_window_manage[WINDOW_PAGE5].row_id,LV_ANIM_OFF);
		}
		lv_colorwheel_set_rgb(g_colorwheel_ledadj,g_menu_param.led_color);	 
		switch(index)
		{
			case 0:
					
					break ;
			case 1:
					
					break ;
			default:
				break ;
		}
}

//语言页面ui处理
void user_page6_ui_process(menu_u8 index)
{
	if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_PAGE6].window_obj)) //判断是否为当前界面
	{
			#ifdef MENU_DEBUG_EN
			printf("switch_page:%d\r\n",index);
			#endif
			lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_PAGE6].col_id, user_menu_window_manage[WINDOW_PAGE6].row_id,LV_ANIM_OFF);
	}
	seting_page_backdrop_recolor(g_menu_param.select_item);
	switch(index)//调整参数
	{
		case 0:
					
		      break ;
		case 1:
					
		      break ;
		default:
					break ;
	}
}

//子页面ui处理
void user_page7_ui_process(menu_u8 index)
{
	switch(index)
	{
		case 0:
		  	 
		     break ;
		case 1:
				 
		     break ;
		case 2:
				
		     break ;
		default:
					break ;
	}
}
void user_work_ui_process(menu_u8 index)
{
	if(1 != judge_current_tileview(g_title_main,user_menu_window_manage[WINDOW_WORK].window_obj)) //判断是否为当前界面
	{
			#ifdef MENU_DEBUG_EN
			printf("switch_page:%d\r\n",index);
			#endif
		  //lv_scr_load(g_start_page_obj);//直接切换过去 不延迟 
			//lv_animimg_start(g_animing_work);
			lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_WORK].col_id, user_menu_window_manage[WINDOW_WORK].row_id,LV_ANIM_OFF);
		  
	}
	
	switch(index)
	{
		case 0:
		  	 
		     break ;
		case 1:
				 
		     break ;
		case 2:
				
		     break ;
		default:
					break ;
	}
}

static void find_display_handler(window_manage_t *p_window,uint8_t i)  //查找当前的初始化函数
{
    if(((void *)p_window[i].Init) != NULL)
    {
        ((window_manage_t *)&p_window[i])->Init(p_window[i].window_obj);
    }
}
 
/************************************************************
 * @brief    菜单任务处理回调函数
  * @param   param 具体事件值
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
static int user_menu_task_func(os_event_t *param) 
{
		static int8_t page_index_value=0;
    switch(param->event_id)
    {
        case USER_GUI_EVT_PAGE_SWITCH:		
						break;
  
					  case USER_GUI_EVT_ENCODER_LEFT:
					  case USER_GUI_EVT_ENCODER_RIGHT:
					  case USER_GUI_EVT_ENCODER_SHORT_PRESS:
					  case USER_GUI_EVT_ENCODER_LONG_PRESS:
						menu_switch_item(g_menu_param.current_page_id,param->event_id);//菜单项切换
						break;
						
						
    }
    return EVT_CONSUMED;
}

/************************************************************
  * @brief    主界面初始化
  * @param   p_title 界面对象
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void user_window_main_init(lv_obj_t *p_title)
{
  main_page_img = lv_img_create(p_title);
	//lv_obj_set_size(main_page_img,xn_smart.header.w,xn_smart.header.h);
	lv_img_set_src(main_page_img,&xn_shouye);//设置图片源
	lv_obj_align(main_page_img, LV_ALIGN_CENTER,0,0);
	main_page_lab = lv_label_create(p_title); // 创建一个标签，
	 if (main_page_lab != NULL)
	 {
			lv_label_set_recolor(main_page_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(main_page_lab, &language_72, 0);
			lv_label_set_text_fmt(main_page_lab, "#ffffff 首页#");
		 lv_obj_set_pos(main_page_lab,120,120+80+90+10);
			//lv_obj_align(main_page_lab, LV_ALIGN_BOTTOM_MID,0,0);
	 }
	
}

//设置圆弧值
static void seting_arcangle_cb(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}


/**
* 事件回调函数
*/
static void scroll_event_cb(lv_event_t* e)
{
    //>>>>>>>>>>>-----------------<!>---Link
    ///E1.----------------------------得到容器
    lv_obj_t* cont = lv_event_get_target(e); //通过绑定了该事件的对象，来获取这个对象
    ///E2.---------------------------通过一定算法，得到容器的y轴中心位置（目的：用于后面与子元素按钮进行y轴中心偏差比较）
    lv_area_t cont_a; //区域 cont_area
    lv_obj_get_coords(cont, &cont_a); //将cont的坐标赋值给cont_a (将cont_a约束为container的大小（200 * 200)  coords：坐标(x1,y1); (x2,y1); (x1,y2); (x2,y2)
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2; //获取Container的y轴中心
    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10; // 200*7 / 10 = 140
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont); //child_cnt : child count 儿子数量 获取container里面元素个数
    ///E3.------------------遍历容器里面的子元素（按钮），以便操作这些元素的属性（用户垂直滚动时候，按钮发生水平偏移，并且设置不同的透明度）
    for (i = 0; i < child_cnt; i++)  //遍历Buttons
    {
        lv_obj_t* child = lv_obj_get_child(cont, i); //获取container的第i个button
        lv_area_t child_a; //创建一个属于儿子区域
        lv_obj_get_coords(child, &child_a); //将child_a约束为button的大小（200 * 200）
        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2; //计算得到button的y轴中心

        lv_coord_t diff_y = child_y_center - cont_y_center; // button的y轴中心 - Container的y轴中心 = Button相对于Container的垂直偏差
        diff_y = LV_ABS(diff_y); // 对偏差取绝对值（ABS）

        ///E3.1------------------根据偏差（按钮相对于容器的y方向中心位置）来产生不同的x值
        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if (diff_y >= r) // diff_y >= 140
        {
            x = r; //x = 140
        }
        else     // diff_y < 140
        {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y; // 140 * 140  - diff_y的平方
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }
        ///E3.2--------------------根据x值，将button移动x个单位距离，根据r映射出不同的透明度值，设置按钮不同透明度
        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0); //将button 移动 x个单位距离

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);  //通过r的不同值，动态映射创建不透明度值 opa: opacity
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0); //给按钮应用不透明度值  opa: opacity
    }
}
static void status_btn_event_cb(lv_event_t* event)
{
    //判断当前的对象

    uint32_t *user_dat = lv_event_get_user_data(event);
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);//获取目标对象
    if (code == LV_EVENT_CLICKED) {
//        printf("Clicked: %s", lv_list_get_btn_text(scroll_cont, obj));
        lv_obj_t* parent = lv_obj_get_parent(obj);//获取父对象
        uint32_t i;
        for (i = 0; i < lv_obj_get_child_cnt(parent); i++) { //获取父对象总共有多少个子对象
            lv_obj_t* child = lv_obj_get_child(parent, i); //获取当前子对象
            if (child == obj) { //判断当前按下的子对象
                printf("imgbtn :%d\r\n",i);
               // lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
              //  lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

#if 0
void user_window_test2_init(lv_obj_t *p_title)
{

    g_temp_image = lv_img_create(p_title);
	  lv_img_set_src(g_temp_image,&temperature);//temp_img设置图片源
	  lv_obj_set_pos(g_temp_image,90,90);


    g_humi_image = lv_img_create(p_title);
	  lv_img_set_src(g_humi_image,&humidity);//设置图片源
	  lv_obj_set_pos(g_humi_image,90,90+80);

      lv_obj_t *wsd_lab = lv_label_create(p_title); // 创建一个标签，
      if (wsd_lab != NULL)
      {
			lv_label_set_recolor(wsd_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(wsd_lab, &language_36, 0);
		    lv_label_set_text_fmt(wsd_lab, "#ffffff Humiture#");

			lv_obj_set_pos(wsd_lab,90+20,90+80+90);
      }
      g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
      if (g_temp_textlab != NULL)
      {
           lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
		   lv_label_set_text_fmt(g_temp_textlab, "#ffffff %d#",tempval);
           lv_obj_set_pos(g_temp_textlab,90+100,90);
      }
      g_humi_textlab = lv_label_create(p_title); // 创建一个标签，
      if (g_humi_textlab != NULL)
      {
           lv_label_set_recolor(g_humi_textlab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(g_humi_textlab, &language_36, 0);
					 lv_label_set_text_fmt(g_humi_textlab, "#ffffff %d#",humival);
           lv_obj_set_pos(g_humi_textlab,90+100,90+80);
      }

    /*Create an Arc 创建圆弧*/
    lv_obj_t * g_arc_handle = lv_arc_create(p_title);
    lv_obj_set_size(g_arc_handle, 360, 360);
    lv_arc_set_rotation(g_arc_handle, 270);
    lv_arc_set_bg_angles(g_arc_handle, 0, 360);
    lv_arc_set_range(g_arc_handle, 0, 100);
    lv_obj_remove_style(g_arc_handle, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed 确保旋钮不显示*/
    lv_obj_clear_flag(g_arc_handle, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click */
    lv_obj_center(g_arc_handle);
    /*Manually update the label for the first time*/
    //lv_event_send(g_arc_handle, LV_EVENT_VALUE_CHANGED, NULL);
    lv_arc_set_value(g_arc_handle,50);
    //lv_arc_set_value(g_arc_handle,80);
    //动态更新圆弧中的值必须使用此方法
    lv_anim_init(&g_temp_anim);
    lv_anim_set_var(&g_temp_anim, g_arc_handle);
    lv_anim_set_exec_cb(&g_temp_anim, seting_arcangle_cb);//设置回调
    lv_anim_set_time(&g_temp_anim, 500);//更新时间
//    lv_anim_set_repeat_count(&g_temp_anim, LV_ANIM_REPEAT_INFINITE);    /*Just for the demo*/
//    lv_anim_set_repeat_delay(&g_temp_anim, 500);
//    lv_anim_start(&g_temp_anim);//开始刷新


}
#endif

/************************************************************
  * @brief    子界面初始化
  * @param   p_title 界面对象
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void user_window_page0_init(lv_obj_t *p_title)
{
//		lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
//		if (g_temp_textlab != NULL)
//		{
//		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
//		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
//			lv_label_set_text_fmt(g_temp_textlab, "#ffffff Page0#");
//		  lv_obj_set_pos(g_temp_textlab,120,130+80+90);
//		}
	//xn_group_map
	lv_obj_t*  page0_img = lv_img_create(p_title);
	//lv_obj_set_size(main_page_img,xn_smart.header.w,xn_smart.header.h);
	lv_img_set_src(page0_img,&xn_group);//设置图片源
	lv_obj_align(page0_img, LV_ALIGN_CENTER,0,0);
 
}
void user_window_page1_init(lv_obj_t *p_title)
{
		g_menu_param.set_temp=30;
    g_temp_image = lv_img_create(p_title);
	  lv_img_set_src(g_temp_image,&temperature);//temp_img设置图片源
	  lv_obj_set_pos(g_temp_image,90,90);

    g_humi_image = lv_img_create(p_title);
	  lv_img_set_src(g_humi_image,&humidity);//设置图片源
	  lv_obj_set_pos(g_humi_image,90,90+80);
      lv_obj_t *wsd_lab = lv_label_create(p_title); // 创建一个标签，
      if (wsd_lab != NULL)
      {
				lv_label_set_recolor(wsd_lab, true);                        /*Enable re-coloring by commands in the text*/
				lv_obj_set_style_text_font(wsd_lab, &language_36, 0);
		    lv_label_set_text_fmt(wsd_lab, "#ffffff Humiture#");

				lv_obj_set_pos(wsd_lab,90+20,90+80+90);
      }
      g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
      if (g_temp_textlab != NULL)
      {
           lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
					 lv_label_set_text_fmt(g_temp_textlab, "#ffffff %d#",g_menu_param.set_temp);
           lv_obj_set_pos(g_temp_textlab,90+100,90);
      }
      g_humi_textlab = lv_label_create(p_title); // 创建一个标签，
      if (g_humi_textlab != NULL)
      {
           lv_label_set_recolor(g_humi_textlab, true);                        /*Enable re-coloring by commands in the text*/
           lv_obj_set_style_text_font(g_humi_textlab, &language_36, 0);
					 lv_label_set_text_fmt(g_humi_textlab, "#ffffff %d#",g_menu_param.set_temp);
           lv_obj_set_pos(g_humi_textlab,90+100,90+80);
      }

    /*Create an Arc 创建圆弧*/
    lv_obj_t * g_arc_handle = lv_arc_create(p_title);
    lv_obj_set_size(g_arc_handle, 400, 400);
    lv_arc_set_rotation(g_arc_handle, 270);
    lv_arc_set_bg_angles(g_arc_handle, 0, 360);
    lv_arc_set_range(g_arc_handle, 0, 100);
    lv_obj_remove_style(g_arc_handle, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed 确保旋钮不显示*/
    lv_obj_clear_flag(g_arc_handle, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click */
    lv_obj_center(g_arc_handle);
    /*Manually update the label for the first time*/
    //lv_event_send(g_arc_handle, LV_EVENT_VALUE_CHANGED, NULL);
			
    lv_arc_set_value(g_arc_handle,g_menu_param.set_temp);
    //lv_arc_set_value(g_arc_handle,80);
    //动态更新圆弧中的值必须使用此方法
    lv_anim_init(&g_temp_anim);
    lv_anim_set_var(&g_temp_anim, g_arc_handle);
    lv_anim_set_exec_cb(&g_temp_anim, seting_arcangle_cb);//设置回调
    lv_anim_set_time(&g_temp_anim, 500);//更新时间
}

void user_window_page2_init(lv_obj_t *p_title)
{
		g_switch_image = lv_img_create(p_title);
	  lv_img_set_src(g_switch_image, &switch_on);
		//lv_obj_set_pos(g_switch_image,360/2-37,60);
	  lv_obj_align(g_switch_image, LV_ALIGN_CENTER,0,0);
    lv_obj_t *swtich_lab = lv_label_create(p_title); // 创建一个标签，
		if (swtich_lab != NULL)
		{
			lv_label_set_recolor(swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
			lv_obj_set_style_text_font(swtich_lab, &language_72, 0);
		  lv_label_set_text_fmt(swtich_lab, "#ffffff 开关#");
			lv_obj_set_pos(swtich_lab,140,110+80+90+20);
		}

}
static void seting_water_level(void * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);
}
void user_window_page3_init(lv_obj_t *p_title)
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
		
		lv_anim_init(&g_water_level_anim);//初始化一个动画对象
    lv_anim_set_exec_cb(&g_water_level_anim, seting_water_level);//设置刷新值回调
    lv_anim_set_time(&g_water_level_anim, 100);//设置刷新时间
    lv_anim_set_var(&g_water_level_anim, g_water_level_bar);//设置值
	  
		g_water_lab = lv_label_create(p_title); // 创建一个标签，
		if (g_water_lab != NULL)
		{
		  lv_label_set_recolor(g_water_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_water_lab, &language_36, 0);
			g_menu_param.water_level=40;
			lv_label_set_text_fmt(g_water_lab, "#ffffff %dL#",g_menu_param.water_level);
		  lv_obj_set_pos(g_water_lab,190,120);
		}
		lv_anim_set_values(&g_water_level_anim, 0, g_menu_param.water_level);
    lv_anim_start(&g_water_level_anim);
		lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
		if (g_temp_textlab != NULL)
		{
		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
			lv_label_set_text_fmt(g_temp_textlab, "#ffffff Water Level");
		  lv_obj_set_pos(g_temp_textlab,120,130+80+90);
		}
}


void user_window_page4_init(lv_obj_t *p_title)
{
		g_worktime_lab = lv_label_create(p_title); // 创建一个标签，
		if (g_worktime_lab != NULL)
		{
		  lv_label_set_recolor(g_worktime_lab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_worktime_lab, &language_36, 0);
			g_menu_param.work_time=50;
			lv_label_set_text_fmt(g_worktime_lab, "#ffffff %dmin#",g_menu_param.work_time);
		  lv_obj_set_pos(g_worktime_lab,200,100);
		}
		lv_obj_t *temp_img = lv_img_create(p_title);
	  lv_img_set_src(temp_img, &xn_time);
		lv_obj_set_pos(temp_img,150,100);
		
	  //lv_obj_align(temp_img, LV_ALIGN_CENTER,0,0);
    lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
		if (g_temp_textlab != NULL)
		{
				lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
				lv_obj_set_style_text_font(g_temp_textlab, &language_72, 0);
		    lv_label_set_text_fmt(g_temp_textlab, "#ffffff 时间#");
				lv_obj_set_pos(g_temp_textlab,140,120+80+90+20);
		}
		g_start_image = lv_img_create(p_title);
	  lv_img_set_src(g_start_image, &xn_play);
		lv_obj_align(g_start_image, LV_ALIGN_CENTER,0,0);
}



void user_window_page5_init(lv_obj_t *p_title)
{
			lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
		if (g_temp_textlab != NULL)
		{
		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
			lv_label_set_text_fmt(g_temp_textlab, "#ffffff LED Adjust#");
		  lv_obj_set_pos(g_temp_textlab,120,140+80+90);
		}
		g_colorwheel_ledadj = lv_colorwheel_create(p_title, true);
    lv_obj_set_size(g_colorwheel_ledadj, 200, 200);
    lv_obj_center(g_colorwheel_ledadj);
    g_menu_param.led_color.full=0x00f8;
    lv_colorwheel_set_rgb(g_colorwheel_ledadj,g_menu_param.led_color);

}


void seting_page_backdrop_recolor(uint8_t index)
{
		#define bg_make_val  0x1a,0x6b,0xe6
    switch (index)
    {
    case 0:
        lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(bg_make_val), 0);//设置背景颜色
        lv_obj_set_style_bg_color(g_main_block_obj2, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj3, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj4, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 1:
        lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(g_main_block_obj2, lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj3, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj4, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 2:
        lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(g_main_block_obj2, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj3, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj4, lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        break;
    case 3:

        lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色
        lv_obj_set_style_bg_color(g_main_block_obj2, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj3, lv_color_make(bg_make_val), 0);//设置背景颜色为黑
        lv_obj_set_style_bg_color(g_main_block_obj4, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
        break;
    case 4:

        break;
    }
}


void user_window_page6_init(lv_obj_t *p_title)
{
		 //初始化样式
    lv_style_init(&g_mainpage_style_base);
    //样式背景颜色：红色
    lv_style_set_bg_color(&g_mainpage_style_base, lv_color_make(0x00, 0x00, 0x00));//背景黑色
    //样式外边距：6pix
    lv_style_set_border_width(&g_mainpage_style_base, 2);
    //样式外边框颜色：蓝色
    lv_style_set_border_color(&g_mainpage_style_base, lv_color_make(0xff, 0xff, 0xff));//边框白色色
     
    //将预定按下样式添加到style_pr中
    //lv_style_set_transition(&style_pr, &trans_pr);
     g_set_block_obj1 = lv_tileview_create(p_title);                 //创建titlewindow
    lv_obj_set_style_bg_color(g_set_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
    lv_obj_set_scrollbar_mode(g_set_block_obj1, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
   // g_set_block_obj1 = lv_obj_create(g_set_title_main);
//    lv_obj_set_size(g_set_block_obj1, 400, 400);                      /* 设置对象的大小 */
//    lv_obj_set_style_bg_color(g_set_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
//    lv_obj_set_scrollbar_mode(g_set_block_obj1, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
//    lv_obj_set_pos(g_set_block_obj1, 0, 0);


    g_main_block_obj1 = lv_obj_create(g_set_block_obj1);
    lv_obj_set_scrollbar_mode(g_main_block_obj1, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    //lv_obj_set_style_bg_color(g_main_block_obj1, lv_color_make(0x00, 0x00, 0x00), 0);//设置背景颜色为黑
    //将style_def设置为默认样式
    lv_obj_add_style(g_main_block_obj1, &g_mainpage_style_base, 0);
    lv_obj_set_size(g_main_block_obj1, 110, 110);
    lv_obj_set_pos(g_main_block_obj1, 10+60, 70);
    g_main_block_led_img = lv_img_create(g_main_block_obj1);
    lv_img_set_src(g_main_block_led_img, &xn_led_main);//设置图片源
    lv_obj_align(g_main_block_led_img, LV_ALIGN_TOP_MID, 0, 0);


    g_main_block_obj2 = lv_obj_create(g_set_block_obj1);
    lv_obj_set_scrollbar_mode(g_main_block_obj2, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(g_main_block_obj2, &g_mainpage_style_base, 0);
    lv_obj_set_size(g_main_block_obj2, 110, 110);
    lv_obj_set_pos(g_main_block_obj2, 10+60+110+20, 70);
    g_main_block_curtain_img = lv_img_create(g_main_block_obj2);
    lv_img_set_src(g_main_block_curtain_img, &xn_curtain_main);//设置图片源
    lv_obj_align(g_main_block_curtain_img, LV_ALIGN_TOP_MID, 0, 0);

    g_main_block_obj3 = lv_obj_create(g_set_block_obj1);
    lv_obj_set_scrollbar_mode(g_main_block_obj3, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(g_main_block_obj3, &g_mainpage_style_base, 0);
    lv_obj_set_size(g_main_block_obj3, 110, 110);
    lv_obj_set_pos(g_main_block_obj3, 10+60, 70 + 110 + 20);
    g_main_block_control_img = lv_img_create(g_main_block_obj3);
    lv_img_set_src(g_main_block_control_img, &xn_temp_contrl);//设置图片源 
    lv_obj_align(g_main_block_control_img, LV_ALIGN_TOP_MID, 0, 0);

    g_main_block_obj4 = lv_obj_create(g_set_block_obj1);
    lv_obj_set_scrollbar_mode(g_main_block_obj4, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
    lv_obj_add_style(g_main_block_obj4, &g_mainpage_style_base, 0);
    lv_obj_set_size(g_main_block_obj4, 110, 110);
    lv_obj_set_pos(g_main_block_obj4, 10+60 + 110 + 20, 70 + 110 + 20);
    g_main_block_ledadj_img = lv_img_create(g_main_block_obj4);
    lv_img_set_src(g_main_block_ledadj_img, &xn_ledadj_main);//设置图片源
    lv_obj_align(g_main_block_ledadj_img, LV_ALIGN_TOP_MID, 0, 0);
		seting_page_backdrop_recolor(0);
    g_main_swtich_lab = lv_label_create(g_main_block_obj1); // 创建一个标签，
    if (g_main_swtich_lab != NULL)
    {
        lv_label_set_recolor(g_main_swtich_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(g_main_swtich_lab, &language_16, 0);
        lv_label_set_text_fmt(g_main_swtich_lab, "#ffffff 开关#");
        lv_obj_align(g_main_swtich_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    g_main_curtain_lab = lv_label_create(g_main_block_obj2); // 创建一个标签，
    if (g_main_curtain_lab != NULL)
    {
        lv_label_set_recolor(g_main_curtain_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(g_main_curtain_lab, &language_16, 0);
        lv_label_set_text_fmt(g_main_curtain_lab, "#ffffff 窗帘    10%%#");
        lv_obj_align(g_main_curtain_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    g_main_control_lab = lv_label_create(g_main_block_obj3); // 创建一个标签，
    if (g_main_control_lab != NULL)
    {
        lv_label_set_recolor(g_main_control_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(g_main_control_lab, &language_16, 0);
        lv_label_set_text_fmt(g_main_control_lab, "#ffffff 温控    25℃#");
        lv_obj_align(g_main_control_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }

    lv_obj_t* led_temp_lab = lv_label_create(g_main_block_obj4); // 创建一个标签，
    if (led_temp_lab != NULL)
    {
        lv_label_set_recolor(led_temp_lab, true);                        /*Enable re-coloring by commands in the text*/
        lv_obj_set_style_text_font(led_temp_lab, &language_16, 0);
        lv_label_set_text_fmt(led_temp_lab, "#ffffff 调色灯#");
        lv_obj_align(led_temp_lab, LV_ALIGN_BOTTOM_LEFT, 0, +10);
    }
		
		
}





void user_window_page7_init(lv_obj_t *p_title)
{
		lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
		if (g_temp_textlab != NULL)
		{
		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
			lv_label_set_text_fmt(g_temp_textlab, "#ffffff Page14#");
		  lv_obj_set_pos(g_temp_textlab,120,130+80+90);
		}
		
}

void user_window_page8_init(lv_obj_t *p_title)
{
//			lv_obj_t *my_img = lv_img_create(p_title);
//	  lv_img_set_src(my_img, &xn_tuos);
//		//lv_obj_set_pos(g_switch_image,360/2-37,60);
//	  lv_obj_align(my_img, LV_ALIGN_CENTER,0,0);
//    /*Create a spinner*/
//	 	lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
//		if (g_temp_textlab != NULL)
//		{
//		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
//		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
//			lv_label_set_text_fmt(g_temp_textlab, "#ffffff Washing...#");
//		  lv_obj_set_pos(g_temp_textlab,120,140+80+90);
//		}

}
void user_window_page9_init(lv_obj_t *p_title)
{

		
}
void user_window_page10_init(lv_obj_t *p_title)
{
}
void user_window_page11_init(lv_obj_t *p_title)
{
}
void user_window_page12_init(lv_obj_t *p_title)
{
	
}
void user_window_page13_init(lv_obj_t *p_title)
{
}
void user_window_page14_init(lv_obj_t *p_title)
{

}
void user_window_work_init(lv_obj_t *p_title)
{
		lv_obj_t *my_img = lv_img_create(p_title);
	  lv_img_set_src(my_img, &xn_tuos);
		//lv_obj_set_pos(g_switch_image,360/2-37,60);
	  lv_obj_align(my_img, LV_ALIGN_CENTER,0,0);
    /*Create a spinner*/
	
	 	lv_obj_t *g_temp_textlab = lv_label_create(p_title); // 创建一个标签，
		if (g_temp_textlab != NULL)
		{
		  lv_label_set_recolor(g_temp_textlab, true);                        /*Enable re-coloring by commands in the text*/
		  lv_obj_set_style_text_font(g_temp_textlab, &language_36, 0);
			lv_label_set_text_fmt(g_temp_textlab, "#ffffff Washing...#");
		  lv_obj_set_pos(g_temp_textlab,120,140+80+90);
		}

}
#if 0
LV_IMG_DECLARE(bixin);
void lv_example_gif_1(void) //显示动态图
{
   lv_obj_t * img;
   g_title_main = lv_tileview_create(lv_scr_act());                 //创建titlewindow
   lv_obj_set_style_bg_color(g_title_main, lv_color_make(0x00,0x00,0x00), 0);//设置背景颜色为黑
   lv_obj_set_scrollbar_mode(g_title_main, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
   img = lv_gif_create(g_title_main);
   lv_gif_set_src(img, &bixin);
   lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
		//lv_gif_restart(img);
}
#endif

 


 

/************************************************************
  * @brief   用户gui菜单任务初始化
  * @param   NULL
  * @return  NULL
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
void gui_menu_task_init(void)
{
   	user_menu_task_id = os_task_create(user_menu_task_func);
    g_title_main = lv_tileview_create(lv_scr_act());                 //创建titlewindow
    lv_obj_set_style_bg_color(g_title_main, lv_color_make(0x00,0x00,0x00), 0);//设置背景颜色为黑
    lv_obj_set_scrollbar_mode(g_title_main, LV_SCROLLBAR_MODE_OFF);  //取消滚动条显
		for(int i=WINDOW_MAIN_PAGE;i<WINDOW_PAGE_MAX;i++)   //创建界面 
    {
        user_menu_window_manage[i].window_obj = lv_tileview_add_tile(g_title_main, user_menu_window_manage[i].col_id, user_menu_window_manage[i].row_id,user_menu_window_manage[i].dir);
        find_display_handler(user_menu_window_manage,i);
    }
		lv_obj_set_tile_id(g_title_main,user_menu_window_manage[WINDOW_MAIN_PAGE].col_id, user_menu_window_manage[WINDOW_MAIN_PAGE].row_id,LV_ANIM_OFF);
    lv_mem_monitor(&mem);
    printf("--------------------mem heap size=%d, free_size=%d,used=%d\r\n",mem.total_size,mem.free_size,mem.total_size - mem.free_size);
		
   
//		lv_timer_t* timer;
//    timer = lv_timer_create(my_lvgl_timer,1000,  NULL);
//    lv_timer_ready(timer);
//		startingup_animing();
//		lv_animimg_start(g_animing_start);

//		lv_example_gif_1();
//	  lv_mem_monitor(&mem);
//		printf("--------------------mem heap size=%d, free_size=%d,used=%d\r\n",mem.total_size,mem.free_size,mem.total_size - mem.free_size);
}

/************************************************************
  * @brief   获取用户菜单任务的id值
  * @param   NULL
  * @return  user_menu_task_id 
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
uint16_t get_user_menu_task_id(void)
{
	return user_menu_task_id;
}

/************************************************************
	* @brief   判断当前界面是否为相同界面
	* @param   NULL
  * @return  0 错误/不相同 1 同界面
  * @author  LH
  * @date    2022-08-26
  * @version v1.0
  ***********************************************************/
uint8_t judge_current_tileview(lv_obj_t *tile,lv_obj_t *page_act)
{
	if((tile==NULL) || (page_act==NULL))
	{
		return 0;	
	}
	lv_obj_t *current_act=lv_tileview_get_tile_act(tile);
	if(current_act == page_act) //判断是否为当前界面
	{
			return 1;	
	}else
	{
		return 0;
	}
}
