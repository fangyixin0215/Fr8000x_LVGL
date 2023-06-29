#ifndef _DRIVER_ST77903_H
#define _DRIVER_ST77903_H

/*
 * driver_st77903.h
 * 
 * Created on: 2023-6-29
 * Author: LH
 */
#include "stdint.h"
#include "stdbool.h"
#include "board_driver.h"


 #if (DEVELOPMENT_BOARD_8000AP||DEVELOPMENT_BOARD_8000_EVB)
#define ST77903_TE_EN
#define ST77903_CS_PORT 	  			GPIO_B 
#define ST77903_CS_GPIO 					GPIO_PIN_1

#define ST77903_SCLK_PORT					GPIO_B
#define ST77903_SCLK_GPIO					GPIO_PIN_0

#define ST77903_QIO0_PORT					GPIO_B
#define ST77903_QIO0_GPIO					GPIO_PIN_2

#define ST77903_QIO1_PORT					GPIO_B
#define ST77903_QIO1_GPIO					GPIO_PIN_3

#define ST77903_QIO2_PORT					GPIO_B
#define ST77903_QIO2_GPIO					GPIO_PIN_4

#define ST77903_QIO3_PORT					GPIO_B
#define ST77903_QIO3_GPIO					GPIO_PIN_5

#define ST77903_RST_PORT 	  			GPIO_D 
#define ST77903_RST_GPIO 					GPIO_PIN_5

#define ST77903_BACKLIGHT_PORT 	  GPIO_D 
#define ST77903_BACKLIGHT_GPIO 		GPIO_PIN_0
#ifdef ST77903_TE_EN
#define ST77903_TE_PORT 	  GPIO_B
#define ST77903_TE_GPIO 		GPIO_PIN_7
#endif

#elif DEVELOPMENT_BOARD_8000GP

#define ST77903_TE_EN
#define ST77903_CS_PORT 	  			GPIO_B 
#define ST77903_CS_GPIO 					GPIO_PIN_6

#define ST77903_SCLK_PORT					GPIO_B
#define ST77903_SCLK_GPIO					GPIO_PIN_0

#define ST77903_QIO0_PORT					GPIO_B
#define ST77903_QIO0_GPIO					GPIO_PIN_2

#define ST77903_QIO1_PORT					GPIO_B
#define ST77903_QIO1_GPIO					GPIO_PIN_3

#define ST77903_QIO2_PORT					GPIO_B
#define ST77903_QIO2_GPIO					GPIO_PIN_4

#define ST77903_QIO3_PORT					GPIO_B
#define ST77903_QIO3_GPIO					GPIO_PIN_5

#define ST77903_RST_PORT 	  			GPIO_D 
#define ST77903_RST_GPIO 					GPIO_PIN_5

#define ST77903_BACKLIGHT_PORT 	  GPIO_D 
#define ST77903_BACKLIGHT_GPIO 		GPIO_PIN_0
#define ST77903_SPI_SEL       		SPIM0

#ifdef  ST77903_TE_EN
#define ST77903_TE_PORT 	  			GPIO_B
#define ST77903_TE_GPIO 					GPIO_PIN_7
#endif

#elif KNOB_DEMO_BOARD
//#define ST77903_TE_EN
#define ST77903_CS_PORT 	  			GPIO_A 
#define ST77903_CS_GPIO 					GPIO_PIN_5

#define ST77903_SCLK_PORT					GPIO_B
#define ST77903_SCLK_GPIO					GPIO_PIN_0

#define ST77903_QIO0_PORT					GPIO_B
#define ST77903_QIO0_GPIO					GPIO_PIN_2

#define ST77903_QIO1_PORT					GPIO_B
#define ST77903_QIO1_GPIO					GPIO_PIN_3

#define ST77903_QIO2_PORT					GPIO_B
#define ST77903_QIO2_GPIO					GPIO_PIN_4

#define ST77903_QIO3_PORT					GPIO_B
#define ST77903_QIO3_GPIO					GPIO_PIN_5

#define ST77903_RST_PORT 	  			GPIO_A 
#define ST77903_RST_GPIO 					GPIO_PIN_2

#define ST77903_BACKLIGHT_PORT 	  GPIO_A 
#define ST77903_BACKLIGHT_GPIO 		GPIO_PIN_4
#define ST77903_SPI_SEL       		SPIM0

#ifdef ST77903_TE_EN
#define ST77903_TE_PORT 	  GPIO_A
#define ST77903_TE_GPIO 		GPIO_PIN_3
#endif

#elif KTX_PCB_BOARD 
#define ST77903_TE_EN
#define ST77903_CS_PORT 	  			GPIO_A 
#define ST77903_CS_GPIO 					GPIO_PIN_7

#define ST77903_SCLK_PORT					GPIO_B
#define ST77903_SCLK_GPIO					GPIO_PIN_0

#define ST77903_QIO0_PORT					GPIO_B
#define ST77903_QIO0_GPIO					GPIO_PIN_2

#define ST77903_QIO1_PORT					GPIO_B
#define ST77903_QIO1_GPIO					GPIO_PIN_3

#define ST77903_QIO2_PORT					GPIO_B
#define ST77903_QIO2_GPIO					GPIO_PIN_4

#define ST77903_QIO3_PORT					GPIO_B
#define ST77903_QIO3_GPIO					GPIO_PIN_5

#define ST77903_RST_PORT 	  			GPIO_A 
#define ST77903_RST_GPIO 					GPIO_PIN_0

#define ST77903_BACKLIGHT_PORT 	  GPIO_D 
#define ST77903_BACKLIGHT_GPIO 		GPIO_PIN_0
#define ST77903_SPI_SEL       		SPIM0

#ifdef  ST77903_TE_EN
#define ST77903_TE_PORT 	  GPIO_B
#define ST77903_TE_GPIO 		GPIO_PIN_7
#endif
#endif

#define ST77903_CS_Release()                        gpio_write_pin(ST77903_CS_PORT, ST77903_CS_GPIO, GPIO_PIN_SET) 
#define ST77903_CS_Select()                         gpio_write_pin(ST77903_CS_PORT, ST77903_CS_GPIO, GPIO_PIN_CLEAR) 
#define ST77903_RST_Release()     									gpio_write_pin(ST77903_RST_PORT, ST77903_RST_GPIO, GPIO_PIN_SET) 
#define ST77903_RST_Select()  											gpio_write_pin(ST77903_RST_PORT, ST77903_RST_GPIO, GPIO_PIN_CLEAR) 

#define ST77903_BLK_Release()     								  gpio_write_pin(ST77903_BACKLIGHT_PORT, ST77903_BACKLIGHT_GPIO, GPIO_PIN_CLEAR) 
#define ST77903_BLK_Select()  										  gpio_write_pin(ST77903_BACKLIGHT_PORT, ST77903_BACKLIGHT_GPIO, GPIO_PIN_SET) 

#ifdef ST77903_TE_EN
#define get_lcd_te_state()  												gpio_read_pin(ST77903_TE_PORT,ST77903_TE_GPIO)
#endif

enum
{
	LCD_SEND_START=0,
	LCD_SEND_BUS=0,
	LCD_SEND_STOP=0,
};

#define INIT_CMD_LEN        			(20)
//#define LCD_BIST_MODE 						0

typedef struct
{
    uint8_t cmd;
    uint8_t len;
    uint8_t buf[INIT_CMD_LEN];
}lcd_reg_param_t;
void display(uint16_t *data);
void st77903_init(void);

 
void st77903_display(uint16_t *data,uint8_t step);

uint8_t get_st77903_send_state(void);
void st77903_dma_isr(void);

#endif    



