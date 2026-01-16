/**
  ******************************************************************************
  * @file           : emt_app.h
  * @brief          : Header for emt_app.c file.
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_APP_H__
#define __EMT_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "emt_types.h"
#include "emt_led.h"
#include "emt_adc.h"
#include "emt_hci.h"


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
typedef enum {
	_BOOT_NORMAL = 0,
	_BOOT_PARKING,
	_BOOT_LOW_PARKING_EVT,
	_BOOT_SUPER_LOW_PARKING_EVT,
	_BOOT_BDF = _BOOT_SUPER_LOW_PARKING_EVT,
	_BOOT_HIGH_TEMP_EVT,
	_BOOT_UPGRADE_N_PARKING,
	_BOOT_1ST_CUTOFF_PARKING,
	_BOOT_MICOM_FW_RECOVERY,
	_BOOT_MAX,
} EMT_BOOT_MODE;
#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
/*
 ZDR-065,D770R
	AQS0 AQS1		Mode					동작 설명
	  0   0   : 	_BOOT_NORMAL			ACC ON (주행 모드)
	  0   1   : 	_BOOT_PARKING			ACC OFF, 주차 상시/타임랩스 녹화 모드
	  1   0   : 	_BOOT_BDF				ACC OFF, AQS 녹화 모드
	  1   1   : 	_BOOT_PARK_REC_OFF		ACC OFF, 주차녹화 Off

ZDR027
    AQS0 : Whether Parking Recording.
    AQS1 : BDF Boot.


*/
typedef enum {
	_AQS_BOOT_NORMAL = 0,
	_AQS_BOOT_PARKING,
	_AQS_BOOT_BDF,
	_AQS_BOOT_PARK_REC_OFF,
	_AQS_ACC_ON  = _AQS_BOOT_NORMAL,
	_AQS_ACC_OFF = _AQS_BOOT_PARK_REC_OFF,
	_AQS_STATUS_MAX,
} EMT_BOOT_MODE;
#endif

void SystemClock_Config(void);
void SystemClock_Config_LowPower(void);
void emt_app_init(EMT_APP_INFO** p_emt_app_info);
void emt_app_jump_to_bootloader(void);
void emt_app_check_cut_off_process(EMT_APP_INFO *p_emt_app_info);
void emt_app_check_temperature(EMT_APP_INFO* p_emt_app_info);
void emt_app_edlc_process(EMT_APP_INFO* p_emt_app_info);
void emt_app_check_acc_status(EMT_APP_INFO *p_emt_app_info);
void emt_gpio_set_aqs_status(EMT_APP_INFO *p_emt_app_info);
bool emt_app_is_bp_cutoff(EMT_APP_INFO* p_emt_app_info);
EMT_BP_TYPE emt_app_get_bp_type(EMT_ADC_STRUCT_T *p_adc_value);

void emt_app_cpu_hci_cmd_proc(EMT_HCI_PACKET_MSG *hci_msg);
#endif /* __EMT_APP_H__ */
