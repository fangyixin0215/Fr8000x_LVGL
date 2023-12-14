/*
@driver_st7796s.c
@author: LH 
Creation Date：2023/10/11
*/

#include "driver_if8080.h"
#include "driver_gpio.h"
#include "driver_dma.h"
#include "driver_system.h"

#include "sys_utils.h"
#include "driver_st7796s.h"

#define LCD_RELEASE_RESET() gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_SET_RESET() gpio_write_pin(GPIO_A, GPIO_PIN_5, GPIO_PIN_CLEAR)


static DMA_HandleTypeDef DISP_DMA_Channel;
static DMA_LLI_InitTypeDef Link_Channel[30];
static dma_LinkParameter_t LinkParameter;

static bool dma_transfer_done = true;

static void (*dma_trans_done_callback)(void) = NULL;

void st7796s_display_wait_transfer_done(void)
{
  while (dma_transfer_done == false)
    ;
}
 

static void st7796s_write_cmd(uint8_t cmd)
{
  if8080_cs_set();
  if8080_write_cmd(cmd);
  if8080_cs_release();
}

static void st7796s_write_param(uint8_t data)
{
  if8080_cs_set();
  if8080_write_param(data);
  if8080_cs_release();
}

void st7796s_init(void)
{
	ST7796S_BACKLIGHT_CLOSE();
 
  GPIO_InitTypeDef GPIO_Handle;
  str_LCDParam_t str_LCDParam;

  // backlight
  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_0, PORTA0_FUNC_A0);
  GPIO_Handle.Pin = GPIO_PIN_0;
  GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init(GPIO_A, &GPIO_Handle);

  // reset
  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_5, PORTA5_FUNC_A5);
  GPIO_Handle.Pin = GPIO_PIN_5;
  GPIO_Handle.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init(GPIO_A, &GPIO_Handle);

  // 8080
  GPIO_Handle.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_Handle.Mode = GPIO_MODE_AF_PP;
  GPIO_Handle.Pull = GPIO_PULLDOWN;
  GPIO_Handle.Alternate = GPIO_FUNCTION_D;
  gpio_init(GPIO_B, &GPIO_Handle);

  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_LCD_WRX); // 8080_WR
  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_LCD_RDX); // 8080_RD
  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_6, PORTA6_FUNC_LCD_CSX); // 8080_CS
  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_7, PORTA7_FUNC_LCD_DCX); // 8080_DC

  __SYSTEM_IF8080_CLK_ENABLE();
  __SYSTEM_IF8080_CLK_SELECT_48M();

  str_LCDParam.LcdMode = MODE_8080;
  str_LCDParam.DataBusSelect = DATA_BUS_8_BIT;
  str_LCDParam.ReadClock = RDCLK_DIV_8;
  str_LCDParam.WriteClock = WDCLK_DIV_1; // WDCLK_DIV_2

  if8080_init(str_LCDParam);
  IF8080->DATA_CFG.DATA_TRANS_SEQ_0 = 0;
  IF8080->DATA_CFG.DATA_TRANS_SEQ_1 = 1;
  IF8080->DATA_CFG.DATA_TRANS_SEQ_2 = 2;
  IF8080->DATA_CFG.DATA_TRANS_SEQ_3 = 3;

  /* DMA */
  __SYSTEM_DMA_CLK_ENABLE();

  DISP_DMA_Channel.Channel = DMA_Channel0;
  DISP_DMA_Channel.Init.Data_Flow = DMA_M2P_DMAC;
  DISP_DMA_Channel.Init.Request_ID = 0;
  DISP_DMA_Channel.Init.Source_Inc = DMA_ADDR_INC_INC;
  DISP_DMA_Channel.Init.Desination_Inc = DMA_ADDR_INC_NO_CHANGE;
  DISP_DMA_Channel.Init.Source_Width = DMA_TRANSFER_WIDTH_32;
  DISP_DMA_Channel.Init.Desination_Width = DMA_TRANSFER_WIDTH_32;

  dma_init(&DISP_DMA_Channel);

  NVIC_EnableIRQ(DMA_IRQn);

  /* init sequeue */
  LCD_RELEASE_RESET();
  co_delay_100us(500);
  LCD_SET_RESET();
  co_delay_100us(1000);
  LCD_RELEASE_RESET();
  co_delay_100us(500);

  // ST7796S
  st7796s_write_cmd(0x11);

  co_delay_100us(120); // ms

  st7796s_write_cmd(0x36);
  st7796s_write_param(0x48);

  st7796s_write_cmd(0x3A);
#ifdef COLORBIT16
  st7796s_write_param(0x05); // 16位色
#else
  st7796s_write_param(0x66);                            // 18/24位色  // st7796s_write_param(0x66);
#endif

  st7796s_write_cmd(0xF0);
  st7796s_write_param(0xC3);

  st7796s_write_cmd(0xF0);
  st7796s_write_param(0x96);

  st7796s_write_cmd(0xB4);
  st7796s_write_param(0x01);

  st7796s_write_cmd(0xB7);
  st7796s_write_param(0xC6);

  st7796s_write_cmd(0xB9);
  st7796s_write_param(0x02);
  st7796s_write_param(0xE0);

  st7796s_write_cmd(0xC0);
  st7796s_write_param(0x80);
  st7796s_write_param(0X16);

  st7796s_write_cmd(0xC1);
  st7796s_write_param(0x19);

  st7796s_write_cmd(0xC2);
  st7796s_write_param(0xA7);

  st7796s_write_cmd(0xC5);
  st7796s_write_param(0x16);

  st7796s_write_cmd(0xE8);
  st7796s_write_param(0x40);
  st7796s_write_param(0x8A);
  st7796s_write_param(0x00);
  st7796s_write_param(0x00);
  st7796s_write_param(0x29);
  st7796s_write_param(0x19);
  st7796s_write_param(0xA5);
  st7796s_write_param(0x33);

  st7796s_write_cmd(0xE0);
  st7796s_write_param(0xF0);
  st7796s_write_param(0x07);
  st7796s_write_param(0x0D);
  st7796s_write_param(0x04);
  st7796s_write_param(0x05);
  st7796s_write_param(0x14);
  st7796s_write_param(0x36);
  st7796s_write_param(0x54);
  st7796s_write_param(0x4C);
  st7796s_write_param(0x38);
  st7796s_write_param(0x13);
  st7796s_write_param(0x14);
  st7796s_write_param(0x2E);
  st7796s_write_param(0x34);

  st7796s_write_cmd(0xE1);
  st7796s_write_param(0xF0);
  st7796s_write_param(0x10);
  st7796s_write_param(0x14);
  st7796s_write_param(0x0E);
  st7796s_write_param(0x0C);
  st7796s_write_param(0x08);
  st7796s_write_param(0x35);
  st7796s_write_param(0x44);
  st7796s_write_param(0x4C);
  st7796s_write_param(0x26);
  st7796s_write_param(0x10);
  st7796s_write_param(0x12);
  st7796s_write_param(0x2C);
  st7796s_write_param(0x32);

  st7796s_write_cmd(0xF0);
  st7796s_write_param(0x3C);

  st7796s_write_cmd(0xF0);
  st7796s_write_param(0x69);

  co_delay_100us(120);

  st7796s_write_cmd(0x21);

  st7796s_write_cmd(0x29);

  if8080_cs_set();
  if8080_write_cmd(0x2a);
  if8080_write_param(0);
  if8080_write_param(0);
  if8080_write_param(319 >> 8);
  if8080_write_param(319 & 0xFF);

  if8080_write_cmd(0x2b);
  if8080_write_param(0);
  if8080_write_param(0);
  if8080_write_param(479 >> 8);
  if8080_write_param(479 & 0xFF);
  if8080_write_cmd(0x2c);

  //初始化刷黑屏
  unsigned int i, j;
  unsigned int src = 0x00;
  for (i = 0; i < 480; i++)
  {
    for (j = 0; j < 320; j++)
    {
#ifdef COLORBIT16
      if8080_write_data(&src, 2); // 16位模式
#else
      if8080_write_data(&src, 3);               // 18/24位模式
#endif
    }
  }
  if8080_cs_release();
}

 
void st7796s_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
  if8080_cs_set();
  if8080_write_cmd(0x2a);
  if8080_write_param(x_s >> 8);
  if8080_write_param(x_s & 0xFF);
  if8080_write_param(x_e >> 8);
  if8080_write_param(x_e & 0xFF);

  if8080_write_cmd(0x2b);
  if8080_write_param(y_s >> 8);
  if8080_write_param(y_s & 0xFF);
  if8080_write_param(y_e >> 8);
  if8080_write_param(y_e & 0xFF);
  if8080_write_cmd(0x2c);
}

void st7796s_display(uint32_t pixel_count, void *ptr, void (*callback)(void))
{
  unsigned int i;
  uint32_t total_pixel = pixel_count;
// unsigned int cnt = (total_pixel * 2 + 1) / 4;
#ifdef COLORBIT16
  uint32_t total_count = (total_pixel * 2) / 4; // 16位色一个像素两个字节
#else
  uint32_t total_count = (total_pixel * 2) / 4; // 18/24	位色一个像素3个字节
#endif
  uint8_t link_count = total_count / 4000; //需要缓存的dma页数

  if (dma_transfer_done == false)
  {
    return;
  }
  else
  {
    dma_transfer_done = false;
  }

  dma_trans_done_callback = callback;

  if (total_count % 4000) //有余数增加一页缓存
  {
    link_count++;
  }
  for (i = 0; i < link_count; i++)
  {
    uint8_t all_set = (total_count <= 4000);
    // all_set = (total_count <= 4000);

    LinkParameter.SrcAddr = (uint32_t)ptr + i * 16000; // 4字节一次 4000*4
    LinkParameter.DstAddr = (uint32_t)&IF8080->TX_FIFO;
    if (all_set)
    {
      LinkParameter.NextLink = 0;
    }
    else
    {
      LinkParameter.NextLink = (uint32_t)&Link_Channel[i + 1];
    }
    LinkParameter.Data_Flow = DMA_M2P_DMAC;
    LinkParameter.Request_ID = DISP_DMA_Channel.Init.Request_ID;
    LinkParameter.Source_Inc = DMA_ADDR_INC_INC;
    LinkParameter.Desination_Inc = DMA_ADDR_INC_NO_CHANGE;
    LinkParameter.Source_Width = DMA_TRANSFER_WIDTH_32;
    LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_32;
    LinkParameter.Burst_Len = DMA_BURST_LEN_4;
    LinkParameter.Size = all_set ? (total_count) : 4000;
    LinkParameter.gather_enable = 0;
    LinkParameter.scatter_enable = 0;
    total_count -= 4000;

    dma_linked_list_init(&Link_Channel[i], &LinkParameter);
  }

  //__DMA_GATHER_FUNC_DISABLE(DISP_DMA_Channel.Channel);
#ifdef COLORBIT16
  IF8080->DATA_WR_LEN.LEN = total_pixel * 2;
#else
  IF8080->DATA_WR_LEN.LEN = total_pixel * 2;
#endif
  dma_linked_list_start_IT(Link_Channel, &LinkParameter, DISP_DMA_Channel.Channel);
}

void st7796s_display_gather(uint32_t pixel_count, uint16_t *data, uint16_t interval, uint16_t count)
{
#define DMA_TRANSFER_SIZE 4000
  uint32_t total_count = (pixel_count * 2) / 4; //  18/24	位色一个像素2个字节
  uint16_t line_count_in_single_list = DMA_TRANSFER_SIZE / (count / 2);
  uint16_t src_width_count_in_single_list = line_count_in_single_list * (count / 2);
  uint8_t link_count, i;

  if (dma_transfer_done == false)
  {
    return;
  }
  else
  {
    dma_transfer_done = false;
  }

  link_count = total_count / src_width_count_in_single_list;
  if (total_count % src_width_count_in_single_list)
  {
    link_count++;
  }

  for (i = 0; i < link_count; i++)
  {
    uint8_t all_set = (total_count <= src_width_count_in_single_list);

    LinkParameter.SrcAddr = (uint32_t)&data[i * interval * line_count_in_single_list];
    LinkParameter.DstAddr = (uint32_t)&IF8080->TX_FIFO;
    if (all_set)
    {
      LinkParameter.NextLink = 0;
    }
    else
    {
      LinkParameter.NextLink = (uint32_t)&Link_Channel[i + 1];
    }
    LinkParameter.Data_Flow = DMA_M2P_DMAC;
    LinkParameter.Request_ID = DISP_DMA_Channel.Init.Request_ID;
    LinkParameter.Source_Inc = DMA_ADDR_INC_INC;
    LinkParameter.Desination_Inc = DMA_ADDR_INC_NO_CHANGE;
    LinkParameter.Source_Width = DMA_TRANSFER_WIDTH_32;
    LinkParameter.Desination_Width = DMA_TRANSFER_WIDTH_32;
    LinkParameter.Burst_Len = DMA_BURST_LEN_4;
    LinkParameter.Size = all_set ? (total_count) : src_width_count_in_single_list;
    LinkParameter.gather_enable = 1;
    LinkParameter.scatter_enable = 0;
    total_count -= src_width_count_in_single_list;

    dma_linked_list_init(&Link_Channel[i], &LinkParameter);
  }
  __DMA_GATHER_FUNC_ENABLE(DISP_DMA_Channel.Channel);
  __DMA_GATHER_INTERVAL(DISP_DMA_Channel.Channel, (interval - count) / 2);
  __DMA_GATHER_COUNT(DISP_DMA_Channel.Channel, count * 2 / 4);
  dma_linked_list_start_IT(Link_Channel, &LinkParameter, DISP_DMA_Channel.Channel);
}

__attribute__((section("ram_code"))) void st7796s_dma_isr(void)
{
  void (*callback)();
  if8080_wait_bus_idle();
  if8080_cs_release();
  dma_clear_tfr_Status(DISP_DMA_Channel.Channel);
  dma_tfr_interrupt_disable(DISP_DMA_Channel.Channel);
  dma_transfer_done = true;
  callback = dma_trans_done_callback;
  dma_trans_done_callback = NULL;
  if (callback)
  {
    callback();
  }
}
 
