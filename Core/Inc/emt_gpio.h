/**
	******************************************************************************
	* @file           : emt_gpio.h
	* @brief          :
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

#ifndef __EMT_GPIO_H__
#define __EMT_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "emt_types.h"
#include "emt_io.h"

void emt_gpio_mcu_reset(void);

void emt_gpio_set_gsensoer_interrupt(bool enable);

bool emt_gpio_is_bp_detect(void);

bool emt_gpio_is_acc_detect(void);

bool emt_gpio_is_usb_detect(void);

bool emt_gpio_is_dc2dc_enable(void);

void emt_gpio_set_edlc_ctrl(bool on);

void emt_gpio_set_aqs_mode(EMT_BOOT_MODE mode);

void emt_gpio_set_acc_check_mode(bool on);

void emt_gpio_micom_power_en(uint8_t mode);

void emt_gpio_low_power_mode(void);

bool emt_gpio_check_PGood_status(GPIO_PinState check_pin);

bool emt_gpio_cpu_powerctrl(EMT_SYSTEM_POWER_MODE mode);

#ifdef __cplusplus
}
#endif

#endif /* __EMT_GPIO_H__ */
