

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
 


static SPI_HandleTypeDef  		ST77903_SPI_Handle;
static DMA_HandleTypeDef 			ST77903_DMA_Handle;
#define ST77903_REFRESH_LINE 	400

volatile uint8_t g_transfer_step=0;
volatile uint8_t g_transfer_status=1;
static uint16_t *ST77903_RAM_ADDR=NULL;
volatile  uint32_t display_count = 0;


/*
第一个字节是屏的指令，第二个是后面参数的长度，没有参数的话就填0
第一个字节如果是0XFF，表示此处需要做delay，delay长度就是后面的参数
*/
static const lcd_reg_param_t lcd_reg_table[] = {
{0xf0,  1, 0xc3},
{0xf0,  1, 0x96},
{0xf0,  1, 0xa5},
{0xe9,  1, 0x20},
{0xe7,  4, 0x80, 0x77, 0x1f, 0xcc},
{0xc1,  4, 0x77, 0x07, 0xc2, 0x07},
{0xc2,  4, 0x77, 0x07, 0xc2, 0x07},
{0xc3,  4, 0x22, 0x02, 0x22, 0x04},
{0xc4,  4, 0x22, 0x02, 0x22, 0x04},
{0xc5,  1, 0x71},
{0xe0, 14, 0x87, 0x09, 0x0c, 0x06, 0x05, 0x03, 0x29, 0x32, 0x49, 0x0f, 0x1b, 0x17, 0x2a, 0x2f},
{0xe1, 14, 0x87, 0x09, 0x0c, 0x06, 0x05, 0x03, 0x29, 0x32, 0x49, 0x0f, 0x1b, 0x17, 0x2a, 0x2f},
{0xe5, 14, 0xb2, 0xf5, 0xbd, 0x24, 0x22, 0x25, 0x10, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
{0xe6, 14, 0xb2, 0xf5, 0xbd, 0x24, 0x22, 0x25, 0x10, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
{0xec,  2, 0x40, 0x03},
{0x36,  1, 0x0c},
//{0x3a,  1, 0x07},
{0x3a,  1, 0x05},
{0xb2,  1, 0x00},
{0xb3,  1, 0x01},
{0xb4,  1, 0x00},
{0xb5,  4, 0x00, 0x08, 0x00, 0x08},
{0xb6,  2, 0xc7, 0x31},
{0xa5,  9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x15, 0x2a, 0x8a, 0x02},
{0xa6,  9, 0x00, 0x00, 0x00, 0x00, 0x20, 0x15, 0x2a, 0x8a, 0x02},
{0xba,  7, 0x0a, 0x5a, 0x23, 0x10, 0x25, 0x02, 0x00},
{0xbb,  8, 0x00, 0x30, 0x00, 0x29, 0x88, 0x87, 0x18, 0x00},
{0xbc,  8, 0x00, 0x30, 0x00, 0x29, 0x88, 0x87, 0x18, 0x00},
{0xbd, 11, 0xa1, 0xb2, 0x2b, 0x1a, 0x56, 0x43, 0x34, 0x65, 0xff, 0xff, 0x0f},
{0x35,  1, 0x00},
{0x21,  0, 0x00},
{0x11,  0, 0x00},
{0xff,  1, 120},
{0x29,  0, 0x00},
{0xff,  1, 120},
#ifdef LCD_BIST_MODE
{0xb0,  1, 0xa5},
{0xcc,  9, 0x40, 0x00, 0x3f, 0x00, 0x14, 0x14, 0x20, 0x20, 0x03},
#endif
};
 

 
static void st77903_write_param_cmd(uint32_t cmd, uint32_t len, uint8_t *buff)
{
	uint8_t spi_data[45];
	spi_data[0]=0xDE;
	spi_data[1]=0x00;
	spi_data[3]=0x00;
    if (len == 0)              
    {           
			  	spi_data[2]=cmd;
				ST77903_CS_Select();
				spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
				ST77903_CS_Release();
    }
    else if (len <= INIT_CMD_LEN)
    {    
				 spi_data[2]=cmd;
			   	 memcpy(&spi_data[4],buff,len);
				 ST77903_CS_Select();
				 spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, (len+4));
				 ST77903_CS_Release();
    }
    else
    {    
 
				ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_8BIT;
				ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
				ST77903_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
				ST77903_SPI_Handle.MultWireParam.Instruct       = 0xDE;
				ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
				ST77903_SPI_Handle.MultWireParam.Address        = cmd<<8;
		  		ST77903_CS_Select();
				spi_master_transmit_X2X4(&ST77903_SPI_Handle, (uint16_t *)buff, len);
				ST77903_CS_Release();
    }
    
   
}

static void st77903_reg_init(void) //lcd reg init
{
    ST77903_RST_Release();
    co_delay_100us(100);
    ST77903_RST_Select();
    co_delay_100us(100);
    ST77903_RST_Release();
    co_delay_100us(200);
    lcd_reg_param_t *ptr = (lcd_reg_param_t *)&lcd_reg_table[0];
    for (uint16_t i = 0; i < sizeof(lcd_reg_table)/sizeof(lcd_reg_param_t); i++)
    {                         
        if (ptr->cmd == 0xff)
        {                       /* delay */
            co_delay_100us(ptr->buf[0]*10);
        }
        else
        {    /* cmd - param */
            st77903_write_param_cmd(ptr->cmd, ptr->len, ptr->buf);
        }
        ptr++;
    }
		  ST77903_BLK_Select();
}

void st77903_init_io(void)
{
	GPIO_InitTypeDef GPIO_Handle;

	__SYSTEM_GPIO_CLK_ENABLE();

	//reset cs backlight
	GPIO_Handle.Pin       = ST77903_RST_GPIO;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	gpio_init(ST77903_RST_PORT, &GPIO_Handle);
	memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));

	GPIO_Handle.Pin       = ST77903_CS_GPIO;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	gpio_init(ST77903_CS_PORT, &GPIO_Handle);
	memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));

	GPIO_Handle.Pin       = ST77903_BACKLIGHT_GPIO;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	gpio_init(ST77903_BACKLIGHT_PORT, &GPIO_Handle);
	memset((uint8_t *)&GPIO_Handle,0,sizeof(GPIO_InitTypeDef));

	#ifdef ST77903_TE_EN
	GPIO_Handle.Pin       = ST77903_TE_GPIO;
	GPIO_Handle.Mode      = GPIO_MODE_INPUT;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	gpio_init(ST77903_TE_PORT, &GPIO_Handle);
	#endif

	//sclk io0 io1 io2 io3
	GPIO_Handle.Pin       = ST77903_QIO0_GPIO|ST77903_QIO1_GPIO|ST77903_QIO2_GPIO|ST77903_QIO3_GPIO|ST77903_SCLK_GPIO;
	GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_2;
	gpio_init(ST77903_SCLK_PORT, &GPIO_Handle);



	ST77903_CS_Release();
	ST77903_BLK_Release();

}



static void st77903_init_spi(void)
{
	 #if KTX_PCB_BOARD
    __SYSTEM_SPI1_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI1_MASTER_CLK_ENABLE();
    ST77903_SPI_Handle.SPIx                       = ST77903_SPI_SEL;
    ST77903_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    ST77903_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    ST77903_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    ST77903_SPI_Handle.Init.TxFIFOEmpty_Threshold = 15;
    ST77903_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;
    spi_master_init(&ST77903_SPI_Handle);
    __SPI_DMA_TX_ENABLE(ST77903_SPI_SEL);
    __SPI_DMA_TX_LEVEL(ST77903_SPI_SEL, 15);
	 #else
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
	 #endif
}

static void st77903_init_dma(void)
{
    __SYSTEM_DMA_CLK_ENABLE();
	  #if KTX_PCB_BOARD
	  __DMA_REQ_ID_SPI1_MASTER_TX(1);
	  #else
    __DMA_REQ_ID_SPI0_MASTER_TX(1);
    #endif
    ST77903_DMA_Handle.Channel = DMA_Channel0;
    ST77903_DMA_Handle.Init.Data_Flow        = DMA_M2P_DMAC;
    ST77903_DMA_Handle.Init.Request_ID       = 1;
    ST77903_DMA_Handle.Init.Source_Inc       = DMA_ADDR_INC_INC;
    ST77903_DMA_Handle.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    ST77903_DMA_Handle.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    ST77903_DMA_Handle.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;
    dma_init(&ST77903_DMA_Handle);
//    NVIC_SetPriority(DMA_IRQn, 3);
    NVIC_EnableIRQ(DMA_IRQn);
}

 
static void  st77903_write_sync_cmd(uint8_t cmd)
{
    uint8_t spi_data[4];
    spi_data[0]=0xD8;//single send cmd
    spi_data[1]=0x00;
    spi_data[2]=cmd;
    spi_data[3]=0x00;
	ST77903_CS_Select();
	spi_master_transmit_X1(&ST77903_SPI_Handle, (uint16_t *)spi_data, 4);
	ST77903_CS_Release();

}



__attribute__((section("ram_code"))) void display(uint16_t *data)
{
GLOBAL_INT_DISABLE();
	{
		 display_count = 0;
		 st77903_write_sync_cmd(0x61);
		 co_delay_10us(5);//
		 for(uint8_t i=0;i<6;i++)
		 {
				st77903_write_sync_cmd(0x60);
			  co_delay_10us(5);//
		}
		for(uint16_t i = 0; i < 400; i++)
		{
			ST77903_CS_Select();
	 
			ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
			ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
			ST77903_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
			ST77903_SPI_Handle.MultWireParam.Instruct       = 0xDE;
			ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
			ST77903_SPI_Handle.MultWireParam.Address        = 0x006000;
			spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);
			dma_start(&ST77903_DMA_Handle, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16, DMA_BURST_LEN_16);
		
			while(!dma_get_tfr_Status(DMA_Channel0));
			dma_clear_tfr_Status(DMA_Channel0);
			while(__SPI_IS_BUSY(ST77903_SPI_SEL) == 1);
			display_count++;
			ST77903_CS_Release();	
			__SPI_DISABLE(ST77903_SPI_SEL);
			//ST77903_SPI_Handle.SPIx->BAUDR = 10;
			__SPI_DATA_FRAME_SIZE(ST77903_SPI_SEL,SPI_FRAME_SIZE_8BIT);		
			__SPI_ENABLE(ST77903_SPI_SEL);
 
		}
		 for(uint8_t i=0;i<6;i++)
		 {
				st77903_write_sync_cmd(0x60);
			  co_delay_10us(5);//
		 }  
	}
	GLOBAL_INT_RESTORE();
}

__attribute__((section("ram_code"))) void st77903_display(uint16_t *data,uint8_t step)
{
	GLOBAL_INT_DISABLE();
	switch(step)
	{
		 case 0:
		 ST77903_RAM_ADDR = data;
		 g_transfer_status=0;
		 display_count = 0;
		 st77903_write_sync_cmd(0x61);
		 co_delay_10us(5);//
		 for(uint8_t i=0;i<6;i++)
		 {
			  st77903_write_sync_cmd(0x60);
			  co_delay_10us(5);//
		 }
		  ST77903_CS_Select();
			ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
			ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
			ST77903_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
			ST77903_SPI_Handle.MultWireParam.Instruct       = 0xDE;
			ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
			ST77903_SPI_Handle.MultWireParam.Address        = 0x006000;
			spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);
			dma_start_IT(&ST77903_DMA_Handle, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16);
		 break;
		 case 1:
		  ST77903_CS_Select();
			ST77903_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
			ST77903_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
			ST77903_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
			ST77903_SPI_Handle.MultWireParam.Instruct       = 0xDE;
			ST77903_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
			ST77903_SPI_Handle.MultWireParam.Address        = 0x006000;
			spi_master_transmit_X2X4_DMA(&ST77903_SPI_Handle);
		    dma_start_IT(&ST77903_DMA_Handle, (uint32_t)&data[display_count*ST77903_REFRESH_LINE], (uint32_t)&ST77903_SPI_Handle.SPIx->DR, ST77903_REFRESH_LINE/2, DMA_BURST_LEN_16);
		 break;
		 case 2:
		 {			 
		  #ifdef ST77903_TE_EN
			 static uint8_t te_signal_cnt=0;
			 if(get_lcd_te_state()==true)
			 {
					if(++te_signal_cnt>=5)//te提前到达 屏幕出现异常情况
					{
						  printf("te signal err\r\n");
							te_signal_cnt = 0;
							//执行屏幕初始化指令 
							st77903_reg_init();
						  g_transfer_step=0;
						  display_count = 0;
						  g_transfer_status=1;
							//st77903_display(ST77903_RAM_ADDR,0);
							return ;
					}
			 }else{
					te_signal_cnt=0;
			 }	
			 #endif
		 //st77903_read_te_handler();
			 for(uint8_t i=0;i<6;i++)
			 {
					st77903_write_sync_cmd(0x60);
					co_delay_10us(5);//
			 }
			 g_transfer_step=0;
			 display_count = 0;
			 g_transfer_status=1;
	   }
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

	/* Clear Transfer complete status */
	dma_clear_tfr_Status(ST77903_DMA_Handle.Channel);//
	/* channel Transfer complete interrupt disable */
	dma_tfr_interrupt_disable(ST77903_DMA_Handle.Channel);
	while(__SPI_IS_BUSY(ST77903_SPI_Handle.SPIx) == 1); //1-2us
	ST77903_CS_Release();	
	__SPI_DISABLE(ST77903_SPI_Handle.SPIx);
	#if(ST77903_320X320==1)//320的屏幕速率要调低一点
	ST77903_SPI_Handle.SPIx->BAUDR = 10;
	#endif
	__SPI_DATA_FRAME_SIZE(ST77903_SPI_Handle.SPIx,SPI_FRAME_SIZE_8BIT);		
	__SPI_ENABLE(ST77903_SPI_Handle.SPIx);
	display_count++;
	if(display_count>=ST77903_REFRESH_LINE)
	{
			g_transfer_step=2;
			st77903_display(ST77903_RAM_ADDR,2);
	}
	else
	{
			st77903_display(ST77903_RAM_ADDR,1);
	}
}

void st77903_init(void)
{
	co_printf("st77903_init\r\n");
	st77903_init_io();
	st77903_init_spi();
	st77903_init_dma();
	co_printf("st77903_init io\r\n");
	st77903_reg_init();
	co_printf("end\r\n");
 
}




