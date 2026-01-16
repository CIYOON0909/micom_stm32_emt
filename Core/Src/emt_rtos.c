/**
  ******************************************************************************
  * @file           : emt_rtos.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os.h"
#include "emt_app.h"
#include "emt_app_main.h"
#include "emt_uart_rx.h"
#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
#include "emt_gsensor.h"
#endif
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
#include "emt_led.h"
#define LED_QUEUE_SIZE 20
#endif

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Definitions for appProc */
osThreadId_t appProcHandle;
const osThreadAttr_t appProc_attributes = {
	.name = "appProc",
	.priority = (osPriority_t) osPriorityNormal,
	.stack_size = 128 * 6
};

/* Definitions for gSensorProc */
osThreadId_t gSensorProcHandle;
const osThreadAttr_t gSensorProc_attributes = {
	.name = "gSenProc",
	.priority = (osPriority_t) osPriorityHigh,
	.stack_size = 128 * 4
};

/* Definitions for uartRxProc */
osThreadId_t uartRxProcHandle;
const osThreadAttr_t uartRxProc_attributes = {
	.name = "uartRxProc",
	.priority = (osPriority_t) osPriorityBelowNormal,
	.stack_size = 128 * 4
};

/* Create LED task */
osThreadId_t ledProcHandle;
const osThreadAttr_t ledProc_attributes = {
    .name = "ledTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityRealtime,
};

/* Definitions for appQueue */
osMessageQueueId_t appQueueHandle;
const osMessageQueueAttr_t appQueue_attributes = {
	.name = "appQueue"
};

/* Definitions for gSensorQueue */
osMessageQueueId_t gSensorQueueHandle;
const osMessageQueueAttr_t gSensorQueue_attributes = {
	.name = "gSensorQueue"
};

osMessageQueueId_t g_ledQueueHandle;
const osMessageQueueAttr_t ledQueue_attributes = {
    .name = "ledQueue"
};

/* Definitions for uartRxSem */
osSemaphoreId_t uartRxSemHandle;
const osSemaphoreAttr_t uartRxSem_attributes = {
	.name = "uartRxSem"
};

osSemaphoreId_t adcRxSemHandle;
const osSemaphoreAttr_t adcRxSem_attributes = {
	.name = "adcRxSem"
};

osMutexId_t uartTxMutexHandle;
const osMutexAttr_t uartTxMutex_attributes = {
	.name = "uartTxMut"
};

osMutexId_t hciTxMutexHandle;
const osMutexAttr_t hciTxMutex_attributes = {
	.name = "hciTxMut"
};

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
static void emt_rtos_create_semaphores(void)
{
	uartRxSemHandle = osSemaphoreNew(10, 0, &uartRxSem_attributes);

	adcRxSemHandle = osSemaphoreNew(1, 0, &adcRxSem_attributes);
}

static void emt_rtos_create_mutex(void)
{
	uartTxMutexHandle = osMutexNew (&uartTxMutex_attributes);

	hciTxMutexHandle = osMutexNew (&hciTxMutex_attributes);
}

static void emt_rtos_create_message_queues(void)
{
	appQueueHandle = osMessageQueueNew (40, sizeof(EMT_MSGQUEUE_T), &appQueue_attributes);
#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
	gSensorQueueHandle = osMessageQueueNew (8, sizeof(EMT_MSGQUEUE_T), &gSensorQueue_attributes);
#endif
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
    g_ledQueueHandle = osMessageQueueNew(LED_QUEUE_SIZE, sizeof(EMT_LED_MSG_T), &ledQueue_attributes);
#endif
}

static void emt_rtos_create_tasks(void)
{
	uartRxProcHandle = osThreadNew(emt_uart_rx_proc_task, NULL, &uartRxProc_attributes);
	appProcHandle = osThreadNew(emt_app_proc_task, NULL, &appProc_attributes);
#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
	gSensorProcHandle = osThreadNew(emt_gsensor_proc_task, NULL, &gSensorProc_attributes);
#endif
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
	ledProcHandle = osThreadNew(emt_led_proc_task, NULL, &ledProc_attributes);
#endif
}

void emt_rtos_init(void)
{
	/* Init scheduler */
	osKernelInitialize();

	emt_rtos_create_semaphores();
	emt_rtos_create_mutex();
	emt_rtos_create_message_queues();
	emt_rtos_create_tasks();

	/* Start scheduler */
	osKernelStart();
	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	while (1)
	{
		osDelay(10);
	}
}
#endif
