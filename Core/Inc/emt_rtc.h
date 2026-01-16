/*
 * emt_rtc.h
 *
 *  Created on: 01.18, 2023
 *      Author: kmjoo
 */
#include <stdbool.h>

#ifndef INC_EMT_RTC_H_
#define INC_EMT_RTC_H_


#define SUCCESS 0
#define FAIL 1



typedef struct emt_rtc_s {
	uint8_t year;
	uint8_t mon;
	uint8_t mday;
	uint8_t wday;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}emt_rtc_t;

uint8_t emt_rtc_initialize(void);
bool emt_rtc_lost_check(void);
uint8_t emt_rtc_read_time(emt_rtc_t *get_rtc);
uint8_t emt_rtc_set_time(emt_rtc_t *set_rtc);
uint8_t emt_rtc_32k_output_disable(void);
void emt_rtc_init(void);
void emt_rtc_deinit(void);
bool emt_is_rtc_lost(void);

#endif
