/*
 * s35390a_reg.c
 *
 *	Created on: 9.19, 2023
 *			Author: sjkim
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "s35390a_reg.h"



int32_t s35390a_read_reg(stmdev_ctx_t *ctx, uint8_t reg,
						  uint8_t *data,
						  uint16_t len)
{
	int32_t ret = FAIL;

	if(ctx->read_reg)
	{
		ret = ctx->read_reg(ctx->handle, reg, data, len);
	}

	return ret;
}


int32_t s35390a_write_reg(stmdev_ctx_t *ctx, uint8_t reg,
						   uint8_t *data,
						   uint16_t len)
{
	int32_t ret = FAIL;

	if(ctx->write_reg)
	{
		ret = ctx->write_reg(ctx->handle, reg, data, len);
	}

	return ret;
}


int32_t s35390a_status1_register_set(stmdev_ctx_t *ctx, uint8_t val)
{
	return s35390a_write_reg(ctx, S35390A_STATUS1_ADDR, &val, 1);
}

int32_t s35390a_status1_register_get(stmdev_ctx_t *ctx, uint8_t *val)
{
	uint8_t buf[1];
	int32_t ret;

	ret = s35390a_read_reg(ctx, S35390A_STATUS1_ADDR, &buf[0], 1);

	if(ret == SUCCESS)
	{
		*val = buf[0];
	}
	return ret;
}


int32_t s35390a_status2_register_set(stmdev_ctx_t *ctx, uint8_t val)
{
	return s35390a_write_reg(ctx, S35390A_STATUS2_ADDR, &val, 1);
}

int32_t s35390a_status2_register_get(stmdev_ctx_t *ctx, uint8_t *val)
{
	uint8_t buf[1];
	int32_t ret;

	ret = s35390a_read_reg(ctx, S35390A_STATUS2_ADDR, &buf[0], 1);

	if(ret == SUCCESS)
	{
		*val = buf[0];
	}
	return ret;
}


int32_t s35390a_real_time_set(stmdev_ctx_t *ctx, uint8_t *val)
{
	return s35390a_write_reg(ctx, S35390A_REAL_TIME_ADDR, val, S35390A_REAL_TIME_LENGTH);
}

int32_t s35390a_real_time_get(stmdev_ctx_t *ctx, uint8_t *val)
{
	int32_t ret;

	ret = s35390a_read_reg(ctx, S35390A_REAL_TIME_ADDR, val, S35390A_REAL_TIME_LENGTH);

	return ret;
}

int32_t s35390a_reset_mode(stmdev_ctx_t *ctx)
{
	uint8_t buf[1];

	s35390a_status1_register_get(ctx, &buf[0]);

	if(!(buf[0] & (S35390A_STATUS1_FLAG_POC | S35390A_STATUS1_FLAG_BLD)))
	{
		if(!(buf[0] & S35390A_STATUS1_FLAG_24H))
		{
			buf[0] |= S35390A_STATUS1_FLAG_24H;
			buf[0] &= 0xf0;
			s35390a_status1_register_set(ctx, buf[0]);
		}
		return FAIL;
	}

	buf[0] |= (S35390A_STATUS1_FLAG_RESET | S35390A_STATUS1_FLAG_24H);
	buf[0] &= 0xf0;


	s35390a_status1_register_set(ctx, buf[0]);

	return SUCCESS;
}

int32_t s35390a_disable_test_mode(stmdev_ctx_t *ctx)
{
	uint8_t buf[1];

	if(s35390a_status2_register_get(ctx, &buf[0]))
		return FAIL;

	if ((!(buf[0] & S35390A_STATUS2_FLAG_TEST)) && (buf[0] & S35390A_STATUS2_FLAG_32KE))
			return FAIL;

	buf[0] &= ~S35390A_STATUS2_FLAG_TEST;
	buf[0] |= S35390A_STATUS2_FLAG_32KE;

	return s35390a_status2_register_set(ctx, buf[0]);
}

int32_t s35390a_disable_32k_output_mode(stmdev_ctx_t *ctx)
{
	uint8_t buf[1];

	if(s35390a_status2_register_get(ctx, &buf[0]))
		return FAIL;

	buf[0] &= ~S35390A_STATUS2_FLAG_32KE;

	return s35390a_status2_register_set(ctx, buf[0]);
}

