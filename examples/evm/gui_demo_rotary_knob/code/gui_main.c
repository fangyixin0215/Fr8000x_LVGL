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
#include "driver_st77903.h"
#include "input_driver.h"
#include "driver_ktm57xx.h"
#include "gif_decoder.h"
#include "user_knob_config.h"

#define TOUCHPAD_ENABLED		0
#define LV_TICK_COUNT           5

static os_timer_t lv_schedule_timer;
 

static lv_disp_draw_buf_t disp_buf;

static lv_disp_drv_t *last_disp = NULL;
#ifdef DISPLAY_TYPE_ST77903
static bool g_update_flag=0;
static uint16_t *ST77903_PSRAM_BUFF =NULL;
static uint16_t g_update_timeout_cnt=0;
static uint8_t g_gif_show_flag=0;
#endif
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
		while((get_st77903_send_state()==0));
//	  while((get_st77903_send_state()==0) && (g_update_timeout_cnt<100));
		ST77903_PSRAM_BUFF = (void *)color_p;
		st77903_display(ST77903_PSRAM_BUFF,0);
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

 
__attribute__((section("ram_code"))) void timer0_isr(void)
{
	
#ifdef DISPLAY_TYPE_ST77903
		g_update_timeout_cnt++;
		if((ST77903_PSRAM_BUFF!=NULL) && get_st77903_send_state() && (g_update_flag == true)) 
		{
			st77903_display(ST77903_PSRAM_BUFF,0);
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

 

static void lv_schedule_timer_handler(void *arg)
{
	
    lv_timer_handler();
}

uint8_t g_key_code=0;
extern lv_indev_t * g_indev_keypad;
extern void knob_return_home_page(void);

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/
   return g_key_code;
}
/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}
/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);
    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        case 1:
            act_key = LV_KEY_NEXT;
            break;
        case 2:
            act_key = LV_KEY_PREV;
            break;
        case 3:
            act_key = LV_KEY_LEFT;
            break;
        case 4:
            act_key = LV_KEY_RIGHT;
            break;
        case 5:
            act_key = LV_KEY_ENTER;
            break;
        }
		if(DBLCLICK_CODE == act_key)
		{		 
			 data->state = LV_INDEV_STATE_REL;
			 last_key = act_key;
			 g_key_code = 0;
			 knob_return_home_page();
		   return;
		}
        last_key = act_key;
        g_key_code = 0;
    //    printf("%d keydown \n",last_key);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;

    /*Return `false` because we are not buffering and no more data to read*/
    return;
}



extern 	void page_change_timer(void);

void gui_main(void)
{
	#if(TOUCHPAD_ENABLED==1)
	cst816_init();
	#endif
	printf("gui_main\r\n");
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
	#if(TOUCHPAD_ENABLED==1)
	/* Implement and register a function which can read an input device. E.g. for a touch pad */
	static lv_indev_drv_t indev_drv;                  /*Descriptor of a input device driver*/
	lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
	indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
	lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/
	#endif
	input_encoder_init();
	static lv_indev_drv_t indev_drv1;
	/*Initialize your keypad or keyboard if you have*/
	/*Register a keypad input device*/
	lv_indev_drv_init(&indev_drv1);
	indev_drv1.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv1.read_cb = keypad_read;
	g_indev_keypad = lv_indev_drv_register(&indev_drv1);
 	knob_gui_init();

 	display_init();
	__SYSTEM_TIMER_CLK_ENABLE();
	timer_init(Timer0, system_get_clock_config()*1000*LV_TICK_COUNT, TIMER_DIV_NONE);
	timer_start(Timer0);
	NVIC_SetPriority(TIMER0_IRQn, 5);
	NVIC_EnableIRQ(TIMER0_IRQn);
	#ifdef DISPLAY_TYPE_ST77903
	g_gif_show_flag=1;
	start_gif_decoder_init();
	#endif
	os_timer_init(&lv_schedule_timer, lv_schedule_timer_handler, NULL);
	os_timer_start(&lv_schedule_timer, 10, true);
	page_change_timer();
    //lv_demo_widgets();
    //lv_demo_benchmark();
    //lv_ex_calendar_1();
    //lv_demo_music();
		

}

