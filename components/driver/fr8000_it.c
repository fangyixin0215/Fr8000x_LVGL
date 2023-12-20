/*
  ******************************************************************************
  * @file    fr8000_it.c
  * @author  FreqChip Firmware Team
  * @version V1.0.0
  * @date    2023
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#include "plf.h"

#include "co_log.h"

static uint32_t *CortexM3_SP;

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 */
void NMI_Handler(void)
{
    printf("NMI_Handler\r\n");
    
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    if (__get_CONTROL() & 0x02)
    {
        CortexM3_SP = (uint32_t *)__get_PSP();
    }
    else
    {
        CortexM3_SP = (uint32_t *)__get_MSP();
    }

    printf("HardFault_Handler\r\n");
    printf("CortexM3_StackFrame\r\n");
    printf("xPSR: %08X\r\n", CortexM3_SP[7]);
    printf("PC:   %08X\r\n", CortexM3_SP[6]);
    printf("LR:   %08X\r\n", CortexM3_SP[5]);
    printf("R12:  %08X\r\n", CortexM3_SP[4]);
    printf("R3:   %08X\r\n", CortexM3_SP[3]);
    printf("R2:   %08X\r\n", CortexM3_SP[2]);
    printf("R1:   %08X\r\n", CortexM3_SP[1]);
    printf("R0:   %08X\r\n", CortexM3_SP[0]); 
    
    printf("\r\n");
    printf("Read memory from the top of the stack \r\n\r\n");
    printf("  Address                          Memory \r\n");
    for(int i = 0; i < 80; i++)
    {
        /* address */
        if ((i % 4 == 0))
        {
            if (i == 0)
                printf("0x%08X = ", (uint32_t)&CortexM3_SP[i]);
            else
                printf("\r\n0x%08X = ", (uint32_t)&CortexM3_SP[i]);
        }
        
        /* memory */
        printf("0x%08X  ", CortexM3_SP[i]);
    }
    
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      MemManage_Handler
 *
 * @brief   This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    printf("MemManage_Handler\r\n");
    
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      BusFault_Handler
 *
 * @brief   This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    printf("BusFault_Handler\r\n");
    
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      UsageFault_Handler
 *
 * @brief   This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    printf("BusFault_Handler\r\n");
    
    while(1)
    {
    }
}