/**
  ******************************************************************************
  * @file           : emt_app_bootloader.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include "string.h"
#include "main.h"
#include "emt_version.h"
#include "emt_types.h"
#include "emt_app.h"

#include "emt_uart_rx.h"
#include "emt_hci.h"
#include "emt_led.h"
#include "emt_gpio.h"
#include "emt_rtc.h"
#include "emt_fw_upgrade.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static EMT_APP_INFO *p_emt_app_info_boot;

/* External variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void emt_app_process(void)
{
	extern uint8_t __flash_mainappl_addr;
	const uint32_t mainappl_start_addr = (uint32_t)&__flash_mainappl_addr;

	bool	is_recover_mode = true;
#if	(__TYPE_LED_CONTROL_MODE__ == __LED_CONTROL_TYPE_GPIO__)
	uint32_t prev_tick;
#endif
	if(((RCC->CSR)& RCC_CSR_WWDGRSTF) == 0)
	{
		if(emt_fw_upgrade_check_newfw_magic_number_in_ram() != EMT_SUCCESS)
		{
			if ((((*(__IO uint32_t*)mainappl_start_addr) & 0x2FFE0000 ) == 0x20000000)
				&&(emt_fw_upgrade_check_magic_number_in_flash() == EMT_SUCCESS))
			{
				/* Jump to user application */
				JumpAddress = *(__IO uint32_t*) (mainappl_start_addr + 4);
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */
				__set_MSP(*(__IO uint32_t*) mainappl_start_addr);
				Jump_To_Application();
				while(1);
			}
		}
		else
		{
			is_recover_mode = false;
		}

		__enable_irq(); // enable interrupt
		HAL_ResumeTick();
	}

	emt_app_init(&p_emt_app_info_boot);

	emt_fw_upgrade_clear_newfw_magic_number_in_ram();
	emt_fw_upgrade_check_newfw_info(&p_emt_app_info_boot->fw_dl_info);
	emt_fw_upgrade_erase_all_newfw_area();

#if defined(__SUPPORT_RTC_CONTROL__)
	emt_rtc_init();
#endif
	if(is_recover_mode == true) //// Recovery Mode, CPU PowerUP
	{
		if(emt_gpio_cpu_powerctrl(EMT_CPU_POWER_ON) == false)
		{
			emt_gpio_mcu_reset();
		}
		HAL_Delay(500);
	}

	emt_uart_init();
	emt_status_led_init();
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	emt_status_led_control(EMT_APP_STATUS_LED_OFF);
#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
#if	(__TYPE_LED_CONTROL_MODE__ == __LED_CONTROL_TYPE_GPIO__)
	emt_status_led_control(EMT_APP_STATUS_LED_O_ON);
#else
#endif
#endif

	if(is_recover_mode == true)
	{
		//recovery mode 에서는 AQS 핀을 이용해 CPU 에 Boot Fw 진입 전달.
	}
	else
	{
		emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_RESP, EMT_HCI_GID_FW_UPDATE, EMT_HCI_CID_ENTER_BOOTLOADER, (uint8_t *)&p_emt_app_info_boot->fw_dl_info.magic_number, sizeof(uint32_t));
	}
#if	(__TYPE_LED_CONTROL_MODE__ == __LED_CONTROL_TYPE_GPIO__)
	prev_tick = HAL_GetTick();
#endif
	do
	{
#if	(__TYPE_LED_CONTROL_MODE__ == __LED_CONTROL_TYPE_GPIO__)
		if((HAL_GetTick() - prev_tick) >= 1000)
		{
			emt_status_led_control(EMT_APP_STATUS_LED_FW_UPDATE);
			prev_tick = HAL_GetTick();
		}
#endif
		emt_uart_rx_check();
		HAL_Delay(1);
	}while(1);
}
