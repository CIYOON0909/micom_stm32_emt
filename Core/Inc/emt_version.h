/**
	******************************************************************************
	* @file           : emt_version.h
	* @brief          : Header for emt_version.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_VERSION_H__
#define __EMT_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "emt_types.h"

#if defined(__MICOM_BOOTLOADER__)
#define EMT_MCIOM_BIN_INFO						"BOOTLOADER"
#else
#define EMT_MCIOM_BIN_INFO						"MAIN_APPL"
#endif

#if (__TARGET_PROJECT__ == __ZDR065__)
#define EMT_MICOM_MODEL_NAME					"ZDR065"
#define EMT_MICOM_VERSION_CHAR					'A'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'3'

#define EMT_MICOM_VERSION_HIDDEN				'1'
#elif (__TARGET_PROJECT__ == __ZDR_D770R__)
#define EMT_MICOM_MODEL_NAME					"ZDR-D770R"
#define EMT_MICOM_VERSION_CHAR					'C'

#define EMT_MICOM_VERSION_MAJOR					'8'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'6'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __ZDR027__)
#define EMT_MICOM_MODEL_NAME					"ZDR027"
#define EMT_MICOM_VERSION_CHAR					'B'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'3'
#define EMT_MICOM_VERSION_REVISOIN				'8'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __LXQ2000__)
#define EMT_MICOM_MODEL_NAME					"LXQ2000"
#define EMT_MICOM_VERSION_CHAR					'A'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'0'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __LXQ2000_CR3__)
#define EMT_MICOM_MODEL_NAME					"CR3_TEMP"
#define EMT_MICOM_VERSION_CHAR					'A'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'0'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __PRIME__)
#define EMT_MICOM_MODEL_NAME					"PRIME"
#define EMT_MICOM_VERSION_CHAR					'D'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'2'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __MAGNUS__)
#define EMT_MICOM_MODEL_NAME					"MAGNUS"
#define EMT_MICOM_VERSION_CHAR					'A'

#define EMT_MICOM_VERSION_MAJOR					'9'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'5'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __TRINITY__)
#define EMT_MICOM_MODEL_NAME					"TRINITY"
#define EMT_MICOM_VERSION_CHAR					'B'

#define EMT_MICOM_VERSION_MAJOR					'1'
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'0'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#elif (__TARGET_PROJECT__ == __CYCLOPS__)
#define EMT_MICOM_MODEL_NAME					"CYCLOPS"
#define EMT_MICOM_VERSION_CHAR					'C'

#define EMT_MICOM_VERSION_MAJOR					'9' //VER -> wS 7**,ES 8**,PP 9**,MP 1**
#define EMT_MICOM_VERSION_MINOR					'0'
#define EMT_MICOM_VERSION_REVISOIN				'3'

#define EMT_MICOM_VERSION_HIDDEN				'0'
#endif


typedef struct {
	char info[16];
	char date[16];
	char time[16];
	char ver_char;
	char ver_major;
	char ver_minor;
	char ver_revision;
	char ver_hidden;
	char model_name[32];
} EMT_FW_VER_INFO_T;

void emt_version_get_info(EMT_FW_VER_INFO_T* p_fw_info);

#ifdef __cplusplus
}
#endif

#endif /* __EMT_VERSION_H__ */
