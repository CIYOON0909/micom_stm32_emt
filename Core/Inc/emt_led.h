/**
	******************************************************************************
	* @file           : emt_led.h
	* @brief          : Header for emt_flash.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_LED_H__
#define __EMT_LED_H__

#include "emt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void emt_led_control_period_tmr_callback(void);
void emt_led_process_timer_init(void);
void emt_led_process_timer_deinit(void);
void emt_app_led_pwm_ctrl(EMT_FRONT_LED_STRUCT_T *p_led);
void emt_status_led_init(void);
void emt_front_led_deinit(void);
void emt_status_led_deinit(void);
void emt_status_led_control(EMT_STATUS_LED_TYPE status);
#if !defined(__MICOM_BOOTLOADER__)
void emt_status_led_rgb_test(EMT_STATUS_LED_RGB_INFO *p_rgb_info);
void emt_security_led_control(EMT_FRONT_LED_TYPE mode);
#endif
void emt_led_set_front_led( EMT_FRONT_LED_TYPE mode);
void emt_led_set_status_led(EMT_STATUS_LED_TYPE mode);

#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
void emt_led_i2c_dev_init(void);
void emt_led_i2c_dev_deinit(void);
void emt_led_proc_task(void *argument);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __EMT_LED_H__ */
