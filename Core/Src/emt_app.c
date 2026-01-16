/**
  ******************************************************************************
  * @file           : emt_app.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os.h"
#endif
#include "main.h"
#include "stm32g0xx_hal.h"

#include "emt_types.h"
#include "emt_version.h"
#include "emt_app.h"
#include "emt_fw_upgrade.h"
#include "emt_led.h"
#include "emt_adc.h"
#include "emt_uart_rx.h"
#include "emt_hci.h"
#include "emt_gpio.h"
#if defined(__SUPPORT_RTC_CONTROL__)
#include "emt_rtc.h"
#endif
/* Private macro -------------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#define ENABLE_HCI_GROUP_LED
#else
#define ENABLE_HCI_GROUP_PARKING
//#define ENABLE_HCI_GROUP_GPS
#define ENABLE_HCI_GROUP_ADC
#define ENABLE_HCI_GROUP_GSENSOR
#define ENABLE_HCI_GROUP_LED

//#define ENABLE_HCI_GROUP_RADAR
#define ENABLE_HCI_GROUP_DIAGNOSTIC
#endif

#define HEX2ASCII( a ) ((a) >= 0x0A ? ((a) - 0x0A + 'A'): ((a)+'0'))
#define ASCII2HEX( a ) ((a) >= 'A' ? ((a) -'A' + 0x0A): ((a)-'0'))

#if (__TARGET_PROJECT__ == __PRIME__)
#define EDLC_VOL_4_0V					(3004)
#define EDLC_VOL_4_5V					(3377)//4.5v
#define EDLC_VOL_4_6V					(3448)//4.6V
#define EDLC_VOL_4_8V					(3595)

#define EDLC_FULL_CHARGE_COUNT			1

#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_6V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_0V		// EDLC OFF 시나리오 가능한 ADC

/*12V VOL ******************************************/
#define CPU_OFF_BP_V12_ADC_MIN			(942)	//6.3V//6V	// CPU 가 OFF 되어야하는 최소 BP ADC // BP_ADC_LOWEST_VALUE_FOR_CPUOFF
#define CPU_ON_BP_V12_ADC_MIN			(1268)	// 7V	// CPU 가 ON 가능한 최소 BP ADC	// BP_ADC_LOWEST_VALUE_FOR_CPUON

#define CPU_ON_BP_10_0_V_ADC_MIN		(1723)			// EDLC 충전이 가능한 최소 BP ADC

#elif (__TARGET_PROJECT__ == __ZDR027__)
#define EDLC_VOL_4_0V					(2687)
#define EDLC_VOL_4_8V					(3225)

#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_0V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_0V		// EDLC OFF 시나리오 가능한 ADC

#define EDLC_FULL_CHARGE_COUNT			30

#define BASE_TEMP_MIN					(-31)
#define BASE_TEMP_TABLE_MAX				(111)

#define CPU_OFF_BP_V12_ADC_MIN			(942) 	//	6V
#define CPU_ON_BP_V12_ADC_MIN			(1268)	//	power suply:6.9V = table:7.6V

#define CPU_ON_BP_10_0_V_ADC_MIN		(1723)

#elif (__TARGET_PROJECT__ == __ZDR_D770R__)
/*EDLC *********************************************/
#define EDLC_VOL_3_9V					(3205)
#define EDLC_VOL_4_5V					(3696)
#define EDLC_VOL_4_8V					(3940)

#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC (MIN ADC)
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_5V		// EDLC 재충전 기준 ADC (MIN ADC)
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_3_9V		// EDLC OFF 시나리오 가능한 ADC (MIN ADC)

#define EDLC_FULL_CHARGE_COUNT			1
/*12V VOL ******************************************/
#define CPU_OFF_BP_V12_ADC_MIN			(1261)	// 6.3V		// CPU 가 OFF 되어야하는 최소 BP ADC
#define CPU_ON_BP_V12_ADC_MIN			(1547)	// 7V		// CPU 가 ON 가능한 최소 BP ADC (CPU OFF 기준 ADC)

#define CPU_ON_BP_10_0_V_ADC_MIN		(2106)				// EDLC 충전이 가능한 최소 BP ADC 

#elif (__TARGET_PROJECT__ == __ZDR065__)
#if (__EMT_DEVELOP_STAGE__ == __WS__)
#define EDLC_VOL_4_0V					(2702)
#define EDLC_VOL_4_8V					(3247)
#else//ES
#define EDLC_VOL_4_0V					(3202)
#define EDLC_VOL_4_5V					(3611)//4.5v
#define EDLC_VOL_4_6V					(3694)//4.6V
#define EDLC_VOL_4_8V					(3859)
#endif

#define EDLC_FULL_CHARGE_COUNT			1

#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_6V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_0V		// EDLC OFF 시나리오 가능한 ADC

/*12V VOL ******************************************/
#define CPU_OFF_BP_V12_ADC_MIN			(1194)	//6.3V//6V	// CPU 가 OFF 되어야하는 최소 BP ADC // BP_ADC_LOWEST_VALUE_FOR_CPUOFF
#define CPU_ON_BP_V12_ADC_MIN			(1540)	// 7V	// CPU 가 ON 가능한 최소 BP ADC	// BP_ADC_LOWEST_VALUE_FOR_CPUON

#define CPU_ON_BP_10_0_V_ADC_MIN		(2084)			// EDLC 충전이 가능한 최소 BP ADC

#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
#define EDLC_VOL_4_0V					(3255)
#define EDLC_VOL_4_8V					(3965)

#define _SYSTEM_OFF_TIME				(60)
#define _SYSTEM_OFF_TEMP				(66)    //72    //67    //70

#define BASE_TEMP_MIN					(-20)
#define BASE_TEMP_TABLE_MAX				(96)

#define BP_TYPE_V12_ADC_MAX				(4050)
#define BP_TYPE_V24_ADC_MIN				(100)

#define CPU_OFF_BP_V12_ADC_MIN			(1161)   // 6V
#define CPU_ON_BP_V12_ADC_MIN			(1391)   // 7V
#define BP_V24_ADC_MIN					(1138)
#define CPU_ON_BP_10_0_V_ADC_MIN		(2072)

#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_0V		// EDLC OFF 시나리오 가능한 ADC
#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_0V		// EDLC 재충전 기준 ADC

#define EDLC_FULL_CHARGE_COUNT			30
#elif (__TARGET_PROJECT__ == __MAGNUS__)

#define EDLC_VOL_4_3V					(3426)//min
#define EDLC_VOL_4_5V					(3669)//max

#define EDLC_VOL_4_8V					(3910) //max


#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_8V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_5V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_3V		// EDLC OFF 시나리오 가능한 ADC

#define EDLC_FULL_CHARGE_COUNT			30

#define LED_PWM_DUTY_MIN				(2)
#define LED_PWM_DUTY_MAX				(27)    //(30) 30*50 = 1500
#define LED_PWM_DUTY_MIN_X5000			(4)
#define LED_PWM_DUTY_MAX_X5000			(30)    //10
#define LED_PWM_CTRL_CYCLE				(50)    //(100 - LED_PWM_DUTY_MAX)

#define _SYSTEM_OFF_TIME				(60)
#define _SYSTEM_OFF_TEMP				(66)    //72    //67    //70

#define BASE_TEMP_MIN					(-20)
#define BASE_TEMP_TABLE_MAX				(96)

#define BP_TYPE_V12_ADC_MAX				(4050)//아직 사용하지 않음.
#define BP_TYPE_V24_ADC_MIN				(100)

#define CPU_OFF_BP_V12_ADC_MIN			(1161)   // 6V
#define CPU_ON_BP_V12_ADC_MIN			(1391)   // 7V
#define BP_V24_ADC_MIN					(1138)
#define CPU_ON_BP_10_0_V_ADC_MIN		(1996)
#define CPU_ON_BP_20_0_V_ADC_MIN		0//해당 없음
#elif (__TARGET_PROJECT__ == __TRINITY__)
/**
 * EDLC 충전 중단 전압: 5.1V
 * EDLC 재충전 전압: 4.9V
 * EDLC 시나리오 Enable 전압: 4.8V
 * ES2 측정값 추가함.
 */
#define EDLC_VOL_4_0V					(2971)//(2943)
#define EDLC_VOL_4_5V					(3349)//(3317)
#define EDLC_VOL_4_6V					(3426)//(3397)
#define EDLC_VOL_4_7V 					(3498)//(3473)
#define EDLC_VOL_4_8V					(3567)//(3544) //4.8V
#define EDLC_VOL_4_9V					(3646)//(3623) //4.9V
#define EDLC_VOL_5_0V					(3723)//(3699) //5.0V
#define EDLC_VOL_5_1V					(3805)//(3768) //5.1V
#define EDLC_VOL_5_2V					(3881)//(3844) //5.2V
#define EDLC_VOL_5_3V					(3957)//()     //5.3V

// EDLC 가 얼마나 버티는지에 대한 측정을 위해 임시로
#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_5_1V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_9V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_8V		// EDLC OFF 시나리오 가능한 ADC

#define EDLC_FULL_CHARGE_COUNT			30

#define LED_PWM_DUTY_MIN				(2)
#define LED_PWM_DUTY_MAX				(27)    //(30) 30*50 = 1500
#define LED_PWM_DUTY_MIN_X5000			(4)
#define LED_PWM_DUTY_MAX_X5000			(30)    //10
#define LED_PWM_CTRL_CYCLE				(50)    //(100 - LED_PWM_DUTY_MAX)

#define _SYSTEM_OFF_TIME				(60)
#define _SYSTEM_OFF_TEMP				(66)    //72    //67    //70

#define BASE_TEMP_MIN					(-20)
#define BASE_TEMP_TABLE_MAX				(96)

#define BP_TYPE_V12_ADC_MAX				(2024)//(2024) //~18.0V
#define BP_TYPE_V24_ADC_MIN				(2488)//(2487) //22.0V~

#define CPU_OFF_BP_V12_ADC_MIN			(719)//(659)   // 시스템 CUTOFF 전원 -> 7V 로 지정
#define CPU_ON_BP_V12_ADC_MIN			(961)//(842)   // 시스템 ON 최소 전원 -> 9V 로 지정 -> CPU ON을 기준으로한 테이블임으로 실제 CPU OFF 상태에서 부팅시 설정 전압보다 -1V 정도의 차이가 있음.
#define BP_V24_ADC_MIN					(2488)//(2487) // 우선 22V~

#define CPU_ON_BP_10_0_V_ADC_MIN		(1082)//(1084) // 우선 10V 기준 //24V에 대한 BP 최소 전압도 필요하다.
#define CPU_ON_BP_20_0_V_ADC_MIN		(2256)// 24v기준 20V 임시.
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
/**
 * EDLC 충전 중단 전압: 5.0V
 * EDLC 재충전 전압: 4.5V
 * EDLC 시나리오 Enable 전압: 4.6V
 * ES2 측정값 추가함.
 */
#define EDLC_VOL_4_0V					(3251)//(2943)
#define EDLC_VOL_4_5V					(3655)//(3317)
#define EDLC_VOL_4_6V					(3737)//(3397)
#define EDLC_VOL_4_7V 					(3818)//(3473)
#define EDLC_VOL_4_8V					(3896)//(3544) //4.8V
#define EDLC_VOL_4_9V					(4979)//(3623) //4.9V
#define EDLC_VOL_5_0V					(4060)//(3699) //5.0V

// EDLC 가 얼마나 버티는지에 대한 측정을 위해 임시로
#define EDLC_FULL_CHARGE_VOL			EDLC_VOL_4_9V		// EDLC 완충 기준 ADC
#define EDLC_RE_CHARGE_VOL				EDLC_VOL_4_5V		// EDLC 재충전 기준 ADC
#define EDLC_OFF_ENABLE_VOL				EDLC_VOL_4_8V		// EDLC OFF 시나리오 가능한 ADC

#define EDLC_FULL_CHARGE_COUNT			30

#define LED_PWM_DUTY_MIN				(2)
#define LED_PWM_DUTY_MAX				(27)    //(30) 30*50 = 1500
#define LED_PWM_DUTY_MIN_X5000			(4)
#define LED_PWM_DUTY_MAX_X5000			(30)    //10
#define LED_PWM_CTRL_CYCLE				(50)    //(100 - LED_PWM_DUTY_MAX)

#define _SYSTEM_OFF_TIME				(60)
#define _SYSTEM_OFF_TEMP				(66)    //72    //67    //70

#define BASE_TEMP_MIN					(-20)
#define BASE_TEMP_TABLE_MAX				(96)

#define BP_TYPE_V12_ADC_MAX				(3897)//~18.0V
#define BP_TYPE_V12_ADC_MIN				(1000)// under 6.5v
#define BP_TYPE_V24_ADC_MIN				(1940)//18.1V ~

#define CPU_OFF_BP_V12_ADC_MIN			(1391)//(659)   // 시스템 CUTOFF 전원 -> 7.0V 로 지정
#define CPU_ON_BP_V12_ADC_MIN			(1612)//(842)  // 시스템 ON 최소 전원 -> 8V 로 지정
#define BP_V24_ADC_MIN					(1940)//(2487) // 18.1V
#define CPU_ON_BP_10_0_V_ADC_MIN		(2075)//(1084) // 우선 10V 기준 //24V에 대한 BP 최소 전압도 필요하다.
#define CPU_ON_BP_20_0_V_ADC_MIN		(2146)//20V 기준
#endif

#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
#define BP_DISCONNECTED_CHECK_COUNT		(3)
#else
#define BP_DISCONNECTED_CHECK_COUNT		(4)
#endif

/* Private variables ---------------------------------------------------------*/
static EMT_APP_INFO g_emt_app_info;
EMT_FW_DL_STATUS_T g_fw_dl_status = {
	.address = 0,
	.fw_packet_index = 0
};

#if (__TARGET_PROJECT__ == __ZDR027__)
const uint16_t iBaseTempadc[BASE_TEMP_TABLE_MAX + 1] =
{
	4092,	3263,	3248,	3233,	3218,	3203,	3188,	3173,	3158,	3143,	// -31, -30, -29, -28, -27, -26, -25, -24, -23, -22
	3127,	3111,	2726,	2694,	2662,	2631,	2600,	2569,	2538,	2507,	// -21, -20, -19, -18, -17, -16, -15, -14, -13, -12
	2476,	2445,	2414,	2383,	2352,	2321,	2290,	2260,	2230,	2200,	// -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2
	2170,	2140,	2108,	2076,	2044,	2012,	1980,	1949,	1918,	1887,	// - 1,   0,   1,   2,   3,   4,   5,   6,   7 ,  8
	1856,	1825,	1794,	1763,	1732,	1701,	1670,	1639,	1608,	1578,	//   9,  10,  11,  12,  13,  14,  15,  16,  17,  18
	1548,	1518,	1491,	1464,	1437,	1410,	1383,	1356,	1329,	1302,	//  19,  20,  21,  22,  23,  24,  25,  26,  27,  28
	1276,	1250,	1222,	1194,	1167,	1140,	1113,	1086,	1059,	1032,	//  29,  30,  31,  32,  33,  34,  35,  36,  37,  38
	1005,	978,	957,	936,	915,	894,	873,	853,	833,	813,	//  39,  40,  41,  42,  43,  44,  45,  46,  47,  48
	793,	773,	756,	739,	723,	707,	691,	675,	659,	643,	//  49,  50,  51,  52,  53,  54,  55,  56,  57,  58
	627,	611,	599,	588,	577,	566,	555,	544,	533,	522,	//  59,  60,  61,  62,  63,  64,  65,  66,  67,  68
	511,    500,	488,	476,	465,	454,	443,	433,	423,	413,	//  69,  70,  71,  72,  73,  74,  75,  76,  77,  78
	403,	393,																		//  79,  80
};
#elif (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
const uint16_t iBaseTempadc[BASE_TEMP_TABLE_MAX +1] =
{
	2322, 2278, 2255, 2225, 2195, 2165, 2135, 2105, 2075, 2045,	//  -20	  -19   -18   -17   -16   -15   -14   -13   -12   -11
	2015, 1985, 1954, 1923, 1892, 1862, 1831, 1800, 1769, 1738,	//  -10    -9    -8    -7    -6    -5   -4     -3    -2    -1
	1708, 1677, 1648, 1618, 1589, 1559, 1529, 1500, 1470, 1440,	//    0     1     2     3     4     5	  6     7     8     9
	1411, 1381, 1358, 1332, 1306, 1281, 1255, 1230, 1204, 1178,	//   10    11    12    13    14    15    16    17    18    19
	1153, 1127, 1106, 1083, 1060, 1037, 1015, 992,  969,  946,	//   20    21    22    23    24    25    26    27    28    29
	923,  901,  883,  864,  845,  826,  807,  788,  769,  750,	//   30    31    32    33    34    35    36    37    38    39
	731,  712,  698,  682,  666,  650,  634,  618,  602,  586,	//   40    41    42    43    44    45    46    47    48    49
	570,  554,  543,  530,  516,  503,  490,  477,  464,  451,	//   50    51    52    53    54    55    56    57    58    59
	438,  425,  414,  402,  390,  378,  367,  355,  343,  331,	//   60    61    62    63    64    65    66    67    68    69
	320,  308,  302,  295,  287,  279,  272						//   70    71    72    73    74    75
};
#endif

/* External variables ----------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#else
extern osMessageQueueId_t	appQueueHandle;
extern osMessageQueueId_t	gSensorQueueHandle;
#endif
/* Private functions ---------------------------------------------------------*/

static void emt_app_lowpowermode(bool enable)
{
	if (enable)
	{
		if ((PWR->SR2 & PWR_SR2_REGLPF) == 0x00u)
		{
			HAL_PWREx_EnableLowPowerRunMode();
		}
	}
	else
	{
		/* If in low-power run mode at this point, exit it */
		if ((PWR->SR2 & PWR_SR2_REGLPF) != 0x00u)
		{
			if (HAL_PWREx_DisableLowPowerRunMode() != HAL_OK)
			{
				reset_mcu();
			}
		}
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	emt_app_lowpowermode(false);
	HAL_RCC_DeInit();

	/** Configure the main internal regulator output voltage
	*/
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV16;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
															|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the peripherals clocks
	*/

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

void SystemClock_Config_LowPower(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	HAL_RCC_DeInit();

	/** Configure the main internal regulator output voltage
	*/
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV8;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	emt_app_lowpowermode(true);
}

void emt_app_init(EMT_APP_INFO** p_emt_app_info)
{
	memset(&g_emt_app_info, 0x00, sizeof(g_emt_app_info));
	memset(&g_fw_dl_status, 0x00, sizeof(g_fw_dl_status));

	*p_emt_app_info = &g_emt_app_info;
}
#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
bool is_manual_parking_enable_get()
{
	return g_emt_app_info.is_parking_manual_enable;
}

bool is_parking_acc_get()
{
	return g_emt_app_info.is_parking_manual_set;
}
#endif

#if defined(__MICOM_BOOTLOADER__)
#else
void emt_app_jump_to_bootloader(void)
{
	extern uint8_t __flash_bootloader_addr;
	uint32_t bootloader_addr = (uint32_t)&__flash_bootloader_addr;

	typedef  void (*pFunction)(void);
	pFunction Jump_To_Application;
	uint32_t JumpAddress;

	emt_led_process_timer_deinit();
	HAL_Delay(10);
	emt_fw_upgrade_save_newfw_magic_number_to_ram();
	emt_uart_deinit();
	emt_adc_deinit();
	emt_front_led_deinit();
	emt_status_led_deinit();
	HAL_RCC_DeInit();
	//HAL_DeInit();

	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	__disable_irq(); // disable interrupt
	__set_PRIMASK(1);

	for (int i=0;i<5;i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}
	__HAL_REMAPMEMORY_SYSTEMFLASH();
	__HAL_RCC_APB1_FORCE_RESET();
	__HAL_RCC_APB1_RELEASE_RESET();
	__HAL_RCC_APB2_FORCE_RESET();
	__HAL_RCC_APB2_RELEASE_RESET();
	__HAL_RCC_AHB_FORCE_RESET();
	__HAL_RCC_AHB_RELEASE_RESET();
	/* Jump to user application */
	JumpAddress = *(__IO uint32_t*) (bootloader_addr + 4);
	Jump_To_Application = (pFunction) JumpAddress;
	/* Initialize user application's Stack Pointer */
//					__set_MSP(*(__IO uint32_t*) FLASH_BOOT_START_ADDR);
	__ASM volatile ("movs r3, #0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");
	Jump_To_Application();

	while(1);
}


void emt_app_check_cut_off_process(EMT_APP_INFO *p_emt_app_info)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__) //|| (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
	uint16_t bp_adc_value = 0;

	if((p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
		&& (p_emt_app_info->bbx_op_mode < EMT_BBX_OP_CUT_OFF_MODE)
		&& (p_emt_app_info->cpu_cutoff.on == true))
	{
		if(p_emt_app_info->bp_type == EMT_BP_TYPE_V12)
		{
			bp_adc_value = p_emt_app_info->adc_value.bp_v12;
		}
		else
		{
			bp_adc_value = p_emt_app_info->adc_value.bp_v24;
		}

		if((p_emt_app_info->cpu_cutoff.check_adc != 0) && (p_emt_app_info->cpu_cutoff.check_adc > bp_adc_value))
		{
			p_emt_app_info->cpu_cutoff.check_count++;
			if(p_emt_app_info->cpu_cutoff.check_count == 60)
			{
				EMT_MSGQUEUE_T msg;
				msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
				msg.data_len = 1;
				msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
				osMessageQueuePut (appQueueHandle, &msg, 0, 0);
				p_emt_app_info->cpu_cutoff.on = false;
				//p_emt_app_info->cpu_cutoff.check_count = 0;
				//p_emt_app_info->cpu_cutoff.check_adc = 0;
			}
		}
		else
		{
			p_emt_app_info->cpu_cutoff.check_count = 0;
		}
	}
#endif
}

void emt_app_check_temperature(EMT_APP_INFO* p_emt_app_info)
{
#if defined (__SUPPORT_TEMP_ADC_CONVERT__)
	EMT_TEMPERATURE_STRUCT_T*  p_temperature = &p_emt_app_info->temperature;
	uint16_t tempADC = p_emt_app_info->adc_value.temperature;
	int i;

	const uint16_t *pTempTable = NULL;

	p_temperature->value = BASE_TEMP_MIN;
	pTempTable = iBaseTempadc;

	if(tempADC < pTempTable[0])
	{
		for(i = 0; i < BASE_TEMP_TABLE_MAX; i++)
		{
			if((tempADC <= pTempTable[i]) && (tempADC >= pTempTable[i+1] +1))
				break;

			p_temperature->value += 1;
		}
	}

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
	{
		if(p_temperature->value >= _SYSTEM_OFF_TEMP)
		{
			p_temperature->check_count++;
		}
		else
		{
			p_temperature->check_count = 0;
		}

		if(p_temperature->check_count >= 60)
		{
			EMT_MSGQUEUE_T msg;
			//TODO : CPU Power Up and Last Record
			msg.command = EMT_MSG_SET_CPU_POWER_UP;
			msg.data_len = 1;
			msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
			osMessageQueuePut (appQueueHandle, &msg, 0, 0);
		}
	}
#endif
#endif //#if defined (__SUPPORT_TEMP_ADC_CONVERT__)
}

bool emt_app_is_bp_cutoff(EMT_APP_INFO* p_emt_app_info)
{
	bool value = false;
#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
#else
	if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
	{
		#if (__TARGET_PROJECT__ == __TRINITY__)
		if(p_emt_app_info->adc_value.bp_v12 < CPU_OFF_BP_V12_ADC_MIN) /*&& (p_emt_app_info->adc_value.bp_v24 < BP_V24_ADC_MIN)*/
		{
			value = true;
		}
		#if 0 //2025.05.14 [arkyoung] 테스트용이니 우선 주석 처리
		//about 24v
		else if ( p_emt_app_info->adc_value.bp_v12 > BP_TYPE_V12_ADC_MAX && p_emt_app_info->adc_value.bp_v12 < BP_TYPE_V24_ADC_MIN)
		{
			value = true;
		}
		#endif
		#else
		if(p_emt_app_info->adc_value.bp_v12 < CPU_OFF_BP_V12_ADC_MIN) /*&& (p_emt_app_info->adc_value.bp_v24 < BP_V24_ADC_MIN)*/
		{
			value = true;
		}
		#endif
	}
	else
	{
		if(p_emt_app_info->adc_value.bp_v12 < CPU_ON_BP_V12_ADC_MIN) /*&& (p_emt_app_info->adc_value.bp_v24 < BP_V24_ADC_MIN)*/
		{
			value = true;
		}
	}
#endif
	return value;
}

static bool emt_app_check_bp_detect(EMT_APP_INFO* p_emt_app_info)
{
	bool val = true; 

#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_ADC__)
	if( emt_app_is_bp_cutoff(p_emt_app_info) == true)
#elif (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
	if( emt_gpio_is_bp_detect() == false )
#endif
	{
		p_emt_app_info->bp.remove_check_count++;
		if(p_emt_app_info->bp.remove_check_count >= BP_DISCONNECTED_CHECK_COUNT)
		{
			p_emt_app_info->bp.remove_check_count = BP_DISCONNECTED_CHECK_COUNT;
			val = false;
		#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
			HAL_Delay(20);
			if( emt_gpio_is_bp_detect() == true )	val = true; // 재 확인 필요. jhpark
		#endif
		}
	}
	else
	{
		p_emt_app_info->edlc.last_edlc = p_emt_app_info->adc_value.edlc;
		p_emt_app_info->bp.remove_check_count = 0;
	}

	return val;
}

static void emt_app_edlc_power_off_process(EMT_APP_INFO* p_emt_app_info)
{
	EMT_MSGQUEUE_T msg;

	if((p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON) && (p_emt_app_info->bp.cpu_off_wait == false))
	{
		if(p_emt_app_info->edlc.charge_process_enable == true)
		{
			if(p_emt_app_info->edlc.last_edlc >= EDLC_OFF_ENABLE_VOL)
			{
				msg.command = EMT_MSG_BP_OFF;
				msg.data_len = 0;
				osMessageQueuePut (appQueueHandle, &msg, 0, 0);
			}
			else
			{
				msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
				msg.data_len = 1;
				msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
				osMessageQueuePut (appQueueHandle, &msg, 0, 0);
			}
		}
		else
		{
			msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
			msg.data_len = 1;
			msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
			osMessageQueuePut (appQueueHandle, &msg, 0, 0);
		}
	}
}

static void emt_app_edlc_charging_process(EMT_APP_INFO* p_emt_app_info)
{
	uint32_t offset_cnt = 20;//50MS 타이머 기준 1초 offset

	// switch (timer_type)
	// {
	// case OS_TIMER_50MS:	offset_cnt = 20;	break;//__BB_COMTEC__
	// case OS_TIMER_250MS:	offset_cnt = 4;		break;//__BB_FINE__
	// default:				offset_cnt = 20;	break;//50ms
	// }

	if(p_emt_app_info->edlc.charge_process_enable == true)
	{
#if 0
		if(p_emt_app_info->edlc.last_edlc >= EDLC_OFF_ENABLE_VOL)
		{
			emt_gpio_micom_power_en(EMT_ON);
		}
		else
		{
			emt_gpio_micom_power_en(EMT_OFF);
		}
#endif
		if(p_emt_app_info->edlc.charging_start_delay_count < (5 * offset_cnt))	//5s
		{	/* 녹화 시작 (EMT_HCI_CID_SYSTEM_READY) 후 N초 이후부터 edlc 충전하도록 함. (소모전류 1A) */
			p_emt_app_info->edlc.charging_start_delay_count++;
			return;
		}
		p_emt_app_info->edlc.charging_start_delay_count = (5 * offset_cnt);
		
		if(p_emt_app_info->bp_type == EMT_BP_TYPE_V12 && \
				p_emt_app_info->adc_value.bp_v12 < CPU_ON_BP_10_0_V_ADC_MIN)
		{
			if(p_emt_app_info->edlc.charging_on == true)
			{
				p_emt_app_info->edlc.low_level_count = 0;
				p_emt_app_info->edlc.high_level_count = 0;
				emt_gpio_set_edlc_ctrl(false);
			}
		}
#if defined(__SUPPORT_24V_ADC_CHANNEL__)
		else if(p_emt_app_info->bp_type == EMT_BP_TYPE_V24 && \
				p_emt_app_info->adc_value.bp_v24 < CPU_ON_BP_20_0_V_ADC_MIN)
		{
			if(p_emt_app_info->edlc.charging_on == true)
			{
				p_emt_app_info->edlc.low_level_count = 0;
				p_emt_app_info->edlc.high_level_count = 0;
				emt_gpio_set_edlc_ctrl(false);
			}
		}
#else
		else if(p_emt_app_info->bp_type == EMT_BP_TYPE_V24 && \
				p_emt_app_info->adc_value.bp_v12 < CPU_ON_BP_20_0_V_ADC_MIN)
		{
			if(p_emt_app_info->edlc.charging_on == true)
			{
				p_emt_app_info->edlc.low_level_count = 0;
				p_emt_app_info->edlc.high_level_count = 0;
				emt_gpio_set_edlc_ctrl(false);
			}
		}
#endif
		else if(p_emt_app_info->edlc.temperature_warning_on == true)
		{
			if(p_emt_app_info->edlc.charging_on == true)
			{
				p_emt_app_info->edlc.low_level_count = 0;
				p_emt_app_info->edlc.high_level_count = 0;
				emt_gpio_set_edlc_ctrl(false);
			}
		}
		else
		{
			if(p_emt_app_info->adc_value.edlc >= EDLC_FULL_CHARGE_VOL)
			{
				if(p_emt_app_info->edlc.high_level_count >= (EDLC_FULL_CHARGE_COUNT * offset_cnt)) //over EDLC_FULL_CHARGE_COUNT seconds edlc charging
				{
					p_emt_app_info->edlc.charging_on = false;
				}
				else
				{
					if(p_emt_app_info->edlc.charging_on == true)
					{
						p_emt_app_info->edlc.high_level_count++;
					}
				}
			}
			else if(p_emt_app_info->adc_value.edlc < EDLC_RE_CHARGE_VOL) // edlc low level, charging start
			{
				p_emt_app_info->edlc.low_level_count = 0;
				p_emt_app_info->edlc.high_level_count = 0;
				p_emt_app_info->edlc.charging_on = true;
			}
			else
			{
				if(p_emt_app_info->edlc.charging_on == true)
				{
					p_emt_app_info->edlc.low_level_count++;
					if(p_emt_app_info->edlc.low_level_count > (90 * offset_cnt)) //control over charging
					{
						p_emt_app_info->edlc.low_level_count = 0;
						p_emt_app_info->edlc.charging_on = false;
					}
				}
			}
			emt_gpio_set_edlc_ctrl(p_emt_app_info->edlc.charging_on);
		}
	}
}

void emt_app_edlc_process(EMT_APP_INFO* p_emt_app_info)
{
	bool	bp_detect;

	bp_detect = emt_app_check_bp_detect(p_emt_app_info);

	if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
	{
		if(bp_detect == false)
		{
			emt_app_edlc_power_off_process(p_emt_app_info);
			p_emt_app_info->edlc.charge_process_enable = false;
			p_emt_app_info->edlc.charging_on = false;
			emt_gpio_set_edlc_ctrl(p_emt_app_info->edlc.charging_on);
		}
		else
		{
			emt_app_edlc_charging_process(p_emt_app_info);
		}
	}
	else
	{
		p_emt_app_info->edlc.last_edlc = 0;
		if(bp_detect == false)
		{
			emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
			p_emt_app_info->temperature.waiting_acc_on = false;
		}
	}
}

void emt_app_check_acc_status(EMT_APP_INFO *p_emt_app_info)
{
	if(emt_gpio_is_acc_detect())
	{
		p_emt_app_info->acc.on_check_count++;
		p_emt_app_info->acc.off_check_count = 0;

		if(p_emt_app_info->acc.on_check_count == 4)
		{
			if(p_emt_app_info->acc.on == false)
			{
				p_emt_app_info->temperature.waiting_acc_on = false;
			}

			p_emt_app_info->acc.on_check_count = 0;
			p_emt_app_info->acc.on = true;
			p_emt_app_info->cpu_watchdog.working_check_count = 0;
		}
	}
	else
	{
		p_emt_app_info->acc.on_check_count = 0;
		p_emt_app_info->acc.off_check_count++;

		if(p_emt_app_info->acc.off_check_count == 4)
		{
			p_emt_app_info->acc.off_check_count = 0;
			p_emt_app_info->acc.on = false;
		}
	}
}
#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
void emt_gpio_set_aqs_status(EMT_APP_INFO *p_emt_app_info)
{
	if(emt_gpio_is_acc_detect())
	{
		emt_gpio_set_aqs_mode(_AQS_BOOT_NORMAL);
	}
	else
	{
		if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
		{
			emt_gpio_set_aqs_mode(_AQS_BOOT_BDF);
		}
		else
		{
			if(p_emt_app_info->is_parking_rec_enable == true)
			{
				emt_gpio_set_aqs_mode(_AQS_BOOT_PARKING);
			}
			else
			{
				emt_gpio_set_aqs_mode(_AQS_BOOT_PARK_REC_OFF);
			}
		}
	}
}
#endif

EMT_BP_TYPE emt_app_get_bp_type(EMT_ADC_STRUCT_T *p_adc_value)
{
	EMT_BP_TYPE bp_type = EMT_BP_TYPE_V12;
//전원 사양 확인 후 적용 필요.
#if defined(__SUPPORT_24V_ADC_CHANNEL__)
#if (__TARGET_PROJECT__ == __CYCLOPS__)
	if (p_adc_value->bp_v12 < BP_TYPE_V12_ADC_MIN || p_adc_value->bp_v12 > BP_TYPE_V12_ADC_MAX)
	{
		if(p_adc_value->bp_v24 > BP_TYPE_V24_ADC_MIN)
		{
			bp_type = EMT_BP_TYPE_V24;
		}
	}
#else
	if (p_adc_value->bp_v12 > BP_TYPE_V12_ADC_MAX)
	{
		if(p_adc_value->bp_v24 > BP_TYPE_V24_ADC_MIN)
		{
			bp_type = EMT_BP_TYPE_V24;
		}
	}
#endif
#else
#if (__TARGET_PROJECT__ == __TRINITY__)
	if(p_adc_value->bp_v12 > BP_TYPE_V12_ADC_MAX)
	{
		{
			bp_type = EMT_BP_TYPE_V24;
		}
	}
#endif
#endif
	return bp_type;
}
#endif

static void emt_app_cpu_hci_cmd_proc_system_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_FW_VERSION://CMD_C2M_CPU_VERSION_ACK
		{
			if(p_hci_msg->payload[0] == 0)	//CPU 에서 ACC ON 판단.
			{
				if(g_emt_app_info.bbx_op_mode != EMT_BBX_OP_NORMAL_MODE)
				{
					g_emt_app_info.bbx_op_mode = EMT_BBX_OP_NORMAL_MODE;
				}
			}

			p_hci_msg->payload_length = 0;
			p_hci_msg->payload[p_hci_msg->payload_length++] = EMT_MICOM_VERSION_CHAR;
			p_hci_msg->payload[p_hci_msg->payload_length++] = EMT_MICOM_VERSION_MAJOR;
			p_hci_msg->payload[p_hci_msg->payload_length++] = '.';
			p_hci_msg->payload[p_hci_msg->payload_length++] = EMT_MICOM_VERSION_MINOR;
			p_hci_msg->payload[p_hci_msg->payload_length++] = '.';
			p_hci_msg->payload[p_hci_msg->payload_length++] = EMT_MICOM_VERSION_REVISOIN;
			p_hci_msg->payload[p_hci_msg->payload_length++] = '_';
			p_hci_msg->payload[p_hci_msg->payload_length++] = EMT_MICOM_VERSION_HIDDEN;
		#if defined(__SUPPORT_SEND_RESETTYPE_TO_CPU__)
			p_hci_msg->payload[p_hci_msg->payload_length++] = g_emt_app_info.cpu_reset_type;
		#endif
		}
		break;
	case EMT_HCI_CID_WATCHDOG_CLEAR:
		break;
	case EMT_HCI_CID_WATCHDOG_CONFIG:
		{
			if(p_hci_msg->payload[0] == 0)
			{
				g_emt_app_info.cpu_watchdog.mode = EMT_CPU_WATCHDOG_OFF;
			}
			else
			{
				g_emt_app_info.cpu_watchdog.mode = EMT_CPU_WATCHDOG_WORKING_CHECK;
			}
		}
		break;
#if defined(__MICOM_BOOTLOADER__)
#else
	case EMT_HCI_CID_SYSTEM_READY:
		{
			g_emt_app_info.edlc.charging_on = true;
			g_emt_app_info.edlc.charge_process_enable = true;
			emt_gpio_micom_power_en(EMT_ON);
		}
		break;

	case EMT_HCI_CID_TEMPERATURE_WARNING:
		{
			g_emt_app_info.edlc.temperature_warning_on = p_hci_msg->payload[0];
		}
		break;
	case EMT_HCI_CID_CPU_POWER_OFF:
		{
			EMT_MSGQUEUE_T msg;
			switch (p_hci_msg->payload[0])
			{
			case EMT_APP_CPU_POWER_OFF_SUSPEND:
				msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
				break;
			case EMT_APP_CPU_POWER_OFF_TEMPOFF:
				msg.data[0] = EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE;
				break;
			case EMT_APP_CPU_POWER_OFF_AI_TEMPOFF:
				msg.data[0] = EMT_BBX_OP_CUT_OFF_HIGH_TEMP_AI_MODE;
				break;
			case EMT_APP_CPU_POWER_OFF_BDF:
				{
					if(g_emt_app_info.bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
					{ /*하차캔슬 초기화 되지 않도록 함.*/
						g_emt_app_info.aqs.delay_count = 1;
					}
					else
					{	/*
						*  When the value of 'delay_time' is 0, the CPU enters the AQS mode
						* after 5 seconds while the parkingaqs value of the HW_TEST mode is active.
						*/
						if (g_emt_app_info.aqs.delay_time == 0)
						{
							g_emt_app_info.aqs.delay_count = 5;
						}
						else
						{
							g_emt_app_info.aqs.delay_count = g_emt_app_info.aqs.delay_time * 60;
						}
					}
					msg.data[0] = EMT_BBX_OP_PARKING_AQS_MODE;
				}
				break;
			case EMT_APP_CPU_POWER_OFF_AI_PARKING:
				g_emt_app_info.aqs.delay_count = g_emt_app_info.aqs.delay_time;
				msg.data[0] = EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE;
				break;
			default: /*if(p_hci_msg->payload[0] == EMT_APP_CPU_POWER_OFF)*/
				msg.data[0] = EMT_BBX_OP_SYSTEM_OFF_MODE;
				break;
			}

			msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
			msg.data_len = 1;
			osMessageQueuePut (appQueueHandle, &msg, 0, 0);
		}
		break;
#endif
	case EMT_HCI_CID_CPU_POWER_RESET:
		{
	#if defined(__MICOM_BOOTLOADER__)
	#else
			EMT_MSGQUEUE_T msg;
		#if defined(__SUPPORT_SEND_RESETTYPE_TO_CPU__)
			g_emt_app_info.cpu_reset_type = 0;
		#endif
			switch (p_hci_msg->payload[0])
			{
			case EMT_APP_CPU_POWER_RESET_NORMAL_TO_PARKING:	msg.data[0] = EMT_BBX_OP_PARKING_NORMAL_MODE;	break;
			case EMT_APP_CPU_POWER_RESET_PARKING_TO_NORMAL:	msg.data[0] = EMT_BBX_OP_NORMAL_MODE;			break;
			case EMT_APP_CPU_POWER_RESET_AI_PARKING:		msg.data[0] = EMT_BBX_OP_PARKING_AI_MODE;		break;
			default: 										msg.data[0] = g_emt_app_info.bbx_op_mode;		break;//EMT_APP_CPU_POWER_RESET
			}
			g_emt_app_info.need_cpu_acc_check = true;
		#if defined(__SUPPORT_SEND_RESETTYPE_TO_CPU__)
			g_emt_app_info.cpu_reset_type = p_hci_msg->payload[1];
		#endif
			msg.command = EMT_MSG_SET_CPU_RESET;
			msg.data_len = 1;
			osMessageQueuePut (appQueueHandle, &msg, 0, 0);
			g_emt_app_info.bbx_communication_on = false;
	#endif
		}
		break;
	case EMT_HCI_CID_MICOM_RESET_CPU_DCDC_OFF:
		{
			emt_status_led_control(EMT_APP_STATUS_LED_OFF);
			emt_uart_deinit();
			emt_led_process_timer_deinit();
			emt_front_led_deinit();
			emt_status_led_deinit();
			emt_gpio_cpu_powerctrl(EMT_CPU_POWER_OFF);
			HAL_Delay(50);
			emt_gpio_mcu_reset();
		}
		break;
	case EMT_HCI_CID_PERIODIC_DATA_ENABLE:
		{
			g_emt_app_info.periodic_data_mode = p_hci_msg->payload[0];
		}
		break;
#if defined(__SUPPORT_RTC_CONTROL__)
	case EMT_HCI_CID_RTC_TIME_GET:
		{
			if(g_emt_app_info.is_rtc_lost == true)
			{
				p_hci_msg->payload[0] = g_emt_app_info.is_rtc_lost;
				p_hci_msg->payload_length = 1;
				g_emt_app_info.is_rtc_lost = false;
			}
			else
			{
				EMT_HCI_RTC_INFO rtc_info;
				rtc_info.status = emt_rtc_read_time((emt_rtc_t *)rtc_info.time);

				memcpy(&p_hci_msg->payload[0], &rtc_info, sizeof(EMT_HCI_RTC_INFO));
				p_hci_msg->payload_length = sizeof(EMT_HCI_RTC_INFO);
			}
		}
		break;
	case EMT_HCI_CID_RTC_TIME_SET:
		{
			EMT_HCI_RTC_INFO rtc_info;
			//rtc set
			memcpy(rtc_info.time, p_hci_msg->payload, sizeof(emt_rtc_t));
			rtc_info.status = emt_rtc_set_time((emt_rtc_t *)rtc_info.time);

			//rtc set result send
			memcpy(&p_hci_msg->payload[0], &rtc_info, sizeof(EMT_HCI_RTC_INFO));
			p_hci_msg->payload_length = sizeof(EMT_HCI_RTC_INFO);
		}
		break;
#endif
	default:
		break;
	}
}

#if defined(ENABLE_HCI_GROUP_PARKING)
static void emt_app_cpu_hci_cmd_proc_parking_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_PARKING_EVENT_IGNORE_TIME:
		{
			// g_emt_app_info.aqs.delay_time = *((int16_t *)p_hci_msg->payload);
			memcpy(&g_emt_app_info.aqs.delay_time,p_hci_msg->payload,sizeof(int8_t));
		}
		break;
	case EMT_HCI_CID_PARKING_QUICK_PRESSED:
		{
			p_hci_msg->payload[0] = g_emt_app_info.is_key_easy_btn_on;
			p_hci_msg->payload_length = 1;
		}
		break;
	case EMT_HCI_CID_PARKING_CUT_OFF_VOL:
		{
			memcpy(&g_emt_app_info.cpu_cutoff.check_adc,p_hci_msg->payload,sizeof(int32_t));
			// g_emt_app_info.cpu_cutoff.check_adc = *((int32_t*)p_hci_msg->payload);

			g_emt_app_info.cpu_cutoff.check_count = 0;
			g_emt_app_info.cpu_cutoff.on = true;
		}
		break;
	case EMT_HCI_CID_PARKING_REC_ENABLE_SET:
		{	/* ZDR055 단말기 기준 ACC ON<->OFF 빠르게 전환 시 주차녹화 동작 싱크 맞추기위해 사용.
		 	 *  SiliconLaps							STM32
			 *  CMD_C2M_SET_TEMPPARKINGONOFF	==	EMT_HCI_CID_PARKING_REC_ENABLE_SET
			 *  g_byQuick_pressed				==	g_emt_app_info.is_parking_rec_enable
		  	 */
			g_emt_app_info.is_parking_rec_enable = p_hci_msg->payload[0];
		}
		break;

#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
	case EMT_HCI_CID_PARKING_MANUAL_ENABLE_SET:
		{
			g_emt_app_info.is_parking_manual_enable = p_hci_msg->payload[0];
		}
		break;
	case EMT_HCI_CID_PARKING_MANUAL_ACC_SET:
		{
			g_emt_app_info.is_parking_manual_set = p_hci_msg->payload[0];
		}
		break;
#endif
	default:
		break;
	}
}
#endif

#if defined(ENABLE_HCI_GROUP_GPS)
static void emt_app_cpu_hci_cmd_proc_gps_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
		case EMT_HCI_CID_GPS_RESET:
			break;
		case EMT_HCI_CID_GPS_CONFIG:
			break;
		case EMT_HCI_CID_GPS_MSG_PVT:
			break;
		case EMT_HCI_CID_GPS_MSG_SAT:
			break;
		case EMT_HCI_CID_GPS_MSG_RAW:
			break;
		case EMT_HCI_CID_GPS_SET_MSG:
			break;
		default:
			break;
	}
}
#endif

#if defined(ENABLE_HCI_GROUP_ADC)
static void emt_app_cpu_hci_cmd_proc_adc_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
		default:
			break;
	}
}
#endif

#if defined(ENABLE_HCI_GROUP_GSENSOR)
static void emt_app_cpu_hci_cmd_proc_gsensor_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_GSENSOR_SENSITIVIY_DATA:
		{
		#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
			if(p_hci_msg->payload_length > 0)
			{
				EMT_MSGQUEUE_T msg;
				msg.command = EMT_MSG_GSENSOR_SET_SENSITIVITY;
				memcpy((void*)&msg.data[0], (void*)p_hci_msg->payload, sizeof(EMT_GSENSOR_VALUE_T));
				msg.data_len = sizeof(EMT_GSENSOR_VALUE_T);
				osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
			}
		#endif
		}
		break;
	case EMT_HCI_CID_GSENSOR_PERIOD:
		break;
	default:
		break;
	}
}
#endif

#if defined(ENABLE_HCI_GROUP_LED)
static void emt_app_cpu_hci_cmd_proc_led_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_LED_PATTEN:
		{
			EMT_STATUS_LED_TYPE status_led_type = EMT_APP_STATUS_LED_OFF;
			#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
			if ( p_hci_msg->payload_length == sizeof(uint16_t) ) 
			{
				memcpy(&status_led_type, p_hci_msg->payload, sizeof(uint16_t));
			}
			else
			{
				status_led_type = (EMT_STATUS_LED_TYPE)p_hci_msg->payload[0];
			}
			#else
			status_led_type = (EMT_STATUS_LED_TYPE)p_hci_msg->payload[0];
			#endif
			
			emt_led_set_status_led(status_led_type);
		}
		break;
	case EMT_HCI_CID_FRONT_LED_PWM:
		{
			EMT_FRONT_LED_TYPE front_led_type = (EMT_FRONT_LED_TYPE)p_hci_msg->payload[0];
			if(front_led_type < EMT_APP_FRONT_LED_MAX)
			{
				emt_led_set_front_led(front_led_type);
			}
		}
		break;
#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
    //led test mode
	#if !defined(__MICOM_BOOTLOADER__)	
	case EMT_HCI_CID_LED_SET_RGB_INFO:
	{
		EMT_STATUS_LED_RGB_INFO *p_rgb_info = (EMT_STATUS_LED_RGB_INFO*)&p_hci_msg->payload[0];

		emt_status_led_rgb_test(p_rgb_info);
		break;
	}
	#endif
#endif
	default:
		break;
	}
}
#endif

#if defined(ENABLE_HCI_GROUP_RADAR)
static void emt_app_cpu_hci_cmd_proc_radar_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_RADAR_RESERVED1:
		break;
	default:
		break;
	}
}
#endif

static void emt_app_cpu_hci_cmd_proc_fw_update_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	int32_t ret;
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_ENTER_BOOTLOADER:
		{
			memcpy((uint8_t*)&g_emt_app_info.fw_dl_info.magic_number, p_hci_msg->payload, p_hci_msg->payload_length);
			ret = emt_fw_upgrade_write_newfw_info(&g_emt_app_info.fw_dl_info);
			if(ret == EMT_SUCCESS)
			{
		#if defined(__MICOM_BOOTLOADER__)
		#else
				EMT_MSGQUEUE_T msg;
				msg.command = EMT_MSG_ENTER_FW_DOWNLOAD_MODE;
				msg.data_len = 0;
				osMessageQueuePut (appQueueHandle, &msg, 0, 0);
				p_hci_msg->type = EMT_HCI_TYPE_NONE;
		#endif
			}
			else
			{
				p_hci_msg->payload[0] = ret;
				p_hci_msg->payload_length = 1;
			}
		}
		break;
	case EMT_HCI_CID_NEW_FW_INFO:
		{
			EMT_HCI_FW_UPDATE_INFO *p_fw_update_info	= (EMT_HCI_FW_UPDATE_INFO*)p_hci_msg->payload;
			g_emt_app_info.fw_dl_info.version 			= p_fw_update_info->version;
			g_emt_app_info.fw_dl_info.image_size 		= p_fw_update_info->fw_size;
			g_emt_app_info.fw_dl_info.number_of_packets = p_fw_update_info->number_of_packets;
		}
		break;
	case EMT_HCI_CID_NEW_FW_PACKET:
		{
			EMT_MICOM_FWUP_STATUS_T status = FWUP_STATUS_ERROR;
			EMT_HCI_FW_PACKET_INFO *p_fw_packet_info;

			//static bool flag = true;
			static uint8_t count = 0;

			if(p_hci_msg->payload_length > 0)
			{
				p_fw_packet_info = (EMT_HCI_FW_PACKET_INFO*)p_hci_msg->payload;
				#if 0
				if((flag == false) && ((p_fw_packet_info->address % (1024*11)) == 0) && count > 3)
				{
					flag = true;
					count = 0;
				}
				else
				#endif
				if((g_fw_dl_status.address < g_emt_app_info.fw_dl_info.image_size) && (g_fw_dl_status.address == p_fw_packet_info->address))
				{
					int ret = EMT_FAILURE;
					ret = emt_fw_upgrade_write_newfw_area(g_fw_dl_status.address, p_fw_packet_info->packet_len, p_fw_packet_info->packets);
					if(ret != EMT_SUCCESS)
					{
						status = FWUP_STATUS_ERROR_FLASH_WRITE;
					}
					else
					{
						ret = emt_fw_upgrade_verify_newfw_area(g_fw_dl_status.address, p_fw_packet_info->packet_len, p_fw_packet_info->packets);
						if(ret != EMT_SUCCESS)
						{
							status = FWUP_STATUS_ERROR_VERIFY;
						}
						else
						{
							g_fw_dl_status.fw_packet_index++;
							g_fw_dl_status.address += p_fw_packet_info->packet_len;
							status = FWUP_STATUS_OK;
						}
					}
					//flag = false;
					count++;
				}

				p_hci_msg->payload_length = 1;
				p_hci_msg->payload[0] = status;
			}
		}
		break;
	case EMT_HCI_CID_NEW_FW_AREA_ERASE:
		{
			uint32_t address;
			EMT_MICOM_FWUP_STATUS_T status = FWUP_STATUS_ERROR;
			if(p_hci_msg->payload_length > 0)
			{
				memcpy((uint8_t*)&address, &p_hci_msg->payload[0], sizeof(uint32_t));
				ret = emt_fw_upgrade_erase_newfw_area(address);
				if(ret == EMT_SUCCESS)
				{
					g_fw_dl_status.address = address;
					status = FWUP_STATUS_OK;
				}
				else
				{
					status = FWUP_STATUS_ERROR_ERASE;
				}
				p_hci_msg->payload_length = 1;
				p_hci_msg->payload[0] = status;
			}
		}
		break;
	default:
		break;
	}
}

#if defined(ENABLE_HCI_GROUP_DIAGNOSTIC)
static void emt_app_cpu_hci_cmd_proc_diagnostic_group(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	switch(p_hci_msg->cid)
	{
	case EMT_HCI_CID_DIAG_RESERVED1:
		break;
	default:
		break;
	}
}
#endif

void emt_app_cpu_hci_cmd_proc(EMT_HCI_PACKET_MSG *p_hci_msg)
{
	uint8_t cmd_type = 0;

	if(p_hci_msg == NULL)
		return;

	g_emt_app_info.bbx_communication_on = true;
	g_emt_app_info.bbx_receive_cmd_count++;

	switch(p_hci_msg->gid)
	{
	case EMT_HCI_GID_SYSTEM:
		{
			emt_app_cpu_hci_cmd_proc_system_group(p_hci_msg);
		}
		break;
#if defined(ENABLE_HCI_GROUP_PARKING)
	case EMT_HCI_GID_PARKING:
		{
			emt_app_cpu_hci_cmd_proc_parking_group(p_hci_msg);
		}
		break;
#endif
#if defined(ENABLE_HCI_GROUP_GPS)
	case EMT_HCI_GID_GPS:
		{
			emt_app_cpu_hci_cmd_proc_gps_group(p_hci_msg);
		}
		break;
#endif
#if defined(ENABLE_HCI_GROUP_ADC)
	case EMT_HCI_GID_ADC:
		{
			emt_app_cpu_hci_cmd_proc_adc_group(p_hci_msg);
		}
		break;
#endif
#if defined(ENABLE_HCI_GROUP_GSENSOR)
	case EMT_HCI_GID_GSENSOR:
		{
			emt_app_cpu_hci_cmd_proc_gsensor_group(p_hci_msg);
		}
		break;
#endif
#if defined(ENABLE_HCI_GROUP_LED)
	case EMT_HCI_GID_LED:
		{
			emt_app_cpu_hci_cmd_proc_led_group(p_hci_msg);
		}
		break;
#endif
#if defined(ENABLE_HCI_GROUP_RADAR)
	case EMT_HCI_GID_RADAR:
		{
			emt_app_cpu_hci_cmd_proc_radar_group(p_hci_msg);
		}
		break;
#endif
	case EMT_HCI_GID_FW_UPDATE:
		{
			emt_app_cpu_hci_cmd_proc_fw_update_group(p_hci_msg);
		}
		break;
#if defined(ENABLE_HCI_GROUP_DIAGNOSTIC)
	case EMT_HCI_GID_DIAGNOSTIC:
		{
			emt_app_cpu_hci_cmd_proc_diagnostic_group(p_hci_msg);
		}
		break;
#endif
	default:
		p_hci_msg->type = EMT_HCI_TYPE_NONE;
		break;
	}

	if(p_hci_msg->type == EMT_HCI_TYPE_CMD)
	{
		cmd_type = EMT_HCI_TYPE_RESP;
		emt_hci_send(p_hci_msg->src, cmd_type, p_hci_msg->gid, p_hci_msg->cid, p_hci_msg->payload, p_hci_msg->payload_length);
		if((p_hci_msg->gid == EMT_HCI_GID_SYSTEM) && (p_hci_msg->cid == EMT_HCI_CID_CPU_POWER_OFF))
		{
			HAL_Delay(10);
			g_emt_app_info.cpu_watchdog.mode = EMT_CPU_WATCHDOG_OFF;
		}
	}
}


EMT_SYSTEM_POWER_MODE emt_app_get_system_mode(void)
{
	return g_emt_app_info.bbx_op_mode;
}
