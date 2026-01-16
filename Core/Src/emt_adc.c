/**
  ******************************************************************************
  * @file           : emt_adc.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include "stdbool.h"
#include "string.h"
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os.h"
#endif
#include "main.h"
#include "emt_types.h"
#include "emt_adc.h"

/* Private macro -------------------------------------------------------------*/
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__)
#define		EMT_BP_ADC_CH					ADC_CHANNEL_4
#define		EMT_TEMP_ADC_CH					ADC_CHANNEL_5
#define		EMT_EDLC_ADC_CH					ADC_CHANNEL_6
#if defined(__STM32_TEMPERATURE_ADC__)
#define		EMT_ADC_CHANNEL_COUNT			5
#else
#define		EMT_ADC_CHANNEL_COUNT			3
#endif
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
#define		EMT_BP_ADC_CH					ADC_CHANNEL_3
#define		EMT_TEMP_ADC_CH					ADC_CHANNEL_5
#define		EMT_EDLC_ADC_CH					ADC_CHANNEL_6
#define		EMT_BP_24_ADC_CH				ADC_CHANNEL_9
#if defined(__STM32_TEMPERATURE_ADC__)
#define		EMT_ADC_CHANNEL_COUNT			6
#else
#define		EMT_ADC_CHANNEL_COUNT			4
#endif
#elif (__TARGET_PROJECT__ == __ZDR027__) || (__TARGET_PROJECT__ == __PRIME__)
#define		EMT_BP_ADC_CH					ADC_CHANNEL_4
#define		EMT_TEMP_ADC_CH					ADC_CHANNEL_5
#define		EMT_EDLC_ADC_CH					ADC_CHANNEL_7
#if defined(__STM32_TEMPERATURE_ADC__)
#define		EMT_ADC_CHANNEL_COUNT			5
#else
#define		EMT_ADC_CHANNEL_COUNT			3
#endif
#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
#define		EMT_BP_24_ADC_CH				ADC_CHANNEL_4
#define		EMT_BP_ADC_CH					ADC_CHANNEL_9
#define		EMT_TEMP_ADC_CH					ADC_CHANNEL_5
#define		EMT_EDLC_ADC_CH					ADC_CHANNEL_6
#if defined(__STM32_TEMPERATURE_ADC__)
#define		EMT_ADC_CHANNEL_COUNT			6
#else
#define		EMT_ADC_CHANNEL_COUNT			4
#endif
#endif

#define		EMT_ADC_BUFFER_COUNT			5
#define		ADC_DMA_BUFF_SIZE				(EMT_ADC_CHANNEL_COUNT*EMT_ADC_BUFFER_COUNT)


#if defined(__MICOM_BOOTLOADER__)
#define ADC_RX_SEM_RELEASE
#else
#define ADC_RX_SEM_RELEASE			osSemaphoreRelease(adcRxSemHandle);
#endif

/* Private variables ---------------------------------------------------------*/
bool g_emt_adc_initialize = false;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef h_dma_adc;
TIM_HandleTypeDef htim6;

static uint16_t g_adc_dma_buffer[ADC_DMA_BUFF_SIZE];
static uint16_t g_adc_user_buffer[ADC_DMA_BUFF_SIZE];

/* External variables --------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#else
extern  osSemaphoreId_t			adcRxSemHandle;
#endif
/* Private functions ---------------------------------------------------------*/
static void MX_ADC_Init(void);
static void MX_TIM6_Init(void);


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	memcpy((void*)&g_adc_user_buffer[0], (void*)&g_adc_dma_buffer[0], sizeof(uint16_t)*(ADC_DMA_BUFF_SIZE));
	ADC_RX_SEM_RELEASE
}

static void MX_ADC_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	int32_t sysclockfreq;

	sysclockfreq = HAL_RCC_GetHCLKFreq();

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	*/
	hadc1.Instance = ADC1;
	if(sysclockfreq == 2000000)
	{
		hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;//ADC_CLOCK_ASYNC_DIV32; //ADC_CLOCK_ASYNC_DIV1;
	}
	else
	{
		hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32;//ADC_CLOCK_ASYNC_DIV32; //ADC_CLOCK_ASYNC_DIV1;
	}
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.LowPowerAutoPowerOff = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = EMT_ADC_CHANNEL_COUNT;

	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_7CYCLES_5;
	hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_19CYCLES_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	*/
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
#if (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) \
	|| (__TARGET_PROJECT__ == __ZDR027__) || (__TARGET_PROJECT__ == __PRIME__) || (__TARGET_PROJECT__ == __TRINITY__)
	sConfig.Channel = EMT_BP_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = EMT_TEMP_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = EMT_EDLC_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
#if defined(__STM32_TEMPERATURE_ADC__)
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_2;
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = ADC_REGULAR_RANK_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
#endif
#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__) || (__TARGET_PROJECT__ == __CYCLOPS__)
#if defined(__SUPPORT_24V_ADC_CHANNEL__)
	sConfig.Channel = EMT_BP_24_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
#endif
	sConfig.Channel = EMT_BP_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = EMT_TEMP_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = EMT_EDLC_ADC_CH;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

#if defined(__STM32_TEMPERATURE_ADC__)
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_2;
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = ADC_REGULAR_RANK_6;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
#endif
#endif
}

static void MX_ADC_DeInit(void)
{
	if (HAL_ADC_DeInit(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM6_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	int32_t Prescaler;

	Prescaler = (HAL_RCC_GetSysClockFreq() / 1000000) - 1;

	if(Prescaler < 0)
		Prescaler = 0;

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = Prescaler;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 2000; //50000;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}


static void MX_TIM6_DeInit(void)
{
	if (HAL_TIM_Base_DeInit(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
}


void emt_adc_init(void)
{
	if(g_emt_adc_initialize == false)
	{
		memset((void*)&g_adc_dma_buffer[0], 0x00, sizeof(g_adc_dma_buffer));
		memset((void*)&g_adc_user_buffer[0], 0x00, sizeof(g_adc_user_buffer));

		MX_ADC_Init();
		MX_TIM6_Init();

		if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
		{
			Error_Handler();
		}
		HAL_Delay(5);

		if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)g_adc_dma_buffer, ARRAY_LEN(g_adc_dma_buffer)) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_TIM_Base_Start(&htim6);
		HAL_Delay(10);
		g_emt_adc_initialize = true;
	}
}


void emt_adc_deinit(void)
{
	if(g_emt_adc_initialize == true)
	{
		memset((void*)&g_adc_dma_buffer[0], 0x00, sizeof(g_adc_dma_buffer));
		memset((void*)&g_adc_user_buffer[0], 0x00, sizeof(g_adc_user_buffer));

		HAL_TIM_Base_Stop(&htim6);
		HAL_ADC_Stop_DMA(&hadc1);

		MX_TIM6_DeInit();
		MX_ADC_DeInit();
		g_emt_adc_initialize = false;
	}
}


void emt_adc_get_value(EMT_ADC_STRUCT_T *p_adc_value)
{
	EMT_ADC_STRUCT_T* p_adc_data = (EMT_ADC_STRUCT_T*)g_adc_user_buffer;
	uint8_t idx;
#if defined(__STM32_TEMPERATURE_ADC__)
	uint16_t internal_reference_voltage;
#endif
	if(p_adc_value == NULL)
		return;

#if defined(__MICOM_BOOTLOADER__)
#else
	if(osSemaphoreAcquire(adcRxSemHandle, 10) == osOK)
#endif
	{
		memset(p_adc_value, 0x00, sizeof(EMT_ADC_STRUCT_T));
		for(idx = 0; idx < EMT_ADC_BUFFER_COUNT; idx++)
		{
#if defined(__SUPPORT_24V_ADC_CHANNEL__)
			p_adc_value->bp_v24 	 	+= p_adc_data[idx].bp_v24;
#endif
			p_adc_value->bp_v12 	 	+= p_adc_data[idx].bp_v12;
			p_adc_value->temperature	+= p_adc_data[idx].temperature;
			p_adc_value->edlc 			+= p_adc_data[idx].edlc;
#if defined(__STM32_TEMPERATURE_ADC__)
			internal_reference_voltage = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(p_adc_data[idx].internal_reference_voltage, ADC_RESOLUTION_12B);
			p_adc_value->internal_reference_voltage += internal_reference_voltage;

			p_adc_value->internal_temperature += __HAL_ADC_CALC_TEMPERATURE(internal_reference_voltage, p_adc_data[idx].internal_temperature, ADC_RESOLUTION_12B);
#endif
		}

#if defined(__SUPPORT_24V_ADC_CHANNEL__)
		p_adc_value->bp_v24 		/= EMT_ADC_BUFFER_COUNT;
#endif
		p_adc_value->bp_v12 		/= EMT_ADC_BUFFER_COUNT;
		p_adc_value->temperature 	/= EMT_ADC_BUFFER_COUNT;
		p_adc_value->edlc 			/= EMT_ADC_BUFFER_COUNT;
#if defined(__STM32_TEMPERATURE_ADC__)
		p_adc_value->internal_reference_voltage /= EMT_ADC_BUFFER_COUNT;
		p_adc_value->internal_temperature 		/= EMT_ADC_BUFFER_COUNT;
#endif
	}
}
