
#include "picture.h"


const lv_img_dsc_t humidity = {
.header.always_zero = 0,
.header.w = 48,
.header.h = 48,
.data_size = 2304* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = humidity_map,
};



const lv_img_dsc_t switch_off = {
.header.always_zero = 0,
.header.w = 144,
.header.h = 144,
.data_size = 20736* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = switch_off_map,
};



const lv_img_dsc_t switch_on = {
.header.always_zero = 0,
.header.w = 144,
.header.h = 144,
.data_size = 20736* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = switch_on_map,
};



const lv_img_dsc_t temperature = {
.header.always_zero = 0,
.header.w = 48,
.header.h = 48,
.data_size = 2304* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = temperature_map,
};



const lv_img_dsc_t watch_time00 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time00_map,
};



const lv_img_dsc_t watch_time01 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time01_map,
};



const lv_img_dsc_t watch_time02 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time02_map,
};



const lv_img_dsc_t watch_time03 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time03_map,
};



const lv_img_dsc_t watch_time04 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time04_map,
};



const lv_img_dsc_t watch_time05 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time05_map,
};



const lv_img_dsc_t watch_time06 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time06_map,
};



const lv_img_dsc_t watch_time07 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time07_map,
};



const lv_img_dsc_t watch_time08 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time08_map,
};



const lv_img_dsc_t watch_time09 = {
.header.always_zero = 0,
.header.w = 43,
.header.h = 56,
.data_size = 2408* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = watch_time09_map,
};



const lv_img_dsc_t xn_cam = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_cam_map,
};



const lv_img_dsc_t xn_chuang = {
.header.always_zero = 0,
.header.w = 210,
.header.h = 210,
.data_size = 44100* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_chuang_map,
};



const lv_img_dsc_t xn_count = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_count_map,
};



const lv_img_dsc_t xn_ct = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_ct_map,
};



const lv_img_dsc_t xn_curtain_main = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_curtain_main_map,
};



const lv_img_dsc_t xn_gd = {
.header.always_zero = 0,
.header.w = 210,
.header.h = 210,
.data_size = 44100* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_gd_map,
};



const lv_img_dsc_t xn_group = {
.header.always_zero = 0,
.header.w = 292,
.header.h = 305,
.data_size = 89060* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_group_map,
};



const lv_img_dsc_t xn_huoh = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_huoh_map,
};



const lv_img_dsc_t xn_iphone = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_iphone_map,
};



const lv_img_dsc_t xn_led = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_led_map,
};



const lv_img_dsc_t xn_ledadj_main = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_ledadj_main_map,
};



const lv_img_dsc_t xn_led_main = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_led_main_map,
};



const lv_img_dsc_t xn_led_status_off = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_led_status_off_map,
};



const lv_img_dsc_t xn_led_status_on = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_led_status_on_map,
};



const lv_img_dsc_t xn_music = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_music_map,
};



const lv_img_dsc_t xn_play = {
.header.always_zero = 0,
.header.w = 50,
.header.h = 50,
.data_size = 2500* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_play_map,
};



const lv_img_dsc_t xn_pwoer = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_pwoer_map,
};



const lv_img_dsc_t xn_shouye = {
.header.always_zero = 0,
.header.w = 210,
.header.h = 210,
.data_size = 44100* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_shouye_map,
};



const lv_img_dsc_t xn_smart = {
.header.always_zero = 0,
.header.w = 210,
.header.h = 210,
.data_size = 44100* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_smart_map,
};



const lv_img_dsc_t xn_stop = {
.header.always_zero = 0,
.header.w = 50,
.header.h = 50,
.data_size = 2500* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_stop_map,
};



const lv_img_dsc_t xn_sw = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_sw_map,
};



const lv_img_dsc_t xn_temppage = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_temppage_map,
};



const lv_img_dsc_t xn_temp_contrl = {
.header.always_zero = 0,
.header.w = 64,
.header.h = 64,
.data_size = 4096* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_temp_contrl_map,
};



const lv_img_dsc_t xn_time = {
.header.always_zero = 0,
.header.w = 48,
.header.h = 48,
.data_size = 2304* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_time_map,
};



const lv_img_dsc_t xn_tuos = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_tuos_map,
};



const lv_img_dsc_t xn_xueh = {
.header.always_zero = 0,
.header.w = 36,
.header.h = 36,
.data_size = 1296* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_xueh_map,
};



const lv_img_dsc_t xn_xyj = {
.header.always_zero = 0,
.header.w = 220,
.header.h = 220,
.data_size = 48400* LV_IMG_PX_SIZE_ALPHA_BYTE,
.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
.data = xn_xyj_map,
};






