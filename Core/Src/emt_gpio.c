/**
  ******************************************************************************
  * @file           : emt_gpio.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include "main.h"
#include "emt_types.h"
#include "emt_app.h"
#include "emt_gpio.h"
#if defined(__SUPPORT_RTC_CONTROL__)
#include "emt_rtc.h"
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* External variables ----------------------------------------------------------*/

void emt_gpio_mcu_reset(void)
{
	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);
	// HAL_GPIO_WritePin(MCU_AQS3_CPU_GPIO_Port, MCU_AQS3_CPU_Pin, GPIO_PIN_RESET);

#if (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V57__)
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
#elif (__TYPE_CPU_CHIP_ID__ ==__ICATCH_SOC_V77__)
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
#if (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);
#endif
	// 파워 시컨스 확인 후 수정 필요.
	HAL_Delay(10);
#else
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
#endif

	reset_mcu();
}

void emt_gpio_set_gsensoer_interrupt(bool enable)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GSEN_INT_MCU_Pin;
	if(enable)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_DeInit(GSEN_INT_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(GSEN_INT_MCU_EXTI_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(GSEN_INT_MCU_EXTI_IRQn);
	}
	else
	{
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(GSEN_INT_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GSEN_INT_MCU_GPIO_Port, GPIO_InitStruct.Pin);
	}
}

bool emt_gpio_is_bp_detect(void)
{
#if ((__TARGET_PROJECT__ == __PRIME__) && (__EMT_DEVELOP_STAGE__ ==	__WS__))
	/* B+ 시정수 조정등으로 인해 HW요청으로 WS에서는 체크하지 않도록 처리 */
	return true;	//B+ on
#else
#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
	if(HAL_GPIO_ReadPin(__BP_DET_MCU_GPIO_Port, __BP_DET_MCU_Pin) == GPIO_PIN_RESET)
	{
		return true;	//B+ on
	}
	return false;		//B+ off
#else
	return true;
#endif
#endif
}

bool emt_gpio_is_acc_detect(void)
{
#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
	if(is_manual_parking_enable_get())
	{
		return !is_parking_acc_get();
	}
	else
#endif
	{
		if(HAL_GPIO_ReadPin(__ACC_DET_MCU_GPIO_Port, __ACC_DET_MCU_Pin) == GPIO_PIN_RESET)
		{
			return true; //ACC on
		}
		return false; //ACC off
	}
}

bool emt_gpio_is_usb_detect(void)
{
#if (__TARGET_PROJECT__ == __ZDR027__)
	if(HAL_GPIO_ReadPin(__USB_DET_MCU_GPIO_Port, __USB_DET_MCU_Pin) == GPIO_PIN_RESET)
#else
	if(HAL_GPIO_ReadPin(USB_DET_MCU_GPIO_Port, USB_DET_MCU_Pin) == GPIO_PIN_SET)
#endif
	{
		return true;
	}
	return false;
}

bool emt_gpio_is_dc2dc_enable(void)
{
	if(HAL_GPIO_ReadPin(__DC2DC_EN_GPIO_Port, __DC2DC_EN_Pin) == GPIO_PIN_RESET) //low-active
	{
		return true;
	}

	return false;
}

void emt_gpio_set_edlc_ctrl(bool on)
{
	GPIO_PinState PinState = GPIO_PIN_RESET;
	if(on == true)
	{
		PinState = GPIO_PIN_SET;
	}
#if (__TARGET_PROJECT__ == __PRIME__) &&  defined(__MCU_SWD_DEBUG_ENABLE__)
    return;
#endif
	HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, PinState);
}

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
/*	LX11 AQS Mode
	AQS0	AQS1	AQS2	Mode 									동작 설명
	0		0		0 		Normal boot								ACC ON  CPU Booting
	1		0		0 		Parking boot ( Motion, TimeLapse)		ACC OFF 감지후 Motion, TimeLapse 주차모드 진입(CPU Reset)
	0		1		0 		저전력 boot 								저전력 모드에서 이벤트 발생 시 CPU Booting(AI 충격 EVT)
	1		1		0 		초저전력 boot								초저전력 모드에서 이벤트 발생 시 CPU Booting(G-Sensor EVT)
	0		0		1 		고온 event ( 저전력, 초저전력 )				저전력, 초저전력 모드(MICOM Only)에서 고온으로 인한 CutOff시 Event 녹화를 위해 CPU Booting
	1		0		1 		F/W update 후 주차 boot					?
	0		1		1 		1차 차단 후 충격							?
	1		1		1 		MICOM FW Recovery						MICOM FW Update 시 Power Off 로 MICOM Bootloader에서 Recovery Mode에서 CPU Booting 시
*/
void emt_gpio_set_aqs_mode(EMT_BOOT_MODE mode)
{
	GPIO_PinState AQS0 = GPIO_PIN_RESET, AQS1 = GPIO_PIN_RESET, AQS2 = GPIO_PIN_RESET;

	switch(mode)
	{
	case _BOOT_PARKING:
		{
			AQS0 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_LOW_PARKING_EVT:
		{
			AQS1 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_SUPER_LOW_PARKING_EVT:
		{
			AQS0 = GPIO_PIN_SET;
			AQS1 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_HIGH_TEMP_EVT:
		{
			AQS2 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_UPGRADE_N_PARKING:
		{
			AQS0 = GPIO_PIN_SET;
			AQS2 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_1ST_CUTOFF_PARKING:
		{
			AQS1 = GPIO_PIN_SET;
			AQS2 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_MICOM_FW_RECOVERY:
		{
			AQS0 = GPIO_PIN_SET;
			AQS1 = GPIO_PIN_SET;
			AQS2 = GPIO_PIN_SET;
		}
		break;
	case _BOOT_NORMAL:
	default:
		break;
	}

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, AQS0);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, AQS1);
	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, AQS2);
}
#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
#if (__TYPE_ACC_CONNECTED__ == __ACC_CONNECTED_ONLY_MCU__)
/*
	AQS0 AQS1		Mode						동작 설명
	  0   0   : 	_AQS_BOOT_NORMAL			ACC ON (주행 모드)
	  0   1   : 	_AQS_BOOT_PARKING			ACC OFF, 주차 상시/타임랩스 녹화 모드
	  1   0   : 	_AQS_BOOT_BDF				ACC OFF, AQS 녹화 모드
	  1   1   : 	_AQS_BOOT_PARK_REC_OFF		ACC OFF, 주차 녹화 Off
	  
	  AQS2    :		need cpu acc check 
	  AQS3	  : 	usb download mode & mcu recovery
*/
void emt_gpio_set_aqs_mode(EMT_BOOT_MODE mode)
{
	GPIO_PinState AQS0 = GPIO_PIN_RESET, AQS1 = GPIO_PIN_RESET;

	switch(mode)
	{
	case _AQS_BOOT_PARKING:
		{
			AQS1 = GPIO_PIN_SET;
		}
		break;
	case _AQS_BOOT_BDF:
		{
			AQS0 = GPIO_PIN_SET;
		}
		break;
	case _AQS_BOOT_PARK_REC_OFF:
	//case _AQS_ACC_OFF:
		{
			AQS0 = GPIO_PIN_SET;
			AQS1 = GPIO_PIN_SET;
		}
		break;
	case _AQS_BOOT_NORMAL:
	//case _AQS_ACC_ON:
	default:
		break;
	}

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, AQS0);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, AQS1);
}
#elif (__TYPE_ACC_CONNECTED__ == __ACC_CONNECTED_CPU_MCU__)
/*
	AQS0 : Whether Parking Recording.
    AQS1 : BDF Boot.
	AQS2 : PARKING AQS MODE -> (ACC ON) -> ACC 2 seconds Check Mode

	AQS0 AQS1 AQS2
	 1    1    1   : MICOM_FW_RECOVERY MODE
*/
void emt_gpio_set_aqs_mode(EMT_BOOT_MODE mode)
{
	GPIO_PinState AQS0 = GPIO_PIN_RESET, AQS1 = GPIO_PIN_RESET;

	switch(mode)
	{
	case _AQS_BOOT_PARKING:
		{
			AQS0 = GPIO_PIN_SET;
		}
		break;
	case _AQS_BOOT_BDF:
		{
			AQS0 = GPIO_PIN_SET;
			AQS1 = GPIO_PIN_SET;
		}
		break;
	case _AQS_BOOT_PARK_REC_OFF:
	//case _AQS_ACC_OFF:
		{
			AQS0 = GPIO_PIN_RESET;
			AQS1 = GPIO_PIN_RESET;
		}
		break;
	case _AQS_BOOT_NORMAL:
	//case _AQS_ACC_ON:
	default:
		break;
	}

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, AQS0);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, AQS1);
}
#endif

void emt_gpio_set_acc_check_mode(bool on)
{//AQS 대기 모드에서 ACC ON 에 의해 CPU ON 되면 2초 시나리오 (ACC CHECK) Enable.
	GPIO_PinState PinState = GPIO_PIN_RESET;
	if(on == true)
	{
		PinState = GPIO_PIN_SET;
	}

	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, PinState);
}
#endif

void emt_gpio_micom_power_en(uint8_t mode)
{
	GPIO_PinState PinState = GPIO_PIN_RESET;
	if(mode == EMT_ON)
	{
		PinState = GPIO_PIN_SET;
	}
	HAL_GPIO_WritePin(MCU_EN_MCUPWR_GPIO_Port, MCU_EN_MCUPWR_Pin, PinState);
}

void emt_gpio_low_power_mode(void)
{
#if (__TARGET_PROJECT__ == __ZDR_D770R__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin,GPIO_PIN_RESET);

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

#if (__EMT_DEVELOP_STAGE__ == __PROTO__)
	GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin | MCU_EN_CPU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MCU_EN_PMIC_Pin;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#else
	GPIO_InitStruct.Pin = MCU_EN_CPU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin | MCU_EN_PMIC_Pin ;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
	GPIO_InitStruct.Pin = MCU_EN_5V_Pin;
	HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
#elif (__TARGET_PROJECT__ == __ZDR065__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = CPU_TXD3_MCU_Pin | PMIC_PG_MCU_Pin | CPU_RXD3_MCU_Pin | MCU_EN_CPU_Pin | MCU_AQS0_CPU_Pin | MCU_AQS1_CPU_Pin | MCU_AQS2_CPU_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MCU_EN_PMIC_Pin;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MCU_EN_5V_Pin;
	HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	__HAL_RCC_GPIOC_CLK_DISABLE();
#elif (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin,GPIO_PIN_RESET);

	HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);
	#if (__TARGET_PROJECT__ == __MAGNUS__)
	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN0V9_1V8_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN_EDLC_Pin;
	#elif (__TARGET_PROJECT__ == __TRINITY__)
	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin|MCU_EN0V9_1V8_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN_EDLC_Pin;
	#elif (__TARGET_PROJECT__ == __CYCLOPS__)
	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin|MCU_EN0V9_1V8_DCDC_Pin|MCU_EN_MCUPWR_Pin|MCU_EN_CPU_Pin;
	#endif
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	#if (__TARGET_PROJECT__ == __CYCLOPS__)
	GPIO_InitStruct.Pin = CPU_UART_TX_MCU_Pin|CPU_UART_RX_MCU_Pin|MCU_EN3V0_DCDC_Pin|MCU_PWR_RCAM_Pin|MCU_EN1V5_DCDC_Pin|MCU_EN_EDLC_Pin;
	#else
	GPIO_InitStruct.Pin =  MCU_EN_CPU_Pin|CPU_UART_TX_MCU_Pin|CPU_UART_RX_MCU_Pin|MCU_EN3V0_DCDC_Pin|MCU_PWR_RCAM_Pin|MCU_PWR_PCAM_Pin|MCU_EN1V5_DCDC_Pin;
	#endif
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	#if (__TARGET_PROJECT__ == __CYCLOPS__)
	GPIO_InitStruct.Pin = MCU_EN_5V_Pin|SYS_RESET_Pin;
	#else
	GPIO_InitStruct.Pin = MCU_EN_5V_Pin;
	#endif
	HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	//emt_adc_deinit();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
#endif
}

static bool emt_gpio_check_dc2dcen_pin(void)
{
	// Try checking the status of dc2dc pin.
	bool ret = false;
	int retry_cnt = 5;

	while (retry_cnt--) {
		ret = emt_gpio_is_dc2dc_enable();
		if (ret)	break;

		HAL_Delay(1);//(10);
	}

	return ret;
}

static bool emt_gpio_check_dc2dcen_sequence(void)
{
	bool ret = true;

	// dc2dcen 핀 상태를 확인하고, low active가 아니면 mcu_en_cpu pin을 재설정하여 다시 확인하는 코드
	if(emt_gpio_check_dc2dcen_pin() == false)
	{
		// MCU_EN_CPU_Pin을 재설정.
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(10);

		if(emt_gpio_check_dc2dcen_pin() == false)
		{
			ret = false;
		}
	}

	return ret;
}

#if (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V37__)
#if (__TARGET_PROJECT__ == __ZDR027__)

bool emt_gpio_cpu_powerctrl(EMT_SYSTEM_POWER_MODE mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(mode == EMT_CPU_POWER_ON)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		GPIO_InitStruct.Pin = SYS_RESET_Pin|MCU_EN3V0_DCDC_Pin|MCU_PWR_5V0_Pin|
								MCU_EN0V9_DCDC_Pin|MCU_EN1V5_DCDC_Pin|MCU_AQS1_CPU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = MCU_EN_MCUPWR_Pin|MCU_EN_RLED_Pin|MCU_AQS0_CPU_Pin
								|MCU_AQS2_CPU_Pin|MCU_PWR_RCAM_Pin|MCU_EN_EDLC_Pin|MCU_EN_GLED_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		GPIO_InitStruct.Pin = MCU_EN_CPU_Pin|MCU_EN1V8_DCDC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = __DC2DC_EN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(__DC2DC_EN_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(__DC2DC_EN_GPIO_Port, &GPIO_InitStruct);

		emt_gpio_micom_power_en(EMT_OFF);

		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);

		//CPU Power On Sequence
		HAL_Delay(5);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(1);

		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}

		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

#if defined(__MICOM_BOOTLOADER__)
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_SET);
#endif
	}
	else if((mode == EMT_CPU_POWER_OFF) || (mode == EMT_CPU_POWER_OFF_WITH_MICOM))
	{
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(50);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);
		HAL_Delay(200);	
		
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

		emt_gpio_micom_power_en(EMT_OFF);

		GPIO_InitStruct.Pin = SYS_RESET_Pin|MCU_EN3V0_DCDC_Pin|MCU_PWR_5V0_Pin|MCU_EN0V9_DCDC_Pin
																	|MCU_EN1V5_DCDC_Pin|MCU_AQS1_CPU_Pin|__DC2DC_EN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = MCU_EN_MCUPWR_Pin|MCU_EN_RLED_Pin|MCU_AQS0_CPU_Pin|MCU_AQS2_CPU_Pin
																	|MCU_PWR_RCAM_Pin|MCU_EN_EDLC_Pin|MCU_EN_GLED_Pin;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = MCU_EN_CPU_Pin|MCU_EN1V8_DCDC_Pin;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);

//		__HAL_RCC_GPIOB_CLK_DISABLE(); //manaul key 동작을 위해 주석.
		__HAL_RCC_GPIOC_CLK_DISABLE();
	}
	else if(mode == EMT_CPU_POWER_RESET)
	{
		//CPU Power Off Sequence
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);
		
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(50);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);

		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);
		HAL_Delay(200);
		
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

		//CPU Power On Sequence
		HAL_Delay(1500);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(1);

		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}

		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
			
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);
	}

	return true;
}
#endif
#elif (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V50__)
bool emt_gpio_cpu_powerctrl(EMT_SYSTEM_POWER_MODE mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#if (__TARGET_PROJECT__ == __LXQ2000__)
	GPIO_InitStruct.Pin = MCU_CTRL_LED_PWM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin, GPIO_PIN_RESET);
#endif

	if(mode == EMT_CPU_POWER_ON)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		GPIO_InitStruct.Pin = MCU_EN_EDLC_Pin|MCU_PWR_RCAM_Pin|MCU_AQS1_CPU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = MCU_AQS2_CPU_Pin|MCU_EN0V9_DCDC_Pin|MCU_EN3V0_DCDC_Pin|MCU_EN1V5_DCDC_Pin
															|MCU_EN1V8_DCDC_Pin|MCU_PWR_5V0_Pin|MCU_EN_CPU_Pin|MCU_EN_MCUPWR_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SYS_RESET_Pin|MCU_AQS0_CPU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = __DC2DC_EN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(__DC2DC_EN_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(__DC2DC_EN_GPIO_Port, &GPIO_InitStruct);

		emt_gpio_micom_power_en(EMT_OFF);

		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);

		//CPU Power On Sequence
		HAL_Delay(5);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);

#if defined(__MICOM_BOOTLOADER__)
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_SET);
#endif
	}
	else if((mode == EMT_CPU_POWER_OFF) || (mode == EMT_CPU_POWER_OFF_WITH_MICOM))
	{
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

		emt_gpio_micom_power_en(EMT_OFF);

		GPIO_InitStruct.Pin = MCU_EN_EDLC_Pin|MCU_PWR_RCAM_Pin|MCU_AQS1_CPU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = MCU_EN0V9_DCDC_Pin|MCU_EN3V0_DCDC_Pin|MCU_EN1V5_DCDC_Pin
																|MCU_EN1V8_DCDC_Pin|MCU_PWR_5V0_Pin|MCU_EN_CPU_Pin|MCU_EN_MCUPWR_Pin;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = SYS_RESET_Pin|MCU_AQS0_CPU_Pin|__DC2DC_EN_Pin;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
	}
	else if(mode == EMT_CPU_POWER_RESET)
	{
		//CPU Power Off Sequence
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

		//CPU Power On Sequence
		HAL_Delay(1000);

		HAL_GPIO_WritePin(MCU_PWR_5V0_GPIO_Port, MCU_PWR_5V0_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(MCU_EN0V9_DCDC_GPIO_Port, MCU_EN0V9_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V8_DCDC_GPIO_Port, MCU_EN1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
	}
#if (__TARGET_PROJECT__ == __LXQ2000__)
	HAL_GPIO_DeInit(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin);
#endif
}

#elif (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V57__)
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
bool emt_gpio_check_PGood_status(GPIO_PinState check_pin)
{
	// Check Power-Good pin status of pmic
	bool ret = false;
	int retry_cnt = 100;
	GPIO_PinState pinState = GPIO_PIN_RESET;

	while (retry_cnt--) {
		pinState = HAL_GPIO_ReadPin(PMIC_PG_MCU_GPIO_Port, PMIC_PG_MCU_Pin);
		if (pinState == check_pin)
		{
			ret = true;
			break;
		}
		HAL_Delay(10);
	}

	return ret;
}
#endif

bool emt_gpio_cpu_powerctrl(EMT_SYSTEM_POWER_MODE mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

#if (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	GPIO_InitStruct.Pin = MCU_CTRL_LED_PWM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin, GPIO_PIN_RESET);
#endif
	if(mode == EMT_CPU_POWER_ON)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		/* GPIOA OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOA_DEFAULT_PINSET;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* GPIOB OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOB_DEFAULT_PINSET;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* GPIOC OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOC_DEFAULT_PINSET;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* 1. __DC2DC_EN_ & PMIC_PG_MCU_Pin INPUT SETTING ****************/
		GPIO_InitStruct.Pin = __DC2DC_EN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(__DC2DC_EN_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(__DC2DC_EN_GPIO_Port, &GPIO_InitStruct);

#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_DeInit(PMIC_PG_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(PMIC_PG_MCU_GPIO_Port, &GPIO_InitStruct);
#endif

		/* 2. CPU POWER CONTROL 관련 핀 GPIO_PIN_RESET SETTING ****************/
#if (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	#ifdef __MCU_SWD_DEBUG_ENABLE__
		HAL_GPIO_WritePin(GPIOA, GPIOA_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIOB_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIOC_DEFAULT_PINSET, GPIO_PIN_RESET);
	#else
		HAL_GPIO_WritePin(GPIOA, GPIOA_DEFAULT_PINSET|MCU_EN_LEDDN_Pin|MCU_EN_LEDUP_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIOB_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIOC_DEFAULT_PINSET, GPIO_PIN_RESET);
	#endif
#else
		HAL_GPIO_WritePin(GPIOA, GPIOA_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIOB_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIOC_DEFAULT_PINSET, GPIO_PIN_RESET);
#endif
		emt_gpio_micom_power_en(EMT_OFF);

		/* 3. CR3 POWER Sequence ****************/
		// 3-1. SYS_RESET HIGH SET
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
#if defined(__SUPPORT_RTC_CONTROL__)
		// 3-2. EXT_RTC INIT ( 32.768KHz)
		emt_rtc_initialize();
#endif		
		HAL_Delay(1);
		// 3-3. MCU_EN_VDD5V0 HIGH SET
		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_SET);
		#if 0
		if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
		{//software reset -> VDD_0V9 가 Low 로 떨어지는데까지 Delay 가 더 필요하기 때문에 MCU_EN_PIMC 핀을 더 늦게 올려주기 위함.
			HAL_Delay(50);
		}
		else
		{
			HAL_Delay(10);
		}
		HAL_RCC_GetResetSource();//  __HAL_RCC_CLEAR_RESET_FLAGS();
		#else
		HAL_Delay(10);
		#endif

		// 3-4. PMIC_PG_MCU LOW 상태인지 확인. ( FAIL : MCU RESET 필요.)
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		if (emt_gpio_check_PGood_status(GPIO_PIN_RESET) == false)
		{ 	//PG_STA_pin error -> need MCU Reset
			return false;
		}
#endif
		// 3-5. MCU_EN_CPU HIGH SET
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(1);//(10);

		// 3-6. DC2DC_EN LOW 상태인지 확인. (FAIL : MCU RESET 필요)
		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}

		// 3-7. MCU_EN_PMIC & PWR_RCAM HIGH SET
		HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(80); /*이후 emt_gpio_check_PGood_status 에서 최대 1000ms 까지 대기.*/

		// 3-8. PMIC_PG_MCU HIGH 상태인지 확인. ( FAIL : MCU RESET 필요.)
#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		if (emt_gpio_check_PGood_status(GPIO_PIN_SET) == false)
		{ 	//PG_STA_pin error -> need MCU Reset
			return false;
		}
#endif

		// 3-9. SYS_RESET & MCU_EN_CPU LOW SET
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);//(20);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

#if defined(__MICOM_BOOTLOADER__)
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS3_CPU_GPIO_Port, MCU_AQS3_CPU_Pin, GPIO_PIN_SET);
#endif
	}
	else if((mode == EMT_CPU_POWER_OFF) || (mode == EMT_CPU_POWER_OFF_WITH_MICOM))
	{
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		//gpio-input mode->gpio-output mode because of current 
		GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(USB_DET_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(USB_DET_MCU_GPIO_Port, USB_DET_MCU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
	//	HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);//no need to set
	///	HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_RESET);

#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(PMIC_PG_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(PMIC_PG_MCU_GPIO_Port, &GPIO_InitStruct);
#endif
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);
		HAL_Delay(10);

		emt_gpio_micom_power_en(EMT_OFF);

		GPIO_InitStruct.Pin = MCU_EN_EDLC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(MCU_EN_EDLC_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(MCU_EN_EDLC_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_DeInit(MCU_EN_EDLC_GPIO_Port, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = MCU_PWR_RCAM_Pin;
		HAL_GPIO_DeInit(MCU_PWR_RCAM_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(MCU_PWR_RCAM_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_DeInit(MCU_PWR_RCAM_GPIO_Port, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = SYS_RESET_Pin|__DC2DC_EN_Pin;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);

#if defined(__SUPPORT_RTC_CONTROL__)
		emt_rtc_32k_output_disable();
#endif
	}
	else if(mode == EMT_CPU_POWER_RESET)
	{
		//CPU Power Off Sequence
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(MCU_AQS3_CPU_GPIO_Port, MCU_AQS3_CPU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		//HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);//no need to set
		///HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_RESET);

#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_DeInit(PMIC_PG_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(PMIC_PG_MCU_GPIO_Port, &GPIO_InitStruct);
#endif
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);
		HAL_Delay(10);

		HAL_Delay(1500);

		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_SET);
		HAL_Delay(10);

#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		if (emt_gpio_check_PGood_status(GPIO_PIN_RESET) == false)
		{ 	//PG_STA_pin error -> need MCU Reset
			return false;
		}
#endif
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(1);//10

		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}

#if (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__)
		HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);

		GPIO_InitStruct.Pin = PMIC_PG_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_DeInit(PMIC_PG_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(PMIC_PG_MCU_GPIO_Port, &GPIO_InitStruct);
		HAL_Delay(80);
		if (emt_gpio_check_PGood_status(GPIO_PIN_SET) == false)
		{ 	//PG_STA_pin error -> need MCU Reset
			return false;
		}
#else
		HAL_GPIO_WritePin(MCU_EN_PMIC_GPIO_Port, MCU_EN_PMIC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
		HAL_Delay(80);
#endif
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);//(20);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
	}

#if (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	HAL_GPIO_DeInit(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin);
#endif

	return true;
}

#elif (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V77__)

bool emt_gpio_cpu_powerctrl(EMT_SYSTEM_POWER_MODE mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(mode == EMT_CPU_POWER_ON)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		/* GPIOA OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOA_DEFAULT_PINSET;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* GPIOB OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOB_DEFAULT_PINSET;
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* GPIOC OUTPUT PINSET ****************/
		GPIO_InitStruct.Pin = GPIOC_DEFAULT_PINSET;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* 1. __DC2DC_EN_ & PMIC_PG_MCU_Pin INPUT SETTING ****************/
		GPIO_InitStruct.Pin = __DC2DC_EN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		HAL_GPIO_DeInit(__DC2DC_EN_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(__DC2DC_EN_GPIO_Port, &GPIO_InitStruct);

		/* 2. CPU POWER CONTROL 관련 핀 GPIO_PIN_RESET SETTING ****************/
		HAL_GPIO_WritePin(GPIOA, GPIOA_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIOB_DEFAULT_PINSET, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIOC_DEFAULT_PINSET, GPIO_PIN_RESET);
		emt_gpio_micom_power_en(EMT_OFF);

		/* 3. CR3 POWER Sequence ****************/
		// 3-1. SYS_RESET HIGH SET
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN0V9_1V8_DCDC_GPIO_Port, MCU_EN0V9_1V8_DCDC_Pin, GPIO_PIN_RESET);

		HAL_Delay(5);
#if defined(__SUPPORT_RTC_CONTROL__)
		// 3-2. EXT_RTC INIT ( 32.768KHz)
		emt_rtc_initialize();
#endif
		HAL_Delay(1);
		// 3-3. MCU_EN_VDD5V0 HIGH SET
		#if (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_EN_AON_GPIO_Port, MCU_EN_AON_Pin, GPIO_PIN_SET);
		HAL_Delay(1);
		#endif
		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_SET);

		#if 0
		if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
		{//software reset -> VDD_0V9 가 Low 로 떨어지는데까지 Delay 가 더 필요하기 때문에 MCU_EN_PIMC 핀을 더 늦게 올려주기 위함.
			HAL_Delay(50);
		}
		else
		{
			HAL_Delay(10);
		}
		HAL_RCC_GetResetSource();//  __HAL_RCC_CLEAR_RESET_FLAGS();
		#else
		HAL_Delay(5);
		#endif

		// 3-4. MCU_PWR_RCAM, MCU_PWR_PCAM HIGH SET
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
#if	(__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_PWR_PCAM_GPIO_Port, MCU_PWR_PCAM_Pin, GPIO_PIN_SET);
#endif
		HAL_Delay(5);

		// 3-5. MCU_EN_CPU HIGH SET
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(10);//(10);

		// 3-6. DC2DC_EN LOW 상태인지 확인. (FAIL : MCU RESET 필요)
		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}
		// 3-7. MCU_EN_CPU LOW SET
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);

		// 3-8. PWR ON Sequence SET
		HAL_GPIO_WritePin(MCU_EN0V9_1V8_DCDC_GPIO_Port, MCU_EN0V9_1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);

		// 3-9. SYS_RESET LOW SET
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);

#if defined(__MICOM_BOOTLOADER__)
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_SET);
#endif
	}
	else if((mode == EMT_CPU_POWER_OFF) || (mode == EMT_CPU_POWER_OFF_WITH_MICOM))
	{
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		//gpio-input mode->gpio-output mode because of current
		GPIO_InitStruct.Pin = USB_DET_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_DeInit(USB_DET_MCU_GPIO_Port, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(USB_DET_MCU_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(USB_DET_MCU_GPIO_Port, USB_DET_MCU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
#if	(__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_PWR_PCAM_GPIO_Port, MCU_PWR_PCAM_Pin, GPIO_PIN_RESET);
#endif
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN0V9_1V8_DCDC_GPIO_Port, MCU_EN0V9_1V8_DCDC_Pin, GPIO_PIN_RESET);

		HAL_Delay(100);
#if (__TARGET_PROJECT__ == __PRIME__) &&  defined(__MCU_SWD_DEBUG_ENABLE__)
#else
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);
#endif

		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);

#if (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_Delay(1);
		HAL_GPIO_WritePin(MCU_EN_AON_GPIO_Port, MCU_EN_AON_Pin, GPIO_PIN_RESET);
#endif
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_Delay(10);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
#if (__TARGET_PROJECT__ == __PRIME__) &&  defined(__MCU_SWD_DEBUG_ENABLE__)
#else
		GPIO_InitStruct.Pin = MCU_EN_EDLC_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;

		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
#endif

#if (__TARGET_PROJECT__ == __MAGNUS__) || (__TARGET_PROJECT__ == __TRINITY__)
		GPIO_InitStruct.Pin = MCU_PWR_RCAM_Pin|MCU_PWR_PCAM_Pin;
#else
		GPIO_InitStruct.Pin = MCU_PWR_RCAM_Pin;
#endif
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);

		GPIO_InitStruct.Pin = SYS_RESET_Pin|__DC2DC_EN_Pin;
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_GPIO_DeInit(GPIOC, GPIO_InitStruct.Pin);

#if defined(__SUPPORT_RTC_CONTROL__)
		emt_rtc_32k_output_disable();
#endif
	}
	else if(mode == EMT_CPU_POWER_RESET)
	{
		//CPU Power Off Sequence
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_GPIO_WritePin(MCU_AQS0_CPU_GPIO_Port, MCU_AQS0_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS1_CPU_GPIO_Port, MCU_AQS1_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_AQS2_CPU_GPIO_Port, MCU_AQS2_CPU_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_RESET);
#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_PWR_PCAM_GPIO_Port, MCU_PWR_PCAM_Pin, GPIO_PIN_RESET);
#endif
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(MCU_EN0V9_1V8_DCDC_GPIO_Port, MCU_EN0V9_1V8_DCDC_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
#if (__TARGET_PROJECT__ == __PRIME__) &&  defined(__MCU_SWD_DEBUG_ENABLE__)
#else
		HAL_GPIO_WritePin(MCU_EN_EDLC_GPIO_Port, MCU_EN_EDLC_Pin, GPIO_PIN_RESET);
#endif
		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_RESET);
#if (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_Delay(1);
		HAL_GPIO_WritePin(MCU_EN_AON_GPIO_Port, MCU_EN_AON_Pin, GPIO_PIN_RESET);
#endif
		emt_gpio_micom_power_en(EMT_OFF);
		HAL_Delay(10);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_Delay(1500);

#if (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_EN_AON_GPIO_Port, MCU_EN_AON_Pin, GPIO_PIN_SET);
		HAL_Delay(1);
#endif
		HAL_GPIO_WritePin(MCU_EN_5V_GPIO_Port, MCU_EN_5V_Pin, GPIO_PIN_SET);

		HAL_Delay(5);

		HAL_GPIO_WritePin(MCU_PWR_RCAM_GPIO_Port, MCU_PWR_RCAM_Pin, GPIO_PIN_SET);
#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __MAGNUS__)
		HAL_GPIO_WritePin(MCU_PWR_PCAM_GPIO_Port, MCU_PWR_PCAM_Pin, GPIO_PIN_SET);
#endif
		HAL_Delay(5);

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_SET);
		HAL_Delay(10);

		if(emt_gpio_check_dc2dcen_sequence() == false)
		{
			return false;
		}

		HAL_GPIO_WritePin(MCU_EN_CPU_GPIO_Port, MCU_EN_CPU_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_EN0V9_1V8_DCDC_GPIO_Port, MCU_EN0V9_1V8_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN1V5_DCDC_GPIO_Port, MCU_EN1V5_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(MCU_EN3V0_DCDC_GPIO_Port, MCU_EN3V0_DCDC_Pin, GPIO_PIN_SET);
		HAL_Delay(50);

		HAL_GPIO_WritePin(SYS_RESET_GPIO_Port, SYS_RESET_Pin, GPIO_PIN_RESET);
	}

	return true;
}
#endif
