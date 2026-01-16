/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file         stm32g0xx_hal_msp.c
	* @brief        This file provides code for the MSP Initialization
	*               and de-Initialization codes.
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

/* USER CODE BEGIN Includes */
#include "emt_io.h"
/* USER CODE END Includes */
extern DMA_HandleTypeDef h_dma_adc;

extern DMA_HandleTypeDef h_dma_uart_rx_cpu_if;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
	* Initializes the Global MSP.
	*/
void HAL_MspInit(void)
{
	/* USER CODE BEGIN MspInit 0 */

	/* USER CODE END MspInit 0 */

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* System interrupt init*/
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);

	/* USER CODE BEGIN MspInit 1 */

	/* USER CODE END MspInit 1 */
}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hadc->Instance==ADC1)
	{
		/* USER CODE BEGIN ADC1_MspInit 0 */

		/* USER CODE END ADC1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_ADC_CLK_ENABLE();
#ifdef __NEW_CONFIG_FEATURE__
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;

		if(ADC_IN_A_PORT_PINS != DISABLE)
		{
			__HAL_RCC_GPIOA_CLK_ENABLE();
			GPIO_InitStruct.Pin = ADC_IN_A_PORT_PINS;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}

		if(ADC_IN_B_PORT_PINS != DISABLE)
		{
			__HAL_RCC_GPIOB_CLK_ENABLE();
			GPIO_InitStruct.Pin = ADC_IN_B_PORT_PINS;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
#else
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __ZDR027__)\
	|| (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__)
		/**ADC1 ZDR-065,D770R GPIO Configuration
		PA4 	------>  ADC_IN4
		PA5 	------>  ADC_IN5
		PA6 	------>  ADC_IN6
		*/
		/**ADC1 ZDR027 GPIO Configuration
		PA4 	------>  ADC_IN4
		PA5 	------>  ADC_IN5
		PA7 	------>  ADC_IN7
		*/
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitStruct.Pin = BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
		/**ADC1 GPIO Configuration
		PA3     ------> ADC1_IN3
		PA5     ------> ADC1_IN5
		PA6     ------> ADC1_IN6
		PB1     ------> ADC1_IN9
		*/
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = BP24_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BP24_ADC_MCU_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
		/**ADC1 GPIO Configuration
		PA4     ------> ADC1_IN4
		PA5     ------> ADC1_IN5
		PA6     ------> ADC1_IN6
		PB1     ------> ADC1_IN9
		*/
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin = BP_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(BP_ADC_MCU_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin|V24BP_ADC_MCU_Pin;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#endif
		/* ADC1 DMA Init */
		/* ADC1 Init */
		/* DMA controller clock enable */
		__HAL_RCC_DMA1_CLK_ENABLE();

		h_dma_adc.Instance = DMA1_Channel2;
		h_dma_adc.Init.Request = DMA_REQUEST_ADC1;
		h_dma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
		h_dma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
		h_dma_adc.Init.MemInc = DMA_MINC_ENABLE;
		h_dma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		h_dma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		h_dma_adc.Init.Mode = DMA_CIRCULAR;
		h_dma_adc.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&h_dma_adc) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc,DMA_Handle,h_dma_adc);

	/* USER CODE BEGIN ADC1_MspInit 1 */
		HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 0); //adc channel3
		HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	/* USER CODE END ADC1_MspInit 1 */
	}

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hadc->Instance==ADC1)
	{
	/* USER CODE BEGIN ADC1_MspDeInit 0 */

	/* USER CODE END ADC1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_ADC_CLK_DISABLE();
#ifdef __NEW_CONFIG_FEATURE__
		if(ADC_IN_A_PORT_PINS != DISABLE)
		{
			HAL_GPIO_DeInit(GPIOA, ADC_IN_A_PORT_PINS);
		}
		if(ADC_IN_B_PORT_PINS != DISABLE)
		{
			HAL_GPIO_DeInit(GPIOB, ADC_IN_B_PORT_PINS);
		}
#else
#if (__TARGET_PROJECT__ == __ZDR027__) || (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)\
	|| (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__)
		HAL_GPIO_DeInit(GPIOA, BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin);
		//adc deinit시 port mode analog 설정 요청. HW
		GPIO_InitStruct.Pin = BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
		HAL_GPIO_DeInit(BP24_ADC_MCU_GPIO_Port, BP24_ADC_MCU_Pin);
		//adc deinit시 port mode analog 설정 요청. HW
		GPIO_InitStruct.Pin = BP24_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(BP24_ADC_MCU_GPIO_Port, &GPIO_InitStruct);

		HAL_GPIO_DeInit(GPIOA, BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin);
		//adc deinit시 port mode analog 설정 요청. HW
		GPIO_InitStruct.Pin = BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
		HAL_GPIO_DeInit(GPIOA, V24BP_ADC_MCU_Pin|TEMP_ADC_MCU_Pin|EDLC_ADC_MCU_Pin);
		HAL_GPIO_DeInit(BP_ADC_MCU_GPIO_Port, BP_ADC_MCU_Pin);
#endif
#endif
		/* ADC1 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	/* USER CODE BEGIN ADC1_MspDeInit 1 */
		HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	/* USER CODE END ADC1_MspDeInit 1 */
	}

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#if  (__TARGET_PROJECT__ == __ZDR027__)
	if(huart->Instance==USART2)
	{
		RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */

		/** Initializes the peripherals clocks
		*/
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
				Error_Handler();
		}

		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 DMA Init */
		/* DMA controller clock enable */
		__HAL_RCC_DMA1_CLK_ENABLE();

		/* USART2_RX Init */
		h_dma_uart_rx_cpu_if.Instance = DMA1_Channel1;
		h_dma_uart_rx_cpu_if.Init.Request = DMA_REQUEST_USART2_RX;
		h_dma_uart_rx_cpu_if.Init.Direction = DMA_PERIPH_TO_MEMORY;
		h_dma_uart_rx_cpu_if.Init.PeriphInc = DMA_PINC_DISABLE;
		h_dma_uart_rx_cpu_if.Init.MemInc = DMA_MINC_ENABLE;
		h_dma_uart_rx_cpu_if.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.Mode = DMA_CIRCULAR;
		h_dma_uart_rx_cpu_if.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&h_dma_uart_rx_cpu_if) != HAL_OK)
		{
		Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmarx,h_dma_uart_rx_cpu_if);

		/* USART2 interrupt Init */
		HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);

		/* DMA interrupt init */
		/* DMA1_Channel1_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}
#elif (__TARGET_PROJECT__ == __LXQ2000__)
	if(huart->Instance == USART4)
	{
		/* USER CODE BEGIN USART4_MspInit 0 */

		/* USER CODE END USART4_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART4_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART4 GPIO Configuration
		PA0     ------> USART4_TX
		PA1     ------> USART4_RX
		*/
		GPIO_InitStruct.Pin = CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART4 DMA Init */
		/* DMA controller clock enable */
		__HAL_RCC_DMA1_CLK_ENABLE();

		/* USART4_RX Init */
		h_dma_uart_rx_cpu_if.Instance = DMA1_Channel1;
		h_dma_uart_rx_cpu_if.Init.Request = DMA_REQUEST_USART4_RX;
		h_dma_uart_rx_cpu_if.Init.Direction = DMA_PERIPH_TO_MEMORY;
		h_dma_uart_rx_cpu_if.Init.PeriphInc = DMA_PINC_DISABLE;
		h_dma_uart_rx_cpu_if.Init.MemInc = DMA_MINC_ENABLE;
		h_dma_uart_rx_cpu_if.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.Mode = DMA_CIRCULAR;
		h_dma_uart_rx_cpu_if.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&h_dma_uart_rx_cpu_if) != HAL_OK)
		{
				Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmarx,h_dma_uart_rx_cpu_if);


		/* USART4 interrupt Init */
		HAL_NVIC_SetPriority(USART3_4_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(USART3_4_IRQn);

		/* DMA interrupt init */
		/* DMA1_Channel1_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0); //uart rx channel1
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	}
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__) || (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) \
	|| (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __PRIME__) || (__TARGET_PROJECT__ == __TRINITY__) \
	|| (__TARGET_PROJECT__ == __CYCLOPS__)
	if(huart->Instance==USART3)
	{
		/* USER CODE BEGIN USART3_MspInit 0 */

		/* USER CODE END USART3_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**USART3 GPIO Configuration
		PB2     ------> USART3_TX
		PB0     ------> USART3_RX
		*/
		GPIO_InitStruct.Pin = CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART3 DMA Init */
		/* DMA controller clock enable */
		__HAL_RCC_DMA1_CLK_ENABLE();

		/* USART3_RX Init */
		h_dma_uart_rx_cpu_if.Instance = DMA1_Channel1;
		h_dma_uart_rx_cpu_if.Init.Request = DMA_REQUEST_USART3_RX;
		h_dma_uart_rx_cpu_if.Init.Direction = DMA_PERIPH_TO_MEMORY;
		h_dma_uart_rx_cpu_if.Init.PeriphInc = DMA_PINC_DISABLE;
		h_dma_uart_rx_cpu_if.Init.MemInc = DMA_MINC_ENABLE;
		h_dma_uart_rx_cpu_if.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		h_dma_uart_rx_cpu_if.Init.Mode = DMA_CIRCULAR;
		h_dma_uart_rx_cpu_if.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&h_dma_uart_rx_cpu_if) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmarx,h_dma_uart_rx_cpu_if);


		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_4_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(USART3_4_IRQn);

		/* DMA interrupt init */
		/* DMA1_Channel1_IRQn interrupt configuration */
		HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0); //uart rx channel1
		HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

		/* USER CODE BEGIN USART3_MspInit 1 */

		/* USER CODE END USART3_MspInit 1 */
	}
#endif
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	//GPIO_InitTypeDef GPIO_InitStruct = {0};
#if (__TARGET_PROJECT__ == __ZDR027__)
	if(huart->Instance==USART2)
	{
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* DMA controller clock disable */
//		__HAL_RCC_DMA1_CLK_DISABLE();

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin);
		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		/* USART2 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
#elif (__TARGET_PROJECT__ == __LXQ2000__)
	if(huart->Instance==USART4)
	{
		/* USER CODE BEGIN USART4_MspDeInit 0 */
		//__HAL_RCC_DMA1_CLK_DISABLE();

		//HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);

		/* USER CODE END USART4_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART4_CLK_DISABLE();

		/**USART4 GPIO Configuration
		PA0     ------> USART4_TX
		PA1     ------> USART4_RX
		*/
		HAL_GPIO_DeInit(GPIOA, CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin);

		/* USART4 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);

		/* USART4 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART3_4_IRQn);
		HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		/* USER CODE BEGIN USART4_MspDeInit 1 */

		/* USER CODE END USART4_MspDeInit 1 */
	}
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__) || (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) \
	|| (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __PRIME__) || (__TARGET_PROJECT__ == __TRINITY__) \
	|| (__TARGET_PROJECT__ == __CYCLOPS__)
	if(huart->Instance==USART3)
	{
		/* USER CODE BEGIN USART3_MspDeInit 0 */

		/* USER CODE END USART3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration
		PB2     ------> USART3_TX
		PB0     ------> USART3_RX
		*/
		HAL_GPIO_DeInit(GPIOB, CPU_UART_RX_MCU_Pin|CPU_UART_TX_MCU_Pin);

		/* USART3 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		//    HAL_DMA_DeInit(huart->hdmatx);

		/* USART3 interrupt DeInit */
		HAL_NVIC_DisableIRQ(USART3_4_IRQn);
		HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		/* USER CODE BEGIN USART4_MspDeInit 1 */
	}
#endif
}

/* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#ifdef __NEW_CONFIG_FEATURE__
	if(hi2c->Instance == I2C1)
	{
		if((I2C1_SCL_Pin != DISABLE) && (I2C1_SDA_Pin != DISABLE))
		{
			RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

			/* Initializes the peripherals clocks  */
			PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
			PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
			{
				Error_Handler();
			}

			if((I2C1_SCL_GPIO_Port == GPIOA) || (I2C1_SDA_GPIO_Port == GPIOA))
			{
				__HAL_RCC_GPIOA_CLK_ENABLE();
			}
			if((I2C1_SCL_GPIO_Port == GPIOB) || (I2C1_SDA_GPIO_Port == GPIOB))
			{
				__HAL_RCC_GPIOB_CLK_ENABLE();
			}

			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		
			GPIO_InitStruct.Pin = I2C1_SCL_Pin;
			GPIO_InitStruct.Alternate = I2C1_SCL_GPIO_AF;
			HAL_GPIO_Init(I2C1_SCL_GPIO_Port, &GPIO_InitStruct);
	
			GPIO_InitStruct.Pin = I2C1_SDA_Pin;
			GPIO_InitStruct.Alternate = I2C1_SDA_GPIO_AF;
			HAL_GPIO_Init(I2C1_SDA_GPIO_Port, &GPIO_InitStruct);
	
			/* Peripheral clock enable */
			__HAL_RCC_I2C1_CLK_ENABLE();
		}
	}
	if(hi2c->Instance==I2C2)
	{
		if((I2C2_SCL_Pin != DISABLE) && (I2C2_SDA_Pin != DISABLE))
		{
			if((I2C2_SCL_GPIO_Port == GPIOA) || (I2C2_SDA_GPIO_Port == GPIOA))
			{
				__HAL_RCC_GPIOA_CLK_ENABLE();
			}
			if((I2C2_SCL_GPIO_Port == GPIOB) || (I2C2_SDA_GPIO_Port == GPIOB))
			{
				__HAL_RCC_GPIOB_CLK_ENABLE();
			}
		
			GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			
			GPIO_InitStruct.Pin = I2C2_SCL_Pin;
			GPIO_InitStruct.Alternate = I2C2_SCL_GPIO_AF;
			HAL_GPIO_Init(I2C2_SCL_GPIO_Port, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = I2C2_SDA_Pin;
			GPIO_InitStruct.Alternate = I2C2_SDA_GPIO_AF;
			HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);
			
			/* Peripheral clock enable */
			__HAL_RCC_I2C2_CLK_ENABLE();
		}
	}
#else
#if defined(__SUPPORT_RTC_CONTROL__)
	if(hi2c->Instance==I2C1)
	{
		/* USER CODE BEGIN I2C1_MspInit 0 */
		RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

		/* USER CODE END I2C1_MspInit 0 */
			/* Initializes the peripherals clocks  */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
		PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**I2C1 GPIO Configuration
		PA9      ------> I2C1_SCL
		PA10     ------> I2C1_SDA
		*/
		GPIO_InitStruct.Pin = MCU_SCL1_RTC_Pin|MCU_SDA1_RTC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE();
		/* USER CODE BEGIN I2C1_MspInit 1 */

		/* USER CODE END I2C1_MspInit 1 */
	}
#endif// defined(__SUPPORT_RTC_CONTROL__)

#if (__TARGET_PROJECT__ == __LXQ2000__) ||  (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	if(hi2c->Instance==I2C2)
	{
		/* USER CODE BEGIN I2C2_MspInit 0 */

		/* USER CODE END I2C2_MspInit 0 */
			__HAL_RCC_GPIOA_CLK_ENABLE();

		/**I2C2 GPIO Configuration
		PA11      ------> I2C2_SCL
		PA12      ------> I2C2_SDA
		*/
		GPIO_InitStruct.Pin = MCU_SCL2_GSEN_Pin|MCU_SDA2_GSEN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF6_I2C2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C2_CLK_ENABLE();
		/* USER CODE BEGIN I2C2_MspInit 1 */

		/* EXTI interrupt init*/
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
		/* USER CODE END I2C2_MspInit 1 */
	}
	#endif//(__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
#endif
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
#ifdef __NEW_CONFIG_FEATURE__
	if(hi2c->Instance==I2C1)
	{
		if((I2C1_SCL_Pin != DISABLE) && (I2C1_SDA_Pin != DISABLE))
		{
			/* Peripheral clock disable */
			__HAL_RCC_I2C1_CLK_DISABLE();

			/**I2C1 GPIO Configuration
			PA9     ------> I2C1_SCL
			PA10     ------> I2C1_SDA
			*/
			HAL_GPIO_DeInit(I2C1_SCL_GPIO_Port, I2C1_SCL_Pin);

			HAL_GPIO_DeInit(I2C1_SDA_GPIO_Port, I2C1_SDA_Pin);
		}
	} 
	else if(hi2c->Instance==I2C2)
	{
		if((I2C2_SCL_Pin != DISABLE) && (I2C2_SDA_Pin != DISABLE))
		{
			/* Peripheral clock disable */
			__HAL_RCC_I2C2_CLK_DISABLE();

			/**I2C2 GPIO Configuration
			PA11 [PA9]     ------> I2C2_SCL
			PA12 [PA10]     ------> I2C2_SDA
			*/
			HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);

			HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);
		}
	}
#else
#if defined(__SUPPORT_RTC_CONTROL__)
	if(hi2c->Instance==I2C1)
	{
		/* USER CODE BEGIN I2C1_MspDeInit 0 */

		/* USER CODE END I2C1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C1_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PA9     ------> I2C1_SCL
		PA10     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(GPIOA, MCU_SCL1_RTC_Pin);

		HAL_GPIO_DeInit(GPIOA, MCU_SDA1_RTC_Pin);

		/* USER CODE BEGIN I2C1_MspDeInit 1 */

		/* USER CODE END I2C1_MspDeInit 1 */
	} 
	 //else 
#endif /*defined(__SUPPORT_RTC_CONTROL__)*/

#if (__TARGET_PROJECT__ == __LXQ2000__) ||  (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	if(hi2c->Instance==I2C2)
	{
		/* USER CODE BEGIN I2C2_MspDeInit 0 */
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		/* USER CODE END I2C2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_I2C2_CLK_DISABLE();

		/**I2C2 GPIO Configuration
		PA11 [PA9]     ------> I2C2_SCL
		PA12 [PA10]     ------> I2C2_SDA
		*/
		HAL_GPIO_DeInit(MCU_SCL2_GSEN_GPIO_Port, MCU_SCL2_GSEN_Pin);

		HAL_GPIO_DeInit(MCU_SDA2_GSEN_GPIO_Port, MCU_SDA2_GSEN_Pin);

		/* USER CODE BEGIN I2C2_MspDeInit 1 */

		/* USER CODE END I2C2_MspDeInit 1 */
	}
#endif
#endif
}

/* USER CODE BEGIN 1 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM6)
	{
		/* USER CODE BEGIN TIM6_MspInit 0 */

		/* USER CODE END TIM6_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();
		/* USER CODE BEGIN TIM6_MspInit 1 */

		/* USER CODE END TIM6_MspInit 1 */
	}
	else if(htim_base->Instance==TIM7)
	{
		/* USER CODE BEGIN TIM7_MspInit 0 */

		/* USER CODE END TIM7_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM7_CLK_ENABLE();
		/* TIM7 interrupt Init */
		HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
		/* USER CODE BEGIN TIM7_MspInit 1 */

		/* USER CODE END TIM7_MspInit 1 */
	}
	else if(htim_base->Instance==TIM14)
	{
		/* USER CODE BEGIN TIM14_MspInit 0 */

		/* USER CODE END TIM14_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM14_CLK_ENABLE();
		/* USER CODE BEGIN TIM14_MspInit 1 */

		/* USER CODE END TIM14_MspInit 1 */
	}

}


/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM6)
	{
		/* USER CODE BEGIN TIM6_MspDeInit 0 */

		/* USER CODE END TIM6_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_TIM6_CLK_DISABLE();
		/* USER CODE BEGIN TIM6_MspDeInit 1 */

		/* USER CODE END TIM6_MspDeInit 1 */
	}
	else if(htim_base->Instance==TIM7)
	{
		/* USER CODE BEGIN TIM7_MspDeInit 0 */

		/* USER CODE END TIM7_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_TIM7_CLK_DISABLE();

		/* TIM7 interrupt DeInit */
		HAL_NVIC_DisableIRQ(TIM7_IRQn);
		/* USER CODE BEGIN TIM7_MspDeInit 1 */

		/* USER CODE END TIM7_MspDeInit 1 */
	}
	else if(htim_base->Instance==TIM14)
	{
		/* USER CODE BEGIN TIM14_MspDeInit 0 */

		/* USER CODE END TIM14_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_TIM14_CLK_DISABLE();
		/* USER CODE BEGIN TIM14_MspDeInit 1 */

		/* USER CODE END TIM14_MspDeInit 1 */
	}

}
/* USER CODE END 1 */
