/**
	******************************************************************************
	* @file           : emt_io_zdr065.h
	* @brief          :
	******************************************************************************
	* Created on      : 17 Jan, 2024
	*      Author     : hkkim
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_IO_ZDR065_H__
#define __EMT_IO_ZDR065_H__

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
#define MCU_EN_GLED_Pin                         GPIO_PIN_0
#define MCU_EN_GLED_GPIO_Port                   GPIOA
#define MCU_EN_RLED_Pin                         GPIO_PIN_1
#define MCU_EN_RLED_GPIO_Port                   GPIOA

#define GSEN_INT_MCU_Pin                       	GPIO_PIN_2
#define GSEN_INT_MCU_GPIO_Port                  GPIOA
// #define USB_DET_MCU_Pin                         GPIO_PIN_3
// #define USB_DET_MCU_GPIO_Port                   GPIOA
#define BP_ADC_MCU_Pin                          GPIO_PIN_4
#define BP_ADC_MCU_GPIO_Port                    GPIOA
#define TEMP_ADC_MCU_Pin                        GPIO_PIN_5
#define TEMP_ADC_MCU_GPIO_Port                  GPIOA
#define EDLC_ADC_MCU_Pin                        GPIO_PIN_6
#define EDLC_ADC_MCU_GPIO_Port                  GPIOA
#define __SW_EASY_MCU_Pin                    	GPIO_PIN_7
#define __SW_EASY_MCU_GPIO_Port              	GPIOA
#define CPU_TXD3_MCU_Pin                      	GPIO_PIN_0
#define CPU_TXD3_MCU_GPIO_Port                	GPIOB
#define PMIC_PG_MCU_Pin							GPIO_PIN_1	/*PG_STA_MCU_Pin*/
#define PMIC_PG_MCU_GPIO_Port					GPIOB		/*PG_STA_MCU_GPIO_Port*/

#define CPU_RXD3_MCU_Pin                      	GPIO_PIN_2
#define CPU_RXD3_MCU_GPIO_Port                	GPIOB
#define MCU_EN_PMIC_Pin                  	   	GPIO_PIN_8
#define MCU_EN_PMIC_GPIO_Port           	    GPIOA
#define MCU_SCL1_RTC_Pin                       	GPIO_PIN_9
#define MCU_SCL1_RTC_GPIO_Port                 	GPIOA
#define MCU_SCL1_RTC_GPIO_AF                	GPIO_AF6_I2C1

#define MCU_EN_5V_Pin                       	GPIO_PIN_6
#define MCU_EN_5V_GPIO_Port                 	GPIOC
#define MCU_SDA1_RTC_Pin                       	GPIO_PIN_10
#define MCU_SDA1_RTC_GPIO_Port                 	GPIOA
#define MCU_SDA1_RTC_GPIO_AF                	GPIO_AF6_I2C1

#define MCU_PWR_RCAM_Pin                       	GPIO_PIN_11
#define MCU_PWR_RCAM_GPIO_Port                 	GPIOA
#define MCU_EN_MCUPWR_Pin                       GPIO_PIN_12
#define MCU_EN_MCUPWR_GPIO_Port               	GPIOA

#define MCU_EN_EDLC_Pin                  	    GPIO_PIN_15
#define MCU_EN_EDLC_GPIO_Port	                GPIOA
#define __ACC_DET_MCU_Pin                       GPIO_PIN_3
#define __ACC_DET_MCU_GPIO_Port                 GPIOB
#define MCU_AQS3_CPU_Pin                        GPIO_PIN_4
#define MCU_AQS3_CPU_GPIO_Port                  GPIOB
#define MCU_AQS0_CPU_Pin                        GPIO_PIN_5
#define MCU_AQS0_CPU_GPIO_Port                  GPIOB
#define MCU_EN_CPU_Pin	                     	GPIO_PIN_6
#define MCU_EN_CPU_GPIO_Port	               	GPIOB
#define MCU_AQS2_CPU_Pin                        GPIO_PIN_8
#define MCU_AQS2_CPU_GPIO_Port                  GPIOB

#define CPU_UART_RX_MCU_Pin                     CPU_RXD3_MCU_Pin
#define CPU_UART_RX_MCU_GPIO_Port               CPU_RXD3_MCU_GPIO_Port
#define CPU_UART_TX_MCU_Pin                     CPU_TXD3_MCU_Pin
#define CPU_UART_TX_MCU_GPIO_Port               CPU_TXD3_MCU_GPIO_Port

/*AQS 3번 핀을 USB DETECT CHECK 용으로 사용하므로 Define 변경.*/
#define USB_DET_MCU_Pin                         MCU_AQS3_CPU_Pin
#define USB_DET_MCU_GPIO_Port                   MCU_AQS3_CPU_GPIO_Port

#define GSEN_INT_MCU_EXTI_IRQn					EXTI2_3_IRQn
#define __ACC_DET_MCU_EXTI_IRQn					EXTI2_3_IRQn
#define __SW_EASY_MCU_EXTI_IRQn					EXTI4_15_IRQn

#define CPU_UART_PORT                           USART3

/* MX_GPIO_Init() -> GPIO_PIN_RESET*/
#if (__EMT_DEVELOP_STAGE__ == __WS__)
#define	GPIOA_DEFAULT_PINSET					USB_DET_MCU_Pin|MCU_EN_PMIC_Pin|MCU_PWR_RCAM_Pin|MCU_EN_MCUPWR_Pin|MCU_EN_EDLC_Pin
#else
#define	GPIOA_DEFAULT_PINSET					MCU_EN_PMIC_Pin|MCU_PWR_RCAM_Pin|MCU_EN_MCUPWR_Pin|MCU_EN_EDLC_Pin
#endif
#define GPIOB_DEFAULT_PINSET					MCU_AQS0_CPU_Pin|MCU_AQS1_CPU_Pin|MCU_AQS2_CPU_Pin|MCU_EN_CPU_Pin
#define GPIOC_DEFAULT_PINSET					MCU_EN_5V_Pin|SYS_RESET_Pin

/* ADC IN Pins */
#define ADC_IN_A_PORT_PINS						(BP_ADC_MCU_Pin | TEMP_ADC_MCU_Pin | EDLC_ADC_MCU_Pin)
#define ADC_IN_B_PORT_PINS						DISABLE

/* I2C Pins */
#define I2C1_SCL_Pin							MCU_SCL1_RTC_Pin
#define I2C1_SCL_GPIO_Port						MCU_SCL1_RTC_GPIO_Port
#define I2C1_SCL_GPIO_AF						MCU_SCL1_RTC_GPIO_AF

#define I2C1_SDA_Pin							MCU_SDA1_RTC_Pin
#define I2C1_SDA_GPIO_Port						MCU_SDA1_RTC_GPIO_Port
#define I2C1_SDA_GPIO_AF						MCU_SDA1_RTC_GPIO_AF

#define RTC_I2C_PORT                           	I2C1

#define I2C2_SCL_Pin							DISABLE
#define I2C2_SCL_GPIO_Port						DISABLE
#define I2C2_SCL_GPIO_AF						DISABLE

#define I2C2_SDA_Pin							DISABLE
#define I2C2_SDA_GPIO_Port						DISABLE
#define I2C2_SDA_GPIO_AF						DISABLE

#ifdef __cplusplus
}
#endif

#endif /* __EMT_IO_ZDR065_H__ */
