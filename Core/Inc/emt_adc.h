/**
	******************************************************************************
	* @file           : emt_adc.h
	* @brief          : Header for emt_adc.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
	*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_ADC_H__
#define __EMT_ADC_H__

#include "emt_types.h"


void emt_adc_init(void);
void emt_adc_deinit(void);
void emt_adc_get_value(EMT_ADC_STRUCT_T *p_adc_value);
#endif /* __EMT_ADC_H__ */
