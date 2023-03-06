#include <stdint.h>
#include <string.h>

#include "driver_st77903.h"
#include "os_task.h"
#include "os_msg_q.h"
#include "co_printf.h"

#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "sys_utils.h"
#include "lvgl.h"
#include "board_driver.h"

#define ST77903_400X400		1					
//#define ST77903_320X320 1							

#if (ST77903_400X400==1)
#define ST77903_REFRESH_LINE	400
#endif
#if (ST77903_320X320==1)
#define ST77903_REFRESH_LINE	320
#endif
 
//  //
#if DEVELOPMENT_BOARD_8000AP
#define LCD_SPI_CS_Release()                        GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_5;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SPI_CS_Select()                         GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_5;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)
#elif DEVELOPMENT_BOARD_8000_EVB
#define LCD_SPI_CS_Release()                        gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_SET)
#define LCD_SPI_CS_Select()                         gpio_write_pin(GPIO_B, GPIO_PIN_1, GPIO_PIN_CLEAR)
#define ST77903_SPI_SEL          SPIM0

#elif DEVELOPMENT_BOARD_8000GP
#define LCD_SPI_CS_Release()                        GPIO->PortB_DATA = GPIO->PortB_DATA | GPIO_PIN_6;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SPI_CS_Select()                         GPIO->PortB_DATA = GPIO->PortB_DATA & ~GPIO_PIN_6;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)
#define LCD_BCAKLIGHT_SET()     										GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_4;
#define LCD_BCAKLIGHT_CLR()  												GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_4;
#define LCD_RST_Release()     											GPIO->PortD_DATA = GPIO->PortD_DATA | GPIO_PIN_5;
#define LCD_RST_Select()  													GPIO->PortD_DATA = GPIO->PortD_DATA & ~GPIO_PIN_5;
#define ST77903_SPI_SEL          										SPIM0

#elif KNOB_DEMO_BOARD
#define LCD_SPI_CS_Release()                        GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_5;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SPI_CS_Select()                         GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_5;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)

#define LCD_RST_Release()     											GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_2;
#define LCD_RST_Select()  													GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_2;

#define LCD_BCAKLIGHT_SET()     										GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_4;
#define LCD_BCAKLIGHT_CLR()  												GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_4;

#define ST77903_SPI_SEL          										SPIM0

#elif KTX_PCB_BOARD 
#define LCD_SPI_CS_Release()                        GPIO->PortA_DATA = GPIO->PortA_DATA | GPIO_PIN_7;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SPI_CS_Select()                         GPIO->PortA_DATA = GPIO->PortA_DATA & ~GPIO_PIN_7;//gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)

#define LCD_RST_Release()     											GPIO->PortD_DATA = GPIO->PortD_DATA | GPIO_PIN_0;
#define LCD_RST_Select()  													GPIO->PortD_DATA = GPIO->PortD_DATA & ~GPIO_PIN_0;
#define ST77903_SPI_SEL          SPIM0
#endif


SPI_HandleTypeDef  ST77903_SPI_Handle;
DMA_HandleTypeDef DMA_Channel_0;

volatile  uint32_t display_count = 0;
volatile uint8_t g_transfer_step=0;
volatile uint8_t g_transfer_status=1;
static uint16_t *p_st77903_psram=NULL;
void st77903_init_io(void)
{
    GPIO_InitTypeDef GPIO_Handle;
		__SYSTEM_GPIO_CLK_ENABLE();
#if DEVELOPMENT_BOARD_8000AP
	//reset cs BLK
	GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_5;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_A, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
	
	GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
	
		
		LCD_SPI_CS_Release();
//		gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET);
		
		gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
    co_delay_100us(200);
    gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_CLEAR);
    co_delay_100us(200);
    gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
    co_delay_100us(200);
#elif KNOB_DEMO_BOARD
	//reset cs BLK
		GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_4;
		GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_A, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
	
		GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
		GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
	
		
		LCD_SPI_CS_Release();
//		gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET);
		LCD_BCAKLIGHT_CLR();
		gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
    co_delay_100us(200);
    gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_CLEAR);
    co_delay_100us(200);
    gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
    co_delay_100us(200);
#elif DEVELOPMENT_BOARD_8000_EVB
	    // reset
		GPIO_Handle.Pin       = GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_A, &GPIO_Handle);
		
	GPIO_Handle.Pin       = GPIO_PIN_1;
	GPIO_Handle.Pull      = GPIO_PULLUP;
  GPIO_Handle.Alternate = GPIO_FUNCTION_0;
  GPIO_Handle.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init(GPIO_B, &GPIO_Handle);
	LCD_SPI_CS_Release();
			
		GPIO_Handle.Pin       = GPIO_PIN_5;
	  GPIO_Handle.Mode      = GPIO_MODE_INPUT;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_A, &GPIO_Handle);
	
		GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	  GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
		

		
		
		gpio_write_pin(GPIO_A, GPIO_PIN_6, GPIO_PIN_SET);
    co_delay_100us(500);
    gpio_write_pin(GPIO_A, GPIO_PIN_6, GPIO_PIN_CLEAR);
    co_delay_100us(500);
    gpio_write_pin(GPIO_A, GPIO_PIN_6, GPIO_PIN_SET);
    co_delay_100us(500);
#elif DEVELOPMENT_BOARD_8000GP
		//reset cs BLK
	  GPIO_Handle.Pin       = GPIO_PIN_6;
	  GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_B, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
		GPIO_Handle.Pin       = GPIO_PIN_5;
	  GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_D, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
	
	  GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	  GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_B, &GPIO_Handle);
	
		
		LCD_SPI_CS_Release();
//		gpio_write_pin(GPIO_A, GPIO_PIN_7, GPIO_PIN_SET);
		
		//gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
		LCD_RST_Release();
    co_delay_100us(200);
    //gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_CLEAR);
		LCD_RST_Select();
    co_delay_100us(200);
		LCD_RST_Release();
    //gpio_write_pin(GPIO_A, GPIO_PIN_2, GPIO_PIN_SET);
    co_delay_100us(200);
#elif KTX_PCB_BOARD
		//reset cs BLK
	  GPIO_Handle.Pin       = GPIO_PIN_0;
	  GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_D, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
		
		GPIO_Handle.Pin       = GPIO_PIN_7;
	  GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_0;
    gpio_init(GPIO_A, &GPIO_Handle);
		memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));
	
	  GPIO_Handle.Pin       = GPIO_PIN_6|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	  GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_A, &GPIO_Handle);
	
		LCD_SPI_CS_Release();
		LCD_RST_Release();
    co_delay_100us(200);
		LCD_RST_Select();
    co_delay_100us(200);
		LCD_RST_Release();
    co_delay_100us(200);
#endif
}


static void st77903_init_spi(void)
{
    __SYSTEM_SPI0_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI0_MASTER_CLK_ENABLE();
    
    ST77903_SPI_Handle.SPIx                       = ST77903_SPI_SEL;
    ST77903_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    ST77903_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    ST77903_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    ST77903_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    ST77903_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&ST77903_SPI_Handle);

    __SPI_DMA_TX_ENABLE(ST77903_SPI_SEL);
    __SPI_DMA_TX_LEVEL(ST77903_SPI_SEL, 15);
}

static void st77903_init_dma(void)
{
    __SYSTEM_DMA_CLK_ENABLE();
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    
    DMA_Channel_0.Channel = DMA_Channel0;
    DMA_Channel_0.Init.Data_Flow        = DMA_M2P_DMAC;
    DMA_Channel_0.Init.Request_ID       = 1;
    DMA_Channel_0.Init.Source_Inc       = DMA_ADDR_INC_INC;
    DMA_Channel_0.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    DMA_Channel_0.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    DMA_Channel_0.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&DMA_Channel_0);

    NVIC_SetPriority(DMA_IRQn, 3);
    NVIC_EnableIRQ(DMA_IRQn);
}

static void  st77903_write_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];
    spi_data[0]=0xDE;
    spi_data[1]=0x00;
    spi_data[2]=cmd;
    spi_data[3]=0x00;
		LCD_SPI_CS_Select();
    spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
		LCD_SPI_CS_Release();
   
}

static void  st77903_write_cmd2(uint8_t cmd)
{
    uint8_t spi_data[4];

    spi_data[0]=0xD8;
    spi_data[1]=0x00;
    spi_data[2]=cmd;
    spi_data[3]=0x00;
		LCD_SPI_CS_Select();
    spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
		LCD_SPI_CS_Release();
}

static void  st77903_write_cmd_with_param(uint8_t cmd, uint8_t para)
{
    uint8_t spi_data[5];
    
  
    spi_data[0]=0xDE;
    spi_data[1]=0x00;
    spi_data[2]=cmd;
    spi_data[3]=0x00;
    spi_data[4]=para;
		LCD_SPI_CS_Select();
		spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 5);
		LCD_SPI_CS_Release();
}

static void  st77903_write_buff(uint8_t *buffer, uint8_t len)
{
	LCD_SPI_CS_Select();
	spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)buffer, len);
	LCD_SPI_CS_Release();
}



__attribute__((section("ram_code"))) void display(uint16_t *data)
{
	GLOBAL_INT_DISABLE();
	{
		 display_count = 0;
		 st77903_write_cmd2(0x61);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		for(uint16_t i = 0; i < 400; i++)//400*400
		{
			LCD_SPI_CS_Select();
			uint8_t spi_data[4];
			spi_data[0]=0xDE;
			spi_data[1]=0x00;
			spi_data[2]=0x60;
			spi_data[3]=0x00;
			spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
				
			ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
			ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
			ST77903_SPI_Handle.MultWireParam.InstructLength = INST_0BIT;
			ST77903_SPI_Handle.MultWireParam.Instruct       = 0x0;
			ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_0BIT;
			ST77903_SPI_Handle.MultWireParam.Address        = 0x0;
			spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);


			dma_start(&DMA_Channel_0, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16, DMA_BURST_LEN_16);
		
			while(!dma_get_tfr_Status(DMA_Channel0));
			dma_clear_tfr_Status(DMA_Channel0);
			while(__SPI_IS_BUSY(ST77903_SPI_SEL) == 1);
			display_count++;

	
				
			
			LCD_SPI_CS_Release();	
			
		__SPI_DISABLE(ST77903_SPI_SEL);
		ST77903_SPI_Handle.SPIx->BAUDR = 10;
		__SPI_DATA_FRAME_SIZE(ST77903_SPI_SEL,SPI_FRAME_SIZE_8BIT);		
    __SPI_ENABLE(ST77903_SPI_SEL);
	
			//co_delay_10us(1);

		}
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
	}
	GLOBAL_INT_RESTORE();
}




__attribute__((section("ram_code"))) void st77903_display(uint16_t *data,uint8_t step)
{
	uint8_t spi_data[4];
	GLOBAL_INT_DISABLE();
	switch(step)
	{
		 case 0:
		 p_st77903_psram = data;
		 g_transfer_status=0;
		 display_count = 0;
		 st77903_write_cmd2(0x61);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);//
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 LCD_SPI_CS_Select();
		spi_data[0]=0xDE;
		spi_data[1]=0x00;
		spi_data[2]=0x60;
		spi_data[3]=0x00;
		spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
			
		ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
		ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
		ST77903_SPI_Handle.MultWireParam.InstructLength = INST_0BIT;
		ST77903_SPI_Handle.MultWireParam.Instruct       = 0x0;
		ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_0BIT;
		ST77903_SPI_Handle.MultWireParam.Address        = 0x0;
		spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);
		//dma_start(&DMA_Channel_0, (uint32_t)&data[display_count*400], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, 400/2, DMA_BURST_LEN_16, DMA_BURST_LEN_16);
		dma_start_IT(&DMA_Channel_0, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16);
		break;
		 
		 case 1:
		  LCD_SPI_CS_Select();
			spi_data[0]=0xDE;
			spi_data[1]=0x00;
			spi_data[2]=0x60;
			spi_data[3]=0x00;
			spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);//ºĊ±5us
			ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
			ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
			ST77903_SPI_Handle.MultWireParam.InstructLength = INST_0BIT;
			ST77903_SPI_Handle.MultWireParam.Instruct       = 0x0;
			ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_0BIT;
			ST77903_SPI_Handle.MultWireParam.Address        = 0x0;
			spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);

			//dma_start(&DMA_Channel_0, (uint32_t)&data[display_count*400], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, 400/2, DMA_BURST_LEN_16, DMA_BURST_LEN_16);
		  dma_start_IT(&DMA_Channel_0, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16);

		   
		 break;
		 case 2:
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 st77903_write_cmd2(0x60);
		 co_delay_10us(5);
		 g_transfer_step=0;
		 display_count = 0;
		 g_transfer_status=1;
		 break;
		}
		GLOBAL_INT_RESTORE();
}

uint8_t get_st77903_send_state(void)
{
		return g_transfer_status;
}
__attribute__((section("ram_code"))) void st77903_dma_isr(void)
{
 
		void (*callback)();
    /* Clear Transfer complete status */
    dma_clear_tfr_Status(DMA_Channel_0.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(DMA_Channel_0.Channel);
		while(__SPI_IS_BUSY(ST77903_SPI_SEL) == 1);
		LCD_SPI_CS_Release();	
		__SPI_DISABLE(ST77903_SPI_SEL);
		#if(ST77903_320X320==1)//320的屏幕速率要调低一点
  	ST77903_SPI_Handle.SPIx->BAUDR = 10;
		#endif
		__SPI_DATA_FRAME_SIZE(ST77903_SPI_SEL,SPI_FRAME_SIZE_8BIT);		
		__SPI_ENABLE(ST77903_SPI_SEL);
	  display_count++;
	  if(display_count>=ST77903_REFRESH_LINE)
		{
				g_transfer_step=2;
			  st77903_display(p_st77903_psram,2);
		}
		else
		{
				st77903_display(p_st77903_psram,1);
		}
}

void st77903_init(void)
{
	  uint8_t dat[45] = {0xDE, 0x00 , 0x00, 0x00};
	  co_printf("st77903_init\r\n");
	  st77903_init_io();
    st77903_init_spi();
    st77903_init_dma();
	  co_printf("st77903_init io\r\n");
		LCD_BCAKLIGHT_SET();
   #if(ST77903_400X400==1)
		
    st77903_write_cmd_with_param(0xF0, 0xc3);
    st77903_write_cmd_with_param(0xF0, 0x96);
    st77903_write_cmd_with_param(0xF0, 0xA5);
	
    st77903_write_cmd_with_param(0xE9, 0x20);

    dat[2] =  0xE7;
    dat[4] =  0x80;
    dat[5] =  0x77;
    dat[6] =  0x1F;
    dat[7] =  0xCC;
    st77903_write_buff(dat, 8);//
		
	dat[2] =  0xc1;
    dat[4] =  0x77;
    dat[5] =  0x07;
    dat[6] =  0xcf;
    dat[7] =  0x16;
    st77903_write_buff(dat, 8);//	
		
	dat[2] =  0xc2;
    dat[4] =  0x77;
    dat[5] =  0x07;
    dat[6] =  0xcf;
    dat[7] =  0x16;
    st77903_write_buff(dat, 8);//	
			
	dat[2] =  0xc3;
    dat[4] =  0x22;
    dat[5] =  0x02;
    dat[6] =  0x22;
    dat[7] =  0x04;
    st77903_write_buff(dat, 8);//	
	
	dat[2] =  0xc4;
    dat[4] =  0x22;
    dat[5] =  0x02;
    dat[6] =  0x22;
    dat[7] =  0x04;
    st77903_write_buff(dat, 8);//	

    st77903_write_cmd_with_param(0xC5, 0xED);//
	
	dat[2] =   0xE0;
    dat[4] =   0x87;
    dat[5] =   0x09;
    dat[6] =   0x0c;
    dat[7] =   0x06;
    dat[8] =   0x05;
    dat[9] =   0x03;
    dat[10] =  0x29;
    dat[11] =  0x32;
    dat[12] =  0x49;
    dat[13] =  0x0f;
    dat[14] =  0x1b;
    dat[15] =  0x17;
    dat[16] =  0x2a;
    dat[17] =  0x2f;
    st77903_write_buff(dat, 18);//	
	 dat[2] =   0xE1;
    dat[4] =   0x87;
    dat[5] =   0x09;
    dat[6] =   0x0c;
    dat[7] =   0x06;
    dat[8] =   0x05;
    dat[9] =   0x03;
    dat[10] =  0x29;
    dat[11] =  0x32;
    dat[12] =  0x49;
    dat[13] =  0x0f;
    dat[14] =  0x1b;
    dat[15] =  0x17;
    dat[16] =  0x2a;
    dat[17] =  0x2f;
    st77903_write_buff(dat, 18);//


    dat[2] = 0xE5;
    dat[4] = 0xbe;
    dat[5] = 0xf5;
    dat[6] = 0xb1;
    dat[7] = 0x22;
    dat[8] = 0x22;
    dat[9] = 0x25;
    dat[10] =0x10;
    dat[11] =0x22;
    dat[12] =0x22;
    dat[13] =0x22;
    dat[14] =0x22;
    dat[15] =0x22;
    dat[16] =0x22;
    dat[17] =0x22;
    st77903_write_buff(dat, 18);//
 
    dat[2] =  0xE6;
    dat[4] =  0xbe;
    dat[5] =  0xf5;
    dat[6] =  0xb1;
    dat[7] =  0x22;
    dat[8] =  0x22;
    dat[9] =  0x25;
    dat[10] = 0x10;
    dat[11] = 0x22;
    dat[12] = 0x22;
    dat[13] = 0x22;
    dat[14] = 0x22;
    dat[15] = 0x22;
    dat[16] = 0x22;
    dat[17] = 0x22;
    st77903_write_buff(dat, 18);//
	
	dat[2] =  0xec;
    dat[4] =  0x40;
    dat[5] =  0x03;
    st77903_write_buff(dat, 6);

	st77903_write_cmd_with_param(0xb2, 0x00);//
	st77903_write_cmd_with_param(0xb3, 0x01);//
	st77903_write_cmd_with_param(0xb4, 0x00);//
	dat[2] =  0xb5;
    dat[4] =  0x00;
    dat[5] =  0x08;
    dat[6] =  0x00;
    dat[7] =  0x08;
    st77903_write_buff(dat, 8);
	dat[2] =  0xb6;
    dat[4] =  0xc7;
    dat[5] =  0x31;
    st77903_write_buff(dat, 6);//
	
	dat[2] =  0xa5;
    dat[4] =  0x00;
    dat[5] =  0x00;
    dat[6] =  0x00;
    dat[7] =  0x00;
    dat[8] =  0x00;
    dat[9] =  0x15;
    dat[10] = 0x2a;
    dat[11] = 0x8a;
    dat[12] = 0x02;
    st77903_write_buff(dat, 13);//	
	
	dat[2] =  0xa6;
    dat[4] =  0x00;
    dat[5] =  0x00;
    dat[6] =  0x00;
    dat[7] =  0x00;
    dat[8] =  0x00;
    dat[9] =  0x15;
    dat[10] = 0x2a;
	  dat[11] = 0x8a;
    dat[12] = 0x02;
    st77903_write_buff(dat, 13);//	
	
	dat[2] =  0xba;
    dat[4] =  0x0a;
    dat[5] =  0x5a;
    dat[6] =  0x23;
    dat[7] =  0x10;
    dat[8] =  0x25;
    dat[9] =  0x02;
    dat[10] = 0x00;
    st77903_write_buff(dat, 11);//	
	
	dat[2] =  0xbb;
    dat[4] =  0x00;
    dat[5] =  0x30;
    dat[6] =  0x00;
    dat[7] =  0x2c;
    dat[8] =  0x82;
    dat[9] =  0x87;
    dat[10] = 0x18;
	dat[11] = 0x00;
    st77903_write_buff(dat, 12);//	
	dat[2] =  0xbc;
    dat[4] =  0x00;
    dat[5] =  0x30;
    dat[6] =  0x00;
    dat[7] =  0x2c;
    dat[8] =  0x82;
    dat[9] =  0x87;
    dat[10] = 0x18;
	dat[11] = 0x00;
    st77903_write_buff(dat, 12);//
	dat[2] =  0xbd;
    dat[4] =  0xa1;
    dat[5] =  0xb2;
    dat[6] =  0x2b;
    dat[7] =  0x1a;
    dat[8] =  0x56;
    dat[9] =  0x43;
    dat[10] = 0x34;
    dat[11] = 0x65;
    dat[12] = 0xff;
	dat[13] = 0xff;
    st77903_write_buff(dat, 14);//
	
	st77903_write_cmd_with_param(0x35, 0x00);//
	st77903_write_cmd_with_param(0x36, 0x0c);//
  st77903_write_cmd_with_param(0x3A, 0x05);//565
	//st77903_write_cmd_with_param(0x3a, 0x07);//888
	st77903_write_cmd(0x21);
	st77903_write_cmd(0x11);
    co_delay_100us(1200);
    st77903_write_cmd(0x29);

#endif
	
	
	 #if(ST77903_320X320==1)
		st77903_write_cmd_with_param(0xF0, 0xc3);
    st77903_write_cmd_with_param(0xF0, 0x96);
    st77903_write_cmd_with_param(0xF0, 0xA5);
    dat[2] = 0xc1;
    dat[4] = 0x77;
    dat[5] = 0x05;
    dat[6] = 0xCF;
    dat[7] = 0x11;
    st77903_write_buff(dat, 8);

    dat[2] = 0xc2;
    dat[4] = 0x77;
    dat[5] = 0x05;
    dat[6] = 0xcf;
    dat[7] = 0x11;
    st77903_write_buff(dat, 8);//


    dat[2] = 0xc3;
    dat[4] = 0x32;
    dat[5] = 0x03;
    dat[6] = 0x33;
    dat[7] = 0x04;
    st77903_write_buff(dat, 8);//


    dat[2] =  0xC4;
    dat[4] =  0x32;
    dat[5] =  0x03;
    dat[6] =  0x33;
    dat[7] =  0x04;
    st77903_write_buff(dat, 8);
#if 0
    dat[2] = 0xc4;
    dat[4] = 0x42;
    dat[5] = 0x02;
    dat[6] = 0x24;
    dat[7] = 0x03;
    st77903_write_buff(dat, 8);
#endif
st77903_write_cmd_with_param(0xc5, 0x7d);
st77903_write_cmd_with_param(0xd6, 0x00);
st77903_write_cmd_with_param(0xd7, 0x00);








    dat[2] =  0xE0;
    dat[4] =  0xF0;
    dat[5] =  0x04;
    dat[6] =  0x08;
    dat[7] =  0x09;
    dat[8] =  0x08;
    dat[9] =  0x05;
    dat[10] = 0x2E;
    dat[11] = 0x43;
    dat[12] = 0x47;
    dat[13] = 0x19;
    dat[14] = 0x16;
    dat[15] = 0x15;
    dat[16] = 0x2D;
    dat[17] = 0x34;
    st77903_write_buff(dat, 18);//

  
    dat[2] =   0xE1;
    dat[4] =   0xF0;
    dat[5] =   0x03;
    dat[6] =   0x07;
    dat[7] =   0x08;
    dat[8] =   0x07;
    dat[9] =   0x24;
    dat[10] =  0x2D;
    dat[11] =  0x33;
    dat[12] =  0x47;
    dat[13] =  0x39;
    dat[14] =  0x15;
    dat[15] =  0x15;
    dat[16] =  0x2D;
    dat[17] =  0x33;
    st77903_write_buff(dat, 18);//


    dat[2] = 0xE5;
    dat[4] = 0xDE;
    dat[5] = 0xF5;
    dat[6] = 0xE0;
    dat[7] = 0x11;
    dat[8] = 0x22;
    dat[9] = 0x25;
    dat[10] =0x10;
    dat[11] =0x22;
    dat[12] =0x22;
    dat[13] =0x22;
    dat[14] =0x22;
    dat[15] =0x22;
    dat[16] =0x22;
    dat[17] =0x22;
    st77903_write_buff(dat, 18);//
 
    dat[2] =  0xE6;
    dat[4] =  0xDE;
    dat[5] =  0xF5;
    dat[6] =  0xE0;
    dat[7] =  0x11;
    dat[8] =  0x22;
    dat[9] =  0x25;
    dat[10] = 0x10;
    dat[11] = 0x22;
    dat[12] = 0x22;
    dat[13] = 0x22;
    dat[14] = 0x22;
    dat[15] = 0x22;
    dat[16] = 0x22;
    dat[17] = 0x22;
    st77903_write_buff(dat, 18);//
		

    dat[2] =  0xE7;
    dat[4] =  0x80;
    dat[5] =  0x77;
    dat[6] =  0x1F;
    dat[7] =  0xCC;

    st77903_write_buff(dat, 8);//
		
		
		
		

    dat[2] =  0xec;
		dat[4] =  0x00;
    dat[5] =  0x55;
    dat[6] =  0x00;
		dat[7] =  0x00;
		dat[8] =  0x00;
		dat[9] =  0x88;
    st77903_write_buff(dat, 10);//

    st77903_write_cmd_with_param(0x36, 0x08);//
    st77903_write_cmd_with_param(0x3A, 0x05);//
		
		dat[2] =  0xb1;
    dat[4] =  0xfe;
    dat[5] =  0xdf;
    st77903_write_buff(dat, 6);//
		

    st77903_write_cmd_with_param(0xb2, 0x09);//

    st77903_write_cmd_with_param(0xb3, 0x01);//

    st77903_write_cmd_with_param(0xb4, 0x01);//

    dat[2] =  0xb5;
    dat[4] =  0x00;
    dat[5] =  0x08;
    dat[6] =  0x00;
    dat[7] =  0x08;
    st77903_write_buff(dat, 8);//

    dat[2] =  0xb6;
    dat[4] =  0x9f;
    dat[5] =  0x27;//// 320RGB x 320
    st77903_write_buff(dat, 6);//



    dat[2] =  0xA4;
    dat[4] =  0xc0;
    dat[5] =  0x6b;
 
    st77903_write_buff(dat, 6);//



    dat[2] =  0xA5;
    dat[4] =  0x20;
    dat[5] =  0x12;
    dat[6] =  0x40;
    dat[7] =  0x01;
    dat[8] =  0x00;
    dat[9] =  0x14;
    dat[10] = 0x2A;
    dat[11] = 0x0A;
    dat[12] = 0x00;
    st77903_write_buff(dat, 13);//


   

    dat[2] =  0xA6;
    dat[4] =  0x20;
    dat[5] =  0x12;
    dat[6] =  0x40;
    dat[7] =  0x01;
    dat[8] =  0x00;
    dat[9] =  0x14;
    dat[10] = 0x2A;
    dat[11] = 0x0A;
    dat[12] = 0x00;
    st77903_write_buff(dat, 13);//
 

    dat[2] =   0xBA;
    dat[4] =   0x1E;
    dat[5] =   0x06;
    dat[6] =   0x03;
    dat[7] =   0x00;
    dat[8] =   0x31;
    dat[9] =   0x01;
    dat[10] =  0x00;
    st77903_write_buff(dat, 11);//

    dat[2] =  0xBB;
    dat[4] =  0x00;
    dat[5] =  0x3E;
    dat[6] =  0x00;
    dat[7] =  0x35;
    dat[8] =  0x0C;
    dat[9] =  0x07;
    dat[10] = 0x07;
    dat[11] = 0x04;
    st77903_write_buff(dat, 12);//


    dat[2] =  0xBC;
    dat[4] =  0x00;
    dat[5] =  0x3E;
    dat[6] =  0x00;
    dat[7] =  0x35;
    dat[8] =  0x0C;
    dat[9] =  0x07;
    dat[10] = 0x07;
    dat[11] = 0x04;
    st77903_write_buff(dat, 12);//

    dat[2] =  0xBD;
    dat[4] =  0x19;
    dat[5] =  0x91;
    dat[6] =  0xFF;
    dat[7] =  0xFF;
    dat[8] =  0x25;
    dat[9] =  0x34;
    dat[10] = 0x43;
    dat[11] = 0x52;
    dat[12] = 0x87;
    dat[13] = 0xFF;
    dat[14] = 0x0F;
    st77903_write_buff(dat, 15);//
		dat[2] =  0xCC;
    dat[4] =  0x00;
    dat[5] =  0x00;
    dat[6] =  0x3f;
    dat[7] =  0x00;
   dat[8] =  0x0a;
    dat[9] =  0x0a;
    dat[10] = 0xea;
    dat[11] = 0xea;
    dat[12] = 0x03;
    st77903_write_buff(dat, 13);//
    st77903_write_cmd_with_param(0x35, 0x00);//
		st77903_write_cmd_with_param(0xed, 0xc3);//
		st77903_write_cmd_with_param(0x36, 0x0c);//	
		st77903_write_cmd(0x21);
    st77903_write_cmd(0x11);
    co_delay_100us(1200);
    st77903_write_cmd(0x29);
#endif

		co_printf("st77903_end\r\n");
}




