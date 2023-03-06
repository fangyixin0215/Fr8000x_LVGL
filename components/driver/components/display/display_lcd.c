/*
  ******************************************************************************
  * @file    display_lcd.c
  * @author  FreqChip Firmware Team
  * @version V1.0.0
  * @date    2021
  * @brief   Timer module driver.
  *          This file provides firmware functions to manage the Timer peripheral
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#include "driver_timer.h"
#include "driver_spi.h"
#include "driver_gpio.h"
#include "driver_dma.h"

SPI_HandleTypeDef  Display_SPI_Handle;

DMA_HandleTypeDef  DMA_Channel_1;

/************************************************************************************
 * @fn      display_peripheral_init
 *
 * @brief   Initialize the associated peripherals.
 */
void display_peripheral_init(void)
{
    /* SPI IO Init */
    __SYSTEM_GPIO_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_Handle;

    /* backlight */
    GPIO_Handle.Pin       = GPIO_PIN_3;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_B, &GPIO_Handle);
    gpio_write_pin(GPIO_B, GPIO_PIN_3, GPIO_PIN_SET);

    /* reset */
    GPIO_Handle.Pin       = GPIO_PIN_1;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_E, &GPIO_Handle);
	gpio_write_pin(GPIO_E, GPIO_PIN_1, GPIO_PIN_SET);

    /* CS */
    GPIO_Handle.Pin       = GPIO_PIN_7;
    GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
    gpio_init(GPIO_D, &GPIO_Handle);
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_SET);
    
    /* Clock/IO0/IO1/IO2/IO3 */
    GPIO_Handle.Pin       = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLDOWN;
    GPIO_Handle.Alternate = GPIO_FUNCTION_2;
    gpio_init(GPIO_D, &GPIO_Handle);

    /* SPI Init */
    __SYSTEM_SPI1_MASTER_CLK_SELECT_96M();
    __SYSTEM_SPI1_MASTER_CLK_ENABLE();

    Display_SPI_Handle.SPIx                       = SPIM1;
    Display_SPI_Handle.Init.Work_Mode             = SPI_WORK_MODE_0;
    Display_SPI_Handle.Init.Frame_Size            = SPI_FRAME_SIZE_8BIT;
    Display_SPI_Handle.Init.BaudRate_Prescaler    = 2;
    Display_SPI_Handle.Init.TxFIFOEmpty_Threshold = 20;
    Display_SPI_Handle.Init.RxFIFOFull_Threshold  = 0;

    spi_master_init(&Display_SPI_Handle);

    __SPI_DMA_TX_ENABLE(SPIM1);
    __SPI_DMA_TX_LEVEL(SPIM1, 20);

    /* DMA Init */
	__SYSTEM_DMA_CLK_ENABLE();
	__DMA_REQ_ID_SPI1_MASTER_TX(1);

    DMA_Channel_1.Channel = DMA_Channel0;
    DMA_Channel_1.Init.Data_Flow        = DMA_M2P_DMAC;
    DMA_Channel_1.Init.Request_ID       = 1;
    DMA_Channel_1.Init.Source_Inc       = DMA_ADDR_INC_INC;
    DMA_Channel_1.Init.Desination_Inc   = DMA_ADDR_INC_NO_CHANGE;
    DMA_Channel_1.Init.Source_Width     = DMA_TRANSFER_WIDTH_32;
    DMA_Channel_1.Init.Desination_Width = DMA_TRANSFER_WIDTH_16;

    dma_init(&DMA_Channel_1);
}

/************************************************************************************
 * @fn      display_set_window
 *
 * @brief   set window size
 */
void display_set_window(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e)
{
    uint8_t lu8_TempBuffer[8];

    /* X axial */
    lu8_TempBuffer[0] = 0x02;
    lu8_TempBuffer[1] = 0x00;
    lu8_TempBuffer[2] = 0x2a;
    lu8_TempBuffer[3] = 0x00;
    lu8_TempBuffer[4] = x_s>>8;
    lu8_TempBuffer[5] = x_s&0xff;
    lu8_TempBuffer[6] = x_e>>8;
    lu8_TempBuffer[7] = x_e&0xff;
    
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_CLEAR);
    spi_master_transmit_X1(&Display_SPI_Handle, (uint16_t *)lu8_TempBuffer, 8);
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_SET);

    /* Y axial */
    lu8_TempBuffer[0] = 0x02;
    lu8_TempBuffer[1] = 0x00;
    lu8_TempBuffer[2] = 0x2b;
    lu8_TempBuffer[3] = 0x00;
    lu8_TempBuffer[4] = y_s>>8;
    lu8_TempBuffer[5] = y_s&0xff;
    lu8_TempBuffer[6] = y_e>>8;
    lu8_TempBuffer[7] = y_e&0xff;

    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_CLEAR);
    spi_master_transmit_X1(&Display_SPI_Handle, (uint16_t *)lu8_TempBuffer, 8);
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_SET);
}

/************************************************************************************
 * @fn      display_RefreshScreen
 *
 * @brief   set window size
 */
void display_RefreshScreen(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e, uint16_t *data)
{
#if USE_DMA == 1

    uint32_t i;
    uint32_t Tx_length;
    uint32_t Tx_Count;

    // CS SET
	gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_CLEAR);
	
	Display_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
    Display_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
    Display_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
    Display_SPI_Handle.MultWireParam.Instruct       = 0x32;
    Display_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
    Display_SPI_Handle.MultWireParam.Address        = 0x003C00;
    spi_master_transmit_X2X4_DMA(&Display_SPI_Handle);

    /* unit is dma source width, aka 32bits */
    Tx_length = (x_e+1-x_s)*(y_e+1-y_s)/2;
    Tx_Count = Tx_length / 4000;
	
    for (i = 0; i < Tx_Count; i++)
    {
        dma_start(&DMA_Channel_0, (uint32_t)&data[i * 8000], (uint32_t)&SPIM1->DR, 4000, DMA_BURST_LEN_16, DMA_BURST_LEN_4);
        while(!dma_get_tfr_Status(DMA_Channel0));
        dma_clear_tfr_Status(DMA_Channel0);
    }

    if (Tx_length % 4000) 
    {
        dma_start(&DMA_Channel_0, (uint32_t)&data[Tx_Count * 8000], (uint32_t)&SPIM1->DR, Tx_length % 4000, DMA_BURST_LEN_16, DMA_BURST_LEN_4);
        while(!dma_get_tfr_Status(DMA_Channel0));
        dma_clear_tfr_Status(DMA_Channel0);
    }

    while(__SPI_IS_BUSY(SPIM1));

    // CS Release
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_SET);

#else

    // CS SET
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_CLEAR);

    Display_SPI_Handle.Init.Frame_Size              = SPI_FRAME_SIZE_16BIT;
    Display_SPI_Handle.MultWireParam.Wire_X2X4      = Wire_X4;
    Display_SPI_Handle.MultWireParam.InstructLength = INST_8BIT;
    Display_SPI_Handle.MultWireParam.Instruct       = 0x32;
    Display_SPI_Handle.MultWireParam.AddressLength  = ADDR_24BIT;
    Display_SPI_Handle.MultWireParam.Address        = 0x003C00;
    spi_master_transmit_X2X4(&Display_SPI_Handle, data, (x_e+1-x_s)*(y_e+1-y_s));

    // CS Release
    gpio_write_pin(GPIO_D, GPIO_PIN_7, GPIO_PIN_SET);

    __SPI_DISABLE(SPIM1);
    __SPI_DATA_FRAME_SIZE(SPIM1, SPI_FRAME_SIZE_8BIT);
    __SPI_ENABLE(SPIM1);

#endif
}
