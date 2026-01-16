/**
	******************************************************************************
	* @file           : emt_defines.h
	* @brief          :
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
	*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_DEFINES_H__
#define __EMT_DEFINES_H__

/* Includes ------------------------------------------------------------------*/
#define __LED_RGB_TEST__
//#define __MCU_SWD_DEBUG_ENABLE__

// #define __NEW_CONFIG_FEATURE__	//kimjj, temp option for new io config

//Reboot processing flag when an error occurs
#define __REBOOT_PROC_FLAG__			1

//__TARGET_PROJECT__
#define __LXQ2000__						1
#define __LXQ2000_CR3__					2
#define __ZDR_D770R__					3
#define __ZDR065__						4
#define __ZDR027__						5
#define __PRIME__						6	//Benz Japan
#define __MAGNUS__						7
#define __TRINITY__						8
#define __CYCLOPS__						9	//360X

#define __TARGET_PROJECT__			__CYCLOPS__

//EVENT_STAGE
#define __PROTO__						1
#define __WS__							2
#define __ES__							3
#define __PP__							4
#define __MP__							5

//__TYPE_CPU_CHIP_ID__
#define __ICATCH_SOC_V37__				1
#define __ICATCH_SOC_V50__				2
#define __ICATCH_SOC_V57__				3
#define __ICATCH_SOC_V77__				4

//__TYPE_BB_SCENARIO__
#define __BB_COMTEC__					1
#define __BB_FINE__						2
#define	__BB_DOMESTIC__					3

//__TYPE_G_SENSOR_PROC_MODE__
#define __G_SENSOR_DATA_RECV_MODE__		1
#define __G_SENSOR_INTERRUPT_MODE__		2

//__TYPE_ACC_CONNECTED__
#define	__ACC_CONNECTED_ONLY_MCU__		1
#define	__ACC_CONNECTED_CPU_MCU__		2

//__TYPE_BP_DETECT_CHECK__
#define	__BP_DETECT_TYPE_GPIO__			1
#define	__BP_DETECT_TYPE_ADC__			2

//__TYPE_LED_CONTROL_MODE__
#define __LED_CONTROL_TYPE_GPIO__		1
#define __LED_CONTROL_TYPE_I2C__		2

//#define __STM32_TEMPERATURE_ADC__

#if (__TARGET_PROJECT__ == __LXQ2000__)
#define __TYPE_CPU_CHIP_ID__	__ICATCH_SOC_V50__
#define __TYPE_BB_SCENARIO__	__BB_FINE__

#define __TYPE_G_SENSOR_PROC_MODE__		__G_SENSOR_DATA_RECV_MODE__

#define __SUPPORT_24V_ADC_CHANNEL__
#define __SUPPORT_TEMP_ADC_CONVERT__

#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__)

#define __TYPE_CPU_CHIP_ID__	__ICATCH_SOC_V57__
#define __TYPE_BB_SCENARIO__	__BB_FINE__

#define __TYPE_G_SENSOR_PROC_MODE__		__G_SENSOR_DATA_RECV_MODE__

#define __SUPPORT_24V_ADC_CHANNEL__
#define __SUPPORT_RTC_CONTROL__
#define __SUPPORT_TEMP_ADC_CONVERT__

#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __ZDR065__)
#define __EMT_DEVELOP_STAGE__				__PP__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V57__
#define __TYPE_BB_SCENARIO__				__BB_COMTEC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_ADC__

#define __SUPPORT_SEND_RESETTYPE_TO_CPU__
#if (__EMT_DEVELOP_STAGE__ != __ES__)
#define __SUPPORT_RTC_CONTROL__
#endif
#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __ZDR_D770R__)
#define __EMT_DEVELOP_STAGE__				__ES__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V57__
#define __TYPE_BB_SCENARIO__				__BB_COMTEC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_ADC__

#define __SUPPORT_SEND_RESETTYPE_TO_CPU__
#define __SUPPORT_RTC_CONTROL__
#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __ZDR027__)
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V37__
#define __TYPE_BB_SCENARIO__				__BB_COMTEC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_CPU_MCU__
//#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_ADC__

#define __SUPPORT_TEMP_ADC_CONVERT__
#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __PRIME__)
#define __NEW_CONFIG_FEATURE__	//kimjj, temp option for new io config
#define __EMT_DEVELOP_STAGE__				__WS__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V77__
#define __TYPE_BB_SCENARIO__				__BB_COMTEC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_I2C__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_GPIO__

#define __SUPPORT_RTC_CONTROL__

#define __EMT_KEY_COUNT_MAX__			2
#elif (__TARGET_PROJECT__ == __MAGNUS__)
#define __EMT_DEVELOP_STAGE__				__PP__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V77__
#define __TYPE_BB_SCENARIO__				__BB_DOMESTIC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_GPIO__
#define __SUPPORT_MANUAL_PARKINGMODE_CONTROL__
//#define __SUPPORT_SEND_RESETTYPE_TO_CPU__ // ���� �ó����� : � ������ reset�ߴ��� �˸���.
#define __SUPPORT_RTC_CONTROL__
#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __TRINITY__)
#define __EMT_DEVELOP_STAGE__				__ES__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V77__
#define __TYPE_BB_SCENARIO__				__BB_DOMESTIC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_ADC__
#define __SUPPORT_MANUAL_PARKINGMODE_CONTROL__

//#define __SUPPORT_SEND_RESETTYPE_TO_CPU__
#define __SUPPORT_RTC_CONTROL__
#define __EMT_KEY_COUNT_MAX__			1
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
#define __EMT_DEVELOP_STAGE__				__WS__
#define __TYPE_CPU_CHIP_ID__				__ICATCH_SOC_V77__
#define __TYPE_BB_SCENARIO__				__BB_DOMESTIC__

#define __TYPE_G_SENSOR_PROC_MODE__			__G_SENSOR_INTERRUPT_MODE__
#define __TYPE_LED_CONTROL_MODE__			__LED_CONTROL_TYPE_GPIO__
#define __TYPE_ACC_CONNECTED__				__ACC_CONNECTED_ONLY_MCU__
#define __TYPE_BP_DETECT_CHECK__			__BP_DETECT_TYPE_GPIO__
#define __SUPPORT_MANUAL_PARKINGMODE_CONTROL__

#define __SUPPORT_24V_ADC_CHANNEL__
//#define __SUPPORT_SEND_RESETTYPE_TO_CPU__
#define __SUPPORT_RTC_CONTROL__
#define __EMT_KEY_COUNT_MAX__			1
#else
#error
#endif

#if	(__TYPE_LED_CONTROL_MODE__ == __LED_CONTROL_TYPE_I2C__)
#define __SUPPORT_I2C_LED_DEV_KTD202X__
#endif

#endif /* __EMT_DEFINES_H__ */
