/**
  ******************************************************************************
  * @file           : emt_fw_upgrade.c
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


#define vMagicNo_No  (10+15+3)

#if defined(__MICOM_BOOTLOADER__)
const unsigned char cMagic_No_Table[vMagicNo_No]= //10+15+3
{\
	'3', '.' , '1' , '4' , '1' , '5' , '9' , '2' , '6' , '~'\
	,'E' , 'M' , 'T' , 'O' , 'M' , 'E' , 'G' , 'A' , '.' , 'C' , 'O' , '.' , 'L' , 'T' , 'D'\
	,',' , '#',':'\
};
#else
const unsigned char cMagic_No_Table[vMagicNo_No] __attribute__((section(".meta_magic"))) =
{\
	'3', '.' , '1' , '4' , '1' , '5' , '9' , '2' , '6' , '~'\
	,'E' , 'M' , 'T' , 'O' , 'M' , 'E' , 'G' , 'A' , '.' , 'C' , 'O' , '.' , 'L' , 'T' , 'D'\
	,',' , '#',':'\
};
#endif


void emt_fw_upgrade_save_newfw_magic_number_to_ram(void)
{
	extern uint8_t __ram_newfwinfo_addr;
	uint8_t *p_newfwinfo_addr = (uint8_t*)((uint32_t)&__ram_newfwinfo_addr);
	unsigned int i;
	for(i = 0; i < (vMagicNo_No); i++)
	{
		p_newfwinfo_addr[i] = cMagic_No_Table[i];
	}
}

int32_t emt_fw_upgrade_check_newfw_magic_number_in_ram(void)
{
	extern uint8_t __ram_newfwinfo_addr;
	uint8_t *p_newfwinfo_addr = (uint8_t*)((uint32_t)&__ram_newfwinfo_addr);
	unsigned int i;

	for(i = 0; i < (vMagicNo_No); i++)
	{
		if( cMagic_No_Table[i] != p_newfwinfo_addr[i])
		{
			return EMT_FAILURE;
		}
	}
	return EMT_SUCCESS;
}

void emt_fw_upgrade_clear_newfw_magic_number_in_ram(void)
{
	extern uint8_t __ram_newfwinfo_addr;
	uint8_t *p_newfwinfo_addr = (uint8_t*)((uint32_t)&__ram_newfwinfo_addr);

	unsigned int i;
	for(i = 0; i < (vMagicNo_No); i++)
	{
		p_newfwinfo_addr[i] = 0;
	}
}

int32_t emt_fw_upgrade_check_magic_number_in_flash(void)
{
	extern uint8_t __flash_mainappl_magic_num_addr;
	uint8_t *p_mainappl_magic_num_addr = (uint8_t*)((uint32_t)&__flash_mainappl_magic_num_addr);
	unsigned int i;

	for(i = 0; i < (vMagicNo_No); i++)
	{
		if( cMagic_No_Table[i] != p_mainappl_magic_num_addr[i])
		{
			return EMT_FAILURE;
		}
	}
	return EMT_SUCCESS;
}

void emt_fw_upgrade_check_newfw_info(EMT_FW_UPGRADE_INFO_T *p_fw_dl_info)
{
	extern uint8_t __flash_newfwinfo_addr;
	uint8_t *p_newfwinfo_addr = (uint8_t*)((uint32_t)&__flash_newfwinfo_addr);
	uint8_t *p_data = (uint8_t*)p_fw_dl_info;
	unsigned int i;

	for(i = 0; i < sizeof(EMT_FW_UPGRADE_INFO_T); i++)
	{
		p_data[i] = p_newfwinfo_addr[i];
	}
}

int32_t emt_fw_upgrade_erase_newfw_info(void)
{
	extern uint8_t __flash_newfwinfo_addr;
	uint32_t newfwinfo_addr = (uint32_t)&__flash_newfwinfo_addr;
	int32_t ret = EMT_SUCCESS;

	ret = emt_flash_page_erase(newfwinfo_addr, FLASH_PAGE_FW_DN_INFO_CNT);

	return ret;
}


int32_t emt_fw_upgrade_write_newfw_info(EMT_FW_UPGRADE_INFO_T *p_fw_dl_info)
{
	extern uint8_t __flash_newfwinfo_addr;
	uint32_t newfwinfo_addr = (uint32_t)&__flash_newfwinfo_addr;
	int32_t ret = EMT_SUCCESS;
	int32_t retry = 3;

	do
	{
		ret = emt_flash_page_erase(newfwinfo_addr, FLASH_PAGE_FW_DN_INFO_CNT);
		if(ret == EMT_SUCCESS)
		{
			ret = emt_flash_write(newfwinfo_addr, (uint8_t*)p_fw_dl_info, sizeof(EMT_FW_UPGRADE_INFO_T));
		}
	}while((ret != EMT_SUCCESS) && (retry-- > 0 ));

	return ret;
}


int32_t emt_fw_upgrade_erase_newfw_area(uint32_t addr)
{
	extern uint8_t __flash_mainappl_addr, __flash_newfwinfo_addr;
	uint32_t mainappl_start_addr = (uint32_t)&__flash_mainappl_addr;
	uint32_t mainappl_end_addr = (uint32_t)&__flash_newfwinfo_addr;

	int32_t ret = EMT_FAILURE;
	if((addr >= 0) && (addr < (mainappl_end_addr - mainappl_start_addr)))
	{
		ret = emt_flash_page_erase(mainappl_start_addr + addr, 1);
	}

	return ret;
}


int32_t emt_fw_upgrade_erase_all_newfw_area(void)
{
	extern uint8_t __flash_mainappl_addr;
	uint32_t mainappl_start_addr = (uint32_t)&__flash_mainappl_addr;
	int32_t ret = EMT_SUCCESS;

	ret = emt_flash_page_erase(mainappl_start_addr, FLASH_PAGE_APPL_CNT);

	return ret;
}


int32_t emt_fw_upgrade_write_newfw_area(uint32_t addr, uint32_t data_len, uint8_t* p_data)
{
	extern uint8_t __flash_mainappl_addr;
	uint32_t mainappl_start_addr = (uint32_t)&__flash_mainappl_addr;
	int32_t ret = EMT_SUCCESS;

	ret = emt_flash_write(mainappl_start_addr + addr, p_data, data_len);
	return ret;
}


int32_t emt_fw_upgrade_verify_newfw_area(uint32_t addr, uint32_t data_len, uint8_t* p_data)
{
	extern uint8_t __flash_mainappl_addr;
	uint32_t mainappl_start_addr = (uint32_t)&__flash_mainappl_addr;
	int32_t ret = EMT_SUCCESS;
	int32_t idx;

	for(idx = 0; idx < data_len; idx++)
	{
		if(((uint8_t*)(mainappl_start_addr + addr))[idx] != p_data[idx])
		{
			ret = EMT_FAILURE;
			break;
		}
	}
	return ret;
}
