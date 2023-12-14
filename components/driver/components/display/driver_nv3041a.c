/*
@driver_nv3041a.c
@author: LH 
Creation Date£º2023/07/12
*/
#include "driver_dma.h"
#include "driver_system.h"
#include "driver_if8080.h"
#include "driver_gpio.h"
#include "driver_nv3041a.h"

#define LCD_3_9


#define LCD_RELEASE_RESET()         gpio_write_pin(GPIO_E, GPIO_PIN_0, GPIO_PIN_SET)
#define LCD_SET_RESET()             gpio_write_pin(GPIO_E, GPIO_PIN_0, GPIO_PIN_CLEAR)




static DMA_LLI_InitTypeDef Link_Channel[20];
static DMA_HandleTypeDef nv3041a_DMA_Channel;
static dma_LinkParameter_t LinkParameter;
static volatile bool dma_transfer_done = true;
static void (*dma_trans_done_callback)(void) = NULL;



static void nv3041a_init_io(void)
{
     GPIO_InitTypeDef GPIO_Handle;
   __SYSTEM_GPIO_CLK_ENABLE();

    GPIO_Handle.Pin       = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;
    GPIO_Handle.Alternate = GPIO_FUNCTION_D;
    gpio_init(GPIO_B, &GPIO_Handle);

    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, PORTD6_FUNC_LCD_WRX); // 8080_WR
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_7, PORTD7_FUNC_LCD_RDX); // 8080_RD
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_6, PORTA6_FUNC_LCD_CSX); // 8080_CS
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_7, PORTA7_FUNC_LCD_DCX); // 8080_DC
    
    // reset
		GPIO_Handle.Pin       = GPIO_PIN_0;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	  GPIO_Handle.Alternate = GPIO_FUNCTION_0;
		GPIO_Handle.Pull      = GPIO_PULLUP;
    gpio_init(GPIO_E, &GPIO_Handle);
	//	gpio_write_pin(GPIO_E,GPIO_PIN_0,1);

}

static void nv3041a_init_8080(void)
{
    str_LCDParam_t str_LCDParam;
    
    __SYSTEM_IF8080_CLK_ENABLE();
    __SYSTEM_IF8080_CLK_SELECT_48M();
    
    str_LCDParam.LcdMode = MODE_8080;
    str_LCDParam.DataBusSelect = DATA_BUS_8_BIT;
    str_LCDParam.ReadClock = RDCLK_DIV_8;
    str_LCDParam.WriteClock = WDCLK_DIV_1; 

    if8080_init(str_LCDParam);
    IF8080->DATA_CFG.DATA_TRANS_SEQ_0 = 1;
    IF8080->DATA_CFG.DATA_TRANS_SEQ_1 = 0;
    IF8080->DATA_CFG.DATA_TRANS_SEQ_2 = 3;
    IF8080->DATA_CFG.DATA_TRANS_SEQ_3 = 2;
	
	  IF8080->INT.TE_HSYNC_INT_ENABLE = 0;
    IF8080->INT.TE_VSYNC_INT_ENABLE = 0;
    IF8080->INT.TX_AEMPT_INT_ENABLE = 0;
    IF8080->INT.TX_EMPT_INT_ENABLE = 0;
    IF8080->INT.TX_FULL_INT_ENABLE = 0;
}

static void nv3041a_init_dma(void)
{
    __SYSTEM_DMA_CLK_ENABLE();
    
    nv3041a_DMA_Channel.Channel = DMA_Channel0;
    nv3041a_DMA_Channel.Init.Data_Flow        = DMA_M2P_DMAC;
    nv3041a_DMA_Channel.Init.Request_ID       = 0;
    nv3041a_DMA_Channel.Init.Source_Inc       = DMA_ADDR_INC_INC;
    nv3041a_DMA_Channel.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    nv3041a_DMA_Channel.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    nv3041a_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_32;

    dma_init(&nv3041a_DMA_Channel);
    
    NVIC_EnableIRQ(DMA_IRQn);
}

static void nv3041a_write_cmd(uint8_t cmd)
{
  if8080_cs_set();
	if8080_write_cmd(cmd);
//	co_delay_10us(1);
	if8080_cs_release();
}

static void nv3041a_write_param(uint8_t data)
{
	//uint32_t param = data;
	#if 1
  if8080_cs_set();
	if8080_write_param(data);
	//co_delay_10us(1);
	if8080_cs_release();
	#endif
	
}

void nv3041a_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{

  nv3041a_write_cmd(0x2A);             
	nv3041a_write_param(x_s>>8);             
	nv3041a_write_param(x_s&0xff);             
	nv3041a_write_param(x_e>>8);             
	nv3041a_write_param(x_e&0xff);             
	
//	y_s += 20;
//	y_e += 20;
	nv3041a_write_cmd(0x2B);             
	nv3041a_write_param(y_s>>8);             
	nv3041a_write_param(y_s&0xff);             
	nv3041a_write_param(y_e>>8);             
	nv3041a_write_param(y_e&0xff);   
	
	nv3041a_write_cmd(0x2c);
}

static uint8_t color_buffer[100*100*2]={00};


void nv3041a_init(void)
{
//	 lcd_sfrt_init();
//	LCD_Init();
//	 DispBand();
	
	 #if 1
    nv3041a_init_io();
    nv3041a_init_8080();
    nv3041a_init_dma();
	
	 LCD_RELEASE_RESET();
    co_delay_100us(10);
    LCD_SET_RESET();
    co_delay_100us(100);
    LCD_RELEASE_RESET();
    co_delay_100us(1200);
	
		#if 0
		nv3041a_write_cmd(0xff);
		nv3041a_write_param(0xa5);
		nv3041a_write_cmd(0xE7); //TE_output_en
		nv3041a_write_param(0x10);
		nv3041a_write_cmd(0x35); //TE_ interface_en
		nv3041a_write_param(0x00);//01
		nv3041a_write_cmd(0x36); //
		nv3041a_write_param(0x00);//c0
		nv3041a_write_cmd(0x3A); //
		nv3041a_write_param(0x01);//00---666/01---565
		nv3041a_write_cmd(0x41); 
		nv3041a_write_param(0x01);//01--8bit/03--16bit
		nv3041a_write_cmd(0x44); 
		nv3041a_write_param(0x15); 																														
		nv3041a_write_cmd(0x45); 
		nv3041a_write_param(0x15); 
		nv3041a_write_cmd(0x7d);//vdds_trim[2:0]
		nv3041a_write_param(0x03);
		 
		nv3041a_write_cmd(0xc1);//avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]
		nv3041a_write_param(0xbb);//0xbb 88 a2
		nv3041a_write_cmd(0xc2);//vgl_clp_en vgl_clp[2:0]
		nv3041a_write_param(0x05);
		nv3041a_write_cmd(0xc3);//vgl_clp_en vgl_clp[2:0]
		nv3041a_write_param(0x10);
		nv3041a_write_cmd(0xc6);//avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]
		nv3041a_write_param(0x3E); // 3E
		nv3041a_write_cmd(0xc7);//mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]
		nv3041a_write_param(0x25); //25

		nv3041a_write_cmd(0xc8);// VGL_CLK_sel
		nv3041a_write_param(0x11); 
		nv3041a_write_cmd(0x74);
		nv3041a_write_param(0x12); 
		nv3041a_write_cmd(0x7a);// user_vgsp
		nv3041a_write_param(0x39); 
		nv3041a_write_cmd(0x6f);// user_gvdd
		nv3041a_write_param(0x22);
		nv3041a_write_cmd(0x78);// user_gvcl
		nv3041a_write_param(0x4F); 
		nv3041a_write_cmd(0xc9);//
		nv3041a_write_param(0x00); 
		nv3041a_write_cmd(0x67); 
		nv3041a_write_param(0x21);
		//gate_ed
		nv3041a_write_cmd(0x51);//gate_st_o[7:0]
		nv3041a_write_param(0x0a);
		nv3041a_write_cmd(0x52);//gate_ed_o[7:0]
		nv3041a_write_param(0x76); //76
		nv3041a_write_cmd(0x53);//gate_st_e[7:0]
		nv3041a_write_param(0x0a); //76
		nv3041a_write_cmd(0x54);//gate_ed_e[7:0]
		nv3041a_write_param(0x76);
		////sorce 
		nv3041a_write_cmd(0x46);//fsm_hbp_o[5:0]
		nv3041a_write_param(0x0a);
		nv3041a_write_cmd(0x47);//fsm_hfp_o[5:0]
		nv3041a_write_param(0x2a);
		nv3041a_write_cmd(0x48);//fsm_hbp_e[5:0]
		nv3041a_write_param(0x0a);
		nv3041a_write_cmd(0x49);//fsm_hfp_e[5:0]
		nv3041a_write_param(0x1a);
		nv3041a_write_cmd(0x56);//src_ld_wd[1:0] src_ld_st[5:0]
		nv3041a_write_param(0x43);
		nv3041a_write_cmd(0x57);//pn_cs_en src_cs_st[5:0]
		nv3041a_write_param(0x42);
		nv3041a_write_cmd(0x58);//src_cs_p_wd[6:0]
		nv3041a_write_param(0x3c);
		nv3041a_write_cmd(0x59);//src_cs_n_wd[6:0]
		nv3041a_write_param(0x64); 
		nv3041a_write_cmd(0x5a);//src_pchg_st_o[6:0]
		nv3041a_write_param(0x41); //41
		nv3041a_write_cmd(0x5b);//src_pchg_wd_o[6:0]
		nv3041a_write_param(0x3c); 
		nv3041a_write_cmd(0x5c);//src_pchg_st_e[6:0]
		nv3041a_write_param(0x02); //02
		nv3041a_write_cmd(0x5d);//src_pchg_wd_e[6:0]
		nv3041a_write_param(0x3c); //3c
		nv3041a_write_cmd(0x5e);//src_pol_sw[7:0]
		nv3041a_write_param(0x1f);
		nv3041a_write_cmd(0x60);//src_op_st_o[7:0]
		nv3041a_write_param(0x80);
		nv3041a_write_cmd(0x61);//src_op_st_e[7:0]
		nv3041a_write_param(0x3f);
		nv3041a_write_cmd(0x62);//src_op_ed_o[9:8] src_op_ed_e[9:8]
		nv3041a_write_param(0x21);
		nv3041a_write_cmd(0x63);//src_op_ed_o[7:0]
		nv3041a_write_param(0x07);
		nv3041a_write_cmd(0x64);//src_op_ed_e[7:0]
		nv3041a_write_param(0xe0);
		nv3041a_write_cmd(0x65);//chopper
		nv3041a_write_param(0x02);
		nv3041a_write_cmd(0xca); //avdd_mux_st_o[7:0]
		nv3041a_write_param(0x20);
		nv3041a_write_cmd(0xcb); //avdd_mux_ed_o[7:0]
		nv3041a_write_param(0x52); //52
		nv3041a_write_cmd(0xcc); //avdd_mux_st_e[7:0]
		nv3041a_write_param(0x10);
		nv3041a_write_cmd(0xcD); //avdd_mux_ed_e[7:0]
		nv3041a_write_param(0x42);
		nv3041a_write_cmd(0xD0); //avcl_mux_st_o[7:0]
		nv3041a_write_param(0x20);
		nv3041a_write_cmd(0xD1); //avcl_mux_ed_o[7:0]
		nv3041a_write_param(0x52);
		nv3041a_write_cmd(0xD2); //avcl_mux_st_e[7:0]
		nv3041a_write_param(0x10);
		nv3041a_write_cmd(0xD3); //avcl_mux_ed_e[7:0]
		nv3041a_write_param(0x42);
		nv3041a_write_cmd(0xD4); //vgh_mux_st[7:0]
		nv3041a_write_param(0x0a);
		nv3041a_write_cmd(0xD5); //vgh_mux_ed[7:0]
		nv3041a_write_param(0x32);
		///test mode
		#if 1
		nv3041a_write_cmd(0xf8); 
		nv3041a_write_param(0x03);
		nv3041a_write_cmd(0xf9); 
		nv3041a_write_param(0x20);
		nv3041a_write_cmd(0x80); //gam_vrp0 
		nv3041a_write_param(0x00); //00
		nv3041a_write_cmd(0xA0); //gam_VRN0
		nv3041a_write_param(0x00); //00
		nv3041a_write_cmd(0x81); //gam_vrp1 
		nv3041a_write_param(0x03); //03
		nv3041a_write_cmd(0xA1); //gam_VRN1
		nv3041a_write_param(0x03); //03
		nv3041a_write_cmd(0x82); //gam_vrp2 
		nv3041a_write_param(0x03); //03
		nv3041a_write_cmd(0xA2); //gam_VRN2
		nv3041a_write_param(0x03); //03
		nv3041a_write_cmd(0x86); //gam_prp0
		nv3041a_write_param(0x23); //1d
		nv3041a_write_cmd(0xA6); //gam_PRN0
		nv3041a_write_param(0x21); //1a
		nv3041a_write_cmd(0x87); //gam_prp1
		nv3041a_write_param(0x1f); //20//25//1d
		nv3041a_write_cmd(0xA7); //gam_PRN1
		nv3041a_write_param(0x2a); //20//24//2c
		nv3041a_write_cmd(0x83); //gam_vrp3 
		nv3041a_write_param(0x17); //13
		nv3041a_write_cmd(0xA3); //gam_VRN3
		nv3041a_write_param(0x17); //13
		nv3041a_write_cmd(0x84); //gam_vrp4 
		nv3041a_write_param(0x10); //0d
		nv3041a_write_cmd(0xA4); //gam_VRN4
		nv3041a_write_param(0x0f); //0d
		nv3041a_write_cmd(0x85); //gam_vrp5 
		nv3041a_write_param(0x10); //10
		nv3041a_write_cmd(0xA5); //gam_VRN5
		nv3041a_write_param(0x10); //10
		nv3041a_write_cmd(0x88); //gam_pkp0 
		nv3041a_write_param(0x0A); //0a
		nv3041a_write_cmd(0xA8); //gam_PKN0
		nv3041a_write_param(0x09); //09
		nv3041a_write_cmd(0x89); //gam_pkp1 
		nv3041a_write_param(0x12); //12
		nv3041a_write_cmd(0xA9); //gam_PKN1
		nv3041a_write_param(0x11); //11
		nv3041a_write_cmd(0x8a); //gam_pkp2 
		nv3041a_write_param(0x18); //18
		nv3041a_write_cmd(0xAa); //gam_PKN2
		nv3041a_write_param(0x18); //18
		nv3041a_write_cmd(0x8b); //gam_PKP3 
		nv3041a_write_param(0x09); //09
		nv3041a_write_cmd(0xAb); //gam_PKN3
		nv3041a_write_param(0x09); //09
		nv3041a_write_cmd(0x8c); //gam_PKP4 
		nv3041a_write_param(0x19); //19
		nv3041a_write_cmd(0xAc); //gam_PKN4
		nv3041a_write_param(0x0b); //0B
		nv3041a_write_cmd(0x8d); //gam_PKP5
		nv3041a_write_param(0x1c); //1C
		nv3041a_write_cmd(0xAd); //gam_PKN5
		nv3041a_write_param(0x0d); //0D
		nv3041a_write_cmd(0x8e); //gam_PKP6
		nv3041a_write_param(0x0b); //09
		nv3041a_write_cmd(0xAe); //gam_PKN6
		nv3041a_write_param(0x1a); //1c
		nv3041a_write_cmd(0x8f); //gam_PKP7
		nv3041a_write_param(0x11); //0c
		nv3041a_write_cmd(0xAf); //gam_PKN7
		nv3041a_write_param(0x13); //18
		nv3041a_write_cmd(0x90); //gam_PKP8
		nv3041a_write_param(0x04); //06
		nv3041a_write_cmd(0xB0); //gam_PKN8
		nv3041a_write_param(0x08); //06
		nv3041a_write_cmd(0x91); //gam_PKP9
		nv3041a_write_param(0x0a); //0e
		nv3041a_write_cmd(0xB1); //gam_PKN9
		nv3041a_write_param(0x12); //0e
		nv3041a_write_cmd(0x92); //gam_PKP10
		nv3041a_write_param(0x18); //18
		nv3041a_write_cmd(0xB2); //gam_PKN10
		nv3041a_write_param(0x18); //18
		nv3041a_write_cmd(0xff);
		nv3041a_write_param(0x00); 
		nv3041a_write_cmd(0x11);
		co_delay_100us(1200);
		nv3041a_write_cmd(0x29);
		co_delay_100us(200);
		co_delay_100us(500);
		
		#endif
		#endif
		
		
		nv3041a_write_cmd(0xff);
    nv3041a_write_param(0xa5);
    nv3041a_write_cmd(0xE7); //TE_output_en
    nv3041a_write_param(0x10);
    nv3041a_write_cmd(0x35); //TE_ interface_en
    nv3041a_write_param(0x00);//01
    nv3041a_write_cmd(0x36); //
    nv3041a_write_param(0x00);//c0
    nv3041a_write_cmd(0x3A); //
    nv3041a_write_param(0x01);//00---666/01---565
    nv3041a_write_cmd(0x41); 
    nv3041a_write_param(0x01);//01--8bit/03--16bit
    nv3041a_write_cmd(0x44); 
    nv3041a_write_param(0x15); 																														
    nv3041a_write_cmd(0x45); 
    nv3041a_write_param(0x15); 
    nv3041a_write_cmd(0x7d);//vdds_trim[2:0]
    nv3041a_write_param(0x03);
     
    nv3041a_write_cmd(0xc1);//avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]
    nv3041a_write_param(0xbb);//0xbb 88 a2
    nv3041a_write_cmd(0xc2);//vgl_clp_en vgl_clp[2:0]
    nv3041a_write_param(0x05);
    nv3041a_write_cmd(0xc3);//vgl_clp_en vgl_clp[2:0]
    nv3041a_write_param(0x10);
    nv3041a_write_cmd(0xc6);//avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]
    nv3041a_write_param(0x3E); // 3E
    nv3041a_write_cmd(0xc7);//mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]
    nv3041a_write_param(0x25); //25

    nv3041a_write_cmd(0xc8);// VGL_CLK_sel
    nv3041a_write_param(0x11); 
    nv3041a_write_cmd(0x74);
    nv3041a_write_param(0x12); 
    nv3041a_write_cmd(0x7a);// user_vgsp
    nv3041a_write_param(0x39); 
    nv3041a_write_cmd(0x6f);// user_gvdd
    nv3041a_write_param(0x22);
    nv3041a_write_cmd(0x78);// user_gvcl
    nv3041a_write_param(0x4F); 
    nv3041a_write_cmd(0xc9);//
    nv3041a_write_param(0x00); 
    nv3041a_write_cmd(0x67); 
    nv3041a_write_param(0x21);
    //gate_ed
    nv3041a_write_cmd(0x51);//gate_st_o[7:0]
    nv3041a_write_param(0x0a);
    nv3041a_write_cmd(0x52);//gate_ed_o[7:0]
    nv3041a_write_param(0x76); //76
    nv3041a_write_cmd(0x53);//gate_st_e[7:0]
    nv3041a_write_param(0x0a); //76
    nv3041a_write_cmd(0x54);//gate_ed_e[7:0]
    nv3041a_write_param(0x76);
    ////sorce 
    nv3041a_write_cmd(0x46);//fsm_hbp_o[5:0]
    nv3041a_write_param(0x0a);
    nv3041a_write_cmd(0x47);//fsm_hfp_o[5:0]
    nv3041a_write_param(0x2a);
    nv3041a_write_cmd(0x48);//fsm_hbp_e[5:0]
    nv3041a_write_param(0x0a);
    nv3041a_write_cmd(0x49);//fsm_hfp_e[5:0]
    nv3041a_write_param(0x1a);
    nv3041a_write_cmd(0x56);//src_ld_wd[1:0] src_ld_st[5:0]
    nv3041a_write_param(0x43);
    nv3041a_write_cmd(0x57);//pn_cs_en src_cs_st[5:0]
    nv3041a_write_param(0x42);
    nv3041a_write_cmd(0x58);//src_cs_p_wd[6:0]
    nv3041a_write_param(0x3c);
    nv3041a_write_cmd(0x59);//src_cs_n_wd[6:0]
    nv3041a_write_param(0x64); 
    nv3041a_write_cmd(0x5a);//src_pchg_st_o[6:0]
    nv3041a_write_param(0x41); //41
    nv3041a_write_cmd(0x5b);//src_pchg_wd_o[6:0]
    nv3041a_write_param(0x3c); 
    nv3041a_write_cmd(0x5c);//src_pchg_st_e[6:0]
    nv3041a_write_param(0x02); //02
    nv3041a_write_cmd(0x5d);//src_pchg_wd_e[6:0]
    nv3041a_write_param(0x3c); //3c
    nv3041a_write_cmd(0x5e);//src_pol_sw[7:0]
    nv3041a_write_param(0x1f);
    nv3041a_write_cmd(0x60);//src_op_st_o[7:0]
    nv3041a_write_param(0x80);
    nv3041a_write_cmd(0x61);//src_op_st_e[7:0]
    nv3041a_write_param(0x3f);
    nv3041a_write_cmd(0x62);//src_op_ed_o[9:8] src_op_ed_e[9:8]
    nv3041a_write_param(0x21);
    nv3041a_write_cmd(0x63);//src_op_ed_o[7:0]
    nv3041a_write_param(0x07);
    nv3041a_write_cmd(0x64);//src_op_ed_e[7:0]
    nv3041a_write_param(0xe0);
    nv3041a_write_cmd(0x65);//chopper
    nv3041a_write_param(0x02);
    nv3041a_write_cmd(0xca); //avdd_mux_st_o[7:0]
    nv3041a_write_param(0x20);
    nv3041a_write_cmd(0xcb); //avdd_mux_ed_o[7:0]
    nv3041a_write_param(0x52); //52
    nv3041a_write_cmd(0xcc); //avdd_mux_st_e[7:0]
    nv3041a_write_param(0x10);
    nv3041a_write_cmd(0xcD); //avdd_mux_ed_e[7:0]
    nv3041a_write_param(0x42);
    nv3041a_write_cmd(0xD0); //avcl_mux_st_o[7:0]
    nv3041a_write_param(0x20);
    nv3041a_write_cmd(0xD1); //avcl_mux_ed_o[7:0]
    nv3041a_write_param(0x52);
    nv3041a_write_cmd(0xD2); //avcl_mux_st_e[7:0]
    nv3041a_write_param(0x10);
    nv3041a_write_cmd(0xD3); //avcl_mux_ed_e[7:0]
    nv3041a_write_param(0x42);
    nv3041a_write_cmd(0xD4); //vgh_mux_st[7:0]
    nv3041a_write_param(0x0a);
    nv3041a_write_cmd(0xD5); //vgh_mux_ed[7:0]
    nv3041a_write_param(0x32);
    ///test mode
    nv3041a_write_cmd(0xf8); 
    nv3041a_write_param(0x03);
    nv3041a_write_cmd(0xf9); 
    nv3041a_write_param(0x20);
    nv3041a_write_cmd(0x80); //gam_vrp0 
    nv3041a_write_param(0x00); //00
    nv3041a_write_cmd(0xA0); //gam_VRN0
    nv3041a_write_param(0x00); //00
    nv3041a_write_cmd(0x81); //gam_vrp1 
    nv3041a_write_param(0x03); //03
    nv3041a_write_cmd(0xA1); //gam_VRN1
    nv3041a_write_param(0x03); //03
    nv3041a_write_cmd(0x82); //gam_vrp2 
    nv3041a_write_param(0x03); //03
    nv3041a_write_cmd(0xA2); //gam_VRN2
    nv3041a_write_param(0x03); //03
    nv3041a_write_cmd(0x86); //gam_prp0
    nv3041a_write_param(0x23); //1d
    nv3041a_write_cmd(0xA6); //gam_PRN0
    nv3041a_write_param(0x21); //1a
    nv3041a_write_cmd(0x87); //gam_prp1
    nv3041a_write_param(0x1f); //20//25//1d
    nv3041a_write_cmd(0xA7); //gam_PRN1
    nv3041a_write_param(0x2a); //20//24//2c
    nv3041a_write_cmd(0x83); //gam_vrp3 
    nv3041a_write_param(0x17); //13
    nv3041a_write_cmd(0xA3); //gam_VRN3
    nv3041a_write_param(0x17); //13
    nv3041a_write_cmd(0x84); //gam_vrp4 
    nv3041a_write_param(0x10); //0d
    nv3041a_write_cmd(0xA4); //gam_VRN4
    nv3041a_write_param(0x0f); //0d
    nv3041a_write_cmd(0x85); //gam_vrp5 
    nv3041a_write_param(0x10); //10
    nv3041a_write_cmd(0xA5); //gam_VRN5
    nv3041a_write_param(0x10); //10
    nv3041a_write_cmd(0x88); //gam_pkp0 
    nv3041a_write_param(0x0A); //0a
    nv3041a_write_cmd(0xA8); //gam_PKN0
    nv3041a_write_param(0x09); //09
    nv3041a_write_cmd(0x89); //gam_pkp1 
    nv3041a_write_param(0x12); //12
    nv3041a_write_cmd(0xA9); //gam_PKN1
    nv3041a_write_param(0x11); //11
    nv3041a_write_cmd(0x8a); //gam_pkp2 
    nv3041a_write_param(0x18); //18
    nv3041a_write_cmd(0xAa); //gam_PKN2
    nv3041a_write_param(0x18); //18
    nv3041a_write_cmd(0x8b); //gam_PKP3 
    nv3041a_write_param(0x09); //09
    nv3041a_write_cmd(0xAb); //gam_PKN3
    nv3041a_write_param(0x09); //09
    nv3041a_write_cmd(0x8c); //gam_PKP4 
    nv3041a_write_param(0x19); //19
    nv3041a_write_cmd(0xAc); //gam_PKN4
    nv3041a_write_param(0x0b); //0B
    nv3041a_write_cmd(0x8d); //gam_PKP5
    nv3041a_write_param(0x1c); //1C
    nv3041a_write_cmd(0xAd); //gam_PKN5
    nv3041a_write_param(0x0d); //0D
    nv3041a_write_cmd(0x8e); //gam_PKP6
    nv3041a_write_param(0x0b); //09
    nv3041a_write_cmd(0xAe); //gam_PKN6
    nv3041a_write_param(0x1a); //1c
    nv3041a_write_cmd(0x8f); //gam_PKP7
    nv3041a_write_param(0x11); //0c
    nv3041a_write_cmd(0xAf); //gam_PKN7
    nv3041a_write_param(0x13); //18
    nv3041a_write_cmd(0x90); //gam_PKP8
    nv3041a_write_param(0x04); //06
    nv3041a_write_cmd(0xB0); //gam_PKN8
    nv3041a_write_param(0x08); //06
    nv3041a_write_cmd(0x91); //gam_PKP9
    nv3041a_write_param(0x0a); //0e
    nv3041a_write_cmd(0xB1); //gam_PKN9
    nv3041a_write_param(0x12); //0e
    nv3041a_write_cmd(0x92); //gam_PKP10
    nv3041a_write_param(0x18); //18
    nv3041a_write_cmd(0xB2); //gam_PKN10
    nv3041a_write_param(0x18); //18
    nv3041a_write_cmd(0xff);
    nv3041a_write_param(0x00); 
    nv3041a_write_cmd(0x11);
    co_delay_100us(1200);
    nv3041a_write_cmd(0x29);
    co_delay_100us(200);
		
   	nv3041a_set_window(0,480-1,0,272-1);
	  uint16_t *pdata=(uint16_t*)0x22000000;
	  for(int i=0;i<480*272;i++)
		{
					pdata[i]=0x00;
		}
		nv3041a_display(480*272,pdata,NULL);
//		while(1)
//		{
//		 	  printf("show\r\n");
//			  co_delay_100us(10000);
//		   
//				for(int i=0;i<480*272;i++)
//				{
//							pdata[i]=color[index];
//					  
//				}
//				if8080_cs_set();
//				if8080_write_data((uint32_t*)pdata,(480*272*2));
//				co_delay_10us(1);
//				if8080_cs_release();
//				if(++index>sizeof(color)/2)index=0;
//	   }
	#endif
		
}

void nv3041a_display_wait_transfer_done(void)
{
    while(dma_transfer_done == false);
}

uint32_t start_time = 0;

void nv3041a_display(uint32_t pixel_count, uint16_t *data, void(*callback)(void))
{
    uint32_t i=0;
    uint32_t total_count = pixel_count / 2;   // accoding source width
    uint8_t link_count = total_count / 4000;
    
		start_time = system_get_curr_time();
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
        LinkParameter.DstAddr          = (uint32_t)&IF8080->TX_FIFO;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = nv3041a_DMA_Channel.Init.Request_ID;
        LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LinkParameter.Size             = all_set ? (total_count) : 4000;
        LinkParameter.gather_enable    = 0;
        LinkParameter.scatter_enable   = 0;
        total_count -= 4000;

        dma_linked_list_init(&Link_Channel[i], &LinkParameter);
    }

    __DMA_GATHER_FUNC_DISABLE(nv3041a_DMA_Channel.Channel);
    IF8080->DATA_WR_LEN.LEN = pixel_count * 2;
    if8080_cs_set();
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, nv3041a_DMA_Channel.Channel);
}

void nv3041a_display_gather(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count)
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
        LinkParameter.DstAddr          = (uint32_t)&IF8080->TX_FIFO;
        if(all_set)
        {
            LinkParameter.NextLink     = 0;
        }
        else
        {
            LinkParameter.NextLink     = (uint32_t)&Link_Channel[i + 1];
        }
        LinkParameter.Data_Flow        = DMA_M2P_DMAC;
        LinkParameter.Request_ID       = nv3041a_DMA_Channel.Init.Request_ID;
        LinkParameter.Source_Inc       = DMA_ADDR_INC_INC;
        LinkParameter.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
        LinkParameter.Source_Width     = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_32;
        LinkParameter.Burst_Len        = DMA_BURST_LEN_4;
        LinkParameter.Size             = all_set ? (total_count) : src_width_count_in_single_list;
        LinkParameter.gather_enable    = 1;
        LinkParameter.scatter_enable   = 0;
        total_count -= src_width_count_in_single_list;

        dma_linked_list_init(&Link_Channel[i], &LinkParameter);
    }

    __DMA_GATHER_FUNC_ENABLE(nv3041a_DMA_Channel.Channel);
    __DMA_GATHER_INTERVAL(nv3041a_DMA_Channel.Channel, (interval-count)/2);
    __DMA_GATHER_COUNT(nv3041a_DMA_Channel.Channel, count/2);
    IF8080->DATA_WR_LEN.LEN = pixel_count * 2;
    if8080_cs_set();
    dma_linked_list_start_IT(Link_Channel, &LinkParameter, nv3041a_DMA_Channel.Channel);
}

__attribute__((section("ram_code"))) void nv3041a_dma_isr(void)
{
	//printf("time:%d\r\n",(system_get_curr_time()-start_time));
    void (*callback)();
    // CS Release
	if8080_wait_bus_idle();
	
    if8080_cs_release();

    /* Clear Transfer complete status */
    dma_clear_tfr_Status(nv3041a_DMA_Channel.Channel);
    /* channel Transfer complete interrupt disable */
    dma_tfr_interrupt_disable(nv3041a_DMA_Channel.Channel);

    dma_transfer_done = true;
    callback = dma_trans_done_callback;
    dma_trans_done_callback = NULL;
    if(callback) {
        callback();
    }
}
