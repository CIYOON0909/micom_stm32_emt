/**
	******************************************************************************
	* @file           : emt_io_lxq2000.h
	* @brief          :
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_IO_LXQ2000_H__
#define __EMT_IO_LXQ2000_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"


#define MCU_EN0V9_DCDC_Pin                      GPIO_PIN_9
#define MCU_EN0V9_DCDC_GPIO_Port                GPIOB
#define SYS_RESET_Pin                           GPIO_PIN_14
#define SYS_RESET_GPIO_Port                     GPIOC
#define __DC2DC_EN_Pin                          GPIO_PIN_15
#define __DC2DC_EN_GPIO_Port                    GPIOC
#define CPU_RXD3_MCU_Pin                        GPIO_PIN_0
#define CPU_RXD3_MCU_GPIO_Port                  GPIOA
#define CPU_TXD3_MCU_Pin                        GPIO_PIN_1
#define CPU_TXD3_MCU_GPIO_Port                  GPIOA
#define MCU_EN_EDLC_Pin                         GPIO_PIN_2
#define MCU_EN_EDLC_GPIO_Port                   GPIOA

#define MCU_PWR_RCAM_Pin                        GPIO_PIN_3
#define MCU_PWR_RCAM_GPIO_Port                  GPIOA
#define V24BP_ADC_MCU_Pin                       GPIO_PIN_4
#define V24BP_ADC_MCU_GPIO_Port                 GPIOA
#define TEMP_ADC_MCU_Pin                        GPIO_PIN_5
#define TEMP_ADC_MCU_GPIO_Port                  GPIOA
#define EDLC_ADC_MCU_Pin                        GPIO_PIN_6
#define EDLC_ADC_MCU_GPIO_Port                  GPIOA
#define MCU_CTRL_LED_PWM_Pin                    GPIO_PIN_7
#define MCU_CTRL_LED_PWM_GPIO_Port              GPIOA
#define MCU_EN3V0_DCDC_Pin                      GPIO_PIN_0
#define MCU_EN3V0_DCDC_GPIO_Port                GPIOB
#define BP_ADC_MCU_Pin                          GPIO_PIN_1
#define BP_ADC_MCU_GPIO_Port                    GPIOB
#define MCU_EN_MCUPWR_Pin                       GPIO_PIN_2
#define MCU_EN_MCUPWR_GPIO_Port                 GPIOB
#define __BP_DET_MCU_Pin                        GPIO_PIN_8
#define __BP_DET_MCU_GPIO_Port                  GPIOA
#define BP_DET_MCU_EXTI_IRQn                    EXTI4_15_IRQn
#define MCU_AQS1_CPU_Pin                        GPIO_PIN_9
#define MCU_AQS1_CPU_GPIO_Port                  GPIOA
#define MCU_AQS0_CPU_Pin                        GPIO_PIN_6
#define MCU_AQS0_CPU_GPIO_Port                  GPIOC
#define __ACC_DET_MCU_Pin                       GPIO_PIN_10
#define __ACC_DET_MCU_GPIO_Port                 GPIOA
#define __ACC_DET_MCU_EXTI_IRQn                 EXTI4_15_IRQn
#define MCU_SCL2_GSEN_Pin                       GPIO_PIN_11
#define MCU_SCL2_GSEN_GPIO_Port                 GPIOA
#define MCU_SCL2_GSEN_GPIO_AF                 	GPIO_AF6_I2C2
#define MCU_SDA2_GSEN_Pin                       GPIO_PIN_12
#define MCU_SDA2_GSEN_GPIO_Port                 GPIOA
#define MCU_SDA2_GSEN_GPIO_AF                 	GPIO_AF6_I2C2
#define MCU_EN_LEDDN_Pin                        GPIO_PIN_13
#define MCU_EN_LEDDN_GPIO_Port                  GPIOA
#define MCU_EN_LEDUP_Pin                        GPIO_PIN_14
#define MCU_EN_LEDUP_GPIO_Port                  GPIOA
#define GSEN_INT_MCU_Pin                        GPIO_PIN_15
#define GSEN_INT_MCU_GPIO_Port                  GPIOA
#define GSEN_INT_MCU_EXTI_IRQn                  EXTI4_15_IRQn
#define MCU_EN1V5_DCDC_Pin                      GPIO_PIN_3
#define MCU_EN1V5_DCDC_GPIO_Port                GPIOB
#define MCU_EN1V8_DCDC_Pin                      GPIO_PIN_4
#define MCU_EN1V8_DCDC_GPIO_Port                GPIOB
#define MCU_PWR_5V0_Pin                         GPIO_PIN_5
#define MCU_PWR_5V0_GPIO_Port                   GPIOB
#define MCU_EN_CPU_Pin                          GPIO_PIN_6
#define MCU_EN_CPU_GPIO_Port                    GPIOB
#define USB_DET_MCU_Pin                         GPIO_PIN_7
#define USB_DET_MCU_GPIO_Port                   GPIOB
#define USB_DET_MCU_EXTI_IRQn                   EXTI4_15_IRQn
#define MCU_AQS2_CPU_Pin                        GPIO_PIN_8
#define MCU_AQS2_CPU_GPIO_Port                  GPIOB


#define __SW_EASY_MCU_Pin                       GPIO_PIN_9
#define __SW_EASY_MCU_GPIO_Port                 GPIOA
#define __SW_EASY_MCU_EXTI_IRQn                 EXTI4_15_IRQn

#define CPU_UART_RX_MCU_Pin                     CPU_RXD3_MCU_Pin
#define CPU_UART_RX_MCU_GPIO_Port               CPU_RXD3_MCU_GPIO_Port
#define CPU_UART_TX_MCU_Pin                     CPU_TXD3_MCU_Pin
#define CPU_UART_TX_MCU_GPIO_Port               CPU_TXD3_MCU_GPIO_Port

#define CPU_UART_PORT                           USART4


/* MX_GPIO_Init() -> GPIO_PIN_RESET*/
#define GPIOA_DEFAULT_PINSET					MCU_EN_EDLC_Pin|__ACC_DET_MCU_Pin|MCU_CTRL_LED_PWM_Pin|MCU_AQS1_CPU_Pin
#define GPIOB_DEFAULT_PINSET					USB_DET_MCU_Pin|MCU_EN0V9_DCDC_Pin|MCU_EN3V0_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN1V5_DCDC_Pin|MCU_EN1V8_DCDC_Pin|MCU_PWR_5V0_Pin|MCU_EN_CPU_Pin
#define GPIOC_DEFAULT_PINSET					__DC2DC_EN_Pin|SYS_RESET_Pin|MCU_AQS0_CPU_Pin

/* ADC IN Pins */
#define ADC_IN_A_PORT_PINS						(TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin|V24BP_ADC_MCU_Pin)
#define ADC_IN_B_PORT_PINS						BP_ADC_MCU_Pin

/* I2C Pins */
#define I2C1_SCL_Pin							DISABLE
#define I2C1_SCL_GPIO_Port						DISABLE
#define I2C1_SCL_GPIO_AF						DISABLE

#define I2C1_SDA_Pin							DISABLE
#define I2C1_SDA_GPIO_Port						DISABLE
#define I2C1_SDA_GPIO_AF						DISABLE

#define I2C2_SCL_Pin							MCU_SCL2_GSEN_Pin
#define I2C2_SCL_GPIO_Port						MCU_SCL2_GSEN_GPIO_Port
#define I2C2_SCL_GPIO_AF						MCU_SCL2_GSEN_GPIO_AF

#define I2C2_SDA_Pin							MCU_SDA2_GSEN_Pin
#define I2C2_SDA_GPIO_Port						MCU_SDA2_GSEN_GPIO_Port
#define I2C2_SDA_GPIO_AF						MCU_SDA2_GSEN_GPIO_AF

#define GSEN_I2C_PORT                           I2C2

#ifdef __cplusplus
}
#endif

#endif /* __EMT_IO_LXQ2000_H__ */
