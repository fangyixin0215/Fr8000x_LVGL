/*
  ******************************************************************************
  * @file    driver_gpio.h
  * @author  FreqChip Firmware Team
  * @version V1.0.0
  * @date    2021
  * @brief   Header file of GPIO HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 FreqChip.
  * All rights reserved.
  ******************************************************************************
*/
#ifndef __DRIVER_GPIO_H__
#define __DRIVER_GPIO_H__

#include <stdint.h>
#include <stdbool.h>

#include "driver_system.h"
#include "plf.h"

/*
 *  PortA MUX
 *              | PA0            | PA1            | PA2            | PA3            | PA4            | PA5            | PA6            | PA7            |
 * -----------------------------------------------------------------------------------------------------------------------------------------------
 * 0x0          | GPIO_A0        | GPIO_A1        | GPIO_A2        | GPIO_A3        | GPIO_A4        | GPIO_A5        | GPIO_A6        | GPIO_A7        |
 * 0x1          | I2C0_CLK       | I2C0_DAT       | I2C1_CLK       | I2C1_DAT       | I2C0_CLK       | I2C0_DAT       | I2C1_CLK       | I2C1_DAT       |
 * 0x2          | SPI0_M_CLK     | SPI0_M_CS      | SPI0_M_IO0     | SPI0_M_IO1     | SPI0_M_IO2     | SPI0_M_IO3     | SPI0_M_CLK     | SPI0_M_CS      |
 * 0x3          | SPI_S_CLK      | SPI_S_CS       | SPI_S_MOSI     | SPI_S_MISO     | SPI_S_CLK      | SPI_S_CS       | SPI_S_MOSI     | SPI_S_MISO     |
 * 0x4          | UART0_Rx       | UART0_Tx       | UART0_RTS      | UART0_CTS      | UART0_Rx       | UART0_Tx       | UART0_RTS      | UART0_CTS      |
 * 0x5          | USB_DP         | USB_DM         | UART1_Rx       | UART1_Tx       | USB_DP         | USB_DM         | UART1_Rx       | UART1_Tx       |
 * 0x6          | PWM0           | PWM1           | PWM2           | PWM3           | PWM4           | PWM5           | PWM6           | PWM7           |
 * 0x7          | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       |
 * 0x8          |                |                |                |                | CLK_OUT        | RF_PA_TXEN     | RF_PA_RXEN     | CLK_OUT        |
 * 0x9          | IrDA_IN        | IrDA_OUT       |                |                |                | IrDA_IN        | IrDA_OUT       |                |
 * 0xA          | I2S_CK         | I2S_WS         | I2S_SD_OUT     | I2S_SD_IN      | I2S_CK         | I2S_WS         | I2S_SD_OUT     | I2S_SD_IN      |
 * 0xB          |                |                |                |                |                |                |                |                |
 * 0xC          |                |                |                |                |                |                |                |                |
 * 0xD          | LCD_CS         | LCD_DC         | LCD_WR         | LCD_RD         | LCD_TE         | LCD_TE         | LCD_CS         | LCD_DC         |
 * 0xE          |                |                |                |                |                |                |                |                |
 * 0xF          |                |                |                |                |                |                |                |                |
 */


/*
 *  PortB MUX
 *              | PB0            | PB1            | PB2            | PB3            | PB4            | PB5            | PB6            | PB7            |
 * -----------------------------------------------------------------------------------------------------------------------------------------------
 * 0x0          | GPIO_B0        | GPIO_B1        | GPIO_B2        | GPIO_B3        | GPIO_B4        | GPIO_B5        | GPIO_B6        | GPIO_B7        |
 * 0x1          | I2C0_CLK       | I2C0_DAT       | I2C1_CLK       | I2C1_DAT       | I2C0_CLK       | I2C0_DAT       | I2C1_CLK       | I2C1_DAT       |
 * 0x2          | SPI0_M_CLK     | SPI0_M_CS      | SPI0_M_IO0     | SPI0_M_IO1     | SPI0_M_IO2     | SPI0_M_IO3     | SPI0_M_CLK     | SPI0_M_CS      |
 * 0x3          | SPI_S_CLK      | SPI_S_CS       | SPI_S_MOSI     | SPI_S_MISO     | SPI_S_CLK      | SPI_S_CS       | SPI_S_MOSI     | SPI_S_MISO     |
 * 0x4          | UART0_Rx       | UART0_Tx       | UART0_RTS      | UART0_CTS      | UART0_Rx       | UART0_Tx       | UART0_RTS      | UART0_CTS      |
 * 0x5          | USB_DP         | USB_DM         | UART1_Rx       | UART1_Tx       | USB_DP         | USB_DM         | UART1_Rx       | UART1_Tx       |
 * 0x6          | PWM0           | PWM1           | PWM2           | PWM3           | PWM4           | PWM5           | PWM6           | PWM7           |
 * 0x7          | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       | PDM_CLK        | PDM_DATA       |
 * 0x8          |                |                |                |                | CLK_OUT        | RF_PA_TXEN     | RF_PA_RXEN     | CLK_OUT        |
 * 0x9          | IrDA_IN        | IrDA_OUT       |                |                |                | IrDA_IN        | IrDA_OUT       |                |
 * 0xA          |                |                |                |                | I2S_CK         | I2S_WS         | I2S_SD_OUT     | I2S_SD_IN      |
 * 0xB          |                |                |                |                |                |                |                |                |
 * 0xC          |                |                |                |                |                |                |                |                |
 * 0xD          | LCD_D0         | LCD_D1         | LCD_D2         | LCD_D3         | LCD_D4         | LCD_D5         | LCD_D6         | LCD_D7         |
 * 0xE          |                |                |                |                |                |                |                |                |
 * 0xF          |                |                |                |                |                |                |                |                |
 */


/*
 *  PortC MUX
 *             | PC0          | PC1          | PC2          | PC3          | PC4          | PC5          | PC6          | PC7          |
 * -------------------------------------------------------------------------------------------------------------------------------------
 * 0x0         | GPIO_C0      | GPIO_C1      | GPIO_C2      | GPIO_C3      | GPIO_C4      | GPIO_C5      | GPIO_C6      | GPIO_C7      |
 * 0x1         | I2C0_CLK     | I2C0_DAT     | I2C1_CLK     | I2C1_DAT     | I2C0_CLK     | I2C0_DAT     | I2C1_CLK     | I2C1_DAT     |
 * 0x2         | SPI1_M_CLK   | SPI1_M_CS    | SPI1_M_IO0   | SPI1_M_IO1   | SPI1_M_IO2   | SPI1_M_IO3   | SPI1_M_CLK   | SPI1_M_CS    |
 * 0x3         | SPI_S_CLK    | SPI_S_CS     | SPI_S_MOSI   | SPI_S_MISO   | SPI_S_CLK    | SPI_S_CS     | SPI_S_MOSI   | SPI_S_MISO   |
 * 0x4         | UART0_Rx     | UART0_Tx     | UART0_RTS    | UART0_CTS    | UART0_Rx     | UART0_Tx     | UART0_RTS    | UART0_CTS    |
 * 0x5         | LCD_CS       | LCD_DC       | UART1_Rx     | UART1_Tx     | LCD_WR       | LCD_RD       | UART1_Rx     | UART1_Tx     |
 * 0x6         | PWM0         | PWM1         | PWM2         | PWM3         | PWM4         | PWM5         | PWM6         | PWM7         |
 * 0x7         | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     |
 * 0x8         |              |              |              |              |              |              | SWCLK        | SWDIO        |
 * 0x9         | IrDA_IN      | IrDA_OUT     |              |              |              | IrDA_IN      | IrDA_OUT     |              |
 * 0xA         | I2S_CK       | I2S_WS       | I2S_SD_OUT   | I2S_SD_IN    | I2S_CK       | I2S_WS       | I2S_SD_OUT   | I2S_SD_IN    |
 * 0xB         |              |              |              |              |              |              |              |              |
 * 0xC         | QSPI0_IO3    | QSPI0_CLK    | QSPI0_CS     | QSPI0_IO1    | QSPI0_IO2    | QSPI0_IO0    |              |              |
 * 0xD         | LCD_D8       | LCD_D9       | LCD_D10      | LCD_D11      | LCD_D12      | LCD_D13      | LCD_D14      | LCD_D15      |
 * 0xE         |              |              |              |              |              |              |              |              |
 * 0xF         |              |              |              |              |              |              |              |              |
 */

/*
 *  PortD MUX
 *             | PD0          | PD1          | PD2          | PD3          | PD4          | PD5          | PD6          | PD7          |
 * -------------------------------------------------------------------------------------------------------------------------------------
 * 0x0         | GPIO_D0      | GPIO_D1      | GPIO_D2      | GPIO_D3      | GPIO_D4      | GPIO_D5      | GPIO_D6      | GPIO_D7      |
 * 0x1         | I2C0_CLK     | I2C0_DAT     | I2C1_CLK     | I2C1_DAT     | I2C0_CLK     | I2C0_DAT     | I2C1_CLK     | I2C1_DAT     |
 * 0x2         | SPI1_M_CLK   | SPI1_M_CS    | SPI1_M_IO0   | SPI1_M_IO1   | SPI1_M_IO2   | SPI1_M_IO3   | SPI1_M_CLK   | SPI1_M_CS    |
 * 0x3         | SPI_S_CLK    | SPI_S_CS     | SPI_S_MOSI   | SPI_S_MISO   | SPI_S_CLK    | SPI_S_CS     | SPI_S_MOSI   | SPI_S_MISO   |
 * 0x4         | UART0_Rx     | UART0_Tx     | UART0_RTS    | UART0_CTS    | UART0_Rx     | UART0_Tx     | UART0_RTS    | UART0_CTS    |
 * 0x5         |              |              | UART1_Rx     | UART1_Tx     |              |              | UART1_Rx     | UART1_Tx     |
 * 0x6         | PWM0         | PWM1         | PWM2         | PWM3         | PWM4         | PWM5         | PWM6         | PWM7         |
 * 0x7         | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     | PDM_CLK      | PDM_DATA     |
 * 0x8         | AuxADC7      | AuxADC6      | AuxADC5      | AuxADC4      | AuxADC3      | AuxADC2      | AuxADC1      | AuxADC0      |
 * 0x9         | IrDA_IN      | IrDA_OUT     |              |              |              | IrDA_IN      | IrDA_OUT     |              |
 * 0xA         | I2S_CK       | I2S_WS       | I2S_SD_OUT   | I2S_SD_IN    | I2S_CK       | I2S_WS       | I2S_SD_OUT   | I2S_SD_IN    |
 * 0xB         |              |              |              |              |              |              |              |              |
 * 0xC         |              |              |              |              |              |              |              |              |
 * 0xD         | LCD_WR       | LCD_RD       | LCD_TE       | LCD_TE       | LCD_CS       | LCD_DC       | LCD_WR       | LCD_RD       |
 * 0xE         |              |              |              |              |              |              |              |              |
 * 0xF         |              |              |              |              |              |              |              |              |
 */

/*
 *  PortE MUX
 *             | PE0          | PE1          |
 * ------------------------------------------
 * 0x0         | GPIO_E0      | GPIO_E1      |
 * 0x1         | I2C0_CLK     | I2C0_DAT     |
 * 0x2         | SPI1_M_IO0   | SPI1_M_IO1   |
 * 0x3         | SPI_S_CLK    | SPI_S_CS     |
 * 0x4         | UART0_Rx     | UART0_Tx     |
 * 0x5         | UART1_Rx     | UART1_Tx     |
 * 0x6         | PWM0         | PWM1         |
 * 0x7         | PDM_CLK      | PDM_DATA     |
 * 0x8         |              |              |
 * 0x9         |              |              |
 * 0xA         | USB_DP       | USB_DM       |
 * 0xB         |              |              |
 * 0xC         |              |              |
 * 0xD         | LCD_TE       | LCD_TE       |
 * 0xE         |              |              |
 * 0xF         |              |              |
 */

/** @addtogroup GPIO_Registers_Section
  * @{
  */
/* ################################ Register Section Start ################################ */

/* -----------------------------------------------*/
/*                 GPIO Registers                 */
/* -----------------------------------------------*/
typedef struct 
{
    volatile uint32_t  PortA_DATA;          /* Offset 0x00 */
    volatile uint32_t  PortB_DATA;          /* Offset 0x04 */
    volatile uint32_t  PortC_DATA;          /* Offset 0x08 */
    volatile uint32_t  PortD_DATA;          /* Offset 0x0C */
    volatile uint32_t  PortE_DATA;          /* Offset 0x10 */
    volatile uint32_t  rsv_0[3];
    volatile uint32_t  PortA_OutputEN;      /* Offset 0x20 */
    volatile uint32_t  PortB_OutputEN;      /* Offset 0x24 */
    volatile uint32_t  PortC_OutputEN;      /* Offset 0x28 */
    volatile uint32_t  PortD_OutputEN;      /* Offset 0x2C */
    volatile uint32_t  PortE_OutputEN;      /* Offset 0x30 */
    volatile uint32_t  rsv_1[3];

    volatile uint32_t  EXTI_EN_0;           /* Offset 0x40 */
    volatile uint32_t  EXTI_EN_1;           /* Offset 0x44 */
    volatile uint32_t  rsv_2[8174];
    volatile uint32_t  EXTI_INT_EN_0;       /* Offset 0x8000 */
    volatile uint32_t  EXTI_INT_EN_1;       /* Offset 0x8004 */
    volatile uint32_t  EXTI_INT_STATUS_0;   /* Offset 0x8008 */
    volatile uint32_t  EXTI_INT_STATUS_1;   /* Offset 0x800C */
    volatile uint32_t  EXTI_TYPE0;          /* Offset 0x8010 */
    volatile uint32_t  EXTI_TYPE1;          /* Offset 0x8014 */
    volatile uint32_t  EXTI_TYPE2;          /* Offset 0x8018 */
    volatile uint32_t  rsv_3[1];

    volatile uint32_t  EXTI_CNT[36];        /* Offset 0x8020 */
}struct_GPIO_t;

#define GPIO    ((struct_GPIO_t *)GPIO_BASE)

/* ################################ Register Section END ################################## */
/**
  * @}
  */


/** @addtogroup GPIO_Initialization_Config_Section
  * @{
  */
/* ################################ Initialization/Config Section Start ################################ */

/** @defgroup GPIO_pins GPIO pins
  * @{
  */
#define GPIO_PIN_0                 ((uint8_t)0x01)    /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint8_t)0x02)    /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint8_t)0x04)    /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint8_t)0x08)    /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint8_t)0x10)    /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint8_t)0x20)    /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint8_t)0x40)    /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint8_t)0x80)    /* Pin 7 selected    */
/**
  * @}
  */

/* GPIO mode */
typedef enum
{
    GPIO_MODE_INPUT              = 0x1000u,    /*!< Input Floating Mode               */
    GPIO_MODE_OUTPUT_PP          = 0x1001u,    /*!< Output Push Pull Mode             */
    GPIO_MODE_AF_PP              = 0x0002u,    /*!< Alternate Function Push Pull Mode */
    GPIO_MODE_EXTI_IT_LOW_LEVEL  = 0x1100u,    /*!< External Interrupt Mode with low  level trigger detection   */
    GPIO_MODE_EXTI_IT_HIGH_LEVEL = 0x1101u,    /*!< External Interrupt Mode with high level trigger detection   */
    GPIO_MODE_EXTI_IT_RISING     = 0x1102u,    /*!< External Interrupt Mode with Rising  edge trigger detection */
    GPIO_MODE_EXTI_IT_FALLING    = 0x1103u,    /*!< External Interrupt Mode with Falling edge trigger detection */

    GPIO_MODE_IO_MASK = 0x1000,    /*!< GPIO Function Mask */
    GPIO_MODE_IT_MASK = 0x0100,    /*!< EXTI Function Mask */
}enum_GPIO_MODE_t;

/* GPIO pull */
typedef enum
{
    GPIO_NOPULL,      /*!< No Pull-up or Pull-down activation  */
    GPIO_PULLUP,      /*!< Pull-up activation                  */
    GPIO_PULLDOWN,    /*!< Pull-down activation                */
}enum_Pull_t;

/* function selection */
typedef enum
{
    GPIO_FUNCTION_0,
    GPIO_FUNCTION_1,
    GPIO_FUNCTION_2,
    GPIO_FUNCTION_3,
    GPIO_FUNCTION_4,
    GPIO_FUNCTION_5,
    GPIO_FUNCTION_6,
    GPIO_FUNCTION_7,
    GPIO_FUNCTION_8,
    GPIO_FUNCTION_9,
    GPIO_FUNCTION_A,
    GPIO_FUNCTION_B,
    GPIO_FUNCTION_C,
    GPIO_FUNCTION_D,
    GPIO_FUNCTION_E,
    GPIO_FUNCTION_F,
}enum_Function_t;

/* GPIOx Index */
typedef enum
{
    GPIO_A,
    GPIO_B,
    GPIO_C,
    GPIO_D,
    GPIO_E,
}enum_GPIOx_t;

/* GPIO Bit SET and Bit RESET enumeration */
typedef enum
{
    GPIO_PIN_CLEAR = 0u,
    GPIO_PIN_SET   = 1u,
}enum_PinStatus_t;

/* EXTI line index enumeration */
typedef enum
{
    /* PortA0 ~ PortA7 : EXTI Line0 ~ 7 */
    EXTI_LINE0_PA0,
    EXTI_LINE1_PA1,
    EXTI_LINE2_PA2,
    EXTI_LINE3_PA3,
    EXTI_LINE4_PA4,
    EXTI_LINE5_PA5,
    EXTI_LINE6_PA6,
    EXTI_LINE7_PA7,
    /* PortB0 ~ PortB7 : EXTI Line8 ~ 15 */
    EXTI_LINE8_PB0,
    EXTI_LINE9_PB1,
    EXTI_LINE10_PB2,
    EXTI_LINE11_PB3,
    EXTI_LINE12_PB4,
    EXTI_LINE13_PB5,
    EXTI_LINE14_PB6,
    EXTI_LINE15_PB7,
    /* PortC0 ~ PortC7 : EXTI Line16 ~ 23 */
    EXTI_LINE16_PC0,
    EXTI_LINE17_PC1,
    EXTI_LINE18_PC2,
    EXTI_LINE19_PC3,
    EXTI_LINE20_PC4,
    EXTI_LINE21_PC5,
    EXTI_LINE22_PC6,
    EXTI_LINE23_PC7,
    /* PortD0 ~ PortD7 : EXTI Line24 ~ 31 */
    EXTI_LINE24_PD0,
    EXTI_LINE25_PD1,
    EXTI_LINE26_PD2,
    EXTI_LINE27_PD3,
    EXTI_LINE28_PD4,
    EXTI_LINE29_PD5,
    EXTI_LINE30_PD6,
    EXTI_LINE31_PD7,
    /* PortE0 ~ PortE3 : EXTI Line32 ~ 35 */
    EXTI_LINE32_PE0,
    EXTI_LINE33_PE1,
    EXTI_LINE34_PE2,
    EXTI_LINE35_PE3,
    /* EXTI sum num */
    EXTI_LINE_SUM_NUM,
}enum_EXTILineIndex_t;


/*
 * @brief   GPIO Init structure definition
 */
typedef struct
{
    uint32_t Pin;        /*!< Specifies the GPIO pins to be configured.
                              This parameter can be any value of @ref GPIO_pins */

    uint32_t Mode;       /*!< Specifies the operating mode for the selected pins.
                              This parameter can be a value of @ref enum_GPIO_MODE_t */

    uint32_t Pull;       /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                              This parameter can be a value of @ref enum_Pull_t */

    uint32_t Alternate;  /*!< Peripheral to be connected to the selected pins.
                              This parameter can be a value of @ref GPIOEx_function_selection */
}GPIO_InitTypeDef;

/* ################################ Initialization/Config Section END ################################## */
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/* gpio_init */
void gpio_init(enum_GPIOx_t fe_GPIO, GPIO_InitTypeDef *GPIO_Init);

/* gpio_set_portpull */
void gpio_set_portpull(enum_GPIOx_t fe_GPIO, uint8_t fu8_Pin, enum_Pull_t fe_Pull);

/* gpio_write_group */
/* gpio_write_pin */
void gpio_write_group(enum_GPIOx_t fe_GPIO, uint8_t fu8_GroupStatus);
void gpio_write_pin(enum_GPIOx_t fe_GPIO, uint8_t fu8_Pin, enum_PinStatus_t fe_PinStatus);

/* gpio_read_group */
/* gpio_read_pin */
uint8_t gpio_read_group(enum_GPIOx_t fe_GPIO);
enum_PinStatus_t gpio_read_pin(enum_GPIOx_t fe_GPIO, uint8_t fu16_Pin);

/* exti_interrupt_enable */
/* exti_interrupt_disable */
void exti_interrupt_enable(uint8_t fu8_EXTI_Line);
void exti_interrupt_disable(uint8_t fu8_EXTI_Line);

/* exti_get_LineStatus */
/* exti_clear_LineStatus */
bool exti_get_LineStatus(uint8_t fu16_EXTI_Line);
void exti_clear_LineStatus(uint8_t fu16_EXTI_Line);

/* exti_set_FilterCNT */
void exti_set_Filter(uint8_t fu8_EXTI_Line, uint8_t fu8_DIV, uint8_t fu8_CNT);
 
#endif
