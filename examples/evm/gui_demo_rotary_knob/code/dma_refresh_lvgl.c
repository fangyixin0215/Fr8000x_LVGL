#include "dma_refresh_lvgl.h"
#include "driver_dma.h"
#include "plf.h"
#include "driver_system.h"
#include "driver_pmu.h"
#include "driver_psram.h"
#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"

DMA_HandleTypeDef PSRAM_DMA_Handle;

DMA_HandleTypeDef MEM_COPY_DMA_Handle;
	

void lv_gpu_stm32_dma2d_init()
{
	
	  __SYSTEM_DMA_CLK_ENABLE();
    PSRAM_DMA_Handle.Channel               = DMA_Channel1;
    PSRAM_DMA_Handle.Init.Data_Flow        = DMA_M2M_DMAC;
    PSRAM_DMA_Handle.Init.Source_Inc       = DMA_ADDR_INC_INC;
    PSRAM_DMA_Handle.Init.Desination_Inc   = DMA_ADDR_INC_INC;
    PSRAM_DMA_Handle.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    PSRAM_DMA_Handle.Init.Desination_Width = DMA_TRANSFER_WIDTH_32;
    dma_init(&PSRAM_DMA_Handle);
    DMA->Channels[PSRAM_DMA_Handle.Channel].CFG1.CH_PRIOR = 7;
	

	  MEM_COPY_DMA_Handle.Channel               = DMA_Channel2;
    MEM_COPY_DMA_Handle.Init.Data_Flow        = DMA_M2M_DMAC;
    MEM_COPY_DMA_Handle.Init.Source_Inc       = DMA_ADDR_INC_INC;
    MEM_COPY_DMA_Handle.Init.Desination_Inc   = DMA_ADDR_INC_INC;
    MEM_COPY_DMA_Handle.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    MEM_COPY_DMA_Handle.Init.Desination_Width = DMA_TRANSFER_WIDTH_32;
    dma_init(&MEM_COPY_DMA_Handle);
    DMA->Channels[MEM_COPY_DMA_Handle.Channel].CFG1.CH_PRIOR = 6;
	
	
	
//	//初始化DMA
//    DMA_M2M.Channel = DMA_Channel1;
//    DMA_M2M.Init.Data_Flow        = DMA_M2M_DMAC;
//		//gc9c01_DMA_Channel.Init.Request_ID       = 1;
//	
//	#if 0
//	 DMA_M2M.Init.Source_Inc       = DMA_ADDR_INC_NO_CHANGE;
//	#else
//	 DMA_M2M.Init.Source_Inc       = DMA_ADDR_INC_INC;

//	#endif
//    DMA_M2M.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
//    DMA_M2M.Init.Source_Width     = DMA_TRANSFER_WIDTH_16;
//    DMA_M2M.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
//    dma_init(&DMA_M2M);
}
/**
*用颜色填充缓冲区中的一个区域
* @param buf一个应该被填充的缓冲区
*param buf_w缓冲区的宽度，以像素为单位
* @param color填充颜色
* @param fill_w填充像素(<= buf_w)
* @param fill_h填充像素的高度
* @note ' buf_w - fill_w '被偏移到填充后的下一行
**/
void lv_gpu_stm32_dma2d_fill(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, lv_coord_t fill_w,lv_coord_t fill_h)
{					

			for(int y = 0; y < fill_h; y++){

				
				
	     buf += buf_w;  //指针向下偏移
			}
//			SEGGER_RTT_printf(0,"%s draw_area_h=%d,draw_area_w=%d,color=%x\r\n",__FUNCTION__,fill_h,fill_w,color);						 
}
/**
*复制一个地图(通常是RGB图像)到一个缓冲区
* @param buf一个应该复制map的缓冲区
* @param buf_w缓冲区的宽度，以像素为单位
* @param映射一个要复制的图像
* @param map_w地图的宽度(像素)
* @param copy_w复制区域的宽度，单位是像素(<= buf_w)
* @param copy_h复制区域的高度，单位为像素
* @note ' map_w - fill_w '被偏移到复制后的下一行
*/
void lv_gpu_stm32_dma2d_copy(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map,lv_coord_t map_w,
lv_coord_t copy_w, lv_coord_t copy_h)
{



}




/**
*混合一个映射(如ARGB图像或RGB图像的不透明度)到一个缓冲区
* @param buf一个应该复制' map '的缓冲区
  @param buf_w缓冲区的宽度，以像素为单位
* @param映射一个要复制的图像
* @param opa不透明度的地图
* @param map_w地图的宽度(像素)
* @param copy_w复制区域的宽度，单位是像素(<= buf_w)
* @param copy_h复制区域的高度，单位为像素
* @note ' map_w - fill_w '被偏移到复制后的下一行
**/
void lv_gpu_stm32_dma2d_blend(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_opa_t opa,
lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h)
{




}