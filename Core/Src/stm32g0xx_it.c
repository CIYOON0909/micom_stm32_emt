/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file    stm32g0xx_it.c
	* @brief   Interrupt Service Routines.
	******************************************************************************
	* @attention
	*
	* Copyright (c) 2023 STMicroelectronics.
	* All rights reserved.
	*
	* This software is licensed under terms that can be found in the LICENSE file
	* in the root directory of this software component.
	* If no LICENSE file comes with this software, it is provided AS-IS.
	*
	******************************************************************************
	*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "emt_io.h"
#include "emt_led.h"
#include "emt_gsensor.h"
#include "emt_app_main.h"
#include "emt_key.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef    h_dma_adc;
extern DMA_HandleTypeDef    h_dma_uart_rx_cpu_if;
extern UART_HandleTypeDef   h_uart_cpu_if_port;
extern TIM_HandleTypeDef    h_led_control_period_timer;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
	* @brief This function handles Non maskable interrupt.
	*/
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1)
	{
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
	* @brief This function handles Hard fault interrupt.
	*/
void HardFault_Handler(void)
{
#if (__REBOOT_PROC_FLAG__)
	reset_mcu();
	HAL_Delay(100);
	while(1);
#else
	/* USER CODE BEGIN HardFault_IRQn 0 */
	/* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
#endif
}

/**
	* @brief This function handles System tick timer.
	*/
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
#if defined(__MICOM_BOOTLOADER__)
#else
	#if (INCLUDE_xTaskGetSchedulerState == 1 )
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	#endif /* INCLUDE_xTaskGetSchedulerState */
	{
		xPortSysTickHandler();
	}
#endif
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
	* @brief This function handles EXTI line 2 to 3 interrupts.
	*/
void EXTI0_1_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI0_1_IRQn 0 */

	/* USER CODE END EXTI0_1_IRQn 0 */
}

/**
	* @brief This function handles EXTI line 2 to 3 interrupts.
	*/
void EXTI2_3_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI2_3_IRQn 0 */
#if (__TARGET_PROJECT__ == __LXQ2000_CR3__) || (__TARGET_PROJECT__ == __ZDR065__)
	HAL_GPIO_EXTI_IRQHandler(GSEN_INT_MCU_Pin|__ACC_DET_MCU_Pin);
#elif (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	HAL_GPIO_EXTI_IRQHandler(GSEN_INT_MCU_Pin);
#endif
	/* USER CODE END EXTI2_3_IRQn 0 */
}

/**
	* @brief This function handles EXTI line 4 to 15 interrupts.
	*/
void EXTI4_15_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI4_15_IRQn 0 */
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
	HAL_GPIO_EXTI_IRQHandler(__SW_EASY_MCU_Pin);
#elif (__TARGET_PROJECT__ == __ZDR027__)
	HAL_GPIO_EXTI_IRQHandler(__SW_EASY_MCU_Pin|GSEN_INT_MCU_Pin);
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	HAL_GPIO_EXTI_IRQHandler(GSEN_INT_MCU_Pin|MCU_S_SWITCH_DET_Pin);
#elif (__TARGET_PROJECT__ == __LXQ2000__)
	HAL_GPIO_EXTI_IRQHandler(GSEN_INT_MCU_Pin|__ACC_DET_MCU_Pin);
#elif (__TARGET_PROJECT__ == __PRIME__)
	HAL_GPIO_EXTI_IRQHandler(__SW_EASY_MCU_Pin|__SW_FNKEY_MCU_Pin);
#endif
	/* USER CODE END EXTI4_15_IRQn 0 */
}

/**
	* @brief This function handles DMA1 channel 1 interrupt.
	*/
void DMA1_Channel1_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	HAL_DMA_IRQHandler(&h_dma_uart_rx_cpu_if);
	/* USER CODE END DMA1_Channel1_IRQn 0 */
}

/**
	* @brief This function handles DMA1 channel 2 and channel 3 interrupts.
	*/
void DMA1_Channel2_3_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
#if defined(__MICOM_BOOTLOADER__)
#else
	HAL_DMA_IRQHandler(&h_dma_adc);
#endif
	/* USER CODE END DMA1_Channel2_3_IRQn 0 */
}

void USART2_IRQHandler(void)
{
#if (__TARGET_PROJECT__ == __ZDR027__)
	HAL_UART_IRQHandler(&h_uart_cpu_if_port);
#endif
}


void USART3_4_IRQHandler(void)
{
#if (__TARGET_PROJECT__ == __ZDR027__)
#else
	HAL_UART_IRQHandler(&h_uart_cpu_if_port);
#endif
}

/* USER CODE BEGIN 1 */

void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&h_led_control_period_timer);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
#if defined(__MICOM_BOOTLOADER__)
#else
	#if (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	if(__HAL_GPIO_EXTI_GET_RISING_IT(MCU_S_SWITCH_DET_Pin))
	{
		emt_bp_off_isr_handler();
	}
	#endif
#endif
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
#if defined(__MICOM_BOOTLOADER__)
#else
	if(__HAL_GPIO_EXTI_GET_FALLING_IT(GSEN_INT_MCU_Pin))
	{
	#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
		emt_gsensor_rx_isr_handler();
	#endif

	#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_INTERRUPT_MODE__)
		emt_gsensor_wakeup_isr_handler();
	#endif
	}

	#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __ZDR027__)
	if(__HAL_GPIO_EXTI_GET_FALLING_IT(__SW_EASY_MCU_Pin))
	{
		emt_key_isr_handler(__SW_EASY_MCU_Pin);
	}
	#elif (__TARGET_PROJECT__ == __PRIME__)
	if(__HAL_GPIO_EXTI_GET_FALLING_IT(__SW_EASY_MCU_Pin|__SW_FNKEY_MCU_Pin))
	{
		emt_key_isr_handler(__HAL_GPIO_EXTI_GET_FALLING_IT(__SW_EASY_MCU_Pin|__SW_FNKEY_MCU_Pin));
	}
	#endif
#endif
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if defined(__MICOM_BOOTLOADER__)
#else
	if(htim->Instance == TIM7)
	{
		emt_led_control_period_tmr_callback();
	}
#endif
}
/* USER CODE END 1 */
