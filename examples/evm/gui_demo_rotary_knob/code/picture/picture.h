
#ifndef __PICTURE_H__
#define __PICTURE_H__
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define    img_head_offset    (0x10200000) 



#define    humidity_map                   (uint8_t*)(img_head_offset+0x4) 
#define    switch_off_map                   (uint8_t*)(img_head_offset+0x1B08) 
#define    switch_on_map                   (uint8_t*)(img_head_offset+0x10E0C) 
#define    temperature_map                   (uint8_t*)(img_head_offset+0x20110) 
#define    watch_time00_map                   (uint8_t*)(img_head_offset+0x21C14) 
#define    watch_time01_map                   (uint8_t*)(img_head_offset+0x23850) 
#define    watch_time02_map                   (uint8_t*)(img_head_offset+0x2548C) 
#define    watch_time03_map                   (uint8_t*)(img_head_offset+0x270C8) 
#define    watch_time04_map                   (uint8_t*)(img_head_offset+0x28D04) 
#define    watch_time05_map                   (uint8_t*)(img_head_offset+0x2A940) 
#define    watch_time06_map                   (uint8_t*)(img_head_offset+0x2C57C) 
#define    watch_time07_map                   (uint8_t*)(img_head_offset+0x2E1B8) 
#define    watch_time08_map                   (uint8_t*)(img_head_offset+0x2FDF4) 
#define    watch_time09_map                   (uint8_t*)(img_head_offset+0x31A30) 
#define    xn_cam_map                   (uint8_t*)(img_head_offset+0x3366C) 
#define    xn_chuang_map                   (uint8_t*)(img_head_offset+0x345A0) 
#define    xn_count_map                   (uint8_t*)(img_head_offset+0x54A70) 
#define    xn_ct_map                   (uint8_t*)(img_head_offset+0x559A4) 
#define    xn_curtain_main_map                   (uint8_t*)(img_head_offset+0x790D8) 
#define    xn_gd_map                   (uint8_t*)(img_head_offset+0x7C0DC) 
#define    xn_group_map                   (uint8_t*)(img_head_offset+0x9C5AC) 
#define    xn_huoh_map                   (uint8_t*)(img_head_offset+0xDD95C) 
#define    xn_iphone_map                   (uint8_t*)(img_head_offset+0xDE890) 
#define    xn_led_map                   (uint8_t*)(img_head_offset+0xDF7C4) 
#define    xn_ledadj_main_map                   (uint8_t*)(img_head_offset+0x102EF8) 
#define    xn_led_main_map                   (uint8_t*)(img_head_offset+0x105EFC) 
#define    xn_led_status_off_map                   (uint8_t*)(img_head_offset+0x108F00) 
#define    xn_led_status_on_map                   (uint8_t*)(img_head_offset+0x10BF04) 
#define    xn_music_map                   (uint8_t*)(img_head_offset+0x10EF08) 
#define    xn_play_map                   (uint8_t*)(img_head_offset+0x10FE3C) 
#define    xn_pwoer_map                   (uint8_t*)(img_head_offset+0x111B8C) 
#define    xn_shouye_map                   (uint8_t*)(img_head_offset+0x1352C0) 
#define    xn_smart_map                   (uint8_t*)(img_head_offset+0x15AAC4) 
#define    xn_stop_map                   (uint8_t*)(img_head_offset+0x17AF94) 
#define    xn_sw_map                   (uint8_t*)(img_head_offset+0x17CCE4) 
#define    xn_temppage_map                   (uint8_t*)(img_head_offset+0x1A0418) 
#define    xn_temp_contrl_map                   (uint8_t*)(img_head_offset+0x1C3B4C) 
#define    xn_time_map                   (uint8_t*)(img_head_offset+0x1C6B50) 
#define    xn_tuos_map                   (uint8_t*)(img_head_offset+0x1C8654) 
#define    xn_xueh_map                   (uint8_t*)(img_head_offset+0x1EBD88) 
#define    xn_xyj_map                   (uint8_t*)(img_head_offset+0x1ECCBC) 
#ifdef __cplusplus
}
#endif



#endif