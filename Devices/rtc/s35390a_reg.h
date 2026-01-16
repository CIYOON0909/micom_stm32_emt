/*
 * s35390a_reg.h
 *
 *	Created on: 9.19, 2023
 *			Author: sjkim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef S35390A_REGS_H
#define S35390A_REGS_H


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

typedef int32_t (*stmdev_write_ptr)(void *, uint8_t, uint8_t *,
																		uint16_t);
typedef int32_t (*stmdev_read_ptr)(void *, uint8_t, uint8_t *,
																	 uint16_t);

typedef struct
{
	/** Component mandatory fields **/
	stmdev_write_ptr  write_reg;
	stmdev_read_ptr   read_reg;
	/** Customizable optional pointer **/
	void *handle;
} stmdev_ctx_t;


#define SUCCESS 	0
#define FAIL 		1


/**
	* @}s35390a slave address
	*
	*		Device_Code 4Bit + Command_Code 3Bit + R/W 1bit
	*/


#define S35390A_DEVICE_CODE					0x60

#define S35390A_CMD_STATUS1_REG				0x00
#define S35390A_CMD_STATUS2_REG				0x01
#define S35390A_CMD_REAL_TIME1_REG			0x02
#define S35390A_CMD_REAL_TIME2_REG			0x03
#define S35390A_CMD_INT1_REG				0x04
#define S35390A_CMD_INT2_REG				0x05
#define S35390A_CMD_CLOCK_CORRECTION_REG	0x06
#define S35390A_CMD_FREE_REG				0x07


/**
	* @}s35390a status register
	*
	*		Status register 1 is a 1-byte register
	*			that is used to display and set various modes.
	*/
#define S35390A_STATUS1_FLAG_POC        0x01
#define S35390A_STATUS1_FLAG_BLD        0x02
#define S35390A_STATUS1_FLAG_24H        0x40
#define S35390A_STATUS1_FLAG_RESET      0x80


int32_t s35390a_status1_register_set(stmdev_ctx_t *ctx, uint8_t val);
int32_t s35390a_status1_register_get(stmdev_ctx_t *ctx, uint8_t *val);

/**
	* @}s35390a status 2 register
	*
	*		Status register 2 is a 1-byte register
	*			that is used to display and set various modes.
	*/

#define S35390A_STATUS2_FLAG_TEST       0x01
#define S35390A_STATUS2_FLAG_32KE       0x10

int32_t s35390a_status2_register_set(stmdev_ctx_t *ctx, uint8_t val);
int32_t s35390a_status2_register_get(stmdev_ctx_t *ctx, uint8_t *val);

/**
	* @}s35390a real time data
	*
	* 	The real-time data register is a 7-byte register
	* 		that stores the data of year, month, day, day of the week,
	* 		hour, minute,and second in the BCD code.
	*/
#define S35390A_BYTE_YEAR       	0
#define S35390A_BYTE_MONTH      	1
#define S35390A_BYTE_DAY        	2
#define S35390A_BYTE_WDAY       	3
#define S35390A_BYTE_HOURS      	4
#define S35390A_BYTE_MINS       	5
#define S35390A_BYTE_SECS       	6

#define S35390A_REAL_TIME_LENGTH	7


int32_t s35390a_real_time_set(stmdev_ctx_t *ctx, uint8_t *val);
int32_t s35390a_real_time_get(stmdev_ctx_t *ctx, uint8_t *val);

/**
	* @}s35390a : slave address uesed
	*
	*/
#define S35390A_STATUS1_ADDR		(S35390A_DEVICE_CODE | (S35390A_CMD_STATUS1_REG << 1))
#define S35390A_STATUS2_ADDR		(S35390A_DEVICE_CODE | (S35390A_CMD_STATUS2_REG << 1))
#define S35390A_REAL_TIME_ADDR		(S35390A_DEVICE_CODE | (S35390A_CMD_REAL_TIME1_REG << 1))



int32_t s35390a_reset_mode(stmdev_ctx_t *ctx);
int32_t s35390a_disable_test_mode(stmdev_ctx_t *ctx);
int32_t s35390a_disable_32k_output_mode(stmdev_ctx_t *ctx);



#endif /* S35390A_REGS_H */
