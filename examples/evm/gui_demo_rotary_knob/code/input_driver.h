#ifndef __INPUT_DRIVER_H__

#define __INPUT_DRIVER_H__


#include "driver_gpio.h"
#include "driver_i2c.h"
#include "driver_system.h"

#include "board_driver.h"

#if DEVELOPMENT_BOARD_8000GP
#define EC11_EXTI_LINE  		EXTI_GPIOD_3
#define EC11_EXTI_GPIO_NUM  GPIO_PIN_3
#define EC11_EXTI_PORT  		GPIO_D

#define EC11_DIR_GPIO_NUM  GPIO_PIN_7
#define EC11_DIR_PORT  		 GPIO_A

#define EC11_KEY_GPIO_NUM  GPIO_PIN_2
#define EC11_KEY_PORT  		 GPIO_D
#elif KNOB_DEMO_BOARD
#define EC11_EXTI_LINE  		EXTI_GPIOD_6
#define EC11_EXTI_GPIO_NUM  GPIO_PIN_6
#define EC11_EXTI_PORT  		GPIO_D

#define EC11_DIR_GPIO_NUM  GPIO_PIN_7
#define EC11_DIR_PORT  		 GPIO_D

#define EC11_KEY_GPIO_NUM  GPIO_PIN_1
#define EC11_KEY_PORT  		 GPIO_B
#elif KTX_PCB_BOARD
#define EC11_EXTI_LINE  		EXTI_GPIOB_2
#define EC11_EXTI_GPIO_NUM  GPIO_PIN_2
#define EC11_EXTI_PORT  		GPIO_B

#define EC11_DIR_GPIO_NUM  GPIO_PIN_1
#define EC11_DIR_PORT  		 GPIO_B

#define EC11_KEY_GPIO_NUM  GPIO_PIN_0
#define EC11_KEY_PORT  		 GPIO_B
#endif

#define KEY_OK      gpio_read_pin(GPIO_D, GPIO_PIN_2)//读取按键ok
#define KEY_LEFT    gpio_read_pin(GPIO_D, GPIO_PIN_7)//读取按键left
#define KEY_RIGHT   gpio_read_pin(GPIO_D, GPIO_PIN_6)//读取按键right

#define KEY_OK_PRES	        1		//KEY_OK  
#define KEY_LEFT_PRES	    	2		//KEY_LEFT 
#define KEY_RIGHT_LEFT_PRES	3		//KEY_RIGHT 

void input_encoder_init(void);
void ec11_exti_handle(void);
//void encoder_handle_cb(void);

#define ENCODER_EN 1

#endif



