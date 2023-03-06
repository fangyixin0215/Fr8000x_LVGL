#ifndef __DRIVER_KTM57XX_H__
#define __DRIVER_KTM57XX_H__

#include "driver_gpio.h"
#include "driver_exti.h"
#include "driver_system.h"
#include "board_driver.h"

#if (KTX_PCB_BOARD==1)
#define KTM57XX_EXTI_LINE EXTI_GPIOA_7
#define KTM57XX_EXTI_GPIO GPIO_PIN_7
#define KTM57XX_EXTI_PORT GPIO_A

#define KTM57XX_SCLK_PORT	GPIO_A
#define KTM57XX_SCLK_GPIO	GPIO_PIN_5

#define KTM57XX_CS_PORT	GPIO_A
#define KTM57XX_CS_GPIO	GPIO_PIN_6

#define KTM57XX_MISO_PORT	GPIO_A
#define KTM57XX_MISO_GPIO	GPIO_PIN_3

#define KTM57XX_MOSI_PORT	GPIO_A
#define KTM57XX_MOSI_GPIO	GPIO_PIN_2

#elif (DEVELOPMENT_BOARD_8000GP==1)
#define KTM57XX_EXTI_LINE EXTI_GPIOD_5
#define KTM57XX_EXTI_GPIO GPIO_PIN_5
#define KTM57XX_EXTI_PORT GPIO_D

#define KTM57XX_SCLK_PORT	GPIO_A
#define KTM57XX_SCLK_GPIO	GPIO_PIN_4

#define KTM57XX_MOSI_PORT	GPIO_A
#define KTM57XX_MOSI_GPIO	GPIO_PIN_3

#define KTM57XX_CS_PORT	GPIO_A
#define KTM57XX_CS_GPIO	GPIO_PIN_6

#define KTM57XX_MISO_PORT	GPIO_B
#define KTM57XX_MISO_GPIO	GPIO_PIN_7

#elif (KNOB_DEMO_BOARD==1)
#define KTM57XX_EXTI_LINE EXTI_GPIOD_5
#define KTM57XX_EXTI_GPIO GPIO_PIN_5
#define KTM57XX_EXTI_PORT GPIO_D

#define KTM57XX_SCLK_PORT	GPIO_A
#define KTM57XX_SCLK_GPIO	GPIO_PIN_4

#define KTM57XX_MOSI_PORT	GPIO_A
#define KTM57XX_MOSI_GPIO	GPIO_PIN_3

#define KTM57XX_CS_PORT	GPIO_A
#define KTM57XX_CS_GPIO	GPIO_PIN_6

#define KTM57XX_MISO_PORT	GPIO_B
#define KTM57XX_MISO_GPIO	GPIO_PIN_7
#endif


#define SCL_1_Clr()   gpio_write_pin(KTM57XX_SCLK_PORT, KTM57XX_SCLK_GPIO, GPIO_PIN_CLEAR)          //SCL1      = 0
#define SCL_1_Set()   gpio_write_pin(KTM57XX_SCLK_PORT, KTM57XX_SCLK_GPIO, GPIO_PIN_SET)                // = 1

#define MOSI_1_Clr()  gpio_write_pin(KTM57XX_MOSI_PORT, KTM57XX_MOSI_GPIO, GPIO_PIN_CLEAR)      //MOSI1
#define MOSI_1_Set()  gpio_write_pin(KTM57XX_MOSI_PORT, KTM57XX_MOSI_GPIO, GPIO_PIN_SET)   
 		   
#define nCS_1_Clr()  gpio_write_pin(KTM57XX_CS_PORT, KTM57XX_CS_GPIO, GPIO_PIN_CLEAR)         //CS1
#define nCS_1_Set()  gpio_write_pin(KTM57XX_CS_PORT, KTM57XX_CS_GPIO, GPIO_PIN_SET) 

#define MISO_1_Read() gpio_read_pin(KTM57XX_MISO_PORT,KTM57XX_MISO_GPIO)    //MISO1

#define		CMD_BURST_MODE  0x10
#define		CMD_WAKEUP_ON_CHANGE  0x20
#define		CMD_SINGLE_MEASUREMENT  0x30
#define		CMD_READ_MEASUREMENT  0x40
#define		CMD_READ_REGISTER  0x50
#define		CMD_WRITE_REGISTER  0x60
#define		CMD_EXIT_MODE  0x80
#define		CMD_MEMORY_RECALL  0xd
#define		CMD_MEMORY_STORE  0xe0
#define		CMD_RESET  0xf0
#define   READBACKCOUNTER 9

#define 	JUDGE_ANGLE_LEFT	 	 4 //左转阈值
#define 	JUDGE_ANGLE_RIGHT 	-4 //右转阈值
#define   KTM57XX_SHAKE_TIME  80 //消除抖动时间
#define 	KTM57XX_EXTI_EN		1

typedef struct KTM57XX_STRUCT
{
	uint8_t read_data[10];
	uint32_t angle;
	uint32_t last_angle;
	uint16_t Density_Y;
	uint16_t Density_X;
	uint16_t Density_Z;
	uint32_t last_trigger_time;
	
}ktm57xx_struct_t;


void     ktm57xx_init(void);
void     ktm57xx_write_byte(uint8_t byte);
uint8_t  ktm57xx_read_byte(void);
uint8_t  ktm57xx_continuous_mode (uint8_t axis);
uint8_t  ktm57xx_wakeup_mode(uint8_t axis);
uint8_t  ktm57xx_single_mode(uint8_t axis);
void     ktm57xx_reset(void);
uint8_t  ktm57xx_read_measurement(uint8_t axis);
void     ktm57xx_read_register (uint8_t Register);
uint8_t  ktm57xx_exti_current_mode(void);
uint8_t  ktm57xx_write_register (uint16_t writeData,uint8_t Register);
void     spimemoryStore_1(void);
void     ktm57xx_unlock_otp (void);
void     ktm57xx_register_init(void);
void 		 ktm57xx_exti_isr_handle(void);

#endif




