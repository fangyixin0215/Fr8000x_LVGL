
/*
@driver_st7365.c
@author: LH 
Creation Date：2024/04/28
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "driver_st7365.h"

#include "sys_utils.h"

#define DEVELOP_FR8008GP_BOARD 1

#ifdef DEVELOP_FR8008GP_BOARD

#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_B, GPIO_PIN_7, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_B, GPIO_PIN_7, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_CLEAR)
#define LCD_W 320
#define LCD_H 480
#endif

void st7365_display_wait_transfer_done(void);
static DMA_LLI_InitTypeDef Link_Channel[50];
static DMA_HandleTypeDef st7365_DMA_Channel;
static dma_LinkParameter_t LinkParameter;
static volatile bool dma_transfer_done = true;

SPI_HandleTypeDef st7365_SPI_Handle;

static void (*dma_trans_done_callback)(void) = NULL;
 

 

static void st7365_init_io(void)
{
	 __SYSTEM_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Handle;
    // backlight
//    GPIO_Handle.Pin       = GPIO_PIN_1;
//    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
//    gpio_init(GPIO_E, &GPIO_Handle);
 
	 // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_7;
	 GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_B, &GPIO_Handle);

    

    GPIO_Handle.Pin       = GPIO_PIN_6|GPIO_PIN_2;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
 
    LCD_RELEASE_CS();
    LCD_ENABLE_BACKLIGHT();
//		LCD_DISABLE_BACKLIGHT();
  //init_lcd_pwm();	
	
}

static void st7365_init_spi(void)
{
    __SYSTEM_SPI0_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI0_MASTER_CLK_ENABLE();
    
    st7365_SPI_Handle.SPIx                       = SPIM0;
    st7365_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    st7365_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    st7365_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    st7365_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    st7365_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&st7365_SPI_Handle);
}

static void st7365_init_dma(void)
{    
    __SYSTEM_DMA_CLK_ENABLE();
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    
    st7365_DMA_Channel.Channel = DMA_Channel0;
    st7365_DMA_Channel.Init.Data_Flow        = DMA_M2P_DMAC;
    st7365_DMA_Channel.Init.Request_ID       = 1;
    st7365_DMA_Channel.Init.Source_Inc       = DMA_ADDR_INC_INC;
    st7365_DMA_Channel.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    st7365_DMA_Channel.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    st7365_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&st7365_DMA_Channel);

    NVIC_EnableIRQ(DMA_IRQn);
}

static void st7365_write_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];

    spi_data[0]=cmd;
	LCD_SET_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&st7365_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();

}

static void st7365_write_data(uint8_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data;
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&st7365_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();
}

static void st7365_write_16bit_data(uint16_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data>>8;
	spi_data[1]=data;
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&st7365_SPI_Handle, (uint16_t *)spi_data, 2);
    LCD_RELEASE_CS();
}


static void send_data_disp(uint8_t *pdata,uint32_t len)
{
	
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&st7365_SPI_Handle, (uint16_t *)pdata, len);
	  LCD_RELEASE_CS();
}


static void send_16data_disp(uint16_t *pdata,uint32_t len)
{
	 LCD_RELEASE_DC();
   LCD_SET_CS();
   spi_master_transmit_X1(&st7365_SPI_Handle, (uint16_t *)pdata, len);
	 LCD_RELEASE_CS();
	
}
#if 0
static int8_t display_page = 0;

static uint16_t *PSRAM_LCD_FRAME_BUFFER_ORIGIN = (uint16_t *)(0x22000000+0x200000);
static uint16_t *PSRAM_LCD_FRAME_BUFFER_LEFT =  (uint16_t *)(0x22000000 + 0x50000+0x200000);
static uint16_t *PSRAM_LCD_FRAME_BUFFER_RIGHT = (uint16_t *)(0x22000000 + 0x100000+0x200000);
static uint16_t *psram = NULL;

static uint16_t color[4]={0xFFFF,0xFF00,0x8f00,0x001F};//rgb 565

static void psram_frame_buffer_init(void)
{
	memset(PSRAM_LCD_FRAME_BUFFER_ORIGIN, 0, LCD_W*LCD_H*2);
	memset(PSRAM_LCD_FRAME_BUFFER_LEFT, 0, LCD_W*LCD_H*2);
	memset(PSRAM_LCD_FRAME_BUFFER_RIGHT, 0, LCD_W*LCD_H*2);
	
	for(uint32_t i = 0; i < LCD_W*LCD_H; i++)PSRAM_LCD_FRAME_BUFFER_ORIGIN[i]   = color[1];//clear
	for(uint32_t i = 0; i < LCD_W*LCD_H; i++)PSRAM_LCD_FRAME_BUFFER_LEFT[i]   = color[2];//clear
	for(uint32_t i = 0; i < LCD_W*LCD_H; i++)PSRAM_LCD_FRAME_BUFFER_RIGHT[i]   = color[3];//clear
	uint16_t color_temp=0;
	
 
}

static uint32_t change_count=0;

static void user_display(void)
{
	 
	 if(change_count++ >= 100) 
	{
		change_count = 0;
		
		 switch(display_page)
		{
			case 0://
				psram = PSRAM_LCD_FRAME_BUFFER_ORIGIN;
			
			break;
			case 1://
				psram = PSRAM_LCD_FRAME_BUFFER_LEFT;

			break;
			case 2://
				//for(uint32_t i = 0; i < LCD_W*LCD_H*2; i++)PSRAM_LCD_FRAME_BUFFER_RIGHT[i]   = color[1];//clear
				psram = PSRAM_LCD_FRAME_BUFFER_RIGHT;
				 
			break;
		}
		display_page++;
		if(display_page == 3)display_page=0;
		uint32_t time = system_get_curr_time();		
		
		 __SPI_DATA_FRAME_SIZE(st7365_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
		
		 st7365_set_window(0, LCD_W-1,0, LCD_H-1);	 
		 st7365_display((320*480),(void*)psram,NULL);
		 st7365_display_wait_transfer_done();
		//send_data_disp((uint8_t*)psram,LCD_W*LCD_H*2);	
//		 __SPI_DATA_FRAME_SIZE(st7365_SPI_Handle.SPIx, SPI_FRAME_SIZE_16BIT);
		co_printf("time:%d\r\n",(system_get_curr_time()-time));
		co_printf("display_page%d\r\n",display_page);
	}

		
 
}
#endif


/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void st7365_Clear(uint16_t Color)
{
	uint16_t i,j;  	
	st7365_set_window(0,LCD_W-1,0,LCD_H-1);
	for(i=0;i<LCD_W;i++)
	{
	  for (j=0;j<LCD_H;j++)
	   	{
        	st7365_write_16bit_data(Color);	 			 
	    }
 
	 }
}

 
void st7365_init(void)
{

    st7365_init_io();
    st7365_init_spi();
    st7365_init_dma();
    
	 LCD_RELEASE_RESET();
	co_delay_100us(10);
	 LCD_SET_RESET();
	co_delay_100us(500);
	LCD_RELEASE_RESET();
	co_delay_100us(1200);
    LCD_SET_CS();

	#if 1
	st7365_write_cmd(0x11);     

	co_delay_100us(1200);                //ms

	st7365_write_cmd(0xF0);     
	st7365_write_data(0xC3);   

	st7365_write_cmd(0xF0);     
	st7365_write_data(0x96);   

	st7365_write_cmd(0x36);     
	st7365_write_data(0x48);   

	st7365_write_cmd(0x3A);     
	st7365_write_data(0x55);   

	st7365_write_cmd(0xB0);
	st7365_write_data(0x80);

	st7365_write_cmd(0xB1);
	st7365_write_data(0x80);	
	st7365_write_data(0x10);	
	
	st7365_write_cmd(0xB4);     //1-dot Inversion
	st7365_write_data(0x01);   

	st7365_write_cmd(0xB7);     
	st7365_write_data(0xC6);

	st7365_write_cmd(0xC0);     
	st7365_write_data(0x80);   
	st7365_write_data(0x64); //VGH=15V VGL=-10V  

	st7365_write_cmd(0xC1);     
	st7365_write_data(0x13);  //VOP=4.5V

	st7365_write_cmd(0xC2);     
	st7365_write_data(0xA7);   

	st7365_write_cmd(0xC5);     
	st7365_write_data(0x08);   

	st7365_write_cmd(0xE8);     
	st7365_write_data(0x40);   
	st7365_write_data(0x8a);   
	st7365_write_data(0x00);   
	st7365_write_data(0x00);   
	st7365_write_data(0x29);   
	st7365_write_data(0x19);   
	st7365_write_data(0xA5);   
	st7365_write_data(0x33);   
	st7365_write_cmd(0xE0);
	st7365_write_data(0xF0);
	st7365_write_data(0x06);
	st7365_write_data(0x0B);
	st7365_write_data(0x07);
	st7365_write_data(0x06);
	st7365_write_data(0x05);
	st7365_write_data(0x2E);
	st7365_write_data(0x33);
	st7365_write_data(0x47);
	st7365_write_data(0x3A);
	st7365_write_data(0x17);
	st7365_write_data(0x16);
	st7365_write_data(0x2E);
	st7365_write_data(0x31);

	st7365_write_cmd(0xE1);
	st7365_write_data(0xF0);
	st7365_write_data(0x09);
	st7365_write_data(0x0D);
	st7365_write_data(0x09);
	st7365_write_data(0x08);
	st7365_write_data(0x23);
	st7365_write_data(0x2E);
	st7365_write_data(0x33);
	st7365_write_data(0x46);
	st7365_write_data(0x38);
	st7365_write_data(0x13);
	st7365_write_data(0x13);
	st7365_write_data(0x2C);
	st7365_write_data(0x32);

	st7365_write_cmd(0xF0);     
	st7365_write_data(0x3C);   

	st7365_write_cmd(0xF0);     
	st7365_write_data(0x69);   

	st7365_write_cmd(0x35);     
	st7365_write_data(0x00); 

	st7365_write_cmd(0x21); 

	st7365_write_cmd(0x29);     
	co_delay_100us(500); 

	st7365_write_cmd(0x2A);    //320 
	st7365_write_data(0x00);   
	st7365_write_data(0x00);   
	st7365_write_data(0x01);   
	st7365_write_data(0x3F);   

	st7365_write_cmd(0x2B);    //480
	st7365_write_data(0x00);   
	st7365_write_data(0x00);   
	st7365_write_data(0x01);   
	st7365_write_data(0xDF); 

	st7365_write_cmd(0x2C); 
	
	#else
	//************* ST7796S初始化**********//	
	st7365_write_cmd(0xF0);
	st7365_write_data(0xC3);
	st7365_write_cmd(0xF0);
	st7365_write_data(0x96);
	st7365_write_cmd(0x36);
	st7365_write_data(0x68);	
	st7365_write_cmd(0x3A);
	st7365_write_data(0x05);	
	st7365_write_cmd(0xB0);
	st7365_write_data(0x80);	
	st7365_write_cmd(0xB6);
	st7365_write_data(0x00);
	st7365_write_data(0x02);	
	st7365_write_cmd(0xB5);
	st7365_write_data(0x02);
	st7365_write_data(0x03);
	st7365_write_data(0x00);
	st7365_write_data(0x04);
	st7365_write_cmd(0xB1);
	st7365_write_data(0x80);	
	st7365_write_data(0x10);	
	st7365_write_cmd(0xB4);
	st7365_write_data(0x00);
	st7365_write_cmd(0xB7);
	st7365_write_data(0xC6);
	st7365_write_cmd(0xC5);
	st7365_write_data(0x24);
	st7365_write_cmd(0xE4);
	st7365_write_data(0x31);
	st7365_write_cmd(0xE8);
	st7365_write_data(0x40);
	st7365_write_data(0x8A);
	st7365_write_data(0x00);
	st7365_write_data(0x00);
	st7365_write_data(0x29);
	st7365_write_data(0x19);
	st7365_write_data(0xA5);
	st7365_write_data(0x33);
	st7365_write_cmd(0xC2);
	st7365_write_cmd(0xA7);
	
	st7365_write_cmd(0xE0);
	st7365_write_data(0xF0);
	st7365_write_data(0x09);
	st7365_write_data(0x13);
	st7365_write_data(0x12);
	st7365_write_data(0x12);
	st7365_write_data(0x2B);
	st7365_write_data(0x3C);
	st7365_write_data(0x44);
	st7365_write_data(0x4B);
	st7365_write_data(0x1B);
	st7365_write_data(0x18);
	st7365_write_data(0x17);
	st7365_write_data(0x1D);
	st7365_write_data(0x21);

	st7365_write_cmd(0XE1);
	st7365_write_data(0xF0);
	st7365_write_data(0x09);
	st7365_write_data(0x13);
	st7365_write_data(0x0C);
	st7365_write_data(0x0D);
	st7365_write_data(0x27);
	st7365_write_data(0x3B);
	st7365_write_data(0x44);
	st7365_write_data(0x4D);
	st7365_write_data(0x0B);
	st7365_write_data(0x17);
	st7365_write_data(0x17);
	st7365_write_data(0x1D);
	st7365_write_data(0x21);

  st7365_write_cmd(0X36);
	st7365_write_data(0xEC);
	st7365_write_cmd(0xF0);
	st7365_write_data(0xC3);
	st7365_write_cmd(0xF0);
	st7365_write_data(0x69);
	st7365_write_cmd(0X13);
	st7365_write_cmd(0X11);
	st7365_write_cmd(0X29);
 
#endif
//   st7365_Clear(0x001f);
//   psram_frame_buffer_init();
// 
//	while(1)
//	{
//		user_display();
//		co_delay_100us(100);
//	}
}

void st7365_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
	   
	st7365_write_cmd(0x2a);//列地址设置
	st7365_write_16bit_data(x_s);
	st7365_write_16bit_data(x_e);
	st7365_write_cmd(0x2b);//行地址设置
	st7365_write_16bit_data(y_s);
	st7365_write_16bit_data(y_e);
    st7365_write_cmd(0x2c);//储存器写
}

void st7365_display_wait_transfer_done(void)
{
    while(dma_transfer_done == false);
}

void st7365_display(uint32_t pixel_count, uint16_t *data, void (*callback)(void))
{
	#if 1
	  uint32_t i;
    uint32_t total_count = pixel_count / 2;   // accoding source width
    uint8_t link_count = total_count / 4000;
    
    if(dma_transfer_done == false)
    {
        return;
    }
    else
    {
        dma_transfer_done = false;
    }
    
    dma_trans_done_callback = callback;
    
    if(total_count % 4000)
    {
        link_count++;
    }
    
    for (i = 0; i < link_count; i++)
    {
        uint8_t all_set = (total_count <= 4000);
        
        LinkParameter.SrcAddr          = (uint32_t)&data[i * 8000];
        LinkParameter.DstAddr          = (uint32_t)&st7365_SPI_Handle.SPIx->DR;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = st7365_DMA_Channel.Init.Request_ID;
        LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_8;
        LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LinkParameter.Size             = all_set ? (total_count) : 4000;
        LinkParameter.gather_enable    = 0;
        LinkParameter.scatter_enable   = 0;
        total_count -= 4000;

        dma_linked_list_init(&Link_Channel[i], &LinkParameter);
    }
//	
	//__SPI_DATA_FRAME_SIZE(st7365_SPI_Handle.SPIx,SPI_FRAME_SIZE_8BIT);
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&st7365_SPI_Handle);
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, st7365_DMA_Channel.Channel);
		#endif
 
}

 

__attribute__((section("ram_code"))) void st7365_dma_isr(void)
{
    void (*callback)();
    while(__SPI_IS_BUSY(st7365_SPI_Handle.SPIx));
	//	printf("gc9c01_dma_isr\r\n");
    // CS Release
    LCD_RELEASE_CS();
	
    /* Clear Transfer complete status */
    dma_clear_tfr_Status(st7365_DMA_Channel.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(st7365_DMA_Channel.Channel);

    __SPI_DISABLE(st7365_SPI_Handle.SPIx);
 //   __SPI_DATA_FRAME_SIZE(st7365_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
    
    dma_transfer_done = true;
    callback = dma_trans_done_callback;
    dma_trans_done_callback = NULL;
    if(callback) {
        callback();
    }
}

 