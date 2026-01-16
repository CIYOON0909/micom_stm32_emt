/**
	 ******************************************************************************
	* @file           : emt_app_main.h
	* @brief          : Header for emt_app_main.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
	*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_APP_MAIN_H__
#define __EMT_APP_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "emt_types.h"

void emt_app_proc_task(void *argument);
void emt_bp_off_isr_handler(void);
void emt_gsensor_wakeup_isr_handler(void);
#endif /* __EMT_APP_MAIN_H__ */
