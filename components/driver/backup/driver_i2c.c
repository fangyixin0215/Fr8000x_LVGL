/*
  ******************************************************************************
  * @file    driver_i2c.c
  * @author  FreqChip Firmware Team
  * @version V1.0.0
  * @date    2021
  * @brief   I2C module driver.
  *          This file provides firmware functions to manage the 
  *          Inter-Integrated Circuit bus (I2C) peripheral
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#include "driver_i2c.h"

/************************************************************************************
 * @fn      i2c_IRQHandler
 *
 * @brief   Handle I2C interrupt request.
 *
 * @param   hi2c: I2C handle.
 */
void i2c_IRQHandler(I2C_HandleTypeDef *hi2c)
{
    /* Is INT_TX_EMPTY enabled */
    if (i2c_is_int_enable(hi2c, INT_TX_EMPTY))
    {
        if (i2c_get_int_status(hi2c, INT_TX_EMPTY)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_EMPTY);

            while (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
            {
                if (hi2c->u32_TxCount < hi2c->u32_TxSize) 
                {
                    hi2c->I2Cx->DATA_CMD = hi2c->p_TxData[hi2c->u32_TxCount++];
                }
                else 
                {
                    hi2c->I2Cx->DATA_CMD = hi2c->p_TxData[hi2c->u32_TxCount++] | CMD_STOP;

                    hi2c->b_TxBusy = false;

                    i2c_int_disable(hi2c, INT_TX_EMPTY);

                    break;
                }

                if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
                {
                    i2c_clear_int_status(hi2c, INT_TX_ABRT);

                    __I2C_DISABLE(hi2c->I2Cx);

                    hi2c->b_TxBusy = false;

                    i2c_int_disable(hi2c, INT_TX_EMPTY);

                    break;
                }
            }
        }
    }

    /* Is INT_RX_FULL enabled */
    if (i2c_is_int_enable(hi2c, INT_RX_FULL))
    {
        if (i2c_get_int_status(hi2c, INT_RX_FULL)) 
        {
            i2c_clear_int_status(hi2c, INT_RX_FULL);

            while (!__I2C_IS_RxFIFO_EMPTY(hi2c->I2Cx)) 
            {
                hi2c->p_RxData[hi2c->u32_RxCount++] = hi2c->I2Cx->DATA_CMD & 0xFF;

                if (hi2c->u32_RxCount >= hi2c->u32_RxSize)
                {
                    hi2c->b_RxBusy = false;

                    i2c_int_disable(hi2c, INT_RX_FULL);
                    i2c_int_disable(hi2c, INT_TX_ABRT);
                }
            }

            while ((!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx)) && hi2c->b_TxBusy == true)
            {
                if (hi2c->u32_TxCount < hi2c->u32_RxSize - 1) 
                {
                    hi2c->I2Cx->DATA_CMD = CMD_READ;

                    hi2c->u32_TxCount++;
                }
                else 
                {
                    hi2c->I2Cx->DATA_CMD = CMD_READ | CMD_STOP;

                    hi2c->b_TxBusy = false;

                    break;  
                }
            }
        }
    }

    /* Is INT_TX_ABRT enable */
    if (i2c_is_int_enable(hi2c, INT_TX_ABRT))
    {
        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            hi2c->b_RxBusy = false;
            hi2c->b_TxBusy = false;

            i2c_int_disable(hi2c, INT_RX_FULL);
            i2c_int_disable(hi2c, INT_TX_ABRT);
        }
    }
}

/************************************************************************************
 * @fn      i2c_init
 *
 * @brief   Initialize the I2C according to the specified parameters in the struct_I2CInit_t
 *
 * @param   hi2c: I2C handle.
 */
void i2c_init(I2C_HandleTypeDef *hi2c)
{
    uint32_t lu32_TempValue;

    __I2C_DISABLE(hi2c->I2Cx);

    /* Master */
    if (hi2c->Init.I2C_Mode & I2C_MASK_MASTER) 
    {
        hi2c->I2Cx->CTRL.SLAVE_DISABLE = 1;
        hi2c->I2Cx->CTRL.MASTER_MODE   = 1;
        hi2c->I2Cx->CTRL.SPEED         = 2;

        hi2c->I2Cx->TAR.ADDR_MASTER_10BIT = (hi2c->Init.I2C_Mode & I2C_MASK_10BIT) ? 1 : 0;
        hi2c->I2Cx->TAR.SPECIAL   = 0;
        hi2c->I2Cx->TAR.DEVICE_ID = 0;
    }
    /* Slave */
    else 
    {
        hi2c->I2Cx->CTRL.SLAVE_DISABLE = 0;
        hi2c->I2Cx->CTRL.MASTER_MODE   = 0;
        hi2c->I2Cx->CTRL.SPEED         = 2;

        hi2c->I2Cx->CTRL.ADDR_SLAVE_10BIT = (hi2c->Init.I2C_Mode & I2C_MASK_10BIT) ? 1 : 0;

        hi2c->I2Cx->SAR = hi2c->Init.Slave_Address >> 1;
    }



    /* Rate config */
    if (hi2c->Init.I2C_Mode & I2C_MASK_MASTER) 
    {
        hi2c->I2Cx->SS_SCL_HCNT = hi2c->Init.SCL_HCNT;
        hi2c->I2Cx->SS_SCL_LCNT = hi2c->Init.SCL_LCNT;

        hi2c->I2Cx->FS_SCL_HCNT = hi2c->Init.SCL_HCNT;
        hi2c->I2Cx->FS_SCL_LCNT = hi2c->Init.SCL_LCNT;

        lu32_TempValue = hi2c->Init.SCL_LCNT / 2;

        if (lu32_TempValue > 0xFF) 
        {
            hi2c->I2Cx->SDA_SETUP = 0xFF;
            hi2c->I2Cx->SDA_HOLD.SDA_TX_HOLD = hi2c->Init.SCL_LCNT - 0xFF;

            /* Maximum limit */
            if (hi2c->Init.SCL_LCNT - 0xFF > 0xFFFF) 
            {
                hi2c->I2Cx->SDA_HOLD.SDA_TX_HOLD = 0xFFFF;
            }
        }
        else 
        {
            hi2c->I2Cx->SDA_SETUP = lu32_TempValue;
            /* Minimum limit */
            if (lu32_TempValue < 2) 
            {
                hi2c->I2Cx->SDA_SETUP = 2;
            }

            hi2c->I2Cx->SDA_HOLD.SDA_TX_HOLD = lu32_TempValue;
            /* Minimum limit */
            if (lu32_TempValue < 7) 
            {
                hi2c->I2Cx->SDA_HOLD.SDA_TX_HOLD = 7;
            }
        }
    }
    else 
    {
        hi2c->I2Cx->SS_SCL_HCNT = 2;
        hi2c->I2Cx->SS_SCL_LCNT = 2;
        
        hi2c->I2Cx->SDA_SETUP = 2;
        hi2c->I2Cx->SDA_HOLD.SDA_TX_HOLD = 7;
    }
    /* Rate config end */


    /* Default configuration */
    hi2c->I2Cx->CTRL.STOP_DET_IF_MASTER_ACTIVE = 1;
    hi2c->I2Cx->CTRL.STOP_DET_IF_ADDR_ESSED    = 1;
    hi2c->I2Cx->CTRL.RX_FIFO_FULL_HLD_CTRL     = 1;
    hi2c->I2Cx->CTRL.TX_EMPTY_CTRL             = 1;
    hi2c->I2Cx->CTRL.RESTART_EN                = 1;
    /* nonblocking */
    hi2c->I2Cx->ENABLE.TX_CMD_BLOCK = 0;
    /* Disable all interrupt */
    hi2c->I2Cx->INT_MASK = 0;
}

/************************************************************************************
 * @fn      i2c_master_transmit
 *
 * @brief   master send an amount of data in blocking mode.
 *
 * @param   hi2c: I2C handle.
 */
void i2c_master_transmit(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    if (fu32_Size == 0) 
        return;

    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_ENABLE(hi2c->I2Cx);

    while (fu32_Size - 1) 
    {
        if (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
        {
            hi2c->I2Cx->DATA_CMD = *fp_Data++;
            
            fu32_Size--;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            return;
        }
    }

    /* Last byte with stop */
    while (__I2C_IS_TxFIFO_FULL(hi2c->I2Cx));
    hi2c->I2Cx->DATA_CMD = *fp_Data++ | CMD_STOP;

    while(__I2C_IS_BUSY(hi2c->I2Cx));
}

/************************************************************************************
 * @fn      i2c_master_receive
 *
 * @brief   master receive an amount of data in blocking mode.
 *
 * @param   hi2c: I2C handle.
 */
void i2c_master_receive(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    uint32_t lu32_RxCount = fu32_Size;
    
    if (fu32_Size == 0) 
        return;

    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_ENABLE(hi2c->I2Cx);

    while (fu32_Size - 1) 
    {
        if (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
        {
            hi2c->I2Cx->DATA_CMD = CMD_READ;

            fu32_Size--;
        }

        while (!__I2C_IS_RxFIFO_EMPTY(hi2c->I2Cx)) 
        {
            *fp_Data++ = hi2c->I2Cx->DATA_CMD & 0xFF;

            lu32_RxCount--;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            return;
        }
    }

    /* Last byte with stop */
    while (__I2C_IS_TxFIFO_FULL(hi2c->I2Cx));
    hi2c->I2Cx->DATA_CMD = CMD_READ | CMD_STOP;

    while (lu32_RxCount) 
    {
        if (!__I2C_IS_RxFIFO_EMPTY(hi2c->I2Cx)) 
        {
            *fp_Data++ = hi2c->I2Cx->DATA_CMD & 0xFF;

            lu32_RxCount--;
        }
    }

    while(__I2C_IS_BUSY(hi2c->I2Cx));
}

/************************************************************************************
 * @fn      i2c_master_transmit_IT
 *
 * @brief   master send an amount of data in interrupt mode.
 *
 * @param   hi2c: I2C handle.
 */
void i2c_master_transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    if (fu32_Size == 0)  return;
    if (hi2c->b_TxBusy)  return;
    if (hi2c->b_RxBusy)  return;

    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_TxFIFO_THRESHOLD_LEVEL(hi2c->I2Cx, 16);

    __I2C_ENABLE(hi2c->I2Cx);

    hi2c->u32_TxSize  = fu32_Size - 1;
    hi2c->u32_TxCount = 0;
    hi2c->p_TxData = fp_Data;
    hi2c->b_TxBusy = true;

    i2c_clear_int_status(hi2c, INT_TX_ABRT);
    i2c_clear_int_status(hi2c, INT_TX_EMPTY);

    while (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
    {
        if (hi2c->u32_TxCount < hi2c->u32_TxSize) 
        {
            hi2c->I2Cx->DATA_CMD = hi2c->p_TxData[hi2c->u32_TxCount++];
        }
        else 
        {
            hi2c->I2Cx->DATA_CMD = hi2c->p_TxData[hi2c->u32_TxCount++] | CMD_STOP;

            hi2c->b_TxBusy = false;
            return;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            hi2c->b_TxBusy = false;
            return;
        }
    }

    while(__I2C_IS_TxFIFO_FULL(hi2c->I2Cx));

    /* DevAddress NACK */
    if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
    {
        i2c_clear_int_status(hi2c, INT_TX_ABRT);

        __I2C_DISABLE(hi2c->I2Cx);

        hi2c->b_TxBusy = false;
        return;
    }

    i2c_int_enable(hi2c, INT_TX_EMPTY);
}

/************************************************************************************
 * @fn      i2c_master_receive_IT
 *
 * @brief   master receive an amount of data in interrupt mode.
 *
 * @param   hi2c: I2C handle.
 */
void i2c_master_receive_IT(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    if (fu32_Size == 0)  return;
    if (hi2c->b_TxBusy)  return;
    if (hi2c->b_RxBusy)  return;

    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_RxFIFO_THRESHOLD_LEVEL(hi2c->I2Cx, 0);

    __I2C_ENABLE(hi2c->I2Cx);

    hi2c->u32_RxSize  = fu32_Size;
    hi2c->u32_RxCount = 0;
    hi2c->u32_TxCount = 0;
    hi2c->p_RxData = fp_Data;
    hi2c->b_RxBusy = true;
    hi2c->b_TxBusy = true;

    i2c_clear_int_status(hi2c, INT_TX_ABRT);
    i2c_clear_int_status(hi2c, INT_RX_FULL);

    while (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
    {
        if (hi2c->u32_TxCount < hi2c->u32_RxSize - 1) 
        {
            hi2c->I2Cx->DATA_CMD = CMD_READ;

            hi2c->u32_TxCount++;
        }
        else 
        {
            hi2c->I2Cx->DATA_CMD = CMD_READ | CMD_STOP;

            break;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            hi2c->b_RxBusy = false;
            hi2c->b_TxBusy = false;
            return;
        }
    }

    i2c_int_enable(hi2c, INT_TX_ABRT);
    i2c_int_enable(hi2c, INT_RX_FULL);
}

/************************************************************************************
 * @fn      i2c_memory_write
 *
 * @brief   i2c memory write.
 */
void i2c_memory_write(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint16_t fu16_MemAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_ENABLE(hi2c->I2Cx);

    hi2c->I2Cx->DATA_CMD = (fu16_MemAddress >> 8) & 0xFF;
    hi2c->I2Cx->DATA_CMD = fu16_MemAddress & 0xFF;

    while (!__I2C_IS_TxFIFO_EMPTY(hi2c->I2Cx));

    /* DevAddress NACK */
    if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
    {
        i2c_clear_int_status(hi2c, INT_TX_ABRT);

        __I2C_DISABLE(hi2c->I2Cx);

        return;
    }

    while (fu32_Size - 1) 
    {
        if (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
        {
            hi2c->I2Cx->DATA_CMD = *fp_Data++;
            
            fu32_Size--;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            return;
        }
    }

    /* Last byte with stop */
    while (__I2C_IS_TxFIFO_FULL(hi2c->I2Cx));
    hi2c->I2Cx->DATA_CMD = *fp_Data++ | CMD_STOP;

    while(__I2C_IS_BUSY(hi2c->I2Cx));

    while(i2c_memory_is_busy(hi2c, fu16_DevAddress));
}

/************************************************************************************
 * @fn      i2c_memory_write
 *
 * @brief   i2c memory write.
 */
void i2c_memory_read(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress, uint16_t fu16_MemAddress, uint8_t *fp_Data, uint32_t fu32_Size)
{
    uint32_t lu32_RxCount = fu32_Size;

    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_ENABLE(hi2c->I2Cx);

    hi2c->I2Cx->DATA_CMD = (fu16_MemAddress >> 8) & 0xFF;
    hi2c->I2Cx->DATA_CMD = fu16_MemAddress & 0xFF;

    while (!__I2C_IS_TxFIFO_EMPTY(hi2c->I2Cx));

    /* DevAddress NACK */
    if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
    {
        i2c_clear_int_status(hi2c, INT_TX_ABRT);

        __I2C_DISABLE(hi2c->I2Cx);

        return;
    }

    hi2c->I2Cx->DATA_CMD = CMD_RESTART | CMD_READ;
    
    while (fu32_Size - 2) 
    {
        if (!__I2C_IS_TxFIFO_FULL(hi2c->I2Cx))
        {
            hi2c->I2Cx->DATA_CMD = CMD_READ;

            fu32_Size--;
        }

        while (!__I2C_IS_RxFIFO_EMPTY(hi2c->I2Cx)) 
        {
            *fp_Data++ = hi2c->I2Cx->DATA_CMD & 0xFF;

            lu32_RxCount--;
        }

        if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
        {
            i2c_clear_int_status(hi2c, INT_TX_ABRT);

            __I2C_DISABLE(hi2c->I2Cx);

            return;
        }
    }

    /* Last byte with stop */
    while (__I2C_IS_TxFIFO_FULL(hi2c->I2Cx));
    hi2c->I2Cx->DATA_CMD = CMD_READ | CMD_STOP;

    while (lu32_RxCount) 
    {
        if (!__I2C_IS_RxFIFO_EMPTY(hi2c->I2Cx)) 
        {
            *fp_Data++ = hi2c->I2Cx->DATA_CMD & 0xFF;

            lu32_RxCount--;
        }
    }

    while(__I2C_IS_BUSY(hi2c->I2Cx));
}

/************************************************************************************
 * @fn      i2c_memory_is_busy
 *
 * @brief   i2c memory is busy.
 */
bool i2c_memory_is_busy(I2C_HandleTypeDef *hi2c, uint16_t fu16_DevAddress)
{
    __I2C_DISABLE(hi2c->I2Cx);

    hi2c->I2Cx->TAR.TAR = fu16_DevAddress >> 1;

    __I2C_ENABLE(hi2c->I2Cx);

    hi2c->I2Cx->DATA_CMD = 0x00;
    hi2c->I2Cx->DATA_CMD = 0x00 | CMD_STOP;

    while (!__I2C_IS_TxFIFO_EMPTY(hi2c->I2Cx));

    /* DevAddress NACK */
    if (i2c_get_int_status(hi2c, INT_TX_ABRT)) 
    {
        i2c_clear_int_status(hi2c, INT_TX_ABRT);

        __I2C_DISABLE(hi2c->I2Cx);

        return true;
    }
    else 
    {
        return false;
    }
}

/************************************************************************************
 * @fn      i2c_int_enable
 *
 * @brief   I2C interrupt enable.
 *
 * @param   hi2c: I2C handle.
 *          fe_INT_Index: interrupt index.
 */
void i2c_int_enable(I2C_HandleTypeDef *hi2c, enum_INT_Index_t fe_INT_Index)
{
    hi2c->I2Cx->INT_MASK |= fe_INT_Index;
}

/************************************************************************************
 * @fn      i2c_int_disable
 *
 * @brief   I2C interrupt disable.
 *
 * @param   hi2c: I2C handle.
 *          fe_INT_Index: interrupt index.
 */
void i2c_int_disable(I2C_HandleTypeDef *hi2c, enum_INT_Index_t fe_INT_Index)
{
    hi2c->I2Cx->INT_MASK &= ~fe_INT_Index;
}

/************************************************************************************
 * @fn      i2c_is_int_enable
 *
 * @brief   Is I2C interrupt enable.
 *
 * @param   hi2c: I2C handle.
 *          fe_INT_Index: interrupt index.
 */
bool i2c_is_int_enable(I2C_HandleTypeDef *hi2c, enum_INT_Index_t fe_INT_Index)
{
    return (hi2c->I2Cx->INT_MASK & fe_INT_Index) ? true : false;
}

/************************************************************************************
 * @fn      i2c_get_int_status
 *
 * @brief   I2C interrupt Status.
 *
 * @param   hi2c: I2C handle.
 *          fe_INT_Index: interrupt index.
 */
bool i2c_get_int_status(I2C_HandleTypeDef *hi2c, enum_INT_Index_t fe_INT_Index)
{
    bool lb_Status = (hi2c->I2Cx->RAW_INT_STAT & fe_INT_Index) ? true : false;

    return lb_Status;
}

/************************************************************************************
 * @fn      i2c_clear_int_status
 *
 * @brief   I2C interrupt status clear.
 *
 * @param   hi2c: I2C handle.
 *          fe_INT_Index: interrupt index.
 */
void i2c_clear_int_status(I2C_HandleTypeDef *hi2c, enum_INT_Index_t fe_INT_Index)
{
    volatile uint32_t lu32_Temp;

    switch (fe_INT_Index)
    {
        case INT_RX_UNDER:         lu32_Temp = hi2c->I2Cx->CLR_RX_UNDER;      break;
        case INT_RX_OVER:          lu32_Temp = hi2c->I2Cx->CLR_RX_OVER;       break;
        case INT_TX_OVER:          lu32_Temp = hi2c->I2Cx->CLR_TX_OVER;       break;
        case INT_RD_REQ:           lu32_Temp = hi2c->I2Cx->CLR_RD_REQ;        break;
        case INT_TX_ABRT:          lu32_Temp = hi2c->I2Cx->CLR_TX_ABRT;       break;
        case INT_RX_DONE:          lu32_Temp = hi2c->I2Cx->CLR_RX_DONE;       break;
        case INT_ACTIVITY:         lu32_Temp = hi2c->I2Cx->CLR_ACTIVITY;      break;
        case INT_STOP_DET:         lu32_Temp = hi2c->I2Cx->CLR_STOP_DET;      break;
        case INT_START_DET:        lu32_Temp = hi2c->I2Cx->CLR_START_DET;     break;
        case INT_RESTART_DET:      lu32_Temp = hi2c->I2Cx->CLR_RESTART_DET;   break;
        case INT_SCL_STUCK_AT_LOW: lu32_Temp = hi2c->I2Cx->CLR_SCL_STUCK_DET; break;

        default: break; 
    }
}

/*
    for example:

GPIO_InitTypeDef GPIO_Handle;
I2C_HandleTypeDef I2C1_Handle;

uint8_t TxBufferI2C[256];
uint8_t RxBufferI2C[256];

void i2c_isr1(void)
{
    i2c_IRQHandler(&I2C1_Handle);
}

void main(void)
{
    uint32_t i;
    
    GPIO_Handle.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
    GPIO_Handle.Pull      = GPIO_PULLUP;
    GPIO_Handle.Alternate = GPIO_FUNCTION_1;
    gpio_init(GPIO_A, &GPIO_Handle);

    I2C1_Handle.I2Cx = I2C1;
    I2C1_Handle.Init.I2C_Mode = I2C_MODE_MASTER_7BIT;
    I2C1_Handle.Init.SCL_HCNT = 500;
    I2C1_Handle.Init.SCL_LCNT = 500;
    I2C1_Handle.Init.Slave_Address = 0xAE;

    i2c_init(&I2C1_Handle);

    NVIC_EnableIRQ(I2C1_IRQn);

    // EEPROM write/read
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*0, &TxBufferI2C[32*0], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*1, &TxBufferI2C[32*1], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*2, &TxBufferI2C[32*2], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*3, &TxBufferI2C[32*3], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*4, &TxBufferI2C[32*4], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*5, &TxBufferI2C[32*5], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*6, &TxBufferI2C[32*6], 32);
    i2c_memory_write(&I2C1_Handle, 0xA0, 32*7, &TxBufferI2C[32*7], 32);
    
    i2c_memory_read(&I2C1_Handle, 0xA0, 0, RxBufferI2C, 256);
    
    for (i = 0; i < 256; i++)
    {
        co_printf("%02X \r\n", RxBufferI2C[i]);
    }
}

*/
