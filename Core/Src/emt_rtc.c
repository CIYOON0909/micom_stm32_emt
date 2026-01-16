/*
 * emt_rtc.c
 *
 *	Created on: 1.18, 2023
 *			Author: kmjoo
 */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdio.h"

#include "main.h"
#include "stm32g0xx_hal.h"
#include "emt_rtc.h"
#include "emt_types.h"
#include "emt_io.h"

#include "s35390a_reg.h"

#if defined(__SUPPORT_RTC_CONTROL__)
static I2C_HandleTypeDef h_i2c_rtc;
static stmdev_ctx_t g_dev_ctx;
int8_t g_emt_rtc_initialize = false;
#define 		RTC_BUS 	 h_i2c_rtc

static int32_t platform_write(void *handle, uint8_t reg,
								uint8_t *bufp,
								uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
								uint16_t len);
/**
* @brief Initialize gsensor.
* @param argument: None
* @retval None
*/
void emt_rtc_init(void)
{
	if(g_emt_rtc_initialize == false)
	{
		h_i2c_rtc.Instance = RTC_I2C_PORT;
		h_i2c_rtc.Init.Timing = 0x10BB7DB2;//100K
		h_i2c_rtc.Init.OwnAddress1 = 0;
		h_i2c_rtc.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		h_i2c_rtc.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		h_i2c_rtc.Init.OwnAddress2 = 0;
		h_i2c_rtc.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		h_i2c_rtc.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		h_i2c_rtc.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&h_i2c_rtc) != HAL_OK)
		{
			Error_Handler();
		}
		/** Configure Analogue filter
		*/
		if (HAL_I2CEx_ConfigAnalogFilter(&h_i2c_rtc, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
		{
			Error_Handler();
		}
		/** Configure Digital filter
		*/
		if (HAL_I2CEx_ConfigDigitalFilter(&h_i2c_rtc, 0) != HAL_OK)
		{
			Error_Handler();
		}
		/*	Initialize mems driver interface */
		g_dev_ctx.write_reg = platform_write;
		g_dev_ctx.read_reg = platform_read;
		g_dev_ctx.handle = &RTC_BUS;

		g_emt_rtc_initialize = true;
	}
}

void emt_rtc_deinit(void)
{
	if(g_emt_rtc_initialize == true)
	{
		if (HAL_I2C_DeInit(&h_i2c_rtc) != HAL_OK)
		{
			Error_Handler();
		}
		g_emt_rtc_initialize = false;
	}
}


/* Macros to convert 2-digit BCD into binary and vice versa */
#define bin2bcd(b)  (((b)%10) + (((b)/10)<<4))
#define bcd2bin(b)  (((b)&0xf) + ((b)>>4)*10)

const uint8_t byte_rev_table[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

static uint8_t bitrev8(uint8_t byte)
{
	return byte_rev_table[byte];
}

static int32_t s35390a_reg2hr(uint8_t reg)
{
	return bcd2bin(reg & 0x3f);
}

static uint8_t s35390a_hr2reg(int32_t hour)
{
	return bin2bcd(hour);
}

uint8_t emt_rtc_initialize(void)
{
	s35390a_reset_mode(&g_dev_ctx);
	s35390a_disable_test_mode(&g_dev_ctx);

	return SUCCESS;
}

uint8_t emt_rtc_32k_output_disable(void)
{
	return s35390a_disable_32k_output_mode(&g_dev_ctx);
}

bool emt_is_rtc_lost(void)
{
	int32_t i;
	uint8_t buf[S35390A_REAL_TIME_LENGTH];

	if(s35390a_real_time_get(&g_dev_ctx, &buf[0]))
		return true;

	/* This chip returns the bits of each byte in reverse order */
	for (i = 0; i < S35390A_REAL_TIME_LENGTH; ++i)
		buf[i] = bitrev8(buf[i]);

	//default rtc 2000/1/1 00:00:00
	if( (buf[S35390A_BYTE_YEAR]  == 0x00) &&\
		(buf[S35390A_BYTE_MONTH] == 0x01) &&\
		(buf[S35390A_BYTE_DAY]   == 0x01) &&\
		(buf[S35390A_BYTE_WDAY]  == 0x00) &&\
		(buf[S35390A_BYTE_HOURS] == 0x00) &&\
		(buf[S35390A_BYTE_MINS]  == 0x00) &&\
		(buf[S35390A_BYTE_SECS]  == 0x00))
		return true;

	return false;
}

uint8_t emt_rtc_read_time(emt_rtc_t *get_rtc)
{
	int32_t i;
	uint8_t buf[S35390A_REAL_TIME_LENGTH];

	if(s35390a_real_time_get(&g_dev_ctx, &buf[0]))
		return FAIL;

	/* This chip returns the bits of each byte in reverse order */
	for (i = 0; i < S35390A_REAL_TIME_LENGTH; ++i)
		buf[i] = bitrev8(buf[i]);

	//default rtc 2000/1/1 00:00:00
	if( (buf[S35390A_BYTE_YEAR]  == 0x00) &&\
		(buf[S35390A_BYTE_MONTH] == 0x01) &&\
		(buf[S35390A_BYTE_DAY]   == 0x01) &&\
		(buf[S35390A_BYTE_WDAY]  == 0x00) &&\
		(buf[S35390A_BYTE_HOURS] == 0x00) &&\
		(buf[S35390A_BYTE_MINS]  == 0x00) &&\
		(buf[S35390A_BYTE_SECS]  == 0x00))
		return FAIL;

	get_rtc->sec  = bcd2bin(buf[S35390A_BYTE_SECS]);
	get_rtc->min  = bcd2bin(buf[S35390A_BYTE_MINS]);
	get_rtc->hour = s35390a_reg2hr(buf[S35390A_BYTE_HOURS]);
	get_rtc->wday = bcd2bin(buf[S35390A_BYTE_WDAY]);
	get_rtc->mday = bcd2bin(buf[S35390A_BYTE_DAY]);
	get_rtc->mon  = bcd2bin(buf[S35390A_BYTE_MONTH]);
	get_rtc->year = bcd2bin(buf[S35390A_BYTE_YEAR]) + 100;

	if(((get_rtc->year+1900) < 2000) || ((get_rtc->year+1900) > 2050))		return FAIL;
	if(((get_rtc->mon) < 1) || ((get_rtc->mon) > 12))						return FAIL;
	if(((get_rtc->mday) < 1) || ((get_rtc->mday) > 31))						return FAIL;
	if(((get_rtc->hour) < 0) || ((get_rtc->hour) > 23))						return FAIL;
	if(((get_rtc->min) < 0) || ((get_rtc->min) > 59))						return FAIL;
	if(((get_rtc->sec) < 0) || ((get_rtc->sec) > 60))						return FAIL;

	return SUCCESS;
}

uint8_t emt_rtc_set_time(emt_rtc_t *set_rtc)
{
	int32_t i;
	uint8_t buf[S35390A_REAL_TIME_LENGTH];
	
	if(((set_rtc->year+1900) < 2000) || ((set_rtc->year+1900) > 2050))		return FAIL;
	if(((set_rtc->mon) < 1) || ((set_rtc->mon) > 12))						return FAIL;
	if(((set_rtc->mday) < 1) || ((set_rtc->mday) > 31))						return FAIL;
	if(((set_rtc->hour) < 0) || ((set_rtc->hour) > 23))						return FAIL;
	if(((set_rtc->min) < 0) || ((set_rtc->min) > 59))						return FAIL;
	if(((set_rtc->sec) < 0) || ((set_rtc->sec) > 60))						return FAIL;

	buf[S35390A_BYTE_YEAR]  = bin2bcd(set_rtc->year - 100);
	buf[S35390A_BYTE_MONTH] = bin2bcd(set_rtc->mon);
	buf[S35390A_BYTE_DAY]   = bin2bcd(set_rtc->mday);
	buf[S35390A_BYTE_WDAY]  = bin2bcd(set_rtc->wday);
	buf[S35390A_BYTE_HOURS] = s35390a_hr2reg(set_rtc->hour);
	buf[S35390A_BYTE_MINS]  = bin2bcd(set_rtc->min);
	buf[S35390A_BYTE_SECS]  = bin2bcd(set_rtc->sec);

	/* This chip expects the bits of each byte to be in reverse order */
	for (i = 0; i < S35390A_REAL_TIME_LENGTH; ++i)
		buf[i] = bitrev8(buf[i]);

	if(s35390a_real_time_set(&g_dev_ctx, &buf[0]))
		return FAIL;

	return SUCCESS;
}


/**
* @brief Write generic device register (platform dependent).
* @param argument: Not used
* @param	handle		customizable argument. In this examples is used in
* 									order to select the correct sensor bus handler.
* @param	reg 			register to write
* @param	bufp			pointer to data to write in register reg
* @param	len 			number of consecutive register to write
* @retval None
*/
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#if 0
	HAL_I2C_Mem_Write(handle, LIS2HH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
#else
	return (HAL_OK!=HAL_I2C_Master_Transmit(handle, reg, bufp, len, 1000));
#endif
}

/**
* @brief	Read generic device register (platform dependent)
*
* @param	handle		customizable argument. In this examples is used in
* 									order to select the correct sensor bus handler.
* @param	reg 			register to read
* @param	bufp			pointer to buffer that store the data read
* @param	len 			number of consecutive register to read
*
*/
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
#if 0
	HAL_I2C_Mem_Read(handle, LIS2HH12_I2C_ADD_L, reg,I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
#else
	return (HAL_OK!=HAL_I2C_Master_Receive(handle,reg,bufp,len,1000));
#endif
}
#endif