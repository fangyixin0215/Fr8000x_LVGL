
/*
@driver_gc9a01.c
@author: LH 
Creation Date£º2022/10/11
*/

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "driver_gc9a01.h"

#include "sys_utils.h"

uint16_t data[360*20]={0};

#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_A, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_A, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_CLEAR)




static DMA_LLI_InitTypeDef Link_Channel[30];
static DMA_HandleTypeDef gc9a01_DMA_Channel;
static dma_LinkParameter_t LinkParameter;
static volatile bool dma_transfer_done = true;

SPI_HandleTypeDef GC9A01_SPI_Handle;

static void (*dma_trans_done_callback)(void) = NULL;
 

void lcd_backlight_open(void)
{
	LCD_ENABLE_BACKLIGHT()   ;
}

static void gc9a01_init_io(void)
{
    GPIO_InitTypeDef GPIO_Handle;

    // backlight
    GPIO_Handle.Pin       = GPIO_PIN_1;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_E, &GPIO_Handle);

    // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_A, &GPIO_Handle);

    

    GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;//GPIO_PULLDOWN;   GPIO_PULLUP
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_A, &GPIO_Handle);

    LCD_RELEASE_CS();
    
		LCD_DISABLE_BACKLIGHT();
  //init_lcd_pwm();	
	
}

static void gc9a01_init_spi(void)
{
    __SYSTEM_SPI0_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI0_MASTER_CLK_ENABLE();
    
    GC9A01_SPI_Handle.SPIx                       = SPIM0;
    GC9A01_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    GC9A01_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    GC9A01_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    GC9A01_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    GC9A01_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&GC9A01_SPI_Handle);
}

static void gc9a01_init_dma(void)
{    
    __SYSTEM_DMA_CLK_ENABLE();
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    
    gc9a01_DMA_Channel.Channel = DMA_Channel0;
    gc9a01_DMA_Channel.Init.Data_Flow        = DMA_M2P_DMAC;
    gc9a01_DMA_Channel.Init.Request_ID       = 1;
    gc9a01_DMA_Channel.Init.Source_Inc       = DMA_ADDR_INC_INC;
    gc9a01_DMA_Channel.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    gc9a01_DMA_Channel.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    gc9a01_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&gc9a01_DMA_Channel);

    NVIC_EnableIRQ(DMA_IRQn);
}

static void gc9a01_write_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];

    spi_data[0]=cmd;
		LCD_SET_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&GC9A01_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();

}
unsigned char ReverseByteBits(unsigned char num)
{
	  #if 0
    unsigned char ret=0;
    char i=0;
    for(i=0;i<8;++i)
    {
        ret <<=1;
        ret |=num&1;
        num >>=1;
    }
    return ret;
		#endif
	  num = (((num & 0xaa) >> 1) | ((num & 0x55) << 1));
    num = (((num & 0xcc) >> 2) | ((num & 0x33) << 2));
    
    return ((num >> 4) | (num << 4));
		
}
static void gc9a01_write_data(uint8_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data;
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&GC9A01_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();
}

static void gc9a01_write_data16(uint16_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data>>8;
	spi_data[1]=data;
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&GC9A01_SPI_Handle, (uint16_t *)spi_data, 2);
    LCD_RELEASE_CS();
}


void send_data_disp(uint8_t *pdata,uint32_t len)
{
	
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&GC9A01_SPI_Handle, (uint16_t *)pdata, len);
	  LCD_RELEASE_CS();
}


void send_16data_disp(uint16_t *pdata,uint32_t len)
{
	 LCD_RELEASE_DC();
   LCD_SET_CS();
   spi_master_transmit_X1(&GC9A01_SPI_Handle, (uint16_t *)pdata, len);
	 LCD_RELEASE_CS();
	
}

void gc9a01_init(void)
{

    gc9a01_init_io();
    gc9a01_init_spi();
    gc9a01_init_dma();
    
    LCD_RELEASE_RESET();
    co_delay_100us(1200);
    LCD_SET_RESET();
    co_delay_100us(3500);
    LCD_RELEASE_RESET();
    co_delay_100us(1200);
    //LCD_ENABLE_BACKLIGHT();
	
	#if 1
	gc9a01_write_cmd(0xFE);			 
	gc9a01_write_cmd(0xEF); 

	gc9a01_write_cmd(0xEB);	
	gc9a01_write_data(0x14); 

	gc9a01_write_cmd(0x84);			
	gc9a01_write_data(0x40); 

	gc9a01_write_cmd(0x88);			
	gc9a01_write_data(0x0A);

	gc9a01_write_cmd(0x89);			
	gc9a01_write_data(0x21); 

	gc9a01_write_cmd(0x8A);			
	gc9a01_write_data(0x00); 

	gc9a01_write_cmd(0x8B);			
	gc9a01_write_data(0x80); 

	gc9a01_write_cmd(0x8C);			
	gc9a01_write_data(0x01); 

	gc9a01_write_cmd(0x8D);			
	gc9a01_write_data(0x03); 

	gc9a01_write_cmd(0x8F);			
	gc9a01_write_data(0xFF); 

	gc9a01_write_cmd(0xB6);			
	gc9a01_write_data(0x00); 
	gc9a01_write_data(0x60); 

	gc9a01_write_cmd(0x36);			
	gc9a01_write_data(0x48);

	gc9a01_write_cmd(0x3A);	
	gc9a01_write_data(0x05); 


	gc9a01_write_cmd(0x90);			
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x08); 

	gc9a01_write_cmd(0xBD);			
	gc9a01_write_data(0x06);

	gc9a01_write_cmd(0xBC);			
	gc9a01_write_data(0x00);	

	gc9a01_write_cmd(0xFF);			
	gc9a01_write_data(0x60);
	gc9a01_write_data(0x01);
	gc9a01_write_data(0x04);


	gc9a01_write_cmd(0xC3);			
	gc9a01_write_data(0x2F);
	gc9a01_write_cmd(0xC4);			
	gc9a01_write_data(0x2F);

	gc9a01_write_cmd(0xC9);			
	gc9a01_write_data(0x25);


	gc9a01_write_cmd(0xBE);			
	gc9a01_write_data(0x11); 

	gc9a01_write_cmd(0xE1);			
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x0E);

	gc9a01_write_cmd(0xDF);			
	gc9a01_write_data(0x21);
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x02);

	gc9a01_write_cmd(0xF0);   
	gc9a01_write_data(0x49);
	gc9a01_write_data(0x0e);
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x25);
	gc9a01_write_data(0x2e);

	gc9a01_write_cmd(0xF1);    
	gc9a01_write_data(0x44);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x73);
	gc9a01_write_data(0x2F);
	gc9a01_write_data(0x30);  
	gc9a01_write_data(0x6F);

	gc9a01_write_cmd(0xF2);   
	gc9a01_write_data(0x49);
	gc9a01_write_data(0x0e);
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x25);
	gc9a01_write_data(0x2e);

	gc9a01_write_cmd(0xF3);   
	gc9a01_write_data(0x44);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x73);
	gc9a01_write_data(0x2F);
	gc9a01_write_data(0x30);  
	gc9a01_write_data(0x6F);

	gc9a01_write_cmd(0xED);	
	gc9a01_write_data(0x1B); 
	gc9a01_write_data(0x8B); 

	gc9a01_write_cmd(0xAE);			
	gc9a01_write_data(0x77);

	gc9a01_write_cmd(0xCD);			
	gc9a01_write_data(0x63);		

	gc9a01_write_cmd(0xAC);			
	gc9a01_write_data(0x27);

	gc9a01_write_cmd(0x70);			
	gc9a01_write_data(0x07);
	gc9a01_write_data(0x07);
	gc9a01_write_data(0x04);
	gc9a01_write_data(0x06);//VGH
	gc9a01_write_data(0x0F); //VGL
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x07);
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x03);

	gc9a01_write_cmd(0xE8);			
	gc9a01_write_data(0x24);

	gc9a01_write_cmd(0x60);		
	gc9a01_write_data(0x38);
	gc9a01_write_data(0x0B);
	gc9a01_write_data(0x6D);
	gc9a01_write_data(0x6D);

	gc9a01_write_data(0x39);
	gc9a01_write_data(0xF0);
	gc9a01_write_data(0x6D);
	gc9a01_write_data(0x6D);


	gc9a01_write_cmd(0x61);
	gc9a01_write_data(0x38);
	gc9a01_write_data(0xF4);
	gc9a01_write_data(0x6D);
	gc9a01_write_data(0x6D);

	gc9a01_write_data(0x38);
	gc9a01_write_data(0xF7);
	gc9a01_write_data(0x6D);
	gc9a01_write_data(0x6D);
	/////////////////////////////////////
	gc9a01_write_cmd(0x62);
	gc9a01_write_data(0x38);
	gc9a01_write_data(0x0D);
	gc9a01_write_data(0x71);
	gc9a01_write_data(0xED);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x38);
	gc9a01_write_data(0x0F);
	gc9a01_write_data(0x71);
	gc9a01_write_data(0xEF);
	gc9a01_write_data(0x70); 
	gc9a01_write_data(0x70);

	gc9a01_write_cmd(0x63);			
	gc9a01_write_data(0x38);
	gc9a01_write_data(0x11);
	gc9a01_write_data(0x71);
	gc9a01_write_data(0xF1);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x70);
	gc9a01_write_data(0x38);
	gc9a01_write_data(0x13);
	gc9a01_write_data(0x71);
	gc9a01_write_data(0xF3);
	gc9a01_write_data(0x70); 
	gc9a01_write_data(0x70);


	gc9a01_write_cmd(0x64);			
	gc9a01_write_data(0x28);
	gc9a01_write_data(0x29);
	gc9a01_write_data(0xF1);
	gc9a01_write_data(0x01);
	gc9a01_write_data(0xF1);
	gc9a01_write_data(0x00);//
	gc9a01_write_data(0x1a);//

	gc9a01_write_cmd(0x66);
	gc9a01_write_data(0x3C);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x98);
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x32);
	gc9a01_write_data(0x45);
	gc9a01_write_data(0x01);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_cmd(0x67);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x3C);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x54);
	gc9a01_write_data(0x67);
	gc9a01_write_data(0x45);
	gc9a01_write_data(0xcd);


	gc9a01_write_cmd(0x74);			
	gc9a01_write_data(0x10);	
	gc9a01_write_data(0x85);	//85
	gc9a01_write_data(0x80);
	gc9a01_write_data(0x00); 
	gc9a01_write_data(0x00); 
	gc9a01_write_data(0x4E);
	gc9a01_write_data(0x00);					

	gc9a01_write_cmd(0x98);			
	gc9a01_write_data(0x3e);
	gc9a01_write_data(0x07);
	gc9a01_write_cmd(0x99);			
	gc9a01_write_data(0x3e);
	gc9a01_write_data(0x07);


	gc9a01_write_cmd(0x35);	
	gc9a01_write_cmd(0x21);
	co_delay_100us(120);
	//--------end gamma setting--------------//

	gc9a01_write_cmd(0x11);
	co_delay_100us(320);
	gc9a01_write_cmd(0x29);
	co_delay_100us(120);
	gc9a01_write_cmd(0x2C);
	
	#endif
	
#if 0

#define ORIENTATION 2   // Set the display orientation 0,1,2,3

// Command codes:
#define COL_ADDR_SET        0x2A
#define ROW_ADDR_SET        0x2B
#define MEM_WR              0x2C
#define COLOR_MODE          0x3A
#define COLOR_MODE__12_BIT  0x03
#define COLOR_MODE__16_BIT  0x05
#define COLOR_MODE__18_BIT  0x06
#define MEM_WR_CONT         0x3C

/* Initial Sequence */ 
    
    gc9a01_write_cmd(0xEF);
    
    gc9a01_write_cmd(0xEB);
    gc9a01_write_data(0x14);
    
    gc9a01_write_cmd(0xFE);
    gc9a01_write_cmd(0xEF);
    
    gc9a01_write_cmd(0xEB);
    gc9a01_write_data(0x14);
    
    gc9a01_write_cmd(0x84);
    gc9a01_write_data(0x40);
    
    gc9a01_write_cmd(0x85);
    gc9a01_write_data(0xFF);
    
    gc9a01_write_cmd(0x86);
    gc9a01_write_data(0xFF);
    
    gc9a01_write_cmd(0x87);
    gc9a01_write_data(0xFF);
    
    gc9a01_write_cmd(0x88);
    gc9a01_write_data(0x0A);
    
    gc9a01_write_cmd(0x89);
    gc9a01_write_data(0x21);
    
    gc9a01_write_cmd(0x8A);
    gc9a01_write_data(0x00);
    
    gc9a01_write_cmd(0x8B);
    gc9a01_write_data(0x80);
    
    gc9a01_write_cmd(0x8C);
    gc9a01_write_data(0x01);
    
    gc9a01_write_cmd(0x8D);
    gc9a01_write_data(0x01);
    
    gc9a01_write_cmd(0x8E);
    gc9a01_write_data(0xFF);
    
    gc9a01_write_cmd(0x8F);
    gc9a01_write_data(0xFF);
    
    
    gc9a01_write_cmd(0xB6);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    
    gc9a01_write_cmd(0x36);
    
#if ORIENTATION == 0
    gc9a01_write_data(0x18);
#elif ORIENTATION == 1
    gc9a01_write_data(0x28);
#elif ORIENTATION == 2
    gc9a01_write_data(0x48);
#else
    gc9a01_write_data(0x88);
#endif
    
    gc9a01_write_cmd(COLOR_MODE);
    gc9a01_write_data(COLOR_MODE__16_BIT);
    
    gc9a01_write_cmd(0x90);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x08);
    
    gc9a01_write_cmd(0xBD);
    gc9a01_write_data(0x06);
    
    gc9a01_write_cmd(0xBC);
    gc9a01_write_data(0x00);
    
    gc9a01_write_cmd(0xFF);
    gc9a01_write_data(0x60);
    gc9a01_write_data(0x01);
    gc9a01_write_data(0x04);
    
    gc9a01_write_cmd(0xC3);
    gc9a01_write_data(0x13);
    gc9a01_write_cmd(0xC4);
    gc9a01_write_data(0x13);
    
    gc9a01_write_cmd(0xC9);
    gc9a01_write_data(0x22);
    
    gc9a01_write_cmd(0xBE);
    gc9a01_write_data(0x11);
    
    gc9a01_write_cmd(0xE1);
    gc9a01_write_data(0x10);
    gc9a01_write_data(0x0E);
    
    gc9a01_write_cmd(0xDF);
    gc9a01_write_data(0x21);
    gc9a01_write_data(0x0c);
    gc9a01_write_data(0x02);
    
    gc9a01_write_cmd(0xF0);
    gc9a01_write_data(0x45);
    gc9a01_write_data(0x09);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x26);
    gc9a01_write_data(0x2A);
    
    gc9a01_write_cmd(0xF1);
    gc9a01_write_data(0x43);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x72);
    gc9a01_write_data(0x36);
    gc9a01_write_data(0x37);
    gc9a01_write_data(0x6F);
    
    gc9a01_write_cmd(0xF2);
    gc9a01_write_data(0x45);
    gc9a01_write_data(0x09);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x26);
    gc9a01_write_data(0x2A);
    
    gc9a01_write_cmd(0xF3);
    gc9a01_write_data(0x43);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x72);
    gc9a01_write_data(0x36);
    gc9a01_write_data(0x37);
    gc9a01_write_data(0x6F);
    
    gc9a01_write_cmd(0xED);
    gc9a01_write_data(0x1B);
    gc9a01_write_data(0x0B);
    
    gc9a01_write_cmd(0xAE);
    gc9a01_write_data(0x77);
    
    gc9a01_write_cmd(0xCD);
    gc9a01_write_data(0x63);
    
    gc9a01_write_cmd(0x70);
    gc9a01_write_data(0x07);
    gc9a01_write_data(0x07);
    gc9a01_write_data(0x04);
    gc9a01_write_data(0x0E);
    gc9a01_write_data(0x0F);
    gc9a01_write_data(0x09);
    gc9a01_write_data(0x07);
    gc9a01_write_data(0x08);
    gc9a01_write_data(0x03);
    
    gc9a01_write_cmd(0xE8);
    gc9a01_write_data(0x34);
    
    gc9a01_write_cmd(0x62);
    gc9a01_write_data(0x18);
    gc9a01_write_data(0x0D);
    gc9a01_write_data(0x71);
    gc9a01_write_data(0xED);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x18);
    gc9a01_write_data(0x0F);
    gc9a01_write_data(0x71);
    gc9a01_write_data(0xEF);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x70);
    
    gc9a01_write_cmd(0x63);
    gc9a01_write_data(0x18);
    gc9a01_write_data(0x11);
    gc9a01_write_data(0x71);
    gc9a01_write_data(0xF1);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x18);
    gc9a01_write_data(0x13);
    gc9a01_write_data(0x71);
    gc9a01_write_data(0xF3);
    gc9a01_write_data(0x70);
    gc9a01_write_data(0x70);
    
    gc9a01_write_cmd(0x64);
    gc9a01_write_data(0x28);
    gc9a01_write_data(0x29);
    gc9a01_write_data(0xF1);
    gc9a01_write_data(0x01);
    gc9a01_write_data(0xF1);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x07);
    
    gc9a01_write_cmd(0x66);
    gc9a01_write_data(0x3C);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0xCD);
    gc9a01_write_data(0x67);
    gc9a01_write_data(0x45);
    gc9a01_write_data(0x45);
    gc9a01_write_data(0x10);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    
    gc9a01_write_cmd(0x67);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x3C);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x01);
    gc9a01_write_data(0x54);
    gc9a01_write_data(0x10);
    gc9a01_write_data(0x32);
    gc9a01_write_data(0x98);
    
    gc9a01_write_cmd(0x74);
    gc9a01_write_data(0x10);
    gc9a01_write_data(0x85);
    gc9a01_write_data(0x80);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x00);
    gc9a01_write_data(0x4E);
    gc9a01_write_data(0x00);
    
    gc9a01_write_cmd(0x98);
    gc9a01_write_data(0x3e);
    gc9a01_write_data(0x07);
    
    gc9a01_write_cmd(0x35);
    gc9a01_write_cmd(0x21);
    
    gc9a01_write_cmd(0x11);
//    GC9A01_delay(120);
	  co_delay_100us(1200);
    gc9a01_write_cmd(0x29);
    co_delay_100us(2000);
		
		#endif
		

#if 0
   #define USE_HORIZONTAL 1
  gc9a01_write_cmd(0xFE);
	gc9a01_write_cmd(0xEF);

	gc9a01_write_cmd(0x84);
	gc9a01_write_data(0x40);


	gc9a01_write_cmd(0xB6);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x20);

	gc9a01_write_cmd(0x36);
	if(USE_HORIZONTAL==0)gc9a01_write_data(0x08);
	else if(USE_HORIZONTAL==1)gc9a01_write_data(0xC8);
	else if(USE_HORIZONTAL==2)gc9a01_write_data(0x68);
	else gc9a01_write_data(0xA8);

	gc9a01_write_cmd(0x3A);
	gc9a01_write_data(0x05);

	gc9a01_write_cmd(0xC3);
	gc9a01_write_data(0x13);
	gc9a01_write_cmd(0xC4);
	gc9a01_write_data(0x13);

	gc9a01_write_cmd(0xC9);
	gc9a01_write_data(0x22);


	gc9a01_write_cmd(0xF0);
	gc9a01_write_data(0x45);
	gc9a01_write_data(0x09);
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x08);
	gc9a01_write_data(0x26);
 	gc9a01_write_data(0x2A);

 	gc9a01_write_cmd(0xF1);
 	gc9a01_write_data(0x43);
 	gc9a01_write_data(0x70);
 	gc9a01_write_data(0x72);
 	gc9a01_write_data(0x36);
 	gc9a01_write_data(0x37);
 	gc9a01_write_data(0x6F);


 	gc9a01_write_cmd(0xF2);
 	gc9a01_write_data(0x45);
 	gc9a01_write_data(0x09);
 	gc9a01_write_data(0x08);
 	gc9a01_write_data(0x08);
 	gc9a01_write_data(0x26);
 	gc9a01_write_data(0x2A);

 	gc9a01_write_cmd(0xF3);
 	gc9a01_write_data(0x43);
 	gc9a01_write_data(0x70);
 	gc9a01_write_data(0x72);
 	gc9a01_write_data(0x36);
 	gc9a01_write_data(0x37);
 	gc9a01_write_data(0x6F);

	gc9a01_write_cmd(0xE8);
	gc9a01_write_data(0x34);

	gc9a01_write_cmd(0x66);
	gc9a01_write_data(0x3C);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0xCD);
	gc9a01_write_data(0x67);
	gc9a01_write_data(0x45);
	gc9a01_write_data(0x45);
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);

	gc9a01_write_cmd(0x67);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x3C);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x00);
	gc9a01_write_data(0x01);
	gc9a01_write_data(0x54);
	gc9a01_write_data(0x10);
	gc9a01_write_data(0x32);
	gc9a01_write_data(0x98);

	gc9a01_write_cmd(0x35);
	gc9a01_write_cmd(0x21);

	gc9a01_write_cmd(0x11);
	co_delay_100us(2000);
	gc9a01_write_cmd(0x29);
	co_delay_100us(1000);
  gc9a01_write_cmd(0x2c);
	#endif

}

void gc9a01_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
	   
    gc9a01_write_cmd(0x2a); 

    gc9a01_write_data16(x_s);
    gc9a01_write_data16(x_e);

    gc9a01_write_cmd(0x2b); 
    gc9a01_write_data16(y_s);
    gc9a01_write_data16(y_e);

    gc9a01_write_cmd(0x2c); 
}

void gc9a01_display_wait_transfer_done(void)
{
    while(dma_transfer_done == false);
}

void gc9a01_display(uint32_t pixel_count, uint16_t *data, void (*callback)(void))
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
        LinkParameter.Request_ID       = gc9a01_DMA_Channel.Init.Request_ID;
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
	//__SPI_DATA_FRAME_SIZE(GC9A01_SPI_Handle.SPIx,SPI_FRAME_SIZE_8BIT);
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&GC9A01_SPI_Handle);
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, gc9a01_DMA_Channel.Channel);
	#endif
}

 
__attribute__((section("ram_code"))) void gc9a01_dma_isr(void)
{
    void (*callback)();
    while(__SPI_IS_BUSY(GC9A01_SPI_Handle.SPIx));
    LCD_RELEASE_CS();
	
    /* Clear Transfer complete status */
    dma_clear_tfr_Status(gc9a01_DMA_Channel.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(gc9a01_DMA_Channel.Channel);

    __SPI_DISABLE(GC9A01_SPI_Handle.SPIx);
 //   __SPI_DATA_FRAME_SIZE(GC9A01_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
    dma_transfer_done = true;
    callback = dma_trans_done_callback;
    dma_trans_done_callback = NULL;
    if(callback) {
        callback();
    }
}

