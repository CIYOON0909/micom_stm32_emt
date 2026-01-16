/**
	******************************************************************************
	* @file           : emt_fw_upgrade.h
	* @brief          : Header for emt_fw_upgrade.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_FW_UPGRADE_H__
#define __EMT_FW_UPGRADE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "emt_types.h"

#define vMagicNo_No  (10+15+3)

void emt_fw_upgrade_save_newfw_magic_number_to_ram(void);
int emt_fw_upgrade_check_newfw_magic_number_in_ram(void);
void emt_fw_upgrade_clear_newfw_magic_number_in_ram(void);

int emt_fw_upgrade_check_magic_number_in_flash(void);


void emt_fw_upgrade_check_newfw_info(EMT_FW_UPGRADE_INFO_T *p_fw_dl_info);
int32_t emt_fw_upgrade_erase_newfw_info();
int32_t emt_fw_upgrade_write_newfw_info(EMT_FW_UPGRADE_INFO_T *p_fw_dl_info);
int32_t emt_fw_upgrade_erase_newfw_area(uint32_t addr);
int32_t emt_fw_upgrade_erase_all_newfw_area(void);
int32_t emt_fw_upgrade_write_newfw_area(uint32_t addr, uint32_t data_len, uint8_t* p_data);
int32_t emt_fw_upgrade_verify_newfw_area(uint32_t addr, uint32_t data_len, uint8_t* p_data);

#ifdef __cplusplus
}
#endif

#endif /* __EMT_FW_UPGRADE_H__ */
