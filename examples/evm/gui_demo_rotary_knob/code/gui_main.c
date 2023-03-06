#include <stdint.h>
#include "driver_cst816d.h"
#include "lvgl.h"
#include "lv_demo.h"

#include "co_printf.h"
#include "os_timer.h"

#include "driver_system.h"
#include "driver_timer.h"
#include "driver_display.h"
#include "driver_touchpad.h"
#include "user_main.h"
#include "driver_st77903.h"
#include "user_menu_handle.h"
#include "user_menu.h"
#include "input_driver.h"
#include "driver_ktm57xx.h"
#include "gif_decoder.h"
#define TOUCHPAD_ENABLED						0
#define LV_TICK_COUNT               10

static os_timer_t lv_schedule_timer;
static os_timer_t st77903_handle_timer;

static lv_disp_draw_buf_t disp_buf;

static lv_disp_drv_t *last_disp = NULL;

static bool g_update_flag=0;
static uint16_t *p_st77903_psram =NULL;
static uint16_t g_update_timeout_cnt=0;
static uint8_t g_gif_show_flag=0;
// this function is called in DMA interrupt
static void my_disp_flush_done(void)
{
//    lv_disp_flush_ready(last_disp);
    last_disp = NULL;
}



static void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
	#if 1
    if(last_disp != NULL) {
        display_wait_transfer_done();
    }
    last_disp = disp;
#ifdef DISPLAY_TYPE_ST77903
		if(g_gif_show_flag)
		{
			  g_gif_show_flag=0;
		}
		g_update_flag=false;
		g_update_timeout_cnt=0;
	  while((get_st77903_send_state()==0) && (g_update_timeout_cnt<4));//ok
		p_st77903_psram = (void *)color_p;
		st77903_display(p_st77903_psram,0);
		g_update_flag=true;
#else
		display_set_window(area->x1, area->x2, area->y1, area->y2);
    display_update((area->x2+1-area->x1)*(area->y2+1-area->y1), (void *)color_p, my_disp_flush_done);

	#endif
    lv_disp_flush_ready(last_disp);
	#endif
}


static void my_touchpad_read(struct _lv_indev_drv_t * indev, lv_indev_data_t * data)
{
#if(TOUCHPAD_ENABLED==1)
    uint8_t buffer[8];
		cst816_read_bytes(0x00,buffer,7); 
    //touchpad_read_data_raw(buffer, 8);
    data->state = (buffer[2] != 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(data->state == LV_INDEV_STATE_PR) {
        data->point.x = ((buffer[3]&0x0f)<<8) | buffer[4];
        data->point.y = ((buffer[5]&0x0f)<<8) | buffer[6];
    }
#endif
		
}

struct_Timer_t Timer0_Handle;


__attribute__((section("ram_code"))) void timer0_isr(void)
{
    
#ifdef DISPLAY_TYPE_ST77903
		g_update_timeout_cnt++;
		if((p_st77903_psram!=NULL) && get_st77903_send_state() && (g_update_flag == true))//没变化最快刷30帧 
		{
					st77903_display(p_st77903_psram,0);
		}
		else
		{
			if(g_gif_show_flag==1)
			{
			 
				start_gif_decoder_handler();
				//gif_decoder_handler();
			}
		}
#endif
		timer_int_clear(Timer0);
		lv_tick_inc(LV_TICK_COUNT);
}

__attribute__((section("ram_code"))) void timer1_isr(void)
{
    uint32_t tick;
    timer_int_clear(Timer1);
    timer_stop(Timer1);
	  

    tick = lv_timer_handler();
    timer_init(Timer1, system_get_clock_config()*1000*tick, TIMER_DIV_NONE);
    timer_start(Timer1);
}

static void lv_schedule_timer_handler(void *arg)
{
    lv_timer_handler();
}

static void st77903_handle_cb(void *arg)
{

	 
}
 

void gui_main(void)
{
		#if(TOUCHPAD_ENABLED==1)
		cst816_init();
		#endif
		
		printf("gui_main\r\n");
    display_init();
	 __SYSTEM_TIMER_CLK_ENABLE();
    timer_init(Timer0, system_get_clock_config()*1000*LV_TICK_COUNT, TIMER_DIV_NONE);
    timer_start(Timer0);
    NVIC_SetPriority(TIMER0_IRQn, 5);
    NVIC_EnableIRQ(TIMER0_IRQn);
	  g_gif_show_flag=1;
		start_gif_decoder_init();
	  printf("gui_main111\r\n");
    lv_init();



  /*Initialize `disp_buf` with the buffer(s) */
	
	#if 1 
   lv_disp_draw_buf_init(&disp_buf, (void *)0x22000000, (void *)/*0x22050000*/(0x22000000+LV_HOR_RES_MAX * LV_VER_RES_MAX*2), LV_HOR_RES_MAX * LV_VER_RES_MAX);    /*Initialize the display buffer*/
   //lv_disp_draw_buf_init(&disp_buf, (void *)0x22000000, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);    /*Initialize the display buffer*/
 
 #else
		static lv_color_t buf_1[LV_HOR_RES_MAX * 20];
    static lv_color_t buf_2[LV_HOR_RES_MAX * 20];
	  lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 20);    /*Initialize the display buffer*/

	#endif
    /* Implement and register a function which can copy the rendered image to an area of your display */
    static lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
    disp_drv.draw_buf = &disp_buf;        /*Assign the buffer to the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
		
	  //lv_disp_set_bg_color(last_disp,0x00);

    /* Implement and register a function which can read an input device. E.g. for a touch pad */
    static lv_indev_drv_t indev_drv;                  /*Descriptor of a input device driver*/
    lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/


		gui_menu_task_init();
    os_timer_init(&st77903_handle_timer, lv_schedule_timer_handler, NULL);
    os_timer_start(&st77903_handle_timer, 10, true);
		input_encoder_init();
//    timer_init(Timer1, system_get_clock_config()*1000*20, TIMER_DIV_NONE);
//    timer_start(Timer1);
//    NVIC_SetPriority(TIMER1_IRQn, 6);
//    NVIC_EnableIRQ(TIMER1_IRQn);
		
    //test_1();
		
    //lv_demo_widgets();
    //lv_demo_benchmark();
    //lv_ex_calendar_1();
    //lv_demo_music();
		
	  //window_manager_init();
		// user_guitask_init();
		//lv_example_tileview_1();

}

