/*
  ******************************************************************************
  * @file    driver_pdm.h
  * @author  FreqChip Firmware Team
  * @version V1.0.1
  * @date    2023
  * @brief   Header file of PDM HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#ifndef __DRIVER_PDM_H__
#define __DRIVER_PDM_H__

#include <stdint.h>
#include <stdbool.h>

#include "plf.h"

/** @addtogroup PDM_Registers_Section
  * @{
  */
/* ################################ Register Section Start ################################ */

/* PDM control Register */
typedef struct
{
    uint32_t PDM_EN      : 1;
    uint32_t EDGE_Select : 1;
    uint32_t SR_Mode     : 1;
    uint32_t HPF_EN      : 1;
    uint32_t ZERO_DET    : 1;
    uint32_t FIFO_Enable : 1;
    uint32_t CLK_CFG     : 2;
    uint32_t rsv_0       : 24;
}REG_PDM_Control_t;

/* Volume control Register */
typedef struct
{
    uint32_t VOL_Step   : 10;
    uint32_t rsv_0      : 6;
    uint32_t VOL_Direct : 1;
    uint32_t rsv_1      : 15;
}REG_VolumeCTRL_t;

/* FIFO clear Register */
typedef struct
{
    uint32_t Write_FIFO_Clear : 1;
    uint32_t Read_FIFO_Clear  : 1;
    uint32_t rsv_0            : 30;
}REG_PDM_FIFO_Clear_t;

/* FIFO status Register */
typedef struct
{
    uint32_t FIFO_FULL           : 1;
    uint32_t FIFO_Threshold_FULL : 1;
    uint32_t FIFO_Empty          : 1;
    uint32_t rsv_0               : 29;
}REG_PDM_FIFO_Status_t;

/* FIFO status interrupt Register */
typedef struct
{
    uint32_t FIFO_FULL_INT           : 1;
    uint32_t FIFO_Threshold_FULL_INT : 1;
    uint32_t FIFO_Empty_INT          : 1;
    uint32_t rsv_0                   : 29;
}REG_PDM_FIFO_INTE_t;

/* DMA level config Register */
typedef struct
{
    uint32_t DMA_RX_Level : 5;
    uint32_t DMA_EN       : 1;
    uint32_t rsv_0        : 26;
}REG_PDM_DMA_CFG_t;

/* ------------------------------------------------*/
/*                   PDM Register                  */
/* ------------------------------------------------*/
typedef struct 
{
    volatile REG_PDM_Control_t        CONTROL;          /* Offset 0x00 */
    volatile uint32_t                 Volume;           /* Offset 0x04 */
    volatile REG_VolumeCTRL_t         VolumeCTRL;       /* Offset 0x08 */
    volatile uint32_t                 DATA;             /* Offset 0x0C */
    volatile REG_PDM_FIFO_Clear_t     FIFO_Clear;       /* Offset 0x10 */
    volatile REG_PDM_FIFO_Status_t    FIFO_Status;      /* Offset 0x14 */
    volatile REG_PDM_FIFO_INTE_t      FIFO_INTE;        /* Offset 0x18 */
    volatile uint32_t                 FIFO_Level;       /* Offset 0x1C */
    volatile REG_PDM_DMA_CFG_t        DMA_CFG;          /* Offset 0x20 */
}struct_PDM_t;

#define PDM              ((struct_PDM_t *)PDM_BASE)

/* ################################ Register Section END ################################## */
/**
  * @}
  */


/** @addtogroup PDM_Initialization_Config_Section
  * @{
  */
/* ################################ Initialization, Config Section Start ################################ */

/* Sampling Rate */
typedef enum 
{
    SAMPLING_RATE_8000  = 4,    // 1M   / 125
    SAMPLING_RATE_16000 = 0,    // 1M   / 62.5
    SAMPLING_RATE_24000 = 1,    // 1.5M / 62.5
    SAMPLING_RATE_32000 = 2,    // 2M   / 62.5
    SAMPLING_RATE_48000 = 3,    // 3M   / 62.5
}enum_SamplingRate_t;

/* Sampling Edge */
typedef enum 
{
    SAMPLING_FALLING_EDGE,
    SAMPLING_RISING_EDGE,
}enum_SamplingEdge_t;


/**
  * @brief  PDM Initialization Structure definition
  */
typedef struct 
{
    enum_SamplingRate_t    SamplingRate;      /* This parameter can be a value of @ref enum_SamplingRate_t */

    enum_SamplingEdge_t    SamplingEdge;      /* This parameter can be a value of @ref enum_SamplingEdge_t */

    uint32_t               FIFO_Threshold;    /* This parameter can be a value of 1 ~ 32 */
}struct_PDMInit_t;

/*
 * @brief  PDM handle Structure definition
 */
typedef struct __PDM_HandleTypeDef
{
    struct_PDMInit_t         Init;               /*!< PDM communication parameters      */

    void (*FIFOTHFullCallback)(struct __PDM_HandleTypeDef *hpdm);    /* FIFO Threshold full Callback */

    volatile uint16_t       *p_RxData;           /*!< PDM Receive parameters in interrupt  */
    volatile uint32_t        u32_RxCount;
}PDM_HandleTypeDef;

/* ################################ Initialization, Config Section END ################################## */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

#define __PDM_FIFO_CLEAR()                      do {PDM->FIFO_Clear.Write_FIFO_Clear = 1; \
                                                    PDM->FIFO_Clear.Read_FIFO_Clear  = 1; \
                                                    PDM->FIFO_Clear.Write_FIFO_Clear = 0; \
                                                    PDM->FIFO_Clear.Read_FIFO_Clear  = 0;}while(0)

#define __PDM_FIFO_THRESHOLD_LEVEL(__LEVEL__)      (PDM->FIFO_Level = __LEVEL__ - 1)

#define __PDM_DMA_THRESHOLD_LEVEL(__LEVEL__)       (PDM->DMA_CFG.DMA_RX_Level = __LEVEL__ - 1)

#define __PDM_GET_FIFO_THRESHOLD_LEVEL()           (PDM->FIFO_Level + 1)

#define __PDM_IS_FIFO_FULL()                       (PDM->FIFO_Status.FIFO_FULL           == 1)
#define __PDM_IS_FIFO_EMPTY()                      (PDM->FIFO_Status.FIFO_Empty          == 1)
#define __PDM_IS_FIFO_THRESHOLD_FULL()             (PDM->FIFO_Status.FIFO_Threshold_FULL == 1)

#define __PDM_FIFO_FULL_INT_ENABLE()               (PDM->FIFO_INTE.FIFO_FULL_INT  = 1)
#define __PDM_FIFO_FULL_INT_DISABLE()              (PDM->FIFO_INTE.FIFO_FULL_INT  = 0)
#define __PDM_FIFO_EMPTY_INT_ENABLE()              (PDM->FIFO_INTE.FIFO_Empty_INT = 1)
#define __PDM_FIFO_EMPTY_INT_DISABLE()             (PDM->FIFO_INTE.FIFO_Empty_INT = 0)
#define __PDM_FIFO_THRESHOLD_FULL_INT_ENABLE()     (PDM->FIFO_INTE.FIFO_Threshold_FULL_INT = 1)
#define __PDM_FIFO_THRESHOLD_FULL_INT_DISABLE()    (PDM->FIFO_INTE.FIFO_Threshold_FULL_INT = 0)

/* Exported functions --------------------------------------------------------*/

/* pdm_IRQHandler */
void pdm_IRQHandler(PDM_HandleTypeDef *hpdm);

/* pdm_init */
void pdm_init(PDM_HandleTypeDef *hpdm);

/* pdm_set_volume */
/* pdm_get_volume */
void pdm_set_volume(uint16_t fu16_Volume);
uint16_t pdm_get_volume(void);

/* pdm_enable */
/* pdm_disable */
void pdm_enable(void);
void pdm_disable(void);

/* pdm_read_data */
void pdm_read_data(uint16_t *fp_Data, uint32_t fu32_Size);
void pdm_read_data_IT(PDM_HandleTypeDef *hpdm, uint16_t *fp_Data);
void pdm_read_data_DMA(void);

#endif

