/**
	******************************************************************************
	* @file           : emt_flash.h
	* @brief          : Header for emt_flash.c file.
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
	*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EMT_FLASH_H__
#define __EMT_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "emt_types.h"


#define FLASH_WRITE_UNIT_SIZE						8   // double word, 8bytes(64bit)
#define FLASH_WRITE_FAST_UNIT_SIZE					(FLASH_WRITE_UNIT_SIZE * 32)    // double word, 8bytes(64bit) * 32 row
#define FLASH_PAGE_TOTAL							64
#define FLASH_PAGE_BOOT_CNT							11
#define FLASH_PAGE_APPL_CNT							(FLASH_PAGE_TOTAL - (FLASH_PAGE_BOOT_CNT + FLASH_PAGE_FW_DN_INFO_CNT))
#define FLASH_PAGE_FW_DN_INFO_CNT					1

int32_t emt_flash_page_erase(uint32_t addr, uint32_t NbOfPages);
int32_t emt_flash_write(uint32_t addr, uint8_t *p_write_data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __EMT_FLASH_H__ */
