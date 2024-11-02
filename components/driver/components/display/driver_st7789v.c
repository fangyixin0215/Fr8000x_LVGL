
/*
@driver_st7789v.c
@author: LH 
Creation Date：2023/05/11
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "driver_st7789v.h"

#include "sys_utils.h"

#define ST7789_240X280		0
#define ST7789_135X240		0
#define ST7789_240X320		1
#define DEVELOP_FR8008GP_BOARD 1

#ifdef DEVELOP_FR8008GP_BOARD

#if (ST7789_135X240==1)
#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_B, GPIO_PIN_6, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_B, GPIO_PIN_6, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_D, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_D, GPIO_PIN_5, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_CLEAR)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_W 135
#define LCD_H 240

#elif (ST7789_240X280==1)
#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_CLEAR)
#define LCD_W 240
#define LCD_H 280
#elif (ST7789_240X320==1)
#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_CLEAR)
#define LCD_W 240
#define LCD_H 320

#endif

#endif

#ifdef DEVELOP_DEBUG_BOARD

#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_CLEAR)
#endif

void st7789v_display_wait_transfer_done(void);
static DMA_LLI_InitTypeDef Link_Channel[30];
static DMA_HandleTypeDef st7789v_DMA_Channel;
static dma_LinkParameter_t LinkParameter;
static volatile bool dma_transfer_done = true;

SPI_HandleTypeDef ST7789V_SPI_Handle;

static void (*dma_trans_done_callback)(void) = NULL;
 

 

static void st7789v_init_io(void)
{
    GPIO_InitTypeDef GPIO_Handle;

    // backlight
//    GPIO_Handle.Pin       = GPIO_PIN_1;
//    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
//    gpio_init(GPIO_E, &GPIO_Handle);
#if DEVELOP_DEBUG_BOARD
    // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_A, &GPIO_Handle);

    

    GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_A, &GPIO_Handle);
#endif
	#if DEVELOP_FR8008GP_BOARD
	#if (ST7789_135X240==1)
    // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_B, &GPIO_Handle);

    GPIO_Handle.Pin       = GPIO_PIN_5;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_D, &GPIO_Handle);

    GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
	#elif (ST7789_240X280==1)
	 // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_3;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_B, &GPIO_Handle);

    

    GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
	#elif (ST7789_240X320==1)
	 // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_A, &GPIO_Handle);
	
 	 //Backlight
    GPIO_Handle.Pin       = GPIO_PIN_4;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_B, &GPIO_Handle);   

    GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_A, &GPIO_Handle);
	#endif
	
#endif
	
    LCD_RELEASE_CS();
    LCD_ENABLE_BACKLIGHT();
//		LCD_DISABLE_BACKLIGHT();
  //init_lcd_pwm();	
	
}

static void st7789v_init_spi(void)
{
    __SYSTEM_SPI0_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI0_MASTER_CLK_ENABLE();
    
    ST7789V_SPI_Handle.SPIx                       = SPIM0;
    ST7789V_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    ST7789V_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    ST7789V_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    ST7789V_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    ST7789V_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&ST7789V_SPI_Handle);
}

static void st7789v_init_dma(void)
{    
    __SYSTEM_DMA_CLK_ENABLE();
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    
    st7789v_DMA_Channel.Channel = DMA_Channel0;
    st7789v_DMA_Channel.Init.Data_Flow        = DMA_M2P_DMAC;
    st7789v_DMA_Channel.Init.Request_ID       = 1;
    st7789v_DMA_Channel.Init.Source_Inc       = DMA_ADDR_INC_INC;
    st7789v_DMA_Channel.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    st7789v_DMA_Channel.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    st7789v_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&st7789v_DMA_Channel);

    NVIC_EnableIRQ(DMA_IRQn);
}

static void st7789v_write_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];

    spi_data[0]=cmd;
		LCD_SET_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&ST7789V_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();

}

static void st7789v_write_data(uint8_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data;
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&ST7789V_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();
}

static void st7789_write_16bit_data(uint16_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data>>8;
	spi_data[1]=data;
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&ST7789V_SPI_Handle, (uint16_t *)spi_data, 2);
    LCD_RELEASE_CS();
}


static void send_data_disp(uint8_t *pdata,uint32_t len)
{
	
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&ST7789V_SPI_Handle, (uint16_t *)pdata, len);
	  LCD_RELEASE_CS();
}


static void send_16data_disp(uint16_t *pdata,uint32_t len)
{
	 LCD_RELEASE_DC();
   LCD_SET_CS();
   spi_master_transmit_X1(&ST7789V_SPI_Handle, (uint16_t *)pdata, len);
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
		 st7789v_display_wait_transfer_done();
		 __SPI_DATA_FRAME_SIZE(ST7789V_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
		st7789v_set_window(0, LCD_W-1,0, LCD_H-1);	 
		 
		send_data_disp((uint8_t*)psram,LCD_W*LCD_H*2);	
//		 __SPI_DATA_FRAME_SIZE(ST7789V_SPI_Handle.SPIx, SPI_FRAME_SIZE_16BIT);
//		send_16data_disp(psram,240*240);
		
		//gc9c01_display(240*240,psram,NULL);
		co_printf("display_page%d\r\n",display_page);
 
	}

		
 
}
#endif


/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void st7789v_Clear(uint16_t Color)
{
	uint16_t i,j;  	
	st7789v_set_window(0,LCD_W-1,0,LCD_H-1);
  for(i=0;i<LCD_W;i++)
	{
	  for (j=0;j<LCD_H;j++)
	   	{
        	st7789_write_16bit_data(Color);	 			 
	    }
 
	 }
}

 
void st7789v_init(void)
{

    st7789v_init_io();
    st7789v_init_spi();
    st7789v_init_dma();
    
//    LCD_RELEASE_RESET();
//    co_delay_100us(1200);
//    LCD_SET_RESET();
//    co_delay_100us(3500);
//    LCD_RELEASE_RESET();
//    co_delay_100us(1200);
    //LCD_ENABLE_BACKLIGHT();
	
	 LCD_RELEASE_RESET();
	co_delay_100us(10);
	 LCD_SET_RESET();
	co_delay_100us(500);
	LCD_RELEASE_RESET();
	
	 
	co_delay_100us(1200);
  LCD_SET_CS();
 
	
#if (ST7789_240X280==1)
st7789v_write_cmd(0x11);     
co_delay_100us(1200);                

st7789v_write_cmd(0x36);     
st7789v_write_data(0x00);   
st7789v_write_cmd(0x21); 

st7789v_write_cmd(0x3A);     
st7789v_write_data(0x05);   //565

st7789v_write_cmd(0xB2);     
st7789v_write_data(0x0B);   
st7789v_write_data(0x0B);   
st7789v_write_data(0x00);   
st7789v_write_data(0x33);   
st7789v_write_data(0x33);   

st7789v_write_cmd(0xB7);     
st7789v_write_data(0x11);   

st7789v_write_cmd(0xBB);     
st7789v_write_data(0x2F);   

st7789v_write_cmd(0xC0);     
st7789v_write_data(0x2C);   

st7789v_write_cmd(0xC2);     
st7789v_write_data(0x01);   

st7789v_write_cmd(0xC3);     
st7789v_write_data(0x0D);   

st7789v_write_cmd(0xC4);     
st7789v_write_data(0x20);   //VDV, 0x20:0v

st7789v_write_cmd(0xC6);     
st7789v_write_data(0x13);   //0x13:60Hz   

st7789v_write_cmd(0xD0);     
st7789v_write_data(0xA4);   
st7789v_write_data(0xA1);   

st7789v_write_cmd(0xD6);     
st7789v_write_data(0xA1);   //sleep in后，gate输出为GND

st7789v_write_cmd(0xE0);     
st7789v_write_data(0xF0);   
st7789v_write_data(0x04);   
st7789v_write_data(0x07);   
st7789v_write_data(0x09);   
st7789v_write_data(0x07);   
st7789v_write_data(0x13);   
st7789v_write_data(0x25);   
st7789v_write_data(0x33);   
st7789v_write_data(0x3C);   
st7789v_write_data(0x34);   
st7789v_write_data(0x10);   
st7789v_write_data(0x10);   
st7789v_write_data(0x29);   
st7789v_write_data(0x32);   

st7789v_write_cmd(0xE1);     
st7789v_write_data(0xF0);   
st7789v_write_data(0x05);   
st7789v_write_data(0x08);   
st7789v_write_data(0x0A);   
st7789v_write_data(0x09);   
st7789v_write_data(0x05);   
st7789v_write_data(0x25);   
st7789v_write_data(0x32);   
st7789v_write_data(0x3B);   
st7789v_write_data(0x3B);   
st7789v_write_data(0x17);   
st7789v_write_data(0x18);   
st7789v_write_data(0x2E);   
st7789v_write_data(0x37);   

st7789v_write_cmd(0xE4);     
st7789v_write_data(0x25);   //使用240根gate  (N+1)*8
st7789v_write_data(0x00);   //设定gate起点位置
st7789v_write_data(0x00);   //当gate没有用完时，bit4(TMG)设为0

st7789v_write_cmd(0x21);     

st7789v_write_cmd(0x29);     

st7789v_write_cmd(0x2A);     //Column Address Set
st7789v_write_data(0x00);   
st7789v_write_data(0x00);   //0
st7789v_write_data(0x00);   
st7789v_write_data(0xEF);   //239

st7789v_write_cmd(0x2B);     //Row Address Set
st7789v_write_data(0x00);   
st7789v_write_data(0x14);   //0
st7789v_write_data(0x01);   
st7789v_write_data(0x2B);   


st7789v_write_cmd(0x2C);    
#endif

#if (ST7789_240X320==1)
st7789v_write_cmd(0x11);     //Sleep out

	co_delay_100us(1200);                //Delayms 120ms

	st7789v_write_cmd(0x36);     
	st7789v_write_data(0xa0);   

	st7789v_write_cmd(0x21);     

	st7789v_write_cmd(0xB2);     
	st7789v_write_data(0x05);   
	st7789v_write_data(0x05);   
	st7789v_write_data(0x00);   
	st7789v_write_data(0x33);   
	st7789v_write_data(0x33);   

	st7789v_write_cmd(0xB7);     
	st7789v_write_data(0x75);   

	st7789v_write_cmd(0xBB);     
	st7789v_write_data(0x22);   

	st7789v_write_cmd(0xC0);     
	st7789v_write_data(0x2C);   

	st7789v_write_cmd(0xC2);     
	st7789v_write_data(0x01);   

	st7789v_write_cmd(0xC3);     
	st7789v_write_data(0x13);   

	st7789v_write_cmd(0xC4);     
	st7789v_write_data(0x20);   

	st7789v_write_cmd(0xC6);     
	st7789v_write_data(0x11);   

	st7789v_write_cmd(0xD0);     
	st7789v_write_data(0xA4);   
	st7789v_write_data(0xA1);   

	st7789v_write_cmd(0xD6);     
	st7789v_write_data(0xA1);   

	st7789v_write_cmd(0xE0);     
	st7789v_write_data(0xD0);   
	st7789v_write_data(0x05);   
	st7789v_write_data(0x0A);   
	st7789v_write_data(0x09);   
	st7789v_write_data(0x08);   
	st7789v_write_data(0x05);   
	st7789v_write_data(0x2E);   
	st7789v_write_data(0x44);   
	st7789v_write_data(0x45);   
	st7789v_write_data(0x0F);   
	st7789v_write_data(0x17);   
	st7789v_write_data(0x16);   
	st7789v_write_data(0x2B);   
	st7789v_write_data(0x33);   

	st7789v_write_cmd(0xE1);     
	st7789v_write_data(0xD0);   
	st7789v_write_data(0x05);   
	st7789v_write_data(0x0A);   
	st7789v_write_data(0x09);   
	st7789v_write_data(0x08);   
	st7789v_write_data(0x05);   
	st7789v_write_data(0x2E);   
	st7789v_write_data(0x43);   
	st7789v_write_data(0x45);   
	st7789v_write_data(0x0F);   
	st7789v_write_data(0x16);   
	st7789v_write_data(0x16);   
	st7789v_write_data(0x2B);   
	st7789v_write_data(0x33);   

	st7789v_write_cmd(0x3A);   
	st7789v_write_data(0x55); 
	
	//st7789v_Clear(0x001f); //蓝色
	//st7789v_Clear(0xf800); //红色
	
	st7789v_Clear(0x0000); //黑色
	co_delay_100us(5000);				//Delay 50ms
	st7789v_write_cmd(0x29);	 
	co_delay_100us(500);				//Delay 50ms

#endif

#if (ST7789_135X240==1)

		st7789v_write_cmd(0x11);     //Sleep out

		co_delay_100us(1200);               //Delay 120ms

		st7789v_write_cmd(0x3A);     
		st7789v_write_data(0x05);   //565

		st7789v_write_cmd(0x36);     
		st7789v_write_data(0x00);   

		st7789v_write_cmd(0x21);     

		st7789v_write_cmd(0xB2);     
		st7789v_write_data(0x05);   
		st7789v_write_data(0x05);   
		st7789v_write_data(0x00);   
		st7789v_write_data(0x33);   
		st7789v_write_data(0x33);   

		st7789v_write_cmd(0xB7);     
		st7789v_write_data(0x74);   

		st7789v_write_cmd(0xBB);     
		st7789v_write_data(0x2A);   

		st7789v_write_cmd(0xC0);     
		st7789v_write_data(0x2C);   

		st7789v_write_cmd(0xC2);     
		st7789v_write_data(0x01);   

		st7789v_write_cmd(0xC3);     
		st7789v_write_data(0x13);   

		st7789v_write_cmd(0xC4);     
		st7789v_write_data(0x20);   

		st7789v_write_cmd(0xC6);     
		st7789v_write_data(0x0F);   

		st7789v_write_cmd(0xD0);     
		st7789v_write_data(0xA4);   
		st7789v_write_data(0xA1);   

		st7789v_write_cmd(0xD6);     
		st7789v_write_data(0xA1);   

		st7789v_write_cmd(0xE0);
		st7789v_write_data(0x70);
		st7789v_write_data(0x0B);
		st7789v_write_data(0x10);
		st7789v_write_data(0x0B);
		st7789v_write_data(0x0A);
		st7789v_write_data(0x26);
		st7789v_write_data(0x35);
		st7789v_write_data(0x44);
		st7789v_write_data(0x4C);
		st7789v_write_data(0x38);
		st7789v_write_data(0x17);
		st7789v_write_data(0x17);
		st7789v_write_data(0x2F);
		st7789v_write_data(0x2F);

		st7789v_write_cmd(0xE1);
		st7789v_write_data(0x70);
		st7789v_write_data(0x03);
		st7789v_write_data(0x08);
		st7789v_write_data(0x09);
		st7789v_write_data(0x08);
		st7789v_write_data(0x04);
		st7789v_write_data(0x2E);
		st7789v_write_data(0x22);
		st7789v_write_data(0x47);
		st7789v_write_data(0x39);
		st7789v_write_data(0x18);
		st7789v_write_data(0x19);
		st7789v_write_data(0x2D);
		st7789v_write_data(0x30);

		st7789v_write_cmd(0x29);     //Display on
		co_delay_100us(500); 
		st7789v_write_cmd(0x2C); 

//st7789v_write_cmd(0x11);     
//co_delay_100us(1200);                

//st7789v_write_cmd(0x36);     
//st7789v_write_data(0x00);   
//st7789v_write_cmd(0x21); 

//st7789v_write_cmd(0x3A);     
//st7789v_write_data(0x05);   //565

//st7789v_write_cmd(0xB2);     
//st7789v_write_data(0x0B);   
//st7789v_write_data(0x0B);   
//st7789v_write_data(0x00);   
//st7789v_write_data(0x33);   
//st7789v_write_data(0x33);   

//st7789v_write_cmd(0xB7);     
//st7789v_write_data(0x11);   

//st7789v_write_cmd(0xBB);     
//st7789v_write_data(0x2F);   

//st7789v_write_cmd(0xC0);     
//st7789v_write_data(0x2C);   

//st7789v_write_cmd(0xC2);     
//st7789v_write_data(0x01);   

//st7789v_write_cmd(0xC3);     
//st7789v_write_data(0x0D);   

//st7789v_write_cmd(0xC4);     
//st7789v_write_data(0x20);   //VDV, 0x20:0v

//st7789v_write_cmd(0xC6);     
//st7789v_write_data(0x13);   //0x13:60Hz   

//st7789v_write_cmd(0xD0);     
//st7789v_write_data(0xA4);   
//st7789v_write_data(0xA1);   

//st7789v_write_cmd(0xD6);     
//st7789v_write_data(0xA1);   //sleep in后，gate输出为GND

//st7789v_write_cmd(0xE0);     
//st7789v_write_data(0xF0);   
//st7789v_write_data(0x04);   
//st7789v_write_data(0x07);   
//st7789v_write_data(0x09);   
//st7789v_write_data(0x07);   
//st7789v_write_data(0x13);   
//st7789v_write_data(0x25);   
//st7789v_write_data(0x33);   
//st7789v_write_data(0x3C);   
//st7789v_write_data(0x34);   
//st7789v_write_data(0x10);   
//st7789v_write_data(0x10);   
//st7789v_write_data(0x29);   
//st7789v_write_data(0x32);   

//st7789v_write_cmd(0xE1);     
//st7789v_write_data(0xF0);   
//st7789v_write_data(0x05);   
//st7789v_write_data(0x08);   
//st7789v_write_data(0x0A);   
//st7789v_write_data(0x09);   
//st7789v_write_data(0x05);   
//st7789v_write_data(0x25);   
//st7789v_write_data(0x32);   
//st7789v_write_data(0x3B);   
//st7789v_write_data(0x3B);   
//st7789v_write_data(0x17);   
//st7789v_write_data(0x18);   
//st7789v_write_data(0x2E);   
//st7789v_write_data(0x37);   

//st7789v_write_cmd(0xE4);     
//st7789v_write_data(0x25);   //使用240根gate  (N+1)*8
//st7789v_write_data(0x00);   //设定gate起点位置
//st7789v_write_data(0x00);   //当gate没有用完时，bit4(TMG)设为0

//st7789v_write_cmd(0x21);     

//st7789v_write_cmd(0x29);     

//st7789v_write_cmd(0x2A);     //Column Address Set
//st7789v_write_data(0x00);   
//st7789v_write_data(0x00);   //0
//st7789v_write_data(0x00);   
//st7789v_write_data(0xEF);   //239

//st7789v_write_cmd(0x2B);     //Row Address Set
//st7789v_write_data(0x00);   
//st7789v_write_data(0x14);   //0
//st7789v_write_data(0x01);   
//st7789v_write_data(0x2B);   


//st7789v_write_cmd(0x2C);     

#endif
	

  // st7789v_Clear(0x400);
//	psram_frame_buffer_init();
//		while(1)
//		{
//		  user_display();
//			co_delay_100us(100);
//		}
}

void st7789v_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
	   
	st7789v_write_cmd(0x2a);//列地址设置


	#if (ST7789_240X280==1)
	st7789_write_16bit_data(x_s);
	st7789_write_16bit_data(x_e);
	st7789v_write_cmd(0x2b);//行地址设置
	st7789_write_16bit_data(y_s+0x14);
	st7789_write_16bit_data(y_e+0x14);
	#endif
	
	#if (ST7789_240X320==1)
	st7789_write_16bit_data(x_s);
	st7789_write_16bit_data(x_e);
	st7789v_write_cmd(0x2b);//行地址设置
	st7789_write_16bit_data(y_s);
	st7789_write_16bit_data(y_e);
	#endif	
	
	#if (ST7789_135X240==1)
	st7789_write_16bit_data(x_s+0x34);
	st7789_write_16bit_data(x_e+0x34);
	st7789v_write_cmd(0x2b);//行地址设置
	st7789_write_16bit_data(y_s+0x28);
	st7789_write_16bit_data(y_e+0x28);
	#endif
    st7789v_write_cmd(0x2c);//储存器写
}

void st7789v_display_wait_transfer_done(void)
{
    while(dma_transfer_done == false);
}

void st7789v_display(uint32_t pixel_count, uint16_t *data, void (*callback)(void))
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
        LinkParameter.DstAddr          = (uint32_t)&SPIM0->DR;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = st7789v_DMA_Channel.Init.Request_ID;
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
	//__SPI_DATA_FRAME_SIZE(ST7789V_SPI_Handle.SPIx,SPI_FRAME_SIZE_8BIT);
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&ST7789V_SPI_Handle);
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, st7789v_DMA_Channel.Channel);
		#endif
	#if 0
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
        LinkParameter.DstAddr          = (uint32_t)&SPIM0->DR;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = st7789v_DMA_Channel.Init.Request_ID;
        LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_16;
        LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LinkParameter.Size             = all_set ? (total_count) : 4000;
        LinkParameter.gather_enable    = 0;
        LinkParameter.scatter_enable   = 0;
        total_count -= 4000;

        dma_linked_list_init(&Link_Channel[i], &LinkParameter);
    }

    ST7789V_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
    ST7789V_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
    ST7789V_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
    ST7789V_SPI_Handle.MultWireParam.Instruct       = 0x32;
    ST7789V_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
    ST7789V_SPI_Handle.MultWireParam.Address        = 0x003C00;
    

	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X2X4_DMA(&ST7789V_SPI_Handle);
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, st7789v_DMA_Channel.Channel);
		#endif
		
		//send_data_disp((uint8_t*)data,240*240*2);
}

void st7789v_display_gather(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count)
{
#define DMA_TRANSFER_SIZE       4000
    uint32_t total_count = pixel_count / 2;   // accoding source width
    uint16_t line_count_in_single_list = DMA_TRANSFER_SIZE/(count/2);
    uint16_t src_width_count_in_single_list = line_count_in_single_list*(count/2);
    uint8_t link_count, i;
    
    if(dma_transfer_done == false)
    {
        return;
    }
    else
    {
        dma_transfer_done = false;
    }
    
    link_count = total_count / src_width_count_in_single_list;
    if(total_count % src_width_count_in_single_list)
    {
        link_count++;
    }
    
    for (i = 0; i < link_count; i++)
    {
        uint8_t all_set = (total_count <= src_width_count_in_single_list);
        
        LinkParameter.SrcAddr          = (uint32_t)&data[i * interval * line_count_in_single_list];
        LinkParameter.DstAddr          = (uint32_t)&SPIM1->DR;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = st7789v_DMA_Channel.Init.Request_ID;
        LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_16;
        LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LinkParameter.Size             = all_set ? (total_count) : src_width_count_in_single_list;
        LinkParameter.gather_enable    = 1;
        LinkParameter.scatter_enable   = 0;
        total_count -= src_width_count_in_single_list;

        dma_linked_list_init(&Link_Channel[i], &LinkParameter);
    }
    
//    ST7789V_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
//    ST7789V_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
//    ST7789V_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
//    ST7789V_SPI_Handle.MultWireParam.Instruct       = 0x32;
//    ST7789V_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
//    ST7789V_SPI_Handle.MultWireParam.Address        = 0x003C00;
    __SPI_DATA_FRAME_SIZE(ST7789V_SPI_Handle.SPIx,SPI_FRAME_SIZE_16BIT);
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&ST7789V_SPI_Handle);

    __DMA_GATHER_FUNC_ENABLE(st7789v_DMA_Channel.Channel);
    __DMA_GATHER_INTERVAL(st7789v_DMA_Channel.Channel, (interval-count)/2);
    __DMA_GATHER_COUNT(st7789v_DMA_Channel.Channel, count/2);
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, st7789v_DMA_Channel.Channel);
}

__attribute__((section("ram_code"))) void st7789v_dma_isr(void)
{
    void (*callback)();
    while(__SPI_IS_BUSY(ST7789V_SPI_Handle.SPIx));
	//	printf("gc9c01_dma_isr\r\n");
    // CS Release
    LCD_RELEASE_CS();
	
    /* Clear Transfer complete status */
    dma_clear_tfr_Status(st7789v_DMA_Channel.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(st7789v_DMA_Channel.Channel);

    __SPI_DISABLE(ST7789V_SPI_Handle.SPIx);
 //   __SPI_DATA_FRAME_SIZE(ST7789V_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
    
    dma_transfer_done = true;
    callback = dma_trans_done_callback;
    dma_trans_done_callback = NULL;
    if(callback) {
        callback();
    }
}

