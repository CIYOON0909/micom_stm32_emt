/**
  ******************************************************************************
  * @file           : emt_app_main.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include "string.h"
#include "cmsis_os.h"
#include "main.h"
#include "emt_version.h"
#include "emt_types.h"
#include "emt_app.h"
#include "emt_fw_upgrade.h"
#include "emt_uart_rx.h"
#include "emt_hci.h"
#include "emt_led.h"
#include "emt_gpio.h"
#include "emt_adc.h"
#include "emt_rtc.h"
#include "emt_key.h"

/* Private macro -------------------------------------------------------------*/
#define ENABLE_CPU_WATCHDOG_RESET

#define HEX2ASCII( a ) ((a) >= 0x0A ? ((a) - 0x0A + 'A'): ((a)+'0'))
#define ASCII2HEX( a ) ((a) >= 'A' ? ((a) -'A' + 0x0A): ((a)-'0'))

#define WATCHDOG_COUNT_CPU_BOOT 			(5)			//(s)
#define WATCHDOG_COUNT_CPU_WORKING			(30)		//(s)
#define WATCHDOG_COUNT_CPU_BOOT_RETRY_MAX	(5)			//(count)


#define EMT_KEY_SW_EASY_ID					(0x01)
#if(__TARGET_PROJECT__ == __PRIME__)
#define EMT_KEY_SW_FN_ID					(0x02)
#endif

/* Private variables ---------------------------------------------------------*/
#define OS_TIMER_PERIODIC_TICKS_50MS		(50)
#define OS_TIMER_PERIODIC_TICKS_100MS		(100)
#define OS_TIMER_PERIODIC_TICKS_200MS		(200)
#define OS_TIMER_PERIODIC_TICKS_250MS		(250)
#define OS_TIMER_PERIODIC_TICKS_500MS		(500)
#define OS_TIMER_PERIODIC_TICKS_1000MS		(1000)

#define TEMP_WAKEUP_65Degree		533//Trinity 65 max adc. MAGNUS 65 ADC.

typedef struct {
	osTimerId_t 	timer_id;
	osTimerFunc_t	timer_func;
	osTimerType_t	timer_type;
	uint32_t		timer_tick;
	bool			timer_valid;
} EMT_OS_TIMER_INFO_T;

static EMT_OS_TIMER_INFO_T g_emt_os_timer_info[OS_TIMER_MAX] = {0};
static bool g_periodic_timer_init = false;
uint32_t g_periodic_check_tick = 0;

static EMT_APP_INFO *p_emt_app_info;
/* External variables --------------------------------------------------------*/
extern osMessageQueueId_t	appQueueHandle;
extern osMessageQueueId_t	gSensorQueueHandle;

/* Private functions ---------------------------------------------------------*/


void emt_gsensor_wakeup_isr_handler(void)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_GSENSOR_WAKE_UP;
	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

void emt_bp_off_isr_handler(void)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_BP_OFF;
	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_50ms_callback (void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_50MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_100ms_callback(void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_100MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_200ms_callback(void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_200MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_250ms_callback (void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_250MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_500ms_callback(void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_500MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}

static void emt_timer_periodic_1000ms_callback (void *arg)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_PERIODIC_1000MSEC;

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
}


static void emt_periodic_timer_setting(uint8_t type)
{
	bool sett_valid = true;

	switch (type)
	{
	case OS_TIMER_50MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_50ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerOnce;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_50MS;
		break;
	case OS_TIMER_100MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_100ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerOnce;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_100MS;
		break;
	case OS_TIMER_200MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_200ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerOnce;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_200MS;
		break;
	case OS_TIMER_250MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_250ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerOnce;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_250MS;
		break;
	case OS_TIMER_500MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_500ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerOnce;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_500MS;
		break;
	case OS_TIMER_1000MS:
		g_emt_os_timer_info[type].timer_func = emt_timer_periodic_1000ms_callback;
		g_emt_os_timer_info[type].timer_type = osTimerPeriodic;
		g_emt_os_timer_info[type].timer_tick = OS_TIMER_PERIODIC_TICKS_1000MS;
		break;
	default:
		sett_valid = false;
		break;
	}

	if (sett_valid == false)
	{
		return;
	}

	#if 0
	g_emt_os_timer_info[type].timer_id = osTimerNew(g_emt_os_timer_info[type].timer_func, g_emt_os_timer_info[type].timer_type, NULL, NULL);
	if (g_emt_os_timer_info[type].timer_id != NULL)
	{
		osTimerStart(g_emt_os_timer_info[type].timer_id, g_emt_os_timer_info[type].timer_tick);
		g_emt_os_timer_info[type].timer_valid = true;
	}
	#else
	g_emt_os_timer_info[type].timer_id = osTimerNew(g_emt_os_timer_info[type].timer_func, g_emt_os_timer_info[type].timer_type, NULL, NULL);
	if (g_emt_os_timer_info[type].timer_id != NULL)
	{
		int retry_cnt = 0;
		osStatus_t os_status = osStatusReserved;
		do {
			os_status = osTimerStart(g_emt_os_timer_info[type].timer_id, g_emt_os_timer_info[type].timer_tick);
		} while ((os_status != osOK) && (retry_cnt++ < 3));

		if (os_status == osOK)
		{
			g_emt_os_timer_info[type].timer_valid = true;
		}
	}
	#endif
}

static void emt_timer_init(void)
{
	if (g_periodic_timer_init == true) return;

#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
	emt_periodic_timer_setting(OS_TIMER_50MS);
#elif (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	emt_periodic_timer_setting(OS_TIMER_250MS);
#endif
	emt_periodic_timer_setting(OS_TIMER_1000MS);

	g_periodic_timer_init = true;
}


static void emt_timer_deinit(void)
{
	if (g_periodic_timer_init == false)	return;

	int i=0;
	for (i=0; i<OS_TIMER_MAX; i++)
	{
		if (g_emt_os_timer_info[i].timer_valid == true)
		{
//			osStatus_t os_status = osStatusReserved;
//			os_status = osTimerStop(g_emt_os_timer_info[i].timer_id);
//			if (os_status == osOK)
			{
				g_emt_os_timer_info[i].timer_id		= NULL;
				g_emt_os_timer_info[i].timer_func	= NULL;
				g_emt_os_timer_info[i].timer_type	= 0;
				g_emt_os_timer_info[i].timer_tick	= 0;
				g_emt_os_timer_info[i].timer_valid	= false;
			}
		}
	}

	g_periodic_timer_init = false;
}

static void emt_app_key_evt_handler(uint8_t key_id, emt_key_click_type_t key_click_type)
{
	int32_t ret = EMT_SUCCESS;
	EMT_MSGQUEUE_T msg;

	if (((key_id == EMT_KEY_SW_EASY_ID)
#if(__TARGET_PROJECT__ == __PRIME__)
		|| (key_id == EMT_KEY_SW_FN_ID)
#endif
			) &&
		((key_click_type == APP_KEY_SINGLE_CLICK) || (key_click_type == APP_KEY_LONG_CLICK) || (key_click_type == APP_KEY_CONTINUE_CLICK)))
	{

	}
	else
	{
		ret = EMT_FAILURE;
	}

	if(ret == EMT_SUCCESS)
	{
		msg.command = EMT_MSG_KEY_PROCESS;
		msg.data[0] = key_id;
		msg.data[1] = key_click_type;
		msg.data_len = 2;
		osMessageQueuePut (appQueueHandle, &msg, 0, 0);
	}
}

static void emt_app_key_init(void)
{
	emt_key_gpio_t key_inst[__EMT_KEY_COUNT_MAX__] = { 0 };
	int8_t index = 0;
#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
		/* EASY KEY *********************************/
	key_inst[index].key_id = EMT_KEY_SW_EASY_ID;
	key_inst[index].port = __SW_EASY_MCU_GPIO_Port;

	key_inst[index].gpio.Pin = __SW_EASY_MCU_Pin;
	key_inst[index].gpio.Mode = GPIO_MODE_IT_FALLING;
	key_inst[index].gpio.Pull = GPIO_PULLUP;
	key_inst[index].gpio.Speed = GPIO_SPEED_FREQ_LOW;
	key_inst[index].is_skip_release = false;
#if(__TARGET_PROJECT__ == __PRIME__)
	index++;
	key_inst[index].key_id = EMT_KEY_SW_FN_ID;
	key_inst[index].port = __SW_FNKEY_MCU_GPIO_Port;

	key_inst[index].gpio.Pin = __SW_FNKEY_MCU_Pin;
	key_inst[index].gpio.Mode = GPIO_MODE_IT_FALLING;
	key_inst[index].gpio.Pull = GPIO_PULLUP;
	key_inst[index].gpio.Speed = GPIO_SPEED_FREQ_LOW;
	key_inst[index].is_skip_release = false;
#endif

	emt_key_init(&key_inst[0], __EMT_KEY_COUNT_MAX__, emt_app_key_evt_handler);

	for(index = 0; index < __EMT_KEY_COUNT_MAX__; index++)
	{
		HAL_GPIO_DeInit(key_inst[index].port, key_inst[index].gpio.Pin);
		HAL_GPIO_Init(key_inst[index].port, &key_inst[index].gpio);
		
		if(key_inst[index].gpio.Pin & (GPIO_PIN_0|GPIO_PIN_1))
		{
			HAL_NVIC_SetPriority(EXTI0_1_IRQn, 3, 0);
			HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
		}
		else if(key_inst[index].gpio.Pin & (GPIO_PIN_2|GPIO_PIN_3))
		{
			HAL_NVIC_SetPriority(EXTI2_3_IRQn, 3, 0);
			HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
		}
		else if(key_inst[index].gpio.Pin & 0xFFF0) // GPIO_PIN_4 ~ GPIO_PIN_15
		{
			HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
			HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
		}		
	}
#endif
}

bool check_system_reset(void)
{
	EMT_MSGQUEUE_T msg;

	if(emt_gpio_is_bp_detect() == false) return false;
	if(emt_gpio_is_usb_detect() == true) return false;
	if(emt_gpio_is_acc_detect() == true) return false;

	p_emt_app_info->cpu_watchdog.working_check_count++;

	if(p_emt_app_info->cpu_watchdog.working_check_count >= WATCHDOG_COUNT_CPU_BOOT_RETRY_MAX)
	{
		p_emt_app_info->cpu_watchdog.working_check_count = 0;

		msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
		msg.data_len = 1;
		msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
		osMessageQueuePut (appQueueHandle, &msg, 0, 0);
		
		return true;
	
	}
	return false;

}



void emt_app_proc_task(void *argument)
{
	osStatus_t status = osOK;
	EMT_MSGQUEUE_T msg;
	EMT_FW_UPGRADE_INFO_T fw_download_info;
	EMT_FW_VER_INFO_T fw_ver_info;
	int16_t skip_cnt = 5;

	emt_app_init(&p_emt_app_info);
	emt_adc_init();
	emt_timer_init();
#if defined(__SUPPORT_RTC_CONTROL__)
	emt_rtc_init();
#endif
	emt_fw_upgrade_check_newfw_info(&fw_download_info);

	if(fw_download_info.magic_number != 0xFFFFFFFF)
	{
		emt_fw_upgrade_erase_newfw_info();
	}
	emt_app_key_init();
	emt_version_get_info(&fw_ver_info);
	emt_adc_get_value(&p_emt_app_info->adc_value);

	if((emt_gpio_is_bp_detect() == true) && (emt_app_is_bp_cutoff(p_emt_app_info) == false) && (emt_gpio_is_acc_detect() == true))
	{
		p_emt_app_info->acc.on = true;
		p_emt_app_info->temperature.waiting_acc_on = false;
		msg.command = EMT_MSG_SET_CPU_POWER_UP;
		msg.data_len = 1;
		msg.data[0] = EMT_BBX_OP_NORMAL_MODE;
	}
	else
	{
		p_emt_app_info->acc.on = false;
		msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
		msg.data_len = 1;
		msg.data[0] = EMT_BBX_OP_SYSTEM_OFF_MODE;
	}

	osMessageQueuePut (appQueueHandle, &msg, 0, 0);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		status = osMessageQueueGet (appQueueHandle, &msg, NULL, osWaitForever);
		if (status == osOK)
		{
			switch(msg.command)
			{
			case EMT_MSG_KEY_PROCESS:
				{
					if(msg.data_len > 0)
					{
						if(msg.data[0] == EMT_KEY_SW_EASY_ID)
						{
							if(msg.data[1] == APP_KEY_LONG_CLICK)
							{
								if((p_emt_app_info->bbx_op_mode != EMT_BBX_OP_CUT_OFF_MODE) && (p_emt_app_info->bbx_op_mode != EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE))
								{
									if ((emt_gpio_is_bp_detect() == true) && (emt_app_is_bp_cutoff(p_emt_app_info) == false) && (emt_gpio_is_acc_detect() == false))
									{
										p_emt_app_info->is_key_easy_btn_on = true;
										p_emt_app_info->is_parking_rec_enable = true;
										if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
										{
											msg.command = EMT_MSG_SET_CPU_POWER_UP;
											msg.data_len = 1;
											msg.data[0] = EMT_BBX_OP_PARKING_NORMAL_MODE;
											osMessageQueuePut (appQueueHandle, &msg, 0, 0);
										}
									}
								}
							}
						}
					}
				}
				break;
		#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
			case EMT_MSG_PERIODIC_50MSEC:
				{
					#if 0
					static uint32_t curr_tick = 0;
					if (curr_tick != 0) g_periodic_check_tick = HAL_GetTick() - curr_tick;
					curr_tick = HAL_GetTick();
					#endif
					skip_cnt --;
					if(skip_cnt == 0)
					{
						emt_app_check_acc_status(p_emt_app_info);
					#if (__TYPE_ACC_CONNECTED__ == __ACC_CONNECTED_ONLY_MCU__)
						if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
						{
							emt_gpio_set_aqs_status(p_emt_app_info);
						}
					#endif
						skip_cnt = 5;
					}

					emt_adc_get_value(&p_emt_app_info->adc_value);
					emt_app_edlc_process(p_emt_app_info);
					
					if ((g_emt_os_timer_info[OS_TIMER_50MS].timer_valid == true) && (g_emt_os_timer_info[OS_TIMER_50MS].timer_type == osTimerOnce))
					{
						osTimerStart(g_emt_os_timer_info[OS_TIMER_50MS].timer_id, g_emt_os_timer_info[OS_TIMER_50MS].timer_tick);
					}
				}
				break;		
		#elif (__TYPE_BB_SCENARIO__ == __BB_FINE__)
			case EMT_MSG_PERIODIC_250MSEC:
				{
					emt_adc_get_value(&p_emt_app_info->adc_value);
					emt_app_edlc_process(p_emt_app_info);
					if ((g_emt_os_timer_info[OS_TIMER_250MS].timer_valid == true) && (g_emt_os_timer_info[OS_TIMER_250MS].timer_type == osTimerOnce))
					{
						osTimerStart(g_emt_os_timer_info[OS_TIMER_250MS].timer_id, g_emt_os_timer_info[OS_TIMER_250MS].timer_tick);
					}
				}
				break;
		#endif
			case EMT_MSG_PERIODIC_1000MSEC:
				{
					p_emt_app_info->bp_type = emt_app_get_bp_type(&p_emt_app_info->adc_value);

					if(p_emt_app_info->bp.cpu_off_wait == true)
					{
						if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
						{
							p_emt_app_info->bp.cpu_off_wait_count++;
							if(p_emt_app_info->bp.cpu_off_wait_count == 3)
							{
								p_emt_app_info->bp.cpu_off_wait = false;
								p_emt_app_info->bp.cpu_off_wait_count = 0;
								p_emt_app_info->system_power_mode = EMT_CPU_POWER_OFF;
							}
						}
						else
						{
							p_emt_app_info->bp.cpu_off_wait = false;
							p_emt_app_info->bp.cpu_off_wait_count = 0;
						}
					}

				#if defined (__SUPPORT_TEMP_ADC_CONVERT__)
					emt_app_check_temperature(p_emt_app_info);
				#endif
					if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
					{
						if(p_emt_app_info->aqs.delay_count > 0)
						{
							p_emt_app_info->aqs.delay_count--;
						#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
							if(p_emt_app_info->aqs.delay_count == 0)
							{
								if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
								{
                                    #if(__TARGET_PROJECT__ == __PRIME__)
                                    emt_led_set_status_led(EMT_APP_STATUS_LED_R_PATTERN);
                                    #elif(__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
									emt_led_set_status_led(EMT_APP_STATUS_LED_PARKING_AQS_MODE);
									emt_led_set_front_led(EMT_APP_FRONT_LED_OFF);
									#else
									#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
									if(is_manual_parking_enable_get())
									{
										emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
									}
									else
									#endif
									{
										emt_led_set_status_led(EMT_APP_STATUS_LED_G_ON);
									}
                                    #endif
								}
							#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_INTERRUPT_MODE__)
								emt_gpio_set_gsensoer_interrupt(true);
							#endif
							}
						#endif
						}
						if((emt_gpio_is_bp_detect() == true) && (emt_app_is_bp_cutoff(p_emt_app_info) == false) && (p_emt_app_info->acc.on == true))
						{
							// p_emt_app_info->temperature.waiting_acc_on = false;
							p_emt_app_info->need_cpu_acc_check = true;
							p_emt_app_info->is_parking_rec_enable = false;
							msg.command = EMT_MSG_SET_CPU_POWER_UP;
							msg.data_len = 1;
							msg.data[0] = EMT_BBX_OP_NORMAL_MODE;
							osMessageQueuePut (appQueueHandle, &msg, 0, 0);
						}
					#if (__TYPE_BB_SCENARIO__ == __BB_FINE__) //|| (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
						else
						{
							emt_app_check_cut_off_process(p_emt_app_info);
						}
					#endif

						if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_CUT_OFF_HIGH_TEMP_AI_MODE)
						{
							if ((emt_gpio_is_bp_detect() == true) && (emt_app_is_bp_cutoff(p_emt_app_info) == false) && (emt_gpio_is_acc_detect() == false))
							{
								
								if(p_emt_app_info->adc_value.temperature   > TEMP_WAKEUP_65Degree ) p_emt_app_info->temperature.check_count++;
								else p_emt_app_info->temperature.check_count=0;

								if(p_emt_app_info->temperature.check_count == 60)
								{
									p_emt_app_info->is_parking_rec_enable = true;
									if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
									{
										msg.command = EMT_MSG_SET_CPU_POWER_UP;
										msg.data_len = 1;
										msg.data[0] = EMT_BBX_OP_PARKING_NORMAL_MODE;
										osMessageQueuePut (appQueueHandle, &msg, 0, 0);
									}
								}	
							}
						}
				
					}
					else
					{
						if(emt_gpio_is_usb_detect() == true)
						{
							emt_led_set_status_led(EMT_APP_STATUS_LED_FW_UPDATE);
							break;
						}

						if(p_emt_app_info->bbx_communication_on == true)
						{
							if(p_emt_app_info->periodic_data_mode & EMT_PERIODIC_DATA_ADC)
							{
								EMT_HCI_PERIODIC_ADC_DATA periodic_adc;
							#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
								periodic_adc.bp_type = p_emt_app_info->bp_type;
								periodic_adc.bp_v12 = p_emt_app_info->adc_value.bp_v12;
								periodic_adc.bp_v24 = p_emt_app_info->adc_value.bp_v24;
								periodic_adc.edlc = p_emt_app_info->adc_value.edlc;
								periodic_adc.temperature_value = p_emt_app_info->temperature.value;
							#else//lif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
								periodic_adc.bp_type = p_emt_app_info->bp_type;
								#if defined (__SUPPORT_24V_ADC_CHANNEL__)
								periodic_adc.bp_v24 = p_emt_app_info->adc_value.bp_v24;
								#endif
								periodic_adc.bp_v12 = p_emt_app_info->adc_value.bp_v12;
								periodic_adc.edlc = p_emt_app_info->adc_value.edlc;
								#if defined (__SUPPORT_TEMP_ADC_CONVERT__)
								periodic_adc.temperature_value = p_emt_app_info->temperature.value;
								#endif
								periodic_adc.temperature_adc = p_emt_app_info->adc_value.temperature;
							#endif
							#if defined(__STM32_TEMPERATURE_ADC__)
								periodic_adc.internal_temperature_adc = p_emt_app_info->adc_value.temperature;
								periodic_adc.internal_reference_voltage = p_emt_app_info->adc_value.internal_reference_voltage;
								periodic_adc.internal_temperature = p_emt_app_info->adc_value.internal_temperature;
							#endif
								emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_ADC, EMT_HCI_CID_ADC_PERIODIC_DATA, (uint8_t*)&periodic_adc, sizeof(EMT_HCI_PERIODIC_ADC_DATA));
							}
							
							if(p_emt_app_info->periodic_data_mode & EMT_PERIODIC_DATA_ACC_STATE)
							{
								uint8_t acc_off_check =0;
								acc_off_check = !p_emt_app_info->acc.on;
								emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_SYSTEM, EMT_HCI_CID_ACC_STATE, &acc_off_check, sizeof(uint8_t));
							}
						}

					#ifdef ENABLE_CPU_WATCHDOG_RESET
						if(p_emt_app_info->cpu_watchdog.mode != EMT_CPU_WATCHDOG_OFF)
						{
							p_emt_app_info->cpu_watchdog.discount--;
							if(p_emt_app_info->cpu_watchdog.discount < 0)
							{
								if(p_emt_app_info->cpu_watchdog.mode == EMT_CPU_WATCHDOG_BOOT_CHECK)
								{
									if(emt_gpio_is_acc_detect() == false)
									{
										p_emt_app_info->cpu_watchdog.boot_check_count++;
										if(p_emt_app_info->cpu_watchdog.boot_check_count > WATCHDOG_COUNT_CPU_BOOT_RETRY_MAX)
										{
											msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
											msg.data_len = 1;
											msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
											osMessageQueuePut (appQueueHandle, &msg, 0, 0);
											break;
										}
									}
								}
								msg.command = EMT_MSG_SET_CPU_RESET;
								msg.data_len = 1;
								msg.data[0] = p_emt_app_info->bbx_op_mode;
								osMessageQueuePut (appQueueHandle, &msg, 0, 0);
								p_emt_app_info->bbx_communication_on = false;
							}
						}

						//check the DC2DC EN after recording
						if(p_emt_app_info->edlc.charge_process_enable == true)
						{
						#if (__TYPE_CPU_CHIP_ID__ == __ICATCH_SOC_V57__)\
							&& ((__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__))
							if (emt_gpio_check_PGood_status(GPIO_PIN_SET) == false)
						#else
							if (emt_gpio_is_dc2dc_enable() == false)
						#endif
							{
								if(emt_gpio_is_acc_detect() == false)
								{
									p_emt_app_info->cpu_error_reset_count++;
									if(p_emt_app_info->cpu_error_reset_count > WATCHDOG_COUNT_CPU_BOOT_RETRY_MAX)
									{
										msg.command = EMT_MSG_SET_CPU_POWER_DOWN;
										msg.data_len = 1;
										msg.data[0] = EMT_BBX_OP_CUT_OFF_MODE;
										osMessageQueuePut (appQueueHandle, &msg, 0, 0);
										break;
									}
								}
								else
								{
									p_emt_app_info->cpu_error_reset_count = 0;
								}
								msg.command = EMT_MSG_SET_CPU_RESET;
								msg.data_len = 1;
								msg.data[0] = p_emt_app_info->bbx_op_mode;
								osMessageQueuePut (appQueueHandle, &msg, 0, 0);
								p_emt_app_info->bbx_communication_on = false;
								break;
							}
						}
					#endif
					}

					if ((g_emt_os_timer_info[OS_TIMER_1000MS].timer_valid == true) && (g_emt_os_timer_info[OS_TIMER_1000MS].timer_type == osTimerOnce))
					{
						osTimerStart(g_emt_os_timer_info[OS_TIMER_1000MS].timer_id, g_emt_os_timer_info[OS_TIMER_1000MS].timer_tick);
					}
				}
				break;
			case EMT_MSG_GSENSOR_WAKE_UP:
				{
				#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_INTERRUPT_MODE__)
					emt_gpio_set_gsensoer_interrupt(false);

					if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
					{
					#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
						if(is_manual_parking_enable_get())
						{
							msg.command = EMT_MSG_SET_CPU_POWER_UP;
							msg.data_len = 1;
							msg.data[0] = EMT_BBX_OP_NORMAL_MODE;
							p_emt_app_info->is_parking_manual_set=false;
							osMessageQueuePut (appQueueHandle, &msg, 0, 0);
						}
						else
					#endif
						{
							if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
							{
								msg.command = EMT_MSG_SET_CPU_POWER_UP;
								msg.data_len = 1;
								msg.data[0] = EMT_BBX_OP_PARKING_AQS_MODE;
								osMessageQueuePut (appQueueHandle, &msg, 0, 0);
							}
						}
					}
				#endif
				}
				break;
			case EMT_MSG_BP_OFF:
				{
					// osDelay(100);
					// emt_adc_get_value(&p_emt_app_info->adc_value);
					// if((emt_gpio_is_bp_detect() == false) || (emt_app_is_bp_cutoff(p_emt_app_info) == false))
					if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
					{
						p_emt_app_info->periodic_data_mode = EMT_PERIODIC_DATA_ALL_OFF;
						p_emt_app_info->bp.cpu_off_wait = true;
						emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_SYSTEM, EMT_HCI_CID_BP_DETACHED, NULL, 0);
						emt_led_set_front_led(EMT_APP_FRONT_LED_OFF);
						emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
						// emt_led_process_timer_deinit();
						// emt_front_led_deinit();
						// emt_status_led_deinit();
					}
				}
				break;
			case EMT_MSG_RECV_NEW_HCI_CPU_CMD:
				{
					EMT_HCI_PACKET_MSG *p_hci_msg = (EMT_HCI_PACKET_MSG*)msg.data;
					if((p_hci_msg->gid == EMT_HCI_GID_SYSTEM) && (p_hci_msg->cid == EMT_HCI_CID_LOOPBACK))
					{
					}
					else
					{
						emt_app_cpu_hci_cmd_proc(p_hci_msg);
					}
					p_emt_app_info->cpu_watchdog.discount = WATCHDOG_COUNT_CPU_WORKING;
					//p_emt_app_info->cpu_watchdog.mode = EMT_CPU_WATCHDOG_WORKING_CHECK;
				}
				break;
			case EMT_MSG_RECV_GSENSOR_DATA:
				{
				#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
					if(msg.data_len > 0)
					{
						switch(msg.data[0])
						{
						case EMT_GSENSOR_OP_NORMAL:
							{
								int16_t gsensor_value[3];
								memcpy(gsensor_value, &msg.data[1], sizeof(gsensor_value));
								if(p_emt_app_info->periodic_data_mode & EMT_PERIODIC_DATA_GSENSOR)
								{
									emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_GSENSOR, EMT_HCI_CID_GSENSOR_PERIODIC_DATA, (uint8_t*)&msg.data[1], sizeof(EMT_GSENSOR_VALUE_T));
								}
							}
							break;
					#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
						case EMT_GSENSOR_OP_PARKING_AI:
							{
								if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
								{
									if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE)
									{
										int8_t AI_Status;
										AI_Status = msg.data[1];

										if(AI_Status > 1)
										{
											//memcpy(&p_emt_app_info->ai_parking, &msg.data[2], sizeof(structAI));
											msg.command = EMT_MSG_SET_CPU_POWER_UP;
											msg.data_len = 1;
											msg.data[0] = EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE;
											osMessageQueuePut (appQueueHandle, &msg, 0, 0);
										}
									}
								}
								else
								{
									msg.data_len--;
									if(msg.data_len > 0)
									{
										//emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_PARKING, EMT_HCI_CID_AI_PARKING_EVENT, (uint8_t*)&msg.data[1], sizeof(structAI)+1);
									}
								}
							}
							break;
					#endif
						case EMT_GSENSOR_OP_PARKING_LOW_POWER:
							{
								if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
								{
									if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
									{
										if(p_emt_app_info->aqs.delay_count == 0 || p_emt_app_info->aqs.delay_time == 0 )
										{
											msg.command = EMT_MSG_SET_CPU_POWER_UP;
											msg.data_len = 1;
											msg.data[0] = EMT_BBX_OP_PARKING_AQS_MODE;
											osMessageQueuePut (appQueueHandle, &msg, 0, 0);
										}
									}
								}
							}
							break;
						default:
							break;
						}
					}
				#endif
				}
				break;
			case EMT_MSG_SET_CPU_POWER_UP:
				{
					if(msg.data_len > 0)
					{
						if(p_emt_app_info->temperature.waiting_acc_on == false)
						{
							emt_led_set_front_led(EMT_APP_FRONT_LED_OFF);
							emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
							emt_led_process_timer_deinit();

							p_emt_app_info->bbx_op_mode = msg.data[0];
							if(p_emt_app_info->system_power_mode != EMT_CPU_POWER_ON)
							{
								SystemClock_Config();
								osDelay(10);
								if(emt_gpio_cpu_powerctrl(EMT_CPU_POWER_ON) == false)
								{
									emt_gpio_mcu_reset();
								}
								p_emt_app_info->system_power_mode = EMT_CPU_POWER_ON;
							#if defined(__SUPPORT_RTC_CONTROL__)
								p_emt_app_info->is_rtc_lost = emt_is_rtc_lost();
							#endif
								emt_uart_init();
								emt_adc_deinit();
								emt_adc_init();
								emt_timer_init();
								emt_status_led_init();
							#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
								emt_led_set_status_led(EMT_APP_STATUS_LED_O_ON);
							#else
							#if (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
							#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
								emt_led_set_status_led(EMT_APP_STATUS_BOOT_ON);
							#endif
							#endif
							#endif
							}

						#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
							if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AI_MODE)
							{
								emt_gpio_set_aqs_mode(_BOOT_LOW_PARKING_EVT);
							}
							else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE)
							{
								p_emt_app_info->temperature.waiting_acc_on = true;
								emt_gpio_set_aqs_mode(_BOOT_HIGH_TEMP_EVT);
							}
							else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE)
							{
								emt_gpio_set_aqs_mode(_BOOT_SUPER_LOW_PARKING_EVT);
							}
							else
							{
								emt_gpio_set_aqs_mode(_BOOT_NORMAL);
							}
						#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
							emt_gpio_set_aqs_status(p_emt_app_info);
							#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
							if(p_emt_app_info->need_cpu_acc_check == true)
							{
								p_emt_app_info->need_cpu_acc_check = false;
								emt_gpio_set_acc_check_mode(true);
							}
							#endif
						#endif

					#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
						#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
							if(p_emt_app_info->bbx_op_mode != EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE)
							{
								msg.command = EMT_MSG_GSENSOR_OP_MODE;
								msg.data_len = 1;
								msg.data[0] = EMT_GSENSOR_OP_NORMAL;

								osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
							}
						#endif
					#elif (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_INTERRUPT_MODE__)
							emt_gpio_set_gsensoer_interrupt(false);
					#endif
							p_emt_app_info->bbx_communication_on = false;
							p_emt_app_info->bbx_receive_cmd_count = 0;
							p_emt_app_info->cpu_watchdog.boot_check_count = 0;
							p_emt_app_info->cpu_watchdog.working_check_count = 0;
							p_emt_app_info->cpu_watchdog.discount = WATCHDOG_COUNT_CPU_BOOT;
							p_emt_app_info->cpu_watchdog.mode = EMT_CPU_WATCHDOG_BOOT_CHECK;
							p_emt_app_info->cpu_error_reset_count = 0;
							p_emt_app_info->periodic_data_mode = EMT_PERIODIC_DATA_ALL_ON;
							memset(&p_emt_app_info->edlc, 0x00, sizeof(EMT_EDLC_CHARGE_STRUCT_T));
							#if(__TARGET_PROJECT__ == __PRIME__)
							#else							
							emt_led_process_timer_init();							
							#endif
							emt_adc_get_value(&p_emt_app_info->adc_value);
						}
					}
				}
				break;
			case EMT_MSG_SET_CPU_POWER_DOWN:
				{
					if(msg.data_len > 0)
					{
						p_emt_app_info->is_key_easy_btn_on = false;
						emt_led_set_front_led(EMT_APP_FRONT_LED_OFF);
						emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
						p_emt_app_info->bbx_op_mode = msg.data[0];
						p_emt_app_info->periodic_data_mode = EMT_PERIODIC_DATA_ALL_OFF;
					#if defined(__SUPPORT_SEND_RESETTYPE_TO_CPU__)
						p_emt_app_info->cpu_reset_type = 0;
					#endif
						if(p_emt_app_info->system_power_mode == EMT_CPU_POWER_ON)
						{
							emt_uart_deinit();
							emt_led_process_timer_deinit();
							emt_front_led_deinit();
							emt_status_led_deinit();
							p_emt_app_info->edlc.charge_process_enable = false;
							p_emt_app_info->edlc.charging_on = false;
							if(emt_gpio_cpu_powerctrl(EMT_CPU_POWER_OFF) == false)
							{
								emt_gpio_mcu_reset();
							}
							p_emt_app_info->system_power_mode = EMT_CPU_POWER_OFF;
						}

						emt_gpio_micom_power_en(EMT_OFF);
						
						if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE)
						{
							p_emt_app_info->temperature.waiting_acc_on = true;
						}

					#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
						if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)
						{
							emt_gpio_low_power_mode();
							SystemClock_Config_LowPower();
							osDelay(10);
							#if(__TARGET_PROJECT__ == __MAGNUS__)
							emt_status_led_init();
							emt_led_process_timer_init();

							#if defined(__SUPPORT_MANUAL_PARKINGMODE_CONTROL__)
							if(is_manual_parking_enable_get())
							{
								emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
							}
							else
							#endif
							{
								emt_led_set_status_led(EMT_APP_STATUS_LED_G_ON);
							}
							#elif(__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
							emt_status_led_init();
							emt_led_process_timer_init();
							emt_led_set_status_led(EMT_APP_STATUS_LED_PARKING_AQS_MODE);
							#elif(__TARGET_PROJECT__ == __PRIME__)
							#else
							#endif
						}
						else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_CUT_OFF_HIGH_TEMP_AI_MODE)
						{
							emt_gpio_low_power_mode();
							SystemClock_Config_LowPower();
							osDelay(10);
						}
						else
					#endif
						{
							#if (__TYPE_BP_DETECT_CHECK__ == __BP_DETECT_TYPE_GPIO__)
							emt_adc_deinit();
							#endif
							emt_gpio_low_power_mode();
							SystemClock_Config_LowPower();
							osDelay(10);
						}

						p_emt_app_info->cpu_watchdog.mode = EMT_CPU_WATCHDOG_OFF;
						p_emt_app_info->temperature.check_count = 0;

				#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
						msg.command = EMT_MSG_GSENSOR_OP_MODE;
						msg.data_len = 1;
						if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_CUT_OFF_MODE) // MICOM 2MHZ
						{
							msg.data[0] = EMT_GSENSOR_OP_POWERDOWN;
						}
						else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AQS_MODE)	// MICOM 2MHZ
						{
							msg.data[0] = EMT_GSENSOR_OP_PARKING_LOW_POWER;
						}
					#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
						else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE)	//MICOM 64MHz
						{
							msg.data[0] = EMT_GSENSOR_OP_PARKING_AI;
						}
					#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
						else if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_SYSTEM_OFF_MODE)
						{
							msg.data[0] = EMT_GSENSOR_OP_POWERDOWN;
						}
					#endif
						osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
				#endif
					}
				}
				break;
			case EMT_MSG_SET_CPU_RESET:
				{
					if(msg.data_len > 0)
					{
						if(emt_gpio_is_acc_detect() == false)
						{
							if(check_system_reset() == true) break;
						}
						p_emt_app_info->is_key_easy_btn_on = false;
						emt_led_set_front_led(EMT_APP_FRONT_LED_OFF);
						emt_front_led_deinit();
						emt_led_set_status_led(EMT_APP_STATUS_LED_OFF);
						p_emt_app_info->bbx_op_mode = msg.data[0];
						p_emt_app_info->periodic_data_mode = EMT_PERIODIC_DATA_ALL_ON;
						p_emt_app_info->edlc.charge_process_enable = false;
						p_emt_app_info->edlc.charging_on = false;
						emt_uart_deinit();
						if(emt_gpio_cpu_powerctrl(EMT_CPU_POWER_RESET) == false)
						{
							emt_gpio_mcu_reset();
						}
						p_emt_app_info->system_power_mode = EMT_CPU_POWER_ON;
					#if defined(__SUPPORT_RTC_CONTROL__)
						p_emt_app_info->is_rtc_lost = emt_is_rtc_lost();
					#endif
					#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
						emt_led_set_status_led(EMT_APP_STATUS_LED_O_ON);
					#endif
						emt_uart_init();

					#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
						if(p_emt_app_info->bbx_op_mode == EMT_BBX_OP_PARKING_AI_MODE)	//MICOM 64MHz
						{
						#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
							msg.command = EMT_MSG_GSENSOR_OP_MODE;
							msg.data_len = 1;
							msg.data[0] = EMT_GSENSOR_OP_PARKING_AI;
							osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
						#endif
							emt_gpio_set_aqs_mode(_BOOT_PARKING);
						}
					#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
						emt_gpio_set_aqs_status(p_emt_app_info);
						#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
						if(p_emt_app_info->need_cpu_acc_check == true)
						{
							p_emt_app_info->need_cpu_acc_check = false;
							emt_gpio_set_acc_check_mode(true);
						}
						#endif
					#endif
						if(p_emt_app_info->cpu_watchdog.mode != EMT_CPU_WATCHDOG_BOOT_CHECK)
						{
							p_emt_app_info->cpu_watchdog.boot_check_count = 0;
						}
						p_emt_app_info->cpu_watchdog.discount = WATCHDOG_COUNT_CPU_BOOT;
						p_emt_app_info->cpu_watchdog.mode = EMT_CPU_WATCHDOG_BOOT_CHECK;
						p_emt_app_info->bbx_communication_on = false;
						p_emt_app_info->bbx_receive_cmd_count = 0;
						memset(&p_emt_app_info->edlc, 0x00, sizeof(EMT_EDLC_CHARGE_STRUCT_T));
					}
				}
				break;
			case EMT_MSG_ENTER_FW_DOWNLOAD_MODE:
				{
				#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)
					msg.command = EMT_MSG_GSENSOR_OP_MODE;
					msg.data_len = 1;
					msg.data[0] = EMT_GSENSOR_OP_POWERDOWN;

					osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
				#endif
					emt_gpio_micom_power_en(EMT_OFF);
					emt_timer_deinit();
					emt_app_jump_to_bootloader();
				}
				break;
			}
		}
	}
}

