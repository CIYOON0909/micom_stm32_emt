/**
  ******************************************************************************
  * @file           : emt_flash.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stm32g0xx_hal.h"

#include "emt_types.h"
#include "emt_flash.h"


static uint32_t emt_flash_get_page_number(uint32_t Addr)
{
	return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}


int32_t emt_flash_page_erase(uint32_t addr, uint32_t NbOfPages)
{
	HAL_StatusTypeDef ret = HAL_OK;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	uint32_t FirstPage = 0;
	uint32_t PageError = 0;

	/* Get the 1st page to erase */
	FirstPage = emt_flash_get_page_number(addr);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;
	HAL_FLASH_Unlock();

	ret = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	HAL_FLASH_Lock();

	if(ret != HAL_OK)
	{
		return EMT_FAILURE;
	}

	return EMT_SUCCESS;
}


int32_t emt_flash_write(uint32_t addr, uint8_t *p_write_data, uint16_t len)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint16_t index;
	uint8_t data[FLASH_WRITE_UNIT_SIZE];
	uint16_t write_count = len / FLASH_WRITE_UNIT_SIZE;

	HAL_FLASH_Unlock();

	for ( index = 0; index < write_count; index++ )
	{
		memcpy((void*)data, (void*)p_write_data, FLASH_WRITE_UNIT_SIZE);
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)addr, *((uint64_t *)&data));
		if (ret != HAL_OK)
		{
			break;
		}
		p_write_data += FLASH_WRITE_UNIT_SIZE;
		addr += FLASH_WRITE_UNIT_SIZE;
	}

	if(ret == HAL_OK)
	{
		if(len % FLASH_WRITE_UNIT_SIZE)
		{
			memset(data, 0x00, FLASH_WRITE_UNIT_SIZE);
			memcpy((void*)data, (void*)p_write_data, len % FLASH_WRITE_UNIT_SIZE);
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)addr, *((uint64_t *)&data));
		}
	}

	HAL_FLASH_Lock();

	if(ret != HAL_OK)
	{
		return EMT_FAILURE;
	}
	return EMT_SUCCESS;
}
