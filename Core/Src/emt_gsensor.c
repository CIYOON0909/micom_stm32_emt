/*
 * emt_gsensor.c
 *
 *	Created on: Jun 10, 2021
 *			Author: kimjj
 */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdio.h"
#include "cmsis_os.h"

#include "stm32g0xx_hal.h"
#include "lis2hh12_reg.h"

#include "emt_io.h"
#include "emt_types.h"
#include "emt_gsensor.h"

#if (__TYPE_G_SENSOR_PROC_MODE__ == __G_SENSOR_DATA_RECV_MODE__)

/* Private macro -------------------------------------------------------------*/
#define	BOOT_TIME			20 //ms
#define	WAIT_TIME			100 //ms

#define	GSENSOR_BUS			h_i2c_gsensor

#define	MIN_ST_LIMIT_mg		70.0f
#define	MAX_ST_LIMIT_mg		1500.0f

/* Self test results. */
#define	ST_PASS				1U
#define	ST_FAIL				0U

/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef h_i2c_gsensor;
static uint8_t whoamI, rst;
static stmdev_ctx_t g_dev_ctx;
static EMT_GSENSOR_OP_STRUCT_T g_gsensor_op_info = {
		.sensitivity.data.x = 15,
		.sensitivity.data.y = 15,
		.sensitivity.data.z = 15
};

bool g_emt_gsensorc_initialize = false;

/* Extern variables ----------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#else
extern osMessageQueueId_t       appQueueHandle;
extern osMessageQueueId_t       gSensorQueueHandle;
#endif



/* Private functions ---------------------------------------------------------*/

static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);


void emt_gsensor_rx_isr_handler(void)
{
	EMT_MSGQUEUE_T msg;
	msg.command = EMT_MSG_GSENSOR_RX_DATA;
	osMessageQueuePut (gSensorQueueHandle, &msg, 0, 0);
}



/**
* @brief Initialize gsensor.
* @param argument: None
* @retval None
*/
void emt_gsensor_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(g_emt_gsensorc_initialize == false)
	{
		h_i2c_gsensor.Instance = GSEN_I2C_PORT;
		h_i2c_gsensor.Init.Timing = 0x00602173;
		h_i2c_gsensor.Init.OwnAddress1 = 0;
		h_i2c_gsensor.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		h_i2c_gsensor.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		h_i2c_gsensor.Init.OwnAddress2 = 0;
		h_i2c_gsensor.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		h_i2c_gsensor.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		h_i2c_gsensor.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&h_i2c_gsensor) != HAL_OK)
		{
			Error_Handler();
		}

		/*Configure GPIO pin : GSEN_INT_MCU_Pin */
		GPIO_InitStruct.Pin = GSEN_INT_MCU_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GSEN_INT_MCU_GPIO_Port, &GPIO_InitStruct); //GPIOA
		/* EXTI interrupt init*/
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

		/** Configure Analogue filter
		*/
		if (HAL_I2CEx_ConfigAnalogFilter(&h_i2c_gsensor, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
		{
			Error_Handler();
		}
		/** Configure Digital filter
		*/
		if (HAL_I2CEx_ConfigDigitalFilter(&h_i2c_gsensor, 0) != HAL_OK)
		{
			Error_Handler();
		}

		/*	Initialize mems driver interface */
		g_dev_ctx.write_reg = platform_write;
		g_dev_ctx.read_reg = platform_read;
		g_dev_ctx.handle = &GSENSOR_BUS;

		/* Check device ID */
		lis2hh12_dev_id_get(&g_dev_ctx, &whoamI);
		g_emt_gsensorc_initialize = true;
	}
//	if (whoamI != LIS2HH12_ID)
//		while (1); /* manage here device not found */
}

void emt_gsensor_deinit(void)
{
	if(g_emt_gsensorc_initialize == true)
	{
		if(g_dev_ctx.handle)
			lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_OFF); //LIS2HH12_CTRL1

		if (HAL_I2C_DeInit(&h_i2c_gsensor) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_GPIO_DeInit(GSEN_INT_MCU_GPIO_Port, GSEN_INT_MCU_Pin);
		/* EXTI interrupt deinit*/
		HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

		g_emt_gsensorc_initialize = false;
	}
}

void lis2hh12_self_test(void)
{
	int16_t data_raw[3];
	float val_st_off[3];
	float val_st_on[3];
	float test_val[3];
	uint8_t st_result;
	uint8_t drdy;
	uint8_t rst;
	uint8_t i;
	uint8_t j;

	/* Restore default configuration */
	lis2hh12_dev_reset_set(&g_dev_ctx, PROPERTY_ENABLE);

	do {
		lis2hh12_dev_reset_get(&g_dev_ctx, &rst);
	} while (rst);

	/* Enable Block Data Update */
	lis2hh12_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);
	/*
	 * Accelerometer Self Test
	 */
	/* Set Output Data Rate */
	lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_50Hz);
	/* Set full scale */
	lis2hh12_xl_full_scale_set(&g_dev_ctx, LIS2HH12_4g);
	/* Wait stable output */
	HAL_Delay(WAIT_TIME);

	/* Check if new value available */
	do {
		lis2hh12_xl_flag_data_ready_get(&g_dev_ctx, &drdy);
	} while (!drdy);

	/* Read dummy data and discard it */
	lis2hh12_acceleration_raw_get(&g_dev_ctx, data_raw);
	/* Read 5 sample and get the average vale for each axis */
	memset(val_st_off, 0x00, 3 * sizeof(float));

	for (i = 0; i < 5; i++) {
		/* Check if new value available */
		do {
			lis2hh12_xl_flag_data_ready_get(&g_dev_ctx, &drdy);
		} while (!drdy);

		/* Read data and accumulate the mg value */
		lis2hh12_acceleration_raw_get(&g_dev_ctx, data_raw);

		for (j = 0; j < 3; j++) {
			val_st_off[j] += lis2hh12_from_fs4g_to_mg(data_raw[j]);
		}
	}

	/* Calculate the mg average values */
	for (i = 0; i < 3; i++) {
		val_st_off[i] /= 5.0f;
	}

	/* Enable Self Test positive (or negative) */
	lis2hh12_xl_self_test_set(&g_dev_ctx, LIS2HH12_ST_NEGATIVE);
	//lis2hh12_xl_self_test_set(&dev_ctx, LIS2HH12_ST_POSITIVE);
	/* Wait stable output */
	HAL_Delay(WAIT_TIME);

	/* Check if new value available */
	do {
		lis2hh12_xl_flag_data_ready_get(&g_dev_ctx, &drdy);
	} while (!drdy);

	/* Read dummy data and discard it */
	lis2hh12_acceleration_raw_get(&g_dev_ctx, data_raw);
	/* Read 5 sample and get the average vale for each axis */
	memset(val_st_on, 0x00, 3 * sizeof(float));

	for (i = 0; i < 5; i++) {
		/* Check if new value available */
		do {
			lis2hh12_xl_flag_data_ready_get(&g_dev_ctx, &drdy);
		} while (!drdy);

		/* Read data and accumulate the mg value */
		lis2hh12_acceleration_raw_get(&g_dev_ctx, data_raw);

		for (j = 0; j < 3; j++) {
			val_st_on[j] += lis2hh12_from_fs4g_to_mg(data_raw[j]);
		}
	}

	/* Calculate the mg average values */
	for (i = 0; i < 3; i++) {
		val_st_on[i] /= 5.0f;
	}

	/* Calculate the mg values for self test */
	for (i = 0; i < 3; i++) {
		test_val[i] = fabs((val_st_on[i] - val_st_off[i]));
	}

	/* Check self test limit */
	st_result = ST_PASS;

	for (i = 0; i < 3; i++) {
		if (( MIN_ST_LIMIT_mg > test_val[i] ) ||
				( test_val[i] > MAX_ST_LIMIT_mg)) {
			st_result = ST_FAIL;
		}
	}

	/* Disable Self Test */
	lis2hh12_xl_self_test_set(&g_dev_ctx, LIS2HH12_ST_DISABLE);
	/* Disable sensor. */
	lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_OFF);

	if (st_result == ST_PASS) {
		printf("Self Test - PASS\r\n" );
	}

	else {
		printf("Self Test - FAIL\r\n" );
	}
}

/**
* @brief Configuration gsensor operation mode.
* @param argument: gSensor op mode value
* @retval None
*/
void emt_gsensor_op_mode(EMT_GSENSOR_OP_MODE mode)
{
	switch(mode)
	{
	case EMT_GSENSOR_OP_POWERDOWN:
		{
			lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_OFF); //LIS2HH12_CTRL1
		}
		break;
	case EMT_GSENSOR_OP_PARKING_LOW_POWER : //IG1 INT Mode
		{
			lis2hh12_xl_axis_t axis_t_val;
			lis2hh12_pin_int1_route_t int1_route_t_val;
			lis2hh12_xl_trshld_en_t trshld_en_t_val;
			lis2hh12_ig_src1_t ig_src1;

			/* Restore default configuration */
			lis2hh12_dev_reset_set(&g_dev_ctx, PROPERTY_ENABLE);

			do {
				lis2hh12_dev_reset_get(&g_dev_ctx, &rst);
			} while (rst);

			/* Enable Block Data Update */
			lis2hh12_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE); //LIS2HH12_CTRL1(20h)

			axis_t_val.xen = PROPERTY_ENABLE;
			axis_t_val.yen = PROPERTY_ENABLE;
			axis_t_val.zen = PROPERTY_ENABLE;

			lis2hh12_xl_axis_set(&g_dev_ctx, axis_t_val); //LIS2HH12_CTRL1
			lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_400Hz); //LIS2HH12_CTRL1

			lis2hh12_xl_filter_int_path_set(&g_dev_ctx, LIS2HH12_HP_ON_INT_GEN_1); //LIS2HH12_CTRL2

			memset(&int1_route_t_val, 0x00, sizeof(lis2hh12_pin_int1_route_t));
			int1_route_t_val.int1_ig1  = PROPERTY_ENABLE;
			lis2hh12_pin_int1_route_set(&g_dev_ctx, int1_route_t_val); //LIS2HH12_CTRL3

			lis2hh12_pin_polarity_set(&g_dev_ctx, LIS2HH12_ACTIVE_LOW); //LIS2HH12_CTRL5

			memset(&trshld_en_t_val, 0x00, sizeof(lis2hh12_xl_trshld_en_t));
			trshld_en_t_val.ig1_xhie	= PROPERTY_ENABLE;
			trshld_en_t_val.ig1_yhie	= PROPERTY_ENABLE;
			trshld_en_t_val.ig1_zhie	= PROPERTY_ENABLE;
			lis2hh12_xl_trshld_axis_set(&g_dev_ctx, trshld_en_t_val); //LIS2HH12_IG_CFG1

			memset(&ig_src1, 0x00, sizeof(lis2hh12_ig_src1_t));
			ig_src1.ia = PROPERTY_ENABLE;

			lis2hh12_write_reg(&g_dev_ctx, LIS2HH12_IG_SRC1, (uint8_t *)&ig_src1, 1);

			lis2hh12_xl_trshld_set(&g_dev_ctx, g_gsensor_op_info.sensitivity.data.x, g_gsensor_op_info.sensitivity.data.y, g_gsensor_op_info.sensitivity.data.z, 0); //LIS2HH12_IG_THS_X1, Y1, Z1...
		}
		break;
	case EMT_GSENSOR_OP_PARKING_AI : //DataReady INT Mode
	case EMT_GSENSOR_OP_NORMAL:
		{
			lis2hh12_pin_int1_route_t int1_route_t_val;

			/* Restore default configuration */
			lis2hh12_dev_reset_set(&g_dev_ctx, PROPERTY_ENABLE);

			do {
				lis2hh12_dev_reset_get(&g_dev_ctx, &rst);
			} while (rst);

			/* Enable Block Data Update */
			lis2hh12_block_data_update_set(&g_dev_ctx, PROPERTY_ENABLE);
			/* Set full scale */
			lis2hh12_xl_full_scale_set(&g_dev_ctx, LIS2HH12_2g);
			/* Configure filtering chain */
			/* Accelerometer data output- filter path / bandwidth */
			lis2hh12_xl_filter_aalias_bandwidth_set(&g_dev_ctx, LIS2HH12_AUTO);
			if(mode == EMT_GSENSOR_OP_PARKING_AI)
			{
				lis2hh12_xl_filter_out_path_set(&g_dev_ctx, LIS2HH12_BYPASSED);
			}
			else
			{
				lis2hh12_xl_filter_out_path_set(&g_dev_ctx, LIS2HH12_FILT_LP);
				lis2hh12_xl_filter_low_bandwidth_set(&g_dev_ctx, LIS2HH12_LP_ODR_DIV_50);
			}
			/* Accelerometer interrupt - filter path / bandwidth */
			lis2hh12_xl_filter_int_path_set(&g_dev_ctx, LIS2HH12_HP_DISABLE);
			/* Set Output Data Rate */
			if(mode == EMT_GSENSOR_OP_PARKING_AI)
				lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_100Hz);
			else
				lis2hh12_xl_data_rate_set(&g_dev_ctx, LIS2HH12_XL_ODR_50Hz);

			memset(&int1_route_t_val, 0x00, sizeof(lis2hh12_pin_int1_route_t));
			int1_route_t_val.int1_drdy	= PROPERTY_ENABLE;
			lis2hh12_pin_int1_route_set(&g_dev_ctx, int1_route_t_val); //LIS2HH12_CTRL3

			lis2hh12_pin_polarity_set(&g_dev_ctx, LIS2HH12_ACTIVE_LOW);
		}
		break;
	default:
		break;
	}
}

int32_t emt_gsensor_read_data(int16_t *p_data_raw_acceleration)
{
	int32_t ret = -1;
	uint8_t reg;

	/*	Read output only if new value is available */
	lis2hh12_xl_flag_data_ready_get(&g_dev_ctx, &reg);

	if (reg)
	{
		/* Read acceleration data  */
		ret = lis2hh12_acceleration_raw_get(&g_dev_ctx, p_data_raw_acceleration);
	}
	return ret;
}


uint8_t MakeCheckSum(uint8_t* data, uint8_t PackSize)
{
	uint8_t CheckSum = 0x00;

	for(int i = 0; i < (PackSize - 1); i++){
		CheckSum ^= *(data + i);
	}

	return CheckSum;
}


/* USER CODE BEGIN Header_g_senosr_proc_task */
/**
* @brief Function implementing the gSensorProc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_g_senosr_proc_task */
void emt_gsensor_proc_task(void *argument)
{
	osStatus_t status = osOK;
	int32_t ret = -1;
	EMT_MSGQUEUE_T msg;
	int16_t data_raw_acceleration[3] = {0};
	int32_t data_raw_acceleration_sum[3] = {0};
	int16_t skip_cnt = 5;
	emt_gsensor_init();

	for(;;)
	{
		status = osMessageQueueGet (gSensorQueueHandle, &msg, NULL, osWaitForever);
		if (status == osOK)
		{
			switch(msg.command)
			{
			case EMT_MSG_GSENSOR_INIT:
				{
					emt_gsensor_init();
				}
				break;
			case EMT_MSG_GSENSOR_DEINIT:
				{
					emt_gsensor_deinit();
				}
				break;
			case EMT_MSG_GSENSOR_OP_MODE:
				{
					if(g_gsensor_op_info.mode != msg.data[0])
					{
						g_gsensor_op_info.mode = msg.data[0];
						emt_gsensor_op_mode(g_gsensor_op_info.mode);

						//GSensor Mode 변경시 초기화 적용
						if(osMessageQueueGetCount(gSensorQueueHandle) > 0)
						{
							osMessageQueueReset(gSensorQueueHandle);
							osDelay(10);
							emt_gsensor_read_data(data_raw_acceleration);
						}
					}
				}
				break;
			case EMT_MSG_GSENSOR_SET_SENSITIVITY:
				{
					if(msg.data_len > 0)
					{
						memcpy((void*)&g_gsensor_op_info.sensitivity.data, (void*)&msg.data[0], sizeof(EMT_GSENSOR_VALUE_T));
					}
				}
				break;
			case EMT_MSG_GSENSOR_RX_DATA:
				{
					memset(data_raw_acceleration, 0x00, sizeof(data_raw_acceleration));
					ret = emt_gsensor_read_data(data_raw_acceleration);
					if(ret == 0)
					{
						if((g_gsensor_op_info.mode == EMT_GSENSOR_OP_PARKING_LOW_POWER)
							|| (g_gsensor_op_info.mode == EMT_GSENSOR_OP_NORMAL)
							)
						{
							data_raw_acceleration_sum[0] += data_raw_acceleration[0];
							data_raw_acceleration_sum[1] += data_raw_acceleration[1];
							data_raw_acceleration_sum[2] += data_raw_acceleration[2];
							skip_cnt --;
							if(skip_cnt == 0)
							{
								data_raw_acceleration[0] = data_raw_acceleration_sum[0] / 5;
								data_raw_acceleration[1] = data_raw_acceleration_sum[1] / 5;
								data_raw_acceleration[2] = data_raw_acceleration_sum[2] / 5;
								memset(data_raw_acceleration_sum, 0x00, sizeof(data_raw_acceleration_sum));

								msg.command = EMT_MSG_RECV_GSENSOR_DATA;
								msg.data[0] = (uint8_t)g_gsensor_op_info.mode;
								memcpy(&msg.data[1], data_raw_acceleration, sizeof(data_raw_acceleration));
								msg.data_len = 1 + sizeof(data_raw_acceleration);
								osMessageQueuePut (appQueueHandle, &msg, 0, 0);
							#if (__TARGET_PROJECT__ == __ZDR_D770R__)
								skip_cnt = 1;
							#else
								skip_cnt = 5;
							#endif
							}
						}
					}
				}
				break;
			}
		}
	}
	/* USER CODE END g_senosr_proc_task */
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
	HAL_I2C_Mem_Write(handle, LIS2HH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
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
	HAL_I2C_Mem_Read(handle, LIS2HH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	return 0;
}
#endif
