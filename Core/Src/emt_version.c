/**
  ******************************************************************************
  * @file           : emt_version.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#include <string.h>
#include "emt_types.h"
#include "emt_version.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
const EMT_FW_VER_INFO_T emtFWVerInfo =
#else
const EMT_FW_VER_INFO_T emtFWVerInfo __attribute__((section(".meta_fw_info"))) =
#endif
	{
		EMT_MCIOM_BIN_INFO,
		__DATE__, // Build Date
		__TIME__, // Build Time
		EMT_MICOM_VERSION_CHAR,
		EMT_MICOM_VERSION_MAJOR,
		EMT_MICOM_VERSION_MINOR,
		EMT_MICOM_VERSION_REVISOIN,
		EMT_MICOM_VERSION_HIDDEN,
		EMT_MICOM_MODEL_NAME
	};

void emt_version_get_info(EMT_FW_VER_INFO_T* p_fw_info)
{
	if(p_fw_info != NULL)
	{
		memcpy((void*)p_fw_info, (void*)&emtFWVerInfo, sizeof(EMT_FW_VER_INFO_T));
	}
}
