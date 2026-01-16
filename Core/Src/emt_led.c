/**
  ******************************************************************************
  * @file           : emt_led.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "emt_types.h"
#include "emt_io.h"
#include "emt_led.h"

#ifdef __SUPPORT_I2C_LED_DEV_KTD202X__
#include "led_ktd202x_drv.h"
#endif
/* Private macro -------------------------------------------------------------*/

#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
#define LED_50MS_TIMER_CNT				(1)
#define LED_500MS_TIMER_CNT				(10)
#define LED_1000MS_TIMER_CNT			(20)
#define LED_2000MS_TIMER_CNT			(40)
#endif
/* Private variables ---------------------------------------------------------*/

static bool					g_emt_led_initialize = false;

#if (__TARGET_PROJECT__ == __PRIME__)
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
static uint16_t				g_timer_count = 0;
static uint16_t				g_led_pattern_cnt = 0;
static EMT_STATUS_LED_TYPE	g_status_led = 0xFFFF;
static EMT_FRONT_LED_TYPE	g_front_led = EMT_APP_FRONT_LED_OFF;
static uint8_t              g_led_rgb_test_mode = 0; //2025.05.08 [arkyoung] test codes
#else
static uint16_t				g_timer_count = 0;
static uint16_t				g_led_pattern_cnt = 0;
static EMT_STATUS_LED_TYPE	g_status_led = EMT_APP_STATUS_LED_OFF;
#endif

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__) //|| (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
static bool						g_emt_front_led_initialize = false;
static EMT_FRONT_LED_STRUCT_T	g_front_led =
{
	.mode = EMT_APP_FRONT_LED_OFF,
	.blink_time = FRONT_LED_BLINK_TIME_COUNT,
	.dimming_freq = 50000,
	.dimming_duty = LED_PWM_DUTY_MIN,
	.dimming_up = true
};
#endif

#if (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
#if (__TARGET_PROJECT__ == __PRIME__)
/*
 * (R) 점멸 (0.3s ON <--> 0.1s OFF) x 3 <--> 1s OFF
 * 주차 감시 모드(모션 검출) / 주차 감시 모드(충격 빠른 녹화 시)
 */
const uint8_t g_emt_led_pattern_cycle[] =
{
		SET, 	SET, 	SET,	SET,	SET,	SET, 	RESET, 	RESET,
		SET, 	SET, 	SET,	SET,	SET,	SET, 	RESET, 	RESET,
		SET, 	SET, 	SET,	SET,	SET,	SET, 	RESET, 	RESET,
		RESET,	RESET,	RESET, RESET, RESET,
		RESET,	RESET,	RESET, RESET, RESET,
		RESET,	RESET,	RESET, RESET, RESET,
		RESET,	RESET,	RESET, RESET, RESET,
		RESET,	RESET,	RESET, RESET, RESET

};
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
#if !defined(__MICOM_BOOTLOADER__)
static led_config_group_t g_security_led_config[EMT_APP_FRONT_LED_MAX] =
{
	[EMT_APP_FRONT_LED_OFF] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_W,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_APP_FRONT_LED_2HZ_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_W,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 1000
						},
						{
							.onoff = LED_OFF,
							.time = 1000
						},						
						{
							.onoff = LED_ON,
							.time = 0
						}
					},
					.period_count = 0,
					.scale_ms = 50,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			}
		}
	},
	[EMT_APP_FRONT_LED_5HZ_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_W,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 0,
						}
					},
					.period_count = 0,
					.scale_ms = 50,
					.current_value = LED_ON,
					.total_ms = 0	
				},
				.pattern_idx = 0xFF
			}
		}
	},	
	
	[EMT_APP_FRONT_LED_ALWAYS_ON] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_W,
				.enable = LED_ENABLE_ON,
			}
		}
	}	
};

static led_config_group_t g_recording_led_config[EMT_RECORDING_STATUS_LED_MAX] =
{
	[EMT_RECORDING_STATUS_LED_OFF] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_ON] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_ON,
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_FAST_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
				}
			}
		}
	}
};

static led_config_group_t g_device_led_config[EMT_DEVICE_STATUS_LED_MAX] =
{
	[EMT_DEVICE_STATUS_LED_OFF] = {
		.led_config_real_size = 2,
		.led_configs = {				
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_DEVICE_STATUS_B_R_TOGGLE] = {
		.led_config_real_size = 2,
		.led_configs = {
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 0
						}
						
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_OFF,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			},
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 0
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_OFF,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			}
		}
	},
	[EMT_DEVICE_STATUS_R_FAST_BLINK] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_DEVICE_STATUS_B_ON] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_ON,								
			}
		}
	},
	[EMT_DEVICE_STATUS_B_BLINK] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},			
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
			}
		}
	},
	[EMT_DEVICE_STATUS_R_ON] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_ON,
			}
		}
	}
	
};
#else
static led_config_group_t g_recording_led_config[EMT_RECORDING_STATUS_LED_MAX] =
{
	[EMT_RECORDING_STATUS_LED_OFF] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_ON] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_ON,
			}
		}
	},
	[EMT_RECORDING_STATUS_LED_FAST_BLINK] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_G,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
				}
			}
		}
	}
};

static led_config_group_t g_device_led_config[EMT_DEVICE_STATUS_LED_MAX] =
{
	[EMT_DEVICE_STATUS_LED_OFF] = {
		.led_config_real_size = 2,
		.led_configs = {				
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_DEVICE_STATUS_B_R_TOGGLE] = {
		.led_config_real_size = 2,
		.led_configs = {
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 0
						}
						
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_OFF,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			},
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 0
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_OFF,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			}
		}
	},
	[EMT_DEVICE_STATUS_R_FAST_BLINK] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						},
						{
							.onoff = LED_ON,
							.time = 250
						},
						{
							.onoff = LED_OFF,
							.time = 250
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
				.pattern_idx = 0xFF
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_OFF,
			}
		}
	},
	[EMT_DEVICE_STATUS_B_ON] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_ON,								
			}
		}
	},
	[EMT_DEVICE_STATUS_B_BLINK] = {
		.led_config_real_size = 2,
		.led_configs = {	
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_OFF,
			},			
			{
				.led_type = LED_TYPE_B,
				.enable = LED_ENABLE_PATTERN,
				.pattern_info = {
					.sequence = {
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						},
						{
							.onoff = LED_ON,
							.time = 500
						},
						{
							.onoff = LED_OFF,
							.time = 500
						}
					},
					.period_count = 0,
					.scale_ms = 50,
//					.start_value = LED_ON,
					.current_value = LED_ON,
					.total_ms = 0
				},
			}
		}
	},
	[EMT_DEVICE_STATUS_R_ON] = {
		.led_config_real_size = 1,
		.led_configs = {
			{
				.led_type = LED_TYPE_R,
				.enable = LED_ENABLE_ON,
			}
		}
	}
	
};
#endif
#else
static const uint16_t	g_emt_led_swtich_onoff[EMT_APP_STATUS_LED_MAX] =
{
	LED_500MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_OFF*/
	LED_500MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_G_ON*/
	LED_500MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_O_ON*/
	LED_1000MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_G_NORMAL_BLINK*/
	LED_1000MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_O_NORMAL_BLINK*/
	LED_500MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_G_FAST_BLINK*/
	LED_500MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_O_FAST_BLINK*/
	LED_50MS_TIMER_CNT,		/*EMT_APP_STATUS_LED_G_PATTERN*/
	LED_50MS_TIMER_CNT,		/*EMT_APP_STATUS_LED_O_PATTERN*/
	LED_1000MS_TIMER_CNT,	/*EMT_APP_STATUS_LED_G_O_TOGGLE*/
	LED_500MS_TIMER_CNT		/*EMT_APP_STATUS_LED_G_O_FAST_TOGGLE*/
};

static const GPIO_PinState	g_emt_led_pattern_cycle[LED_2000MS_TIMER_CNT] =
{
	GPIO_PIN_SET,	GPIO_PIN_SET,	GPIO_PIN_SET,	GPIO_PIN_SET, 	GPIO_PIN_RESET,
	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_SET,	GPIO_PIN_SET,	GPIO_PIN_SET,
	GPIO_PIN_SET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_SET,
	GPIO_PIN_SET,	GPIO_PIN_SET,	GPIO_PIN_SET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,
	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,
	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,
	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,
	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET,	GPIO_PIN_RESET
};
#endif
#endif

#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
#define LED_BRIGHTNESS_MAX		(20)

static uint8_t              g_led_i2c_initialize = false;
static I2C_HandleTypeDef    h_i2c_led;
#if defined(__MICOM_BOOTLOADER__)
#else
extern osMessageQueueId_t   g_ledQueueHandle;
osStatus_t emt_led_send_msg(uint8_t cmd, uint8_t status);
#endif
#endif

/* Export variables ----------------------------------------------------------*/
TIM_HandleTypeDef	h_led_pwm_control_timer;
TIM_HandleTypeDef	h_led_control_period_timer;

/* Private functions ---------------------------------------------------------*/
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
static int32_t emt_led_i2c_dev_write(uint16_t reg_addr, uint8_t *p_buf, uint16_t size);
static int32_t emt_led_i2c_dev_read(uint16_t reg_addr, uint8_t *p_buf, uint16_t size);
static EMT_DEV_I2C_CB_T g_emt_dev_led_callback =
{
    .dev_write_cb		= emt_led_i2c_dev_write,
    .dev_read_cb		= emt_led_i2c_dev_read,
};
#endif

void emt_led_control_period_tmr_callback(void)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	emt_app_led_pwm_ctrl(&g_front_led);
	g_timer_count++;
	if(g_timer_count == 10)
	{
		emt_status_led_control(g_status_led);
		g_timer_count = 0;
	}
#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__)
	#if (__TARGET_PROJECT__ == __PRIME__)
		#if defined(__MICOM_BOOTLOADER__)
		#else
	emt_led_send_msg(EMT_MSG_LED_TIMER_UPDATE, 0);
		#endif
	#else
	g_timer_count++;
	if(g_timer_count == g_emt_led_swtich_onoff[g_status_led])
	{
		emt_status_led_control(g_status_led);
		g_timer_count = 0;
	}   
	#endif
#elif (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
	//emt_app_led_pwm_ctrl(&g_front_led);
	g_timer_count++;
	#if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	if (g_led_rgb_test_mode == 1)
	{
		return;
	}
	emt_status_led_control(g_status_led);	
	emt_security_led_control(g_front_led);
	#else
	if(g_timer_count == g_emt_led_swtich_onoff[g_status_led])
	{
		emt_status_led_control(g_status_led);
		g_timer_count = 0;
	}
	#endif
#endif
}

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
static void emt_led_set_pwm_timer(uint32_t prescaler, uint32_t period, uint32_t pulse)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	h_led_pwm_control_timer.Instance = TIM14;
	h_led_pwm_control_timer.Init.Prescaler = prescaler;
	h_led_pwm_control_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
	h_led_pwm_control_timer.Init.Period = period;
	h_led_pwm_control_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	h_led_pwm_control_timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&h_led_pwm_control_timer) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&h_led_pwm_control_timer) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&h_led_pwm_control_timer, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_PWM_Start(&h_led_pwm_control_timer, TIM_CHANNEL_1);
}
#endif

void emt_front_led_set_led_pwm_ctrl(uint32_t freq, uint32_t duty_cycle)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	uint32_t SysClock;
	uint32_t Prescaler;
	uint32_t TimClock;

	uint32_t Period;
	uint32_t Pulse;

	if(g_emt_front_led_initialize == true)
	{
		if((duty_cycle > 0) && (duty_cycle <= 100))
		{
			SysClock = HAL_RCC_GetSysClockFreq();
			Prescaler = (SysClock / 1000000) / 2 - 1;
			TimClock = SysClock / (Prescaler + 1);
			Period = TimClock / freq;
			Pulse = duty_cycle * Period / 100;

			if(h_led_pwm_control_timer.Instance == TIM14)
			{
				HAL_TIM_PWM_Stop(&h_led_pwm_control_timer, TIM_CHANNEL_1);
				HAL_TIM_Base_DeInit(&h_led_pwm_control_timer);
			}
			emt_led_set_pwm_timer(Prescaler, Period, Pulse);
		}
	}
#endif
}

void emt_front_led_init(EMT_FRONT_LED_TYPE  mode)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__) //|| (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(g_emt_front_led_initialize == false)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**TIM14 GPIO Configuration
		PA7     ------> TIM14_CH1
		*/

		//emt_led_on(false);

		GPIO_InitStruct.Pin = MCU_CTRL_LED_PWM_Pin;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

		if(mode == EMT_APP_FRONT_LED_DIMMING)
		{
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Alternate = GPIO_AF4_TIM14;
		}
		else
		{
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		}
		HAL_GPIO_Init(MCU_CTRL_LED_PWM_GPIO_Port, &GPIO_InitStruct);
		g_emt_front_led_initialize = true;
	}
#endif
}

void emt_front_led_deinit(void)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(g_emt_front_led_initialize == true)
	{
		if(h_led_pwm_control_timer.Instance == TIM14)
		{
			HAL_TIM_PWM_Stop(&h_led_pwm_control_timer, TIM_CHANNEL_1);
			HAL_TIM_Base_DeInit(&h_led_pwm_control_timer);
		}
	}

	HAL_GPIO_DeInit(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin);

	GPIO_InitStruct.Pin = MCU_CTRL_LED_PWM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(MCU_CTRL_LED_PWM_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_DeInit(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin);

	g_emt_front_led_initialize = false;
#endif
}

void emt_front_led_on(bool on)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	GPIO_PinState PinState;
	if(on == true)
	{
		PinState = GPIO_PIN_SET;
	}
	else
	{
		PinState = GPIO_PIN_RESET;
	}
	HAL_GPIO_WritePin(MCU_CTRL_LED_PWM_GPIO_Port, MCU_CTRL_LED_PWM_Pin, PinState);
#endif
}

void emt_led_process_timer_init(void)
{
	if(g_emt_led_initialize == false)
	{
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		int32_t Prescaler;

		Prescaler = (HAL_RCC_GetSysClockFreq() / 1000000) - 1;

		if(Prescaler < 0)
			Prescaler = 0;

		h_led_control_period_timer.Instance = TIM7;
		h_led_control_period_timer.Init.Prescaler = Prescaler;
		h_led_control_period_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
		h_led_control_period_timer.Init.Period = 50000;
		h_led_control_period_timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
		if (HAL_TIM_Base_Init(&h_led_control_period_timer) != HAL_OK)
		{
			Error_Handler();
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&h_led_control_period_timer, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
		HAL_TIM_Base_Start_IT(&h_led_control_period_timer);
		g_emt_led_initialize = true;
	}
}

void emt_led_process_timer_deinit(void)
{
	if(g_emt_led_initialize == true)
	{
		if (HAL_TIM_Base_DeInit(&h_led_control_period_timer) != HAL_OK)
		{
			Error_Handler();
		}
		g_emt_led_initialize = false;
	}
}

void emt_app_led_pwm_ctrl(EMT_FRONT_LED_STRUCT_T *p_led) // 50ms HW timer
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__) //|| (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
	if(p_led->previous_mode != p_led->mode)
	{
		if(p_led->mode != EMT_APP_FRONT_LED_OFF)
		{
			p_led->dimming_up = true;
			p_led->dimming_duty = LED_PWM_DUTY_MIN;

			emt_front_led_init(EMT_APP_FRONT_LED_DIMMING);
		}
	}

	if(p_led->mode == EMT_APP_FRONT_LED_OFF)
	{
		emt_front_led_deinit();
	}
	else if(p_led->mode == EMT_APP_FRONT_LED_DIMMING)
	{
		if(p_led->dimming_up == true)
		{
			p_led->dimming_duty++;
			if(p_led->dimming_duty >= LED_PWM_DUTY_MAX)
				p_led->dimming_up = false;
		}
		else
		{
			p_led->dimming_duty--;
			if(p_led->dimming_duty <= LED_PWM_DUTY_MIN)
				p_led->dimming_up = true;
		}

		if(p_led->dimming_duty > 0)
			emt_front_led_set_led_pwm_ctrl(p_led->dimming_freq, p_led->dimming_duty);
	}
	else if(p_led->mode == EMT_APP_FRONT_LED_DIMMING_X5000)
	{
		uint32_t dimming_duty_x5000;
		if(p_led->dimming_up == true)
		{
			p_led->dimming_duty++;
			if(p_led->dimming_duty >= LED_PWM_DUTY_MAX_X5000)
				p_led->dimming_up = false;
		}
		else
		{
			p_led->dimming_duty--;
			if(p_led->dimming_duty <= LED_PWM_DUTY_MIN_X5000)
				p_led->dimming_up = true;
		}

		if(p_led->dimming_duty > 0)
		{
			dimming_duty_x5000 = p_led->dimming_duty/3;
			emt_front_led_set_led_pwm_ctrl(p_led->dimming_freq, dimming_duty_x5000);
		}
	}
	else if(p_led->mode == EMT_APP_FRONT_LED_ALWAYS_ON)
	{
		if(p_led->previous_mode != p_led->mode)
		{
			p_led->dimming_up = false;
			p_led->dimming_duty = LED_PWM_DUTY_MIN_X5000/3;
			emt_front_led_set_led_pwm_ctrl(p_led->dimming_freq, p_led->dimming_duty);
		}
	}
	else
	{
		if(p_led->blink_time > 0)
		{
			if(p_led->blink_wait_count == 0)
			{
				if(p_led->dimming_up == true)
				{
					p_led->dimming_up = false;
					p_led->dimming_duty = LED_PWM_DUTY_MAX;
				}
				else
				{
					p_led->dimming_up = true;
					p_led->dimming_duty = LED_PWM_DUTY_MIN;
				}

				emt_front_led_set_led_pwm_ctrl(p_led->dimming_freq, p_led->dimming_duty);

				if(p_led->mode == EMT_APP_FRONT_LED_2HZ_BLINK) //per 500ms
				{
					p_led->blink_wait_count = 4;
				}
				else if(p_led->mode == EMT_APP_FRONT_LED_5HZ_BLINK) //per 200ms
				{
					p_led->blink_wait_count = 1;
				}
			}

			if(p_led->blink_wait_count > 0)
			{
				p_led->blink_wait_count--;
			}
			p_led->blink_time--;
		}
		else
		{
			p_led->mode = EMT_APP_FRONT_LED_DIMMING;
		}
	}

	p_led->previous_mode = p_led->mode;
#endif
}

void emt_status_led_init(void)
{
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_GPIO__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#if (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	#ifdef __MCU_SWD_DEBUG_ENABLE__
	#else
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = MCU_EN_LEDUP_Pin|MCU_EN_LEDDN_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	#endif
#elif (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __MAGNUS__)
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __ZDR027__)
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#ifdef __MCU_SWD_DEBUG_ENABLE__
	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin;
	#else
	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin|MCU_EN_BLED_Pin|MCU_EN_WLED_Pin;
	#endif
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
#endif
#elif(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
    emt_led_i2c_dev_init();
#endif
}

void emt_status_led_deinit(void)
{
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_GPIO__)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#if (__TARGET_PROJECT__ == __LXQ2000__) || (__TARGET_PROJECT__ == __LXQ2000_CR3__)
	#ifdef __MCU_SWD_DEBUG_ENABLE__
	#else
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = MCU_EN_LEDUP_Pin|MCU_EN_LEDDN_Pin;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	#endif
#elif (__TARGET_PROJECT__ == __ZDR065__) || (__TARGET_PROJECT__ == __ZDR_D770R__) || (__TARGET_PROJECT__ == __MAGNUS__)
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin;
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
#elif (__TARGET_PROJECT__ == __ZDR027__)
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = MCU_EN_GLED_Pin|MCU_EN_RLED_Pin;
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_DeInit(GPIOB, GPIO_InitStruct.Pin);
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	#ifdef __MCU_SWD_DEBUG_ENABLE__
	GPIO_InitStruct.Pin = MCU_EN_RLED_Pin|MCU_EN_GLED_Pin;
	#else
	GPIO_InitStruct.Pin = MCU_EN_RLED_Pin|MCU_EN_GLED_Pin|MCU_EN_BLED_Pin|MCU_EN_WLED_Pin;
	#endif
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_DeInit(GPIOA, GPIO_InitStruct.Pin);
#endif
#elif(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
    emt_led_i2c_dev_deinit();
#endif
}

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
void emt_status_led_control(EMT_STATUS_LED_TYPE status)
{
	GPIO_PinState dn_led_state, up_led_state;

/*
 * Status LED Operation
 * OP                       DN_LED(POWER),      UP_LED(EVENT)
 * 주행녹화                 ON                  ON
 * 주차녹화(대기)           1Hz Blink           OFF
 * 이벤트(주행, 긴급)       ON                  1Hz Blink
 * 이벤트(주차, 모션)       1Hz Blink           1Hz Blink
 * 메뉴 진입                ON                  OFF
 * SD Format                1Hz Blink           OFF
 * 업그레이드               1Hz Blink           1Hz Blink
 * SD Error(No SD)          OFF                 1Hz Blink
 */
#ifdef __MCU_SWD_DEBUG_ENABLE__
	return;
#endif
	up_led_state = HAL_GPIO_ReadPin(MCU_EN_LEDUP_GPIO_Port, MCU_EN_LEDUP_Pin);
	dn_led_state = HAL_GPIO_ReadPin(MCU_EN_LEDDN_GPIO_Port, MCU_EN_LEDDN_Pin);

	switch(status)
	{
		case EMT_APP_STATUS_LED_NORMAL:
		{
			up_led_state = GPIO_PIN_SET;
			dn_led_state = GPIO_PIN_SET;
			break;
		}
		case EMT_APP_STATUS_LED_PARKING:
		case EMT_APP_STATUS_LED_SD_FORMAT:
		{
			up_led_state = GPIO_PIN_RESET;

			if(dn_led_state == GPIO_PIN_RESET)
			{
				dn_led_state = GPIO_PIN_SET;
			}
			else
			{
				dn_led_state = GPIO_PIN_RESET;
			}
			break;
		}
		case EMT_APP_STATUS_LED_EVENT_NORMAL:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
			}

			dn_led_state = GPIO_PIN_SET;
			break;
		}
		case EMT_APP_STATUS_LED_EVENT_PARKING:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
				dn_led_state = GPIO_PIN_RESET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
				dn_led_state = GPIO_PIN_SET;
			}
			break;
		}
		case EMT_APP_STATUS_LED_FW_UPDATE:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
			}

			dn_led_state = up_led_state;
			break;
		}
		case EMT_APP_STATUS_LED_MENU:
		{
			dn_led_state = GPIO_PIN_SET;
			up_led_state = GPIO_PIN_RESET;
			break;
		}
		case EMT_APP_STATUS_LED_SD_ERROR:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
			}

			dn_led_state = GPIO_PIN_RESET;
			break;
		}
		case EMT_APP_STATUS_LED_OFF:
		{
			up_led_state = GPIO_PIN_RESET;
			dn_led_state = GPIO_PIN_RESET;
			break;
		}
		case EMT_APP_STATUS_LED_BLINK_TEST:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
			}

			dn_led_state = up_led_state;
			break;
		}
		case EMT_APP_STATUS_LED_BLINK_TEST2:
		{
			if(up_led_state == GPIO_PIN_RESET)
			{
				up_led_state = GPIO_PIN_SET;
				dn_led_state = GPIO_PIN_RESET;
			}
			else
			{
				up_led_state = GPIO_PIN_RESET;
				dn_led_state = GPIO_PIN_SET;
			}
			break;
		}
		default:
			break;
	}

	HAL_GPIO_WritePin(MCU_EN_LEDUP_GPIO_Port, MCU_EN_LEDUP_Pin, up_led_state);
	HAL_GPIO_WritePin(MCU_EN_LEDDN_GPIO_Port, MCU_EN_LEDDN_Pin, dn_led_state);
}
#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
#if (__TARGET_PROJECT__ == __PRIME__)
void emt_status_led_control(EMT_STATUS_LED_TYPE status)
{
	static EMT_STATUS_LED_TYPE previous_status = EMT_APP_STATUS_LED_OFF;
	EMT_ENUM_LED_MODE_TYPE op_mode = EMT_ENUM_LED_OP_MODE_ON;
    uint8_t red = 0, green = 0, blue = 0;
    uint32_t delay_on = 0, delay_off = 0;

    if(previous_status == status)
    {
    	return;
    }
    previous_status =status;

    ktd_led_rgb_set_off();
    if(status == EMT_APP_STATUS_LED_OFF)
    {
    	return;
    }

    switch(status)
	{
		case EMT_APP_STATUS_LED_O_ON:
		{
			red = LED_BRIGHTNESS_MAX;
			green = LED_BRIGHTNESS_MAX/3;
			break;
		}
		case EMT_APP_STATUS_LED_S_ON:
		{
			blue = LED_BRIGHTNESS_MAX;
			green = LED_BRIGHTNESS_MAX/3*2;
			break;
		}
		case EMT_APP_STATUS_LED_Y_ON:
		{
			 red = LED_BRIGHTNESS_MAX;
			green = LED_BRIGHTNESS_MAX/2;
			break;
		}
		case EMT_APP_STATUS_LED_R_ON:
		{
			red = LED_BRIGHTNESS_MAX;
			break;
		}
		case EMT_APP_STATUS_LED_O_NORMAL_BLINK:
		{
			red = LED_BRIGHTNESS_MAX;
			green = LED_BRIGHTNESS_MAX/3;
			delay_on = 1000;
			delay_off = 1000;
			op_mode = EMT_ENUM_LED_OP_MODE_BLINK;
			break;
		}
		case EMT_APP_STATUS_LED_Y_NORMAL_BLINK:
		{
			red = LED_BRIGHTNESS_MAX;
			green = LED_BRIGHTNESS_MAX/2;
			delay_on = 1000;
			delay_off = 1000;
			op_mode = EMT_ENUM_LED_OP_MODE_BLINK;
			break;
		}
		case EMT_APP_STATUS_LED_R_NORMAL_BLINK:
		{
			red = LED_BRIGHTNESS_MAX;
			delay_on = 1000;
			delay_off = 1000;
			op_mode = EMT_ENUM_LED_OP_MODE_BLINK;
			break;
		}
		case EMT_APP_STATUS_LED_R_PATTERN:
		{
			break;
		}
		case EMT_APP_STATUS_LED_G_O_TOGGLE:
		{
			green = LED_BRIGHTNESS_MAX/3;
			ktd_led_rgb_set(0, green, 0);
			green = 0;

			red = LED_BRIGHTNESS_MAX;
			delay_on = 1000;
			delay_off = 1000;
			op_mode = EMT_ENUM_LED_OP_MODE_BLINK;
			break;
		}
		case EMT_APP_STATUS_LED_G_O_FAST_TOGGLE:
		{
			green = LED_BRIGHTNESS_MAX/3;
			ktd_led_rgb_set(0, green, 0);
			green = 0;

			red = LED_BRIGHTNESS_MAX;
			delay_on = 500;
			delay_off = 500;
			op_mode = EMT_ENUM_LED_OP_MODE_BLINK;
			break;
		}
		default:
			break;
    }

    if(op_mode == EMT_ENUM_LED_OP_MODE_BLINK)
    {
    	ktd_led_rgb_blink_set(red, green, blue, delay_on, delay_off);
    }
    else
    {
    	ktd_led_rgb_set(red, green, blue);
    }
}

void emt_status_led_rgb_test(EMT_STATUS_LED_RGB_INFO *p_rgb_info)
{
	ktd_led_rgb_set_off();
    if((p_rgb_info->delay_on > 0) && (p_rgb_info->delay_off > 0))
    {
    	ktd_led_rgb_blink_set(p_rgb_info->r, p_rgb_info->g, p_rgb_info->b, p_rgb_info->delay_on, p_rgb_info->delay_off);
    }
    else
    {
    	ktd_led_rgb_set(p_rgb_info->r, p_rgb_info->g, p_rgb_info->b);
    }
}
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)

static void
emt_led_reset_pattern_info(led_config_t *p_led_config)
{
	p_led_config->pattern_idx = 0xFF;
	p_led_config->pattern_info.total_ms = 0;
	//p_led_config->pattern_info.current_value = p_led_config->pattern_info.start_value;
	p_led_config->pattern_info.current_value = p_led_config->pattern_info.sequence[0].onoff;
}

static void
emt_led_control_onoff(led_type_t led_type, uint8_t onoff)
{
	GPIO_PinState pin_state = GPIO_PIN_RESET;
	switch (led_type)
	{
		case LED_TYPE_R:
		{
			pin_state = HAL_GPIO_ReadPin(MCU_EN_RLED_GPIO_Port, MCU_EN_RLED_Pin);
			if ( pin_state != onoff )
			{
				HAL_GPIO_WritePin(MCU_EN_RLED_GPIO_Port, MCU_EN_RLED_Pin, onoff);
			}
			break;
		}
		case LED_TYPE_G:
		{
			pin_state = HAL_GPIO_ReadPin(MCU_EN_GLED_GPIO_Port, MCU_EN_GLED_Pin);
			if ( pin_state != onoff )
			{
				HAL_GPIO_WritePin(MCU_EN_GLED_GPIO_Port, MCU_EN_GLED_Pin, onoff);
			}
			break;
		}
#if !defined(__MCU_SWD_DEBUG_ENABLE__)
		case LED_TYPE_B:
		{
			pin_state = HAL_GPIO_ReadPin(MCU_EN_BLED_GPIO_Port, MCU_EN_BLED_Pin);
			if ( pin_state != onoff )
			{
				HAL_GPIO_WritePin(MCU_EN_BLED_GPIO_Port, MCU_EN_BLED_Pin, onoff);
			}
			break;
		}
		case LED_TYPE_W:
		{
			pin_state = HAL_GPIO_ReadPin(MCU_EN_WLED_GPIO_Port, MCU_EN_WLED_Pin);
			if ( pin_state != onoff )
			{
				HAL_GPIO_WritePin(MCU_EN_WLED_GPIO_Port, MCU_EN_WLED_Pin, onoff);
			}
			break;
		}
#endif		
		default:
			break;
	}
}

#if !defined(__MICOM_BOOTLOADER__)
void emt_status_led_rgb_test(EMT_STATUS_LED_RGB_INFO *p_rgb_info)
{
	if ( p_rgb_info == NULL )
	{
		return;
	}
	if (g_led_rgb_test_mode == 0)
	{
		g_led_rgb_test_mode = 1;
	}
	uint32_t rgb_color = RGB(p_rgb_info->r, p_rgb_info->g, p_rgb_info->b);
	GPIO_PinState w_led_state, g_led_state, b_led_state, o_led_state;

	#ifdef __MCU_SWD_DEBUG_ENABLE__
	//2025.02.28 [arkyoung] white and blue led is not working
	if ( rgb_color == LED_RGB_COLOR_WHITE || rgb_color == LED_RGB_COLOR_BLUE )
	{
		return;
	}
	#else
	w_led_state = HAL_GPIO_ReadPin(MCU_EN_WLED_GPIO_Port, MCU_EN_WLED_Pin);
	b_led_state = HAL_GPIO_ReadPin(MCU_EN_BLED_GPIO_Port, MCU_EN_BLED_Pin);
	#endif

	if ( rgb_color == LED_RGB_COLOR_WHITE )
	{
		if ( p_rgb_info->delay_on > 0 )
		{
			w_led_state = GPIO_PIN_SET;
		}
		else
		{
			w_led_state = GPIO_PIN_RESET;
		}
		HAL_GPIO_WritePin(MCU_EN_WLED_GPIO_Port, MCU_EN_WLED_Pin, w_led_state);	
	}
	else 
	{
		switch (rgb_color)
		{			
			case LED_RGB_COLOR_WHITE:
			{
				w_led_state = GPIO_PIN_SET;
				break;
			}			
			case LED_RGB_COLOR_RED:
			{
				g_led_state = GPIO_PIN_RESET;
				b_led_state = GPIO_PIN_RESET;
				o_led_state = GPIO_PIN_SET;
				break;
			}
			case LED_RGB_COLOR_GREEN:
			{
				g_led_state = GPIO_PIN_SET;
				b_led_state = GPIO_PIN_RESET;
				o_led_state = GPIO_PIN_RESET;
				break;
			}
			case LED_RGB_COLOR_BLUE:
			{
				g_led_state = GPIO_PIN_RESET;
				b_led_state = GPIO_PIN_SET;
				o_led_state = GPIO_PIN_RESET;
				break;
			}
			default:
				g_led_state = GPIO_PIN_RESET;
				b_led_state = GPIO_PIN_RESET;
				o_led_state = GPIO_PIN_RESET;
				w_led_state = GPIO_PIN_RESET;
				break;
		}
		HAL_GPIO_WritePin(MCU_EN_GLED_GPIO_Port, MCU_EN_GLED_Pin, g_led_state);		
		HAL_GPIO_WritePin(MCU_EN_RLED_GPIO_Port, MCU_EN_RLED_Pin, o_led_state);
		#if !defined(__MCU_SWD_DEBUG_ENABLE__)
		HAL_GPIO_WritePin(MCU_EN_BLED_GPIO_Port, MCU_EN_BLED_Pin, b_led_state);
		HAL_GPIO_WritePin(MCU_EN_WLED_GPIO_Port, MCU_EN_WLED_Pin, w_led_state);
		#endif
	}
}
#endif

static void
emt_led_config_group_control(led_config_group_t *p_led_config_group)
{
    uint16_t i = 0;
    for ( i = 0; i < p_led_config_group->led_config_real_size; i++ )
    {
        led_config_t *p_led_config = &p_led_config_group->led_configs[i];
        if ( p_led_config->enable == LED_ENABLE_ON )
        {
            emt_led_control_onoff(p_led_config->led_type, LED_ON);
        }
        else if ( p_led_config->enable == LED_ENABLE_PATTERN )
        {
            // 패턴 초기화 (처음 시작 시)
            if ( p_led_config->pattern_info.total_ms == 0 && 
                 p_led_config->pattern_idx == 0xFF )
            {                
                emt_led_control_onoff(p_led_config->led_type, p_led_config->pattern_info.sequence[0].onoff);
                p_led_config->pattern_info.total_ms = 0;
                p_led_config->pattern_idx = 0;
                p_led_config->pattern_info.current_value = p_led_config->pattern_info.sequence[0].onoff;
            }
            else
            {
                // 현재 시간이 현재 패턴 인덱스의 시간을 초과했는지 확인
                if ( p_led_config->pattern_info.total_ms >= p_led_config->pattern_info.sequence[p_led_config->pattern_idx].time )
                {
                    p_led_config->pattern_info.total_ms = 0;
                    p_led_config->pattern_idx++;
                    
                    // 패턴 인덱스가 최대값을 초과하면 처음으로 돌아감
                    if ( p_led_config->pattern_idx >= SAMPLING_COUNT || 
                         p_led_config->pattern_info.sequence[p_led_config->pattern_idx].time == 0 )
                    {
                        p_led_config->pattern_idx = 0;
                    }
                    
                    // 새 패턴 인덱스의 상태값으로 LED 상태 설정
                    p_led_config->pattern_info.current_value = p_led_config->pattern_info.sequence[p_led_config->pattern_idx].onoff;
                    emt_led_control_onoff(p_led_config->led_type, p_led_config->pattern_info.current_value);
                }
            }
            p_led_config->pattern_info.total_ms += p_led_config->pattern_info.scale_ms;
        }
        else
        {
            emt_led_control_onoff(p_led_config->led_type, LED_OFF);
        }
    }    
}

static void
emt_device_status_led_control(EMT_DEVICE_STATUS_LED_TYPE status)
{

	static EMT_DEVICE_STATUS_LED_TYPE previous_device_status = EMT_DEVICE_STATUS_LED_MAX;	
	if ( status >= EMT_DEVICE_STATUS_LED_MAX )
	{
		return;
	}
	if ( previous_device_status != status )
	{
		emt_led_reset_pattern_info(&g_device_led_config[status].led_configs[0]);
		emt_led_reset_pattern_info(&g_device_led_config[status].led_configs[1]);
		if ( previous_device_status < EMT_DEVICE_STATUS_LED_MAX )
		{
			emt_led_reset_pattern_info(&g_device_led_config[previous_device_status].led_configs[0]);
			emt_led_reset_pattern_info(&g_device_led_config[previous_device_status].led_configs[1]);
		}
		previous_device_status = status;
	}
	led_config_group_t *p_led_config_group = &g_device_led_config[status];
	//start_value 0 은 OFF부터 start_value 1 은 ON부터 시작한다.

	emt_led_config_group_control(p_led_config_group);
}

static void
emt_recording_status_led_control(EMT_RECORDING_STATUS_LED_TYPE status)
{
	static EMT_RECORDING_STATUS_LED_TYPE previous_recording_status = EMT_RECORDING_STATUS_LED_MAX;
	if ( status >= EMT_RECORDING_STATUS_LED_MAX )
	{
		return;
	}
	if ( previous_recording_status != status )
	{
		emt_led_reset_pattern_info(&g_recording_led_config[status].led_configs[0]);
		emt_led_reset_pattern_info(&g_recording_led_config[status].led_configs[1]);
		if ( previous_recording_status < EMT_RECORDING_STATUS_LED_MAX )
		{
			emt_led_reset_pattern_info(&g_recording_led_config[previous_recording_status].led_configs[0]);
			emt_led_reset_pattern_info(&g_recording_led_config[previous_recording_status].led_configs[1]);
		}
		previous_recording_status = status;
	}
	led_config_group_t *p_led_config_group = &g_recording_led_config[status];
	//start_value 0 은 OFF부터 start_value 1 은 ON부터 시작한다.

	emt_led_config_group_control(p_led_config_group);
}

#if !defined(__MICOM_BOOTLOADER__)
void
emt_security_led_control(EMT_FRONT_LED_TYPE mode)
{
	static EMT_FRONT_LED_TYPE prev_front_led = EMT_APP_FRONT_LED_MAX;	
	if ( mode >= EMT_APP_FRONT_LED_MAX )
	{
		return;
	}
	if ( prev_front_led != mode )
	{
		emt_led_reset_pattern_info(&g_security_led_config[mode].led_configs[0]);
		emt_led_reset_pattern_info(&g_security_led_config[mode].led_configs[1]);
		if ( prev_front_led < EMT_APP_FRONT_LED_MAX )
		{
			emt_led_reset_pattern_info(&g_security_led_config[prev_front_led].led_configs[0]);
			emt_led_reset_pattern_info(&g_security_led_config[prev_front_led].led_configs[1]);
		}
		prev_front_led = mode;
	}
	led_config_group_t *p_led_config_group = &g_security_led_config[mode];
	//start_value 0 은 OFF부터 start_value 1 은 ON부터 시작한다.

	emt_led_config_group_control(p_led_config_group);	
}
#endif
//주기적으로 LED 동작 제어 함수
void emt_status_led_control(EMT_STATUS_LED_TYPE status)
{
#ifdef __MCU_SWD_DEBUG_ENABLE__
	//return;
#endif
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_GPIO__)
	
	emt_device_status_led_control(DEVICE_STATUS_LED_VALUE(status));
	emt_recording_status_led_control(RECORDING_STATUS_LED_VALUE(status));
#endif
}
#elif (__TARGET_PROJECT__ == __MAGNUS__)
void emt_status_led_control(EMT_STATUS_LED_TYPE status)
{
	GPIO_PinState g_led_state;

#ifdef __MCU_SWD_DEBUG_ENABLE__
	//return;
#endif
#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_GPIO__)
/* EMT_LED_TYPE
 * (G + O) OFF
 * (G) ON									: 상시 녹화 / 고&저온 주의 상태
 * (O) ON									: CPU ON (부팅시 녹화 개시 전까지 )/ 메뉴 (설정 & 재생) / 고&저온 녹화 정지 상태
 * (G) : 1초 점멸 (1s ON, 1s OFF)			: 매뉴얼 녹화 / 충격 녹화 / 주차 충격 녹화 / 긴급 녹화 정지 녹화 중
 * (O) : 1초 점멸 (1s ON, 1s OFF)			: 긴급 녹화 정지 상태 / 덮어쓰기 팝업(이벤트&ALL) / 주차 이벤트 알림 팝업 / 기타 오류 화면
 * (G) : 0.5초 점멸 (0.5s ON, 0.5s OFF)		: 주차 녹화 하차 캔슬 (상시충격&타임라프스)
 * (O) : 0.5초 점멸 (0.5s ON, 0.5s OFF)		: 주차 녹화 하차 캔슬 (충격퀵)
 * (G) : 점멸 (1s (깜빡임 3번) + 1s OFF)	: 주차 상시 녹화 (상시충격&타임라프스)
 * (O) : 점멸 (1s (깜빡임 3번) + 1s OFF)	: 주차 녹화 대기 중 (충격퀵)
 * (G + O) : 주황 1s ON, 녹색 1s ON			: FW Upgrade / DIAG
 * (G + O) : 주황 0.5s ON, 녹색 0.5s ON		: DIAG - GPS Menu
 */
	g_led_state = HAL_GPIO_ReadPin(MCU_EN_GLED_GPIO_Port, MCU_EN_GLED_Pin);

	switch(status)
	{
	case EMT_APP_STATUS_LED_OFF:
		{
			g_led_state = GPIO_PIN_RESET;
		}
		break;
	case EMT_APP_STATUS_LED_G_ON:
		{
			g_led_state = GPIO_PIN_SET;
		}
		break;
	case EMT_APP_STATUS_LED_O_ON:
		{
			g_led_state = GPIO_PIN_RESET;
		}
		break;
	case EMT_APP_STATUS_LED_G_NORMAL_BLINK:
	case EMT_APP_STATUS_LED_G_FAST_BLINK:
		{
			if(	g_led_state == GPIO_PIN_RESET)
			{
				g_led_state = GPIO_PIN_SET;
			}
			else
			{
				g_led_state = GPIO_PIN_RESET;
			}
		}
		break;
	case EMT_APP_STATUS_LED_O_NORMAL_BLINK:
	case EMT_APP_STATUS_LED_O_FAST_BLINK:
		{
			g_led_state = GPIO_PIN_RESET;
		}
		break;
	case EMT_APP_STATUS_LED_G_O_TOGGLE:
	case EMT_APP_STATUS_LED_G_O_FAST_TOGGLE:
		{
			if(g_led_state == GPIO_PIN_RESET)
			{
				g_led_state = GPIO_PIN_SET;
			}
			else
			{
				g_led_state = GPIO_PIN_RESET;
			}
		}
		break;
	case EMT_APP_STATUS_LED_G_PATTERN:
		{
			g_led_state = g_emt_led_pattern_cycle[g_led_pattern_cnt++];

			if(g_led_pattern_cnt == LED_2000MS_TIMER_CNT)
			{
				g_led_pattern_cnt = 0;
			}
		}
		break;
	case EMT_APP_STATUS_LED_O_PATTERN:
		{
			g_led_state = GPIO_PIN_RESET;

			if(g_led_pattern_cnt == LED_2000MS_TIMER_CNT)
			{
				g_led_pattern_cnt = 0;
			}
		}
		break;
	default:
		break;
	}

	HAL_GPIO_WritePin(MCU_EN_GLED_GPIO_Port, MCU_EN_GLED_Pin, g_led_state);
#endif
}
#endif
#endif

void emt_led_set_front_led(EMT_FRONT_LED_TYPE mode)
{
#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
	g_front_led.mode = mode;
#elif (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
    #if (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	#if !defined(__MICOM_BOOTLOADER__)
		if ( g_front_led != mode )
		{
			g_front_led = mode;
			emt_security_led_control(mode);
		}
	#endif
	#endif
#else
#endif
}

//LED 상태 변경 시 설정하는 함수
void emt_led_set_status_led(EMT_STATUS_LED_TYPE mode)
{
#if (__TARGET_PROJECT__ == __PRIME__)
	#if defined(__MICOM_BOOTLOADER__)
	#else
	emt_led_send_msg(EMT_MSG_LED_STATUS, mode);
	#endif
#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
	if (g_status_led != mode)
	{		
		g_timer_count = g_led_pattern_cnt = 0;
		emt_status_led_control(mode);
		g_status_led = mode;
	}
#else    
	if(g_status_led != mode)
	{
		g_timer_count = g_led_pattern_cnt = 0;
		emt_status_led_control(mode);
	}

	g_status_led = mode;
	
#endif
}

#if(__TYPE_LED_CONTROL_MODE__==__LED_CONTROL_TYPE_I2C__)
void emt_led_i2c_dev_init(void)
{
	if(g_led_i2c_initialize == false)
	{
		h_i2c_led.Instance = LED_I2C_PORT;
		h_i2c_led.Init.Timing = 0x10B17DB5; //MCLK: 64Mhz, SCL: 100K, Rise Time: 110ns, Fall Time: 110ns
		h_i2c_led.Init.OwnAddress1 = 0;
		h_i2c_led.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		h_i2c_led.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		h_i2c_led.Init.OwnAddress2 = 0;
		h_i2c_led.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		h_i2c_led.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		h_i2c_led.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&h_i2c_led) != HAL_OK)
		{
			Error_Handler();
		}
		/** Configure Analogue filter
		*/
		if (HAL_I2CEx_ConfigAnalogFilter(&h_i2c_led, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
		{
			Error_Handler();
		}
		/** Configure Digital filter
		*/
		if (HAL_I2CEx_ConfigDigitalFilter(&h_i2c_led, 0) != HAL_OK)
		{
			Error_Handler();
		}

		g_led_i2c_initialize = true;

#ifdef __SUPPORT_I2C_LED_DEV_KTD202X__
		led_ktd202x_drv_init(&g_emt_dev_led_callback);
#endif
	}
}

void emt_led_i2c_dev_deinit(void)
{
	if(g_led_i2c_initialize == true)
	{
#ifdef __SUPPORT_I2C_LED_DEV_KTD202X__
		led_ktd202x_drv_deinit();
#endif
		if (HAL_I2C_DeInit(&h_i2c_led) != HAL_OK)
		{
			Error_Handler();
		}
		g_led_i2c_initialize = false;
	}
}

static int32_t emt_led_i2c_dev_write(uint16_t reg_addr, uint8_t *p_buf, uint16_t size)
{
	HAL_StatusTypeDef ret = HAL_OK;
	if(g_led_i2c_initialize)
	{
		ret = HAL_I2C_Mem_Write(&h_i2c_led, KTD202X_DEVICE_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, p_buf, size, 1000);
	}
	return (HAL_OK!= ret);
}

static int32_t emt_led_i2c_dev_read(uint16_t reg_addr, uint8_t *p_buf, uint16_t size)
{
	HAL_StatusTypeDef ret = HAL_OK;
	if(g_led_i2c_initialize)
	{
		ret = HAL_I2C_Mem_Read(&h_i2c_led, KTD202X_DEVICE_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, p_buf, size, 1000);
	}
	return (HAL_OK != ret);
}

#if defined(__MICOM_BOOTLOADER__)
#else
/* LED task implementation */
void emt_led_proc_task(void *argument)
{
    osStatus_t status = osOK;
    EMT_LED_MSG_T msg;
    uint8_t led_status = 0;
    uint8_t led_pattern_cnt = 0;
    bool 	led_pattern_flag = RESET;

    for(;;)
    {
        status = osMessageQueueGet(g_ledQueueHandle, &msg, NULL, osWaitForever);
        if (status == osOK)
        {
            switch(msg.command)
            {
                case EMT_MSG_LED_STATUS:
                {
                	led_status = msg.data[0];
                	if(EMT_APP_STATUS_LED_R_PATTERN == (EMT_STATUS_LED_TYPE)led_status)
                	{
                		emt_led_process_timer_init();
                		led_pattern_cnt = 0;
                		led_pattern_flag = SET;
                	}
                	else
                	{
                		emt_status_led_control((EMT_STATUS_LED_TYPE)led_status);
                		emt_led_process_timer_deinit();
                		led_pattern_flag = RESET;
                	}
                	break;
                }
                case EMT_MSG_LED_TIMER_UPDATE:
                {
                	if(led_pattern_flag == SET)
                	{
						if(g_emt_led_pattern_cycle[led_pattern_cnt++] == SET)
						{
							emt_status_led_control(EMT_APP_STATUS_LED_R_ON);
						}
						else
						{
							emt_status_led_control(EMT_APP_STATUS_LED_OFF);
						}

						if(led_pattern_cnt >= sizeof(g_emt_led_pattern_cycle))
						{
							led_pattern_cnt = 0;
						}
                	}
                	break;
                }
            }

            // Clear queue if status change is pending
//            if(msg.command == EMT_MSG_LED_STATUS && osMessageQueueGetCount(g_ledQueueHandle) > 0) {
//                osMessageQueueReset(g_ledQueueHandle);
//            }
        }
    }
}

/* Helper function to send messages to LED task */
osStatus_t emt_led_send_msg(uint8_t cmd, uint8_t status)
{
    EMT_LED_MSG_T msg;
    
    msg.command = cmd;
    msg.data[0] = status;
    
    return osMessageQueuePut(g_ledQueueHandle, &msg, 0, 0);
}
#endif
#endif


