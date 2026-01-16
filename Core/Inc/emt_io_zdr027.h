/**
	******************************************************************************
	* @file           : emt_io_zdr027.h
	* @brief          :
	******************************************************************************
	* Created on      : 22 January, 2024
	*      Author     : jrshin
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_IO_ZDR027_H__
#define __EMT_IO_ZDR027_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

#define MCU_AQS2_CPU_Pin                      	GPIO_PIN_9
#define MCU_AQS2_CPU_GPIO_Port                	GPIOB
//#define 										GPIO_PIN_14
//#define             		  					GPIOC
#define SYS_RESET_Pin            		        GPIO_PIN_0
#define SYS_RESET_GPIO_Port            		  	GPIOA
#define __DC2DC_EN_Pin                       		GPIO_PIN_1
#define __DC2DC_EN_GPIO_Port                    	GPIOA
#define MCU_EN_CPU_Pin	                     	GPIO_PIN_15
#define MCU_EN_CPU_GPIO_Port	               	GPIOC

#define CPU_RXD2_MCU_Pin                      	GPIO_PIN_2
#define CPU_RXD2_MCU_GPIO_Port                	GPIOA
#define CPU_TXD2_MCU_Pin                      	GPIO_PIN_3
#define CPU_TXD2_MCU_GPIO_Port                	GPIOA
#define BP_ADC_MCU_Pin                          GPIO_PIN_4
#define BP_ADC_MCU_GPIO_Port                    GPIOA
#define TEMP_ADC_MCU_Pin                        GPIO_PIN_5
#define TEMP_ADC_MCU_GPIO_Port                  GPIOA
#define GSEN_INT_MCU_Pin                       	GPIO_PIN_6
#define GSEN_INT_MCU_GPIO_Port                  GPIOA
#define EDLC_ADC_MCU_Pin                        GPIO_PIN_7
#define EDLC_ADC_MCU_GPIO_Port                  GPIOA
#define MCU_PWR_RCAM_Pin                       	GPIO_PIN_0
#define MCU_PWR_RCAM_GPIO_Port                 	GPIOB
#define MCU_EN_MCUPWR_Pin                       GPIO_PIN_1
#define MCU_EN_MCUPWR_GPIO_Port               	GPIOB

#define MCU_EN_EDLC_Pin                  	    GPIO_PIN_2
#define MCU_EN_EDLC_GPIO_Port	                GPIOB
#define __ACC_DET_MCU_Pin                       GPIO_PIN_8
#define __ACC_DET_MCU_GPIO_Port                 GPIOA
#define MCU_PWR_5V0_Pin                  		GPIO_PIN_9
#define MCU_PWR_5V0_GPIO_Port                	GPIOA
#define MCU_EN1V8_DCDC_Pin                     	GPIO_PIN_6
#define MCU_EN1V8_DCDC_GPIO_Port                GPIOC
#define MCU_EN1V5_DCDC_Pin                      GPIO_PIN_10
#define MCU_EN1V5_DCDC_GPIO_Port                GPIOA
#define MCU_EN0V9_DCDC_Pin                     	GPIO_PIN_11
#define MCU_EN0V9_DCDC_GPIO_Port               	GPIOA
#define MCU_EN3V0_DCDC_Pin                     	GPIO_PIN_12
#define MCU_EN3V0_DCDC_GPIO_Port               	GPIOA

#define MCU_AQS1_CPU_Pin                        GPIO_PIN_15
#define MCU_AQS1_CPU_GPIO_Port                  GPIOA
#define MCU_AQS0_CPU_Pin                        GPIO_PIN_3
#define MCU_AQS0_CPU_GPIO_Port                  GPIOB
#define __SW_EASY_MCU_Pin                    	GPIO_PIN_4
#define __SW_EASY_MCU_GPIO_Port              	GPIOB
#define __USB_DET_MCU_Pin                     	GPIO_PIN_5
#define __USB_DET_MCU_GPIO_Port                 GPIOB
//#define										GPIO_PIN_6
//#define 						   				GPIOB
#define MCU_EN_GLED_Pin                         GPIO_PIN_7
#define MCU_EN_GLED_GPIO_Port                   GPIOB
#define MCU_EN_RLED_Pin                         GPIO_PIN_8
#define MCU_EN_RLED_GPIO_Port                   GPIOB

#define CPU_UART_RX_MCU_Pin                     CPU_RXD2_MCU_Pin
#define CPU_UART_RX_MCU_GPIO_Port               CPU_RXD2_MCU_GPIO_Port
#define CPU_UART_TX_MCU_Pin                     CPU_TXD2_MCU_Pin
#define CPU_UART_TX_MCU_GPIO_Port               CPU_TXD2_MCU_GPIO_Port

#define GSEN_INT_MCU_EXTI_IRQn					EXTI4_15_IRQn	//EXTI6
#define __ACC_DET_MCU_EXTI_IRQn					EXTI4_15_IRQn	//EXTI8
#define __SW_EASY_MCU_EXTI_IRQn					EXTI4_15_IRQn	//EXTI4

#define CPU_UART_PORT                           USART2

/* MX_GPIO_Init() -> GPIO_PIN_RESET*/
#define GPIOA_DEFAULT_PINSET					SYS_RESET_Pin|MCU_PWR_5V0_Pin|MCU_EN1V5_DCDC_Pin|MCU_EN0V9_DCDC_Pin|MCU_EN3V0_DCDC_Pin|MCU_AQS1_CPU_Pin|__ACC_DET_MCU_Pin
#define GPIOB_DEFAULT_PINSET					MCU_PWR_RCAM_Pin|MCU_AQS0_CPU_Pin|MCU_AQS2_CPU_Pin|MCU_EN_EDLC_Pin|__USB_DET_MCU_Pin|MCU_EN_MCUPWR_Pin
#define GPIOC_DEFAULT_PINSET					MCU_EN_CPU_Pin|MCU_EN1V8_DCDC_Pin

/* ADC IN Pins */
#define ADC_IN_A_PORT_PINS						(BP_ADC_MCU_Pin | TEMP_ADC_MCU_Pin | EDLC_ADC_MCU_Pin)
#define ADC_IN_B_PORT_PINS						DISABLE

/* I2C Pins */
#define I2C1_SCL_Pin							DISABLE
#define I2C1_SCL_GPIO_Port						DISABLE
#define I2C1_SCL_GPIO_AF						DISABLE

#define I2C1_SDA_Pin							DISABLE
#define I2C1_SDA_GPIO_Port						DISABLE
#define I2C1_SDA_GPIO_AF						DISABLE

#define I2C2_SCL_Pin							DISABLE
#define I2C2_SCL_GPIO_Port						DISABLE
#define I2C2_SCL_GPIO_AF						DISABLE

#define I2C2_SDA_Pin							DISABLE
#define I2C2_SDA_GPIO_Port						DISABLE
#define I2C2_SDA_GPIO_AF						DISABLE

#ifdef __cplusplus
}
#endif

#endif /* __EMT_IO_ZDR027_H__ */
