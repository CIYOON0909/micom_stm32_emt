/**
	******************************************************************************
	* @file           : emt_io_prime.h
	* @brief          :
	******************************************************************************
	* Created on      : 06 Nov, 2024
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_IO_PRIME_H__
#define __EMT_IO_PRIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

#define MCU_AQS1_CPU_Pin                      	GPIO_PIN_9
#define MCU_AQS1_CPU_GPIO_Port                	GPIOB

#define SYS_RESET_Pin            		        GPIO_PIN_14
#define SYS_RESET_GPIO_Port            		  	GPIOC

#define __DC2DC_EN_Pin                       	GPIO_PIN_15
#define __DC2DC_EN_GPIO_Port                    GPIOC

#define MCU_EN1V5_DCDC_Pin                      GPIO_PIN_0
#define MCU_EN1V5_DCDC_GPIO_Port                GPIOA

#define MCU_EN_MCUPWR_Pin                       GPIO_PIN_1
#define MCU_EN_MCUPWR_GPIO_Port               	GPIOA

#define GSEN_INT_MCU_Pin                       	GPIO_PIN_2
#define GSEN_INT_MCU_GPIO_Port                  GPIOA
#define GSEN_INT_MCU_EXTI_IRQn					EXTI2_3_IRQn

#define __ACC_DET_MCU_Pin                       GPIO_PIN_3
#define __ACC_DET_MCU_GPIO_Port                 GPIOA
#define __ACC_DET_MCU_EXTI_IRQn					EXTI2_3_IRQn

#define BP_ADC_MCU_Pin                          GPIO_PIN_4
#define BP_ADC_MCU_GPIO_Port                    GPIOA

#define TEMP_ADC_MCU_Pin                        GPIO_PIN_5
#define TEMP_ADC_MCU_GPIO_Port                  GPIOA

#define USB_DET_MCU_Pin                     	GPIO_PIN_6
#define USB_DET_MCU_GPIO_Port                 	GPIOA

#define EDLC_ADC_MCU_Pin                        GPIO_PIN_7
#define EDLC_ADC_MCU_GPIO_Port                  GPIOA

#define CPU_RXD3_MCU_Pin                      	GPIO_PIN_0
#define CPU_RXD3_MCU_GPIO_Port                	GPIOB
#define CPU_RXD3_MCU_Pin_AF						GPIO_AF4_USART3

#define __BP_DET_MCU_Pin                        GPIO_PIN_1
#define __BP_DET_MCU_GPIO_Port                  GPIOB
#define BP_DET_MCU_EXTI_IRQn                    EXTI0_1_IRQn

#define CPU_TXD3_MCU_Pin                      	GPIO_PIN_2
#define CPU_TXD3_MCU_GPIO_Port                	GPIOB
#define CPU_TXD3_MCU_Pin_AF						GPIO_AF4_USART3

#define MCU_EN3V0_DCDC_Pin                     	GPIO_PIN_8
#define MCU_EN3V0_DCDC_GPIO_Port               	GPIOA

#define MCU_SCL1_EXTRTC_Pin                     GPIO_PIN_9
#define MCU_SCL1_EXTRTC_GPIO_Port               GPIOA
#define MCU_SCL1_EXTRTC_GPIO_AF    				GPIO_AF6_I2C1

#define MCU_EN_5V_Pin                  			GPIO_PIN_6
#define MCU_EN_5V_GPIO_Port                		GPIOC

#define MCU_SDA1_EXTRTC_Pin                     GPIO_PIN_10
#define MCU_SDA1_EXTRTC_GPIO_Port               GPIOA
#define MCU_SDA1_EXTRTC_GPIO_AF					GPIO_AF6_I2C1

#define MCU_SCL2_LED_Pin                      	GPIO_PIN_11
#define MCU_SCL2_LED_GPIO_Port                	GPIOA
#define MCU_SCL2_LED_GPIO_AF					GPIO_AF6_I2C2

#define MCU_SDA2_LED_Pin                      	GPIO_PIN_12
#define MCU_SDA2_LED_GPIO_Port                	GPIOA
#define MCU_SDA2_LED_GPIO_AF					GPIO_AF6_I2C2

#define MCU_EN_EDLC_Pin                  	    GPIO_PIN_13
#define MCU_EN_EDLC_GPIO_Port	                GPIOA

// SW_RECKEY_MCU
#define __SW_EASY_MCU_Pin                    	GPIO_PIN_15
#define __SW_EASY_MCU_GPIO_Port              	GPIOA
#define __SW_EASY_MCU_EXTI_IRQn					EXTI4_15_IRQn

#define MCU_EN0V9_1V8_DCDC_Pin                 	GPIO_PIN_3
#define MCU_EN0V9_1V8_DCDC_GPIO_Port           	GPIOB

#define MCU_EN_CPU_Pin	                     	GPIO_PIN_4
#define MCU_EN_CPU_GPIO_Port	               	GPIOB

#define MCU_AQS0_CPU_Pin                        GPIO_PIN_5
#define MCU_AQS0_CPU_GPIO_Port                  GPIOB

// SW_FNKEY_MCU
#define __SW_FNKEY_MCU_Pin                    	GPIO_PIN_6
#define __SW_FNKEY_MCU_GPIO_Port              	GPIOB
#define __SW_FNKEY_MCU_EXTI_IRQn				EXTI4_15_IRQn

#define MCU_PWR_RCAM_Pin                       	GPIO_PIN_7
#define MCU_PWR_RCAM_GPIO_Port                 	GPIOB

#define MCU_AQS2_CPU_Pin                        GPIO_PIN_8
#define MCU_AQS2_CPU_GPIO_Port                  GPIOB

#define MCU_EN_GLED_Pin                         
#define MCU_EN_GLED_GPIO_Port                   
#define MCU_EN_RLED_Pin                         
#define MCU_EN_RLED_GPIO_Port                   


#define CPU_UART_RX_MCU_Pin                     CPU_RXD3_MCU_Pin
#define CPU_UART_RX_MCU_GPIO_Port               CPU_RXD3_MCU_GPIO_Port
#define CPU_UART_TX_MCU_Pin                     CPU_TXD3_MCU_Pin
#define CPU_UART_TX_MCU_GPIO_Port               CPU_TXD3_MCU_GPIO_Port

#define CPU_UART_PORT                           USART3

/* MX_GPIO_Init() -> GPIO_PIN_RESET*/
#if defined(__MCU_SWD_DEBUG_ENABLE__)
#define GPIOA_DEFAULT_PINSET					MCU_EN1V5_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN3V0_DCDC_Pin /* |MCU_EN_EDLC_Pin */
#else
#define GPIOA_DEFAULT_PINSET					MCU_EN1V5_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN3V0_DCDC_Pin|MCU_EN_EDLC_Pin
#endif
#define GPIOB_DEFAULT_PINSET					MCU_AQS1_CPU_Pin|MCU_EN0V9_1V8_DCDC_Pin|MCU_EN_CPU_Pin|MCU_AQS0_CPU_Pin|MCU_PWR_RCAM_Pin|MCU_AQS2_CPU_Pin
#define GPIOC_DEFAULT_PINSET					SYS_RESET_Pin|MCU_EN_5V_Pin

/* ADC IN Pins */
#define ADC_IN_A_PORT_PINS						(BP_ADC_MCU_Pin | TEMP_ADC_MCU_Pin | EDLC_ADC_MCU_Pin)
#define ADC_IN_B_PORT_PINS						DISABLE

/* I2C Pins */
#define I2C1_SCL_Pin							MCU_SCL1_EXTRTC_Pin
#define I2C1_SCL_GPIO_Port						MCU_SCL1_EXTRTC_GPIO_Port
#define I2C1_SCL_GPIO_AF						MCU_SCL1_EXTRTC_GPIO_AF

#define I2C1_SDA_Pin							MCU_SDA1_EXTRTC_Pin
#define I2C1_SDA_GPIO_Port						MCU_SDA1_EXTRTC_GPIO_Port
#define I2C1_SDA_GPIO_AF						MCU_SDA1_EXTRTC_GPIO_AF

#define RTC_I2C_PORT                           	I2C1

#define I2C2_SCL_Pin							MCU_SCL2_LED_Pin
#define I2C2_SCL_GPIO_Port						MCU_SCL2_LED_GPIO_Port
#define I2C2_SCL_GPIO_AF						MCU_SCL2_LED_GPIO_AF

#define I2C2_SDA_Pin							MCU_SDA2_LED_Pin
#define I2C2_SDA_GPIO_Port						MCU_SDA2_LED_GPIO_Port
#define I2C2_SDA_GPIO_AF						MCU_SDA2_LED_GPIO_AF

#define LED_I2C_PORT							I2C2

// UART4
#define UART4_RX_Pin							DISABLE
#define UART4_RX_GPIO_Port						DISABLE
#define UART4_RX_GPIO_AF_MODE					DISABLE

#define UART4_TX_Pin							DISABLE
#define UART4_TX_GPIO_Port						DISABLE
#define UART4_TX_GPIO_AF_MODE					DISABLE




#ifdef __cplusplus
}
#endif

#endif /* __EMT_IO_PRIME_H__ */
