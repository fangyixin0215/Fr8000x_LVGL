
/*
@driver_nv3030b.c
@author: LH 
Creation Date：2023/05/11
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "driver_nv3030b.h"

#include "sys_utils.h"
 
 
 
#define LCD_RELEASE_CS()            gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_SET_CS()                gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_CLEAR)

#define LCD_RELEASE_DC()            gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_SET)
#define LCD_SET_DC()                gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_CLEAR)

#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_B, GPIO_PIN_4, GPIO_PIN_CLEAR)

#define LCD_ENABLE_BACKLIGHT()      gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_DISABLE_BACKLIGHT()     gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)


#define LCD_W 240
#define LCD_H 286

 
void nv3030b_display_wait_transfer_done(void);
static DMA_LLI_InitTypeDef LCD1_Link_Channel[82];
static DMA_HandleTypeDef nv3030b_DMA_Channel;
static dma_LinkParameter_t LCD1_LinkParameter;

 
static dma_LinkParameter_t LCD2_LinkParameter;

static volatile bool dma_transfer_done = true;

static SPI_HandleTypeDef nv3030b_SPI_Handle;

static void (*dma_trans_done_callback)(void) = NULL;
 

 

static void nv3030b_init_io(void)
{
    GPIO_InitTypeDef GPIO_Handle;

 
 	 __SYSTEM_GPIO_CLK_ENABLE();
  
    // reset  DC   CS 
    GPIO_Handle.Pin       = GPIO_PIN_4;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_B, &GPIO_Handle);

	GPIO_Handle.Pin       = GPIO_PIN_3;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_B, &GPIO_Handle);
	
	GPIO_Handle.Pin       = GPIO_PIN_1;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_B, &GPIO_Handle);
	
	GPIO_Handle.Pin       = GPIO_PIN_5;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_A, &GPIO_Handle);
	
    GPIO_Handle.Pin       = GPIO_PIN_5;//te 
	GPIO_Handle.Pull      = GPIO_PULLUP; 
    GPIO_Handle.Mode      = GPIO_MODE_INPUT;
    gpio_init(GPIO_B, &GPIO_Handle);

    GPIO_Handle.Pin       = GPIO_PIN_0;//sclk
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN; 
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
 
	GPIO_Handle.Pin       = GPIO_PIN_2;//sda io0
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN; 
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
 
    LCD_RELEASE_CS();
    
 	LCD_DISABLE_BACKLIGHT();
  //init_lcd_pwm();	
	
}

void nv3030b_backlight_open(void)
{
	LCD_ENABLE_BACKLIGHT();
}

void nv3030b_backlight_close(void)
{
	LCD_DISABLE_BACKLIGHT();
}

uint8_t nv3030b_read_te(void)
{
	return gpio_read_pin(GPIO_B, GPIO_PIN_5);
}
static void nv3030b_init_spi(void)
{
    __SYSTEM_SPI0_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI0_MASTER_CLK_ENABLE();
    
    nv3030b_SPI_Handle.SPIx                       = SPIM0;
    nv3030b_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    nv3030b_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    nv3030b_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    nv3030b_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    nv3030b_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&nv3030b_SPI_Handle);
}

static void nv3030b_init_dma(void)
{    
    __SYSTEM_DMA_CLK_ENABLE();
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    
    nv3030b_DMA_Channel.Channel = DMA_Channel0;
    nv3030b_DMA_Channel.Init.Data_Flow        = DMA_M2P_DMAC;
    nv3030b_DMA_Channel.Init.Request_ID       = 1;
    nv3030b_DMA_Channel.Init.Source_Inc       = DMA_ADDR_INC_INC;
    nv3030b_DMA_Channel.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    nv3030b_DMA_Channel.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    nv3030b_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&nv3030b_DMA_Channel);

    NVIC_EnableIRQ(DMA_IRQn);
}

static void nv3030b_write_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];

    spi_data[0]=cmd;
		LCD_SET_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&nv3030b_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();

}

static void nv3030b_write_data(uint8_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data;
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&nv3030b_SPI_Handle, (uint16_t *)spi_data, 1);
    LCD_RELEASE_CS();
}

static void nv3030b_write_16bit_data(uint16_t data)
{
    uint8_t spi_data[4];

    spi_data[0]=data>>8;
	spi_data[1]=data;
	LCD_RELEASE_DC();
    LCD_SET_CS();
	__SPI_DATA_FRAME_SIZE(nv3030b_SPI_Handle.SPIx,SPI_FRAME_SIZE_16BIT);
    spi_master_transmit_X1(&nv3030b_SPI_Handle, (uint16_t *)spi_data, 2);
	
    LCD_RELEASE_CS();
	__SPI_DATA_FRAME_SIZE(nv3030b_SPI_Handle.SPIx,SPI_FRAME_SIZE_8BIT);
}


static void send_data_disp(uint8_t *pdata,uint32_t len)
{
	
	  LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1(&nv3030b_SPI_Handle, (uint16_t *)pdata, len);
	  LCD_RELEASE_CS();
}

 



/******************************************************************************
      函数说明：LCD清屏函数
      入口数据：无
      返回值：  无
******************************************************************************/
void nv3030b_Clear(uint16_t Color)
{
	uint16_t i,j;  
		
 
	nv3030b_write_16bit_data(0);
	nv3030b_write_16bit_data(LCD_W-1);
	nv3030b_write_cmd(0x2b);//行地址设置
	nv3030b_write_16bit_data(0);
	nv3030b_write_16bit_data(LCD_H-1);
 
    nv3030b_write_cmd(0x2c);//储存器写
	for(i=0;i<LCD_W;i++)
	{
	  for (j=0;j<LCD_H;j++)
	   	{
        	nv3030b_write_16bit_data(Color);	 			 
	    }
 
	 }
}
// 函数定义：将16位无符号整数的字节顺序进行大小端转换
static uint16_t swap_endian(uint16_t value) {
   return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	//return value;
}
 
void nv3030b_init(void)
{

    nv3030b_init_io();
    nv3030b_init_spi();
    nv3030b_init_dma();
	
	//VCI=2.8V
	//----------------RESET LCD Driver ------------//
	LCD_RELEASE_RESET();
	co_delay_100us(10);
	LCD_SET_RESET();
	co_delay_100us(200);
	LCD_RELEASE_RESET();
	nv3030b_write_cmd(0x11); // exit sleep
	co_delay_100us(300);     
	//
	
	nv3030b_write_cmd(0xfd);//private_access
	nv3030b_write_data(0x06);
	nv3030b_write_data(0x08);
	////osc
	//nv3030b_write_cmd(0x60);//
	//nv3030b_write_data(0x01);//osc_user_adj[3:0] 08
	//nv3030b_write_data(0x01);//
	//nv3030b_write_data(0x01);//01
	nv3030b_write_cmd(0x61);//
	nv3030b_write_data(0x06);//
	nv3030b_write_data(0x04);//
	//bias
	nv3030b_write_cmd(0x62);//bias setting
	nv3030b_write_data(0x00);//01
	nv3030b_write_data(0x44);//04 44
	nv3030b_write_data(0x40);//44 65 40
	//nv3030b_write_data(0x01);//06 vref_adj 01
	nv3030b_write_cmd(0x63);//VGL
	nv3030b_write_data(0x41);//
	nv3030b_write_data(0x07);//
	nv3030b_write_data(0x12);//
	nv3030b_write_data(0x12);//
	//VSP
	nv3030b_write_cmd(0x65);//Pump1=4.7MHz //PUMP1 VSP
	nv3030b_write_data(0x09);//D6-5:pump1_clk[1:0] clamp 28 2b
	nv3030b_write_data(0x17);//6.26
	nv3030b_write_data(0x21);
	//VSN
	nv3030b_write_cmd(0x66); //pump=2 AVCL
	nv3030b_write_data(0x09); //clamp 08 0b 09
	nv3030b_write_data(0x17); //10
	nv3030b_write_data(0x21);
	//add source_neg_time
	nv3030b_write_cmd(0x67);//pump_sel
	nv3030b_write_data(0x20);//21 20
	nv3030b_write_data(0x40);
	//gamma vap/van
	nv3030b_write_cmd(0x68);//gamma vap/van
	nv3030b_write_data(0x90);//90 9f
	nv3030b_write_data(0x30);//30
	nv3030b_write_data(0x21);//77
	nv3030b_write_data(0x27);//07

	nv3030b_write_cmd(0xb1);//frame rate
	nv3030b_write_data(0x0F);//0x0f fr_h[5:0] 0F
	nv3030b_write_data(0x02);//0x02 fr_v[4:0] 02
	nv3030b_write_data(0x01);//0x04 fr_div[2:0] 03

	nv3030b_write_cmd(0xB4);
	nv3030b_write_data(0x01); //00:column 01:dot
	////porch
	nv3030b_write_cmd(0xB5);
	nv3030b_write_data(0x02);//0x02 vfp[6:0]
	nv3030b_write_data(0x02);//0x02 vbp[6:0]
	nv3030b_write_data(0x0a);//0x0A hfp[6:0]
	nv3030b_write_data(0x14);//0x14 hbp[6:0]
	nv3030b_write_cmd(0xB6);
	nv3030b_write_data(0x44);//04
	nv3030b_write_data(0x01);//01
	nv3030b_write_data(0x9f);//
	nv3030b_write_data(0x00);//
	nv3030b_write_data(0x02);//
	////gamme sel
	nv3030b_write_cmd(0xdf);//
	nv3030b_write_data(0x11);//gofc_gamma_en_sel=1
	//GAMMA---------------------------------/////////////
	nv3030b_write_cmd(0xE2);
	nv3030b_write_data(0x0d);//vrp0[5:0] V63
	nv3030b_write_data(0x0c);//vrp1[5:0] V62
	nv3030b_write_data(0x0d);//vrp2[5:0] V61
	nv3030b_write_data(0x2a);//vrp3[5:0] V2
	nv3030b_write_data(0x2f);//vrp4[5:0] V1
	nv3030b_write_data(0x3f);//vrp5[5:0] V0
	nv3030b_write_cmd(0xE5);
	nv3030b_write_data(0x3f);//vrn0[5:0] V0
	nv3030b_write_data(0x2a);//vrn1[5:0] V1
	nv3030b_write_data(0x28);//vrn2[5:0] V2
	nv3030b_write_data(0x0c);//vrn3[5:0] V61
	nv3030b_write_data(0x0c);//vrn4[5:0] V62
	nv3030b_write_data(0x04);//vrn5[5:0] V63

	nv3030b_write_cmd(0xE1);
	nv3030b_write_data(0x1d);//prp0[6:0] V51
	nv3030b_write_data(0x64);//prp1[6:0] V15
	nv3030b_write_cmd(0xE4);
	nv3030b_write_data(0x5d);//prn0[6:0] V15
	nv3030b_write_data(0x18);//prn1[6:0] V51
	nv3030b_write_cmd(0xE0);
	nv3030b_write_data(0x08);//pkp0[4:0] V60
	nv3030b_write_data(0x09);//pkp1[4:0] V56
	nv3030b_write_data(0x0c);//pkp2[4:0] V45
	nv3030b_write_data(0x12);//pkp3[4:0] V37
	nv3030b_write_data(0x0e);//pkp4[4:0] V29 //
	nv3030b_write_data(0x0f);//pkp5[4:0] V21
	nv3030b_write_data(0x12);//pkp6[4:0] V7
	nv3030b_write_data(0x16);//pkp7[4:0] V3
	nv3030b_write_cmd(0xE3);
	nv3030b_write_data(0x15);//pkn0[4:0] V3
	nv3030b_write_data(0x10);//pkn1[4:0] V7
	nv3030b_write_data(0x0f);//pkn2[4:0] V21
	nv3030b_write_data(0x0c);//pkn3[4:0] V29 //
	nv3030b_write_data(0x0c);//pkn4[4:0] V37
	nv3030b_write_data(0x0a);//pkn5[4:0] V45
	nv3030b_write_data(0x06);//pkn6[4:0] V56
	nv3030b_write_data(0x04);//pkn7[4:0] V60
	//GAMMA---------------------------------/////////////
	nv3030b_write_cmd(0xE6);
	nv3030b_write_data(0x00);
	nv3030b_write_data(0xff);//SC_EN_START[7:0] f0
	nv3030b_write_cmd(0xE7);
	nv3030b_write_data(0x01);//CS_START[3:0] 01
	nv3030b_write_data(0x04);//scdt_inv_sel cs_vp_en
	nv3030b_write_data(0x03);//CS1_WIDTH[7:0] 12
	nv3030b_write_data(0x03);//CS2_WIDTH[7:0] 12
	nv3030b_write_data(0x00);//PREC_START[7:0] 06
	nv3030b_write_data(0x12);//PREC_WIDTH[7:0] 12
	nv3030b_write_cmd(0xE8); //source
	nv3030b_write_data(0x00); //VCMP_OUT_EN 81-vcmp/vref_output pad
	nv3030b_write_data(0x70); //chopper_sel[6:4]
	nv3030b_write_data(0x00); //gchopper_sel[6:4] 60
	////gate
	nv3030b_write_cmd(0xEc);
	nv3030b_write_data(0x50);//50
	nv3030b_write_cmd(0xF1);
	nv3030b_write_data(0x00);//te_pol tem_extend
	nv3030b_write_cmd(0xfd);
	nv3030b_write_data(0xfa);
	nv3030b_write_data(0xfc);

//	nv3030b_write_cmd(0x35);
//	nv3030b_write_data(0x00);//c0
	nv3030b_write_cmd(0x3a);
	nv3030b_write_data(0x55);//SH 0x66
	nv3030b_write_cmd(0x35);
	nv3030b_write_data(0x00);
	nv3030b_write_cmd(0x36);//bgr_[3]
	nv3030b_write_data(0x00);//c0
	
 	nv3030b_write_cmd(0x11); // exit sleep
	co_delay_100us(500);     
	nv3030b_write_cmd(0x29); // display on
#if 0
	#define DRAW_LINE 10
	static uint16_t draw_buff[LCD_W*DRAW_LINE];
	for(uint32_t i = 0; i < LCD_W*DRAW_LINE; i++)draw_buff[i]   =swap_endian(0xf800);
	uint16_t line_cont=0;
	uint8_t color_cw=0;
	while(line_cont<280)
	{
		for(uint32_t i = 0; i < LCD_W*DRAW_LINE; i++)
		{
			switch(color_cw)
			{
				case 0:
					draw_buff[i]   =swap_endian(0xf800);
				break;
				
				case 1:
					draw_buff[i]   =swap_endian(0x001f);
				break;
				
				case 2:
					draw_buff[i]   =swap_endian(0x07e0);
				break;
				
			}
			
		}
		color_cw++;
		color_cw%=3;
		nv3030b_set_window(0, LCD_W-1,line_cont, DRAW_LINE-1);	 
		nv3030b_display((LCD_W*DRAW_LINE),draw_buff,NULL);
		nv3030b_display_wait_transfer_done();
		line_cont+=10;
 
	}
	nv3030b_set_window(0, LCD_W-1,280, 286);	 
	nv3030b_display((LCD_W*6),draw_buff,NULL);
	nv3030b_display_wait_transfer_done();
#endif
//	static uint16_t draw_buff[LCD_W*6]={0};
//	nv3030b_set_window(0, LCD_W-1,280, 286);	 
//	nv3030b_display((LCD_W*6),draw_buff,NULL);
//	nv3030b_display_wait_transfer_done();
	 nv3030b_Clear(0x0000);
	
	LCD_ENABLE_BACKLIGHT();
}

void nv3030b_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
	   
	#if 0
	nv3030b_write_cmd(0x2a);//列地址设置
	nv3030b_write_data(0);
	nv3030b_write_data(x_s);
	nv3030b_write_data(0);
	nv3030b_write_data(x_e);
	nv3030b_write_cmd(0x2b);//行地址设置
	nv3030b_write_data(0);
	nv3030b_write_data(y_s);
	nv3030b_write_data(0);
	nv3030b_write_data(y_e);
 
    nv3030b_write_cmd(0x2c);//储存器写
	#else
	nv3030b_write_cmd(0x2a);//列地址设置
	
	nv3030b_write_16bit_data(x_s);
	nv3030b_write_16bit_data(x_e);
	nv3030b_write_cmd(0x2b);//行地址设置
	nv3030b_write_16bit_data(y_s+50);
	nv3030b_write_16bit_data(y_e+50);
 
    nv3030b_write_cmd(0x2c);//储存器写
	#endif
}

void nv3030b_display_wait_transfer_done(void)
{
    while(dma_transfer_done == false);
}

void nv3030b_display(uint32_t pixel_count, uint16_t *data, void (*callback)(void))
{
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
        
        LCD1_LinkParameter.SrcAddr          = (uint32_t)&data[i * 8000];
        LCD1_LinkParameter.DstAddr          = (uint32_t)&nv3030b_SPI_Handle.SPIx->DR;
        if(all_set)
        {
            LCD1_LinkParameter.NextLink     = 0;
        }
        else
        {
            LCD1_LinkParameter.NextLink     = (uint32_t)&LCD1_Link_Channel[i + 1];
        }
        LCD1_LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LCD1_LinkParameter.Request_ID       = nv3030b_DMA_Channel.Init.Request_ID;
        LCD1_LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LCD1_LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LCD1_LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LCD1_LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_8;
        LCD1_LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LCD1_LinkParameter.Size             = all_set ? (total_count) : 4000;
        LCD1_LinkParameter.gather_enable    = 0;
        LCD1_LinkParameter.scatter_enable   = 0;
        total_count -= 4000;

        dma_linked_list_init(&LCD1_Link_Channel[i], &LCD1_LinkParameter);
    }
	LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&nv3030b_SPI_Handle);
    dma_linked_list_start_IT(LCD1_Link_Channel, &LCD1_LinkParameter, nv3030b_DMA_Channel.Channel);
	
	#else
 
    uint32_t i;
 
    uint32_t num_lines = pixel_count / 160;  // Total number of lines (assuming pixel_count is total pixels for the entire screen)
	
    if(dma_transfer_done == false)
    {
        return;
    }
    else
    {
        dma_transfer_done = false;
    }

    dma_trans_done_callback = callback;

    for (i = 0; i < num_lines; i++)
    {
        LCD1_LinkParameter.SrcAddr          = (uint32_t)&data[i * 160+80]; 
        LCD1_LinkParameter.DstAddr          = (uint32_t)&nv3030b_SPI_Handle.SPIx->DR;
        LCD1_LinkParameter.NextLink         = (i == num_lines - 1) ? 0 : (uint32_t)&LCD1_Link_Channel[i + 1];
        LCD1_LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LCD1_LinkParameter.Request_ID       = nv3030b_DMA_Channel.Init.Request_ID;
        LCD1_LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LCD1_LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LCD1_LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LCD1_LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_8;
        LCD1_LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LCD1_LinkParameter.Size             = 40;   
        LCD1_LinkParameter.gather_enable    = 0;
        LCD1_LinkParameter.scatter_enable   = 0;

        dma_linked_list_init(&LCD1_Link_Channel[i], &LCD1_LinkParameter);
 
    }

    LCD_RELEASE_DC();
    LCD_SET_CS();
    spi_master_transmit_X1_DMA(&nv3030b_SPI_Handle);
    dma_linked_list_start_IT(LCD1_Link_Channel, &LCD1_LinkParameter, nv3030b_DMA_Channel.Channel);
 

	#endif
  
}

/*
针对双屏显示专用api接口一般用不到
uint32_t last_time = system_get_curr_time();
nv3030b_display_pro(320,80,(void *)&color_p->full,true, NULL);//右边屏幕
display_wait_transfer_done();
printf("t:%d\r\n",system_get_curr_time()-last_time);

//display_set_window(area->x1, ((area->x2+1)/2)-1, area->y1, area->y2);
nv3030b_display_pro(320,80,(void *)&color_p->full,false, my_disp_flush_done);//左边屏幕
*/

void nv3030b_display_pro(uint32_t w, uint32_t h, uint16_t *data, bool right_half, void (*callback)(void))
{
    uint32_t i;
    uint32_t num_lines = h;  
    uint32_t half_width = w / 2; 
	
    if(dma_transfer_done == false)
    {
        return;
    }
    else
    {
        dma_transfer_done = false;
    }

    dma_trans_done_callback = callback;

    for (i = 0; i < num_lines; i++)
    {
        uint32_t offset = i * w + (right_half ? half_width : 0);   
        LCD1_LinkParameter.SrcAddr          = (uint32_t)&data[offset];   
        LCD1_LinkParameter.DstAddr          = (uint32_t)&nv3030b_SPI_Handle.SPIx->DR;
        LCD1_LinkParameter.NextLink         = (i == num_lines - 1) ? 0 : (uint32_t)&LCD1_Link_Channel[i + 1];
        LCD1_LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LCD1_LinkParameter.Request_ID       = nv3030b_DMA_Channel.Init.Request_ID;
        LCD1_LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LCD1_LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LCD1_LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;  
        LCD1_LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_8;  
        LCD1_LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LCD1_LinkParameter.Size             = half_width / 2;  
        LCD1_LinkParameter.gather_enable    = 0;
        LCD1_LinkParameter.scatter_enable   = 0;

        dma_linked_list_init(&LCD1_Link_Channel[i], &LCD1_LinkParameter);
    }
    
	if(right_half) // 右边屏幕显示
	{
		nv3030b_set_window(0,half_width-1,0,h-1);
		LCD_SET_CS();
		
	}else{ //左边屏幕显示
		nv3030b_set_window(0,half_width-1,0,h-1);
		LCD_SET_CS();
	}
	LCD_RELEASE_DC();
    spi_master_transmit_X1_DMA(&nv3030b_SPI_Handle);
    dma_linked_list_start_IT(LCD1_Link_Channel, &LCD1_LinkParameter, nv3030b_DMA_Channel.Channel);
}

 

void nv3030b_sleep_in(void)
{
	nv3030b_write_cmd(0x28);//Display off
	co_delay_100us(500);
	nv3030b_write_cmd(0x10);	//Normal-Mode -> Sleep In-Mode
	co_delay_100us(1500);
}

void nv3030b_sleep_out(void)
{
	nv3030b_write_cmd(0x11);	//Sleep In-Mode -> Normal-Mode
	co_delay_100us(1500);
	nv3030b_write_cmd(0x29);//Display on
	co_delay_100us(100);
}

__attribute__((section("ram_code"))) void nv3030b_dma_isr(void)
{
    void (*callback)();
    while(__SPI_IS_BUSY(nv3030b_SPI_Handle.SPIx));
//	 	printf("dma_isr\r\n");
    // CS Release
    LCD_RELEASE_CS();
	
    /* Clear Transfer complete status */
    dma_clear_tfr_Status(nv3030b_DMA_Channel.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(nv3030b_DMA_Channel.Channel);

    __SPI_DISABLE(nv3030b_SPI_Handle.SPIx);
 //   __SPI_DATA_FRAME_SIZE(nv3030b_SPI_Handle.SPIx, SPI_FRAME_SIZE_8BIT);
    
    dma_transfer_done = true;
    callback = dma_trans_done_callback;
    dma_trans_done_callback = NULL;
    if(callback) {
        callback();
    }
}
//__attribute__((section("ram_code"))) void dma_isr(void)
//{
//		nv3030b_dma_isr();
//}
