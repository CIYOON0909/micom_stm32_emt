/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "emt_app.h"
#include "emt_io.h"
#if defined(__MICOM_BOOTLOADER__)
#include "emt_app_bootloader.h"
#else
#include "emt_rtos.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
#if defined(__MICOM_BOOTLOADER__)
#else
static void MX_GPIO_Init(void);
#endif

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/*
	 * Issue
	 * - No ACC change.
	 * Details
	 * - MCU_ACC_Detect_Pin(PA8) Voltage 3V -> 1.2V after the CPU POWER ON sequence(PA9, PA10, PB0) is executed in the ACC OFF state.
	 * - The voltage of MCU_ACC_Detect_Pin(PA8) is low, so it is read as LOW.
	 * Datasheet solution
	 * - upon reset, a pull-down resistor might be present on PA8, PD0, or PD2, depending on the voltage level on PB0, PA9, PC6, PA10, PD1, and PD3.
	 * - In order to disable this resistor, strobe theUCPDx_STROBE bit of the SYSCFG_CFGR1 register during start-up sequence.
	*/
	HAL_SYSCFG_StrobeDBattpinsConfig(SYSCFG_UCPD1_STROBE);

	/* Configure the system clock */
	SystemClock_Config(); //SystemClock_Config_LowPower();//SystemClock_Config();

#if defined(__MICOM_BOOTLOADER__)
	emt_app_process();
#else
	MX_GPIO_Init();
	emt_rtos_init();
#endif

	return 0;
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
#if defined(__MICOM_BOOTLOADER__)
#else
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	/************************* GPIO GROUP B INIT *************************/
	HAL_GPIO_WritePin(GPIOB, GPIOB_DEFAULT_PINSET, GPIO_PIN_RESET);

	/************************* GPIO GROUP C INIT *************************/
	HAL_GPIO_WritePin(GPIOC, GPIOC_DEFAULT_PINSET, GPIO_PIN_RESET);

	/************************* GPIO GROUP A INIT *************************/
	HAL_GPIO_WritePin(GPIOA, GPIOA_DEFAULT_PINSET, GPIO_PIN_RESET);

	/************************* MCU PWR PIN INIT *************************/
	GPIO_InitStruct.Pin = MCU_EN_MCUPWR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_DeInit(MCU_EN_MCUPWR_GPIO_Port, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(MCU_EN_MCUPWR_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(MCU_EN_MCUPWR_GPIO_Port, MCU_EN_MCUPWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_DeInit(MCU_EN_MCUPWR_GPIO_Port, GPIO_InitStruct.Pin);

	/************************* ACC PIN INIT *************************/
	GPIO_InitStruct.Pin = __ACC_DET_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(__ACC_DET_MCU_GPIO_Port, &GPIO_InitStruct);

	/************************* ETC PIN INIT *****************************/
#if (__TARGET_PROJECT__ == __LXQ2000__)
	/*Configure GPIO pin : BP_DET_MCU_Pin */
//	GPIO_InitStruct.Pin = __BP_DET_MCU_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(__BP_DET_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOA

	/*Configure GPIO pin : ACC_DET_MCU_Pin */
//	GPIO_InitStruct.Pin = __ACC_DET_MCU_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(__ACC_DET_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOA

	/*Configure GPIO pin : USB_DET_MCU_Pin */
	GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOB

#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	HAL_GPIO_WritePin(GPIOB, USB_DET_MCU_Pin|__ACC_DET_MCU_Pin, GPIO_PIN_RESET);
//	GPIO_InitStruct.Pin = __ACC_DET_MCU_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(__ACC_DET_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOB

	GPIO_InitStruct.Pin = MCU_S_SWITCH_DET_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(MCU_S_SWITCH_DET_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_DET_MCU_Pin */
	GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOB

	GPIO_InitStruct.Pin = MCU_CTRL_LED_PWM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(MCU_CTRL_LED_PWM_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin, GPIO_PIN_RESET); //GPIOA
	HAL_GPIO_DeInit(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin);

#elif (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
	/*Configure GPIO pin : USB_DET_MCU_Pin */
	GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : GSEN_INT_MCU_Pin */
	GPIO_InitStruct.Pin = GSEN_INT_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __PRIME__) || (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	/*Configure GPIO pin : USB_DET_MCU_Pin */
		GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct);

		/*Configure GPIO pin : GSEN_INT_MCU_Pin */
		GPIO_InitStruct.Pin = GSEN_INT_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct);

		/*Configure GPIO pin : __BP_DET_MCU_Pin */
		GPIO_InitStruct.Pin = __BP_DET_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(__BP_DET_MCU_GPIO_Port, &GPIO_InitStruct);

#elif (__TARGET_PROJECT__ == __ZDR027__)
	/*Configure GPIO pin : USB_DET_MCU_Pin */
	GPIO_InitStruct.Pin = __USB_DET_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(__USB_DET_MCU_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : GSEN_INT_MCU_Pin */
	GPIO_InitStruct.Pin = GSEN_INT_MCU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct);
#endif
}
#endif
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

void reset_mcu(void)
{
	// mcu reset
	HAL_NVIC_SystemReset();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
#if (__REBOOT_PROC_FLAG__)
	reset_mcu();
	HAL_Delay(100);
#endif
	while (1);
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
