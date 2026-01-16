/**
  ******************************************************************************
  * @file           : emt_uart_rx.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "string.h"
#include "stdio.h"
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os.h"
#endif
#include "main.h"

#include "stm32g0xx_hal.h"
#include "emt_types.h"
#include "emt_io.h"
#include "emt_app.h"
#include "emt_uart_rx.h"
#include "emt_hci.h"


/* Private macro -------------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#define UART_TX_LOCK
#define UART_TX_UNLOCK
#define UART_RX_SEM_RELEASE
#else
#define UART_TX_LOCK                osMutexAcquire(uartTxMutexHandle, osWaitForever);
#define UART_TX_UNLOCK              osMutexRelease(uartTxMutexHandle);
#define UART_RX_SEM_RELEASE         osSemaphoreRelease(uartRxSemHandle);
extern osMutexId_t                  uartTxMutexHandle;
extern osSemaphoreId_t              uartRxSemHandle;
#endif



/* Private variables ---------------------------------------------------------*/
#define     CPU_CMD_SIZE                        MSG_DATA_MAX_LEN
#define     DMA_BUFF_SIZE                       (1024+128)

static bool         g_emt_uart_initialize = false;

static uint8_t   	g_uart_rx_dma_buffer[DMA_BUFF_SIZE] = {0};


/* External variables ----------------------------------------------------------*/
DMA_HandleTypeDef   h_dma_uart_rx_cpu_if;
UART_HandleTypeDef  h_uart_cpu_if_port;



/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	UART_RX_SEM_RELEASE
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UART_RX_SEM_RELEASE
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	HAL_UARTEx_ReceiveToIdle_DMA(huart, g_uart_rx_dma_buffer, DMA_BUFF_SIZE);
}

/**
* @brief Initialize uart rx
* @param argument: None
* @retval None
*/
void emt_uart_init(void)
{
	if(g_emt_uart_initialize == false)
	{
		h_uart_cpu_if_port.Instance = CPU_UART_PORT;
		h_uart_cpu_if_port.Init.BaudRate = 115200;
		h_uart_cpu_if_port.Init.WordLength = UART_WORDLENGTH_8B;
		h_uart_cpu_if_port.Init.StopBits = UART_STOPBITS_1;
		h_uart_cpu_if_port.Init.Parity = UART_PARITY_NONE;
		h_uart_cpu_if_port.Init.Mode = UART_MODE_TX_RX;
		h_uart_cpu_if_port.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		h_uart_cpu_if_port.Init.OverSampling = UART_OVERSAMPLING_16;
		h_uart_cpu_if_port.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		h_uart_cpu_if_port.Init.ClockPrescaler = UART_PRESCALER_DIV1;
		h_uart_cpu_if_port.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		if (HAL_UART_Init(&h_uart_cpu_if_port) != HAL_OK)
		{
			Error_Handler();
		}

		/* Make sure that no UART transfer is on-going */
		while (__HAL_UART_GET_FLAG(&h_uart_cpu_if_port, USART_ISR_BUSY) == SET);

		/* Make sure that UART is ready to receive)   */
		while (__HAL_UART_GET_FLAG(&h_uart_cpu_if_port, USART_ISR_REACK) == RESET);

		__HAL_UART_ENABLE_IT(&h_uart_cpu_if_port, USART_ISR_IDLE);
		memset(g_uart_rx_dma_buffer, 0x00, DMA_BUFF_SIZE);
		HAL_UARTEx_ReceiveToIdle_DMA(&h_uart_cpu_if_port, g_uart_rx_dma_buffer, DMA_BUFF_SIZE);
		g_emt_uart_initialize = true;
		emt_hci_init(emt_uart_send);
	}
}


void emt_uart_deinit(void)
{
	if(g_emt_uart_initialize == true)
	{
		__HAL_UART_DISABLE_IT(&h_uart_cpu_if_port, USART_ISR_IDLE);

		if (HAL_UART_DeInit(&h_uart_cpu_if_port) != HAL_OK)
		{
			Error_Handler();
		}
		g_emt_uart_initialize = false;
	}
}


/**
* @brief Send Uart Data to CPU
* @param argument: None
* @retval None
*/
int emt_uart_send(uint8_t *p_data, uint16_t size)
{
	HAL_StatusTypeDef status = HAL_OK;
	if(g_emt_uart_initialize == true)
	{
		UART_TX_LOCK
		status = HAL_UART_Transmit(&h_uart_cpu_if_port, p_data, size, 500);
		UART_TX_UNLOCK
	}

	if( status == HAL_OK )
	{
		return EMT_SUCCESS;
	}
	return EMT_FAILURE;
}


/**
* @brief Check for new data received with DMA
* @param argument: None
* @retval None
*/
void emt_uart_rx_check(void)
{
	static size_t old_pos = 0;
	size_t pos = 0;

	/* Calculate current position in buffer */
	//    pos = ARRAY_LEN(usart_rx_dma_buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_1);
	pos = ARRAY_LEN(g_uart_rx_dma_buffer) - __HAL_DMA_GET_COUNTER(&h_dma_uart_rx_cpu_if);
	if (pos != old_pos)
	{	/* Check change in received data */
		if (pos > old_pos)
		{	/* Current position is over previous one */
			/* We are in "linear" mode */
			/* Process data directly by subtracting "pointers" */
			emt_hci_rx_process_data(&g_uart_rx_dma_buffer[old_pos], pos - old_pos);
		}
		else
		{
			/* We are in "overflow" mode */
			/* First process data to the end of buffer */
			emt_hci_rx_process_data(&g_uart_rx_dma_buffer[old_pos], ARRAY_LEN(g_uart_rx_dma_buffer) - old_pos);

			/* Check and continue with beginning of buffer */
			if (pos > 0)
			{
				emt_hci_rx_process_data(&g_uart_rx_dma_buffer[0], pos);
			}
		}
		old_pos = pos;                          /* Save current position as old */
	}
}


/**
* @brief Function implementing the uartRx thread.
* @param argument: Not used
* @retval None
*/
#if defined(__MICOM_BOOTLOADER__)
#else
void emt_uart_rx_proc_task(void *argument)
{
	/* USER CODE BEGIN uart_rx_proc_task */
	osStatus_t status = osOK;


	/* Infinite loop */
	for(;;)
	{
		status = osSemaphoreAcquire(uartRxSemHandle, osWaitForever);
		if(status == osOK)
		{
			emt_uart_rx_check();
		}
	}
	/* USER CODE END uart_rx_proc_task */
}
#endif
