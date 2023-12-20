/*
  ******************************************************************************
  * @file    driver_pdm.c
  * @author  FreqChip Firmware Team
  * @version V1.0.1
  * @date    2023
  * @brief   PDM module driver.
  *          This file provides firmware functions to manage the 
  *          pulse-duration modulation (PDM) peripheral
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#include "driver_pdm.h"

/************************************************************************************
 * @fn      pdm_IRQHandler
 *
 * @brief   PDM interrupt request Handler.
 *
 * @param   hpdm: PDM handle.
 */
__WEAK void pdm_IRQHandler(PDM_HandleTypeDef *hpdm)
{
    int i;

    if (__PDM_IS_FIFO_THRESHOLD_FULL()) 
    {
        for (i = 0; i < __PDM_GET_FIFO_THRESHOLD_LEVEL(); i++)
        {
            hpdm->p_RxData[hpdm->u32_RxCount++] = PDM->DATA;
        }

        hpdm->u32_RxCount = 0;

        if (hpdm->FIFOTHFullCallback != NULL)
        {
            hpdm->FIFOTHFullCallback(hpdm);
        }
    }
}

/*********************************************************************
 * @fn      pdm_init
 *
 * @brief   Initialize the PDM.
 *
 * @param   hpdm: PDM handle.
 */
void pdm_init(PDM_HandleTypeDef *hpdm)
{
    /* Sampling Rate */
    if (hpdm->Init.SamplingRate == SAMPLING_RATE_8000)
    {
        PDM->CONTROL.CLK_CFG = 0;    // 1M / 125
        PDM->CONTROL.SR_Mode = 1;
    }
    else
    {
        PDM->CONTROL.CLK_CFG = hpdm->Init.SamplingRate;
        PDM->CONTROL.SR_Mode = 0;
    }

    /* Sampling Edge */
    PDM->CONTROL.EDGE_Select = hpdm->Init.SamplingEdge;
    /* FIFO enable */
    PDM->CONTROL.FIFO_Enable = 1;

    PDM->CONTROL.ZERO_DET = 1;
    /*  high-pass filtering */
    PDM->CONTROL.HPF_EN = 1;

    /* All interrupt disable */
    PDM->FIFO_INTE.FIFO_FULL_INT           = 0;
    PDM->FIFO_INTE.FIFO_Threshold_FULL_INT = 0;
    PDM->FIFO_INTE.FIFO_Empty_INT          = 0;

    /* Direct volume adjustment */
    PDM->VolumeCTRL.VOL_Direct = 1;

    __PDM_FIFO_THRESHOLD_LEVEL(hpdm->Init.FIFO_Threshold);
    __PDM_DMA_THRESHOLD_LEVEL(hpdm->Init.FIFO_Threshold);

    __PDM_FIFO_CLEAR();
}

/*********************************************************************
 * @fn      pdm_set_volume
 *
 * @brief   set the volume.
 *
 * @param   fu16_Volume : target volume.(This parameter can be a 12bit value)
 */
void pdm_set_volume(uint16_t fu16_Volume)
{
    PDM->Volume = fu16_Volume;
}

/*********************************************************************
 * @fn      pdm_get_volume
 *
 * @brief   get volume.
 */
uint16_t pdm_get_volume(void)
{
    return PDM->Volume;
}

/************************************************************************************
 * @fn      pdm_enable
 *
 * @brief   PDM enable.
 */
void pdm_enable(void)
{
    PDM->CONTROL.PDM_EN = 1;
}

/************************************************************************************
 * @fn      pdm_disable
 *
 * @brief   PDM disable.
 */
void pdm_disable(void)
{
    PDM->CONTROL.PDM_EN = 0;
}

/************************************************************************************
 * @fn      pdm_read_data
 *
 * @brief   read data from PDM fifo in blocking mode.
 */
void pdm_read_data(uint16_t *fp_Data, uint32_t fu32_Size)
{
    while (fu32_Size) 
    {
        if (!PDM->FIFO_Status.FIFO_Empty) 
        {
            *fp_Data++ = PDM->DATA;

            fu32_Size--;
        }
    }
}

/************************************************************************************
 * @fn      pdm_read_data_IT
 *
 * @brief   Cyclic read data from PDM fifo in interrupt mode.
 */
void pdm_read_data_IT(PDM_HandleTypeDef *hpdm, uint16_t *fp_Data)
{
    hpdm->p_RxData    = fp_Data;
    hpdm->u32_RxCount = 0;

    /* enable fifo almost full interrupt */
    __PDM_FIFO_THRESHOLD_FULL_INT_ENABLE();
}

/************************************************************************************
 * @fn      pdm_read_data_DMA
 *
 * @brief   receive an amount of data in DMA mode.
 */
void pdm_read_data_DMA(void)
{
    PDM->DMA_CFG.DMA_EN = 1;
}
