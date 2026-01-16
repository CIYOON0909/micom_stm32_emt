/**
 *****************************************************************************************
 *
 * @file emt_key.c
 *
 * @brief Key Implementation.
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "emt_key.h"
#include "cmsis_os.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define EMT_KEY_TIMER_INTERVAL   50    /**<  key polling interval interval (in units of 1ms). */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static emt_key_info_t	s_emt_key_info[APP_KEY_REG_COUNT_MAX];
static emt_key_evt_cb_t	s_emt_key_evt_cb;
static uint8_t			s_emt_key_reg_num;

static osTimerId_t		emt_key_timer_handle = NULL;

static bool				s_is_timer_enabled;

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Polling app key pressed state.
 *****************************************************************************************
 */
static void emt_key_press_state_polling(void)
{
	GPIO_PinState	pin_state;
	bool			is_pressed = false;

	for (uint8_t key_idx = 0; key_idx < s_emt_key_reg_num; key_idx++)
	{
		is_pressed = false;

		pin_state = HAL_GPIO_ReadPin(s_emt_key_info[key_idx].gpio_port, s_emt_key_info[key_idx].gpio_pin);

		if (GPIO_PIN_RESET == pin_state && s_emt_key_info[key_idx].pull == GPIO_PULLUP)
		{
			is_pressed = true;
		}
		else if (GPIO_PIN_SET == pin_state && s_emt_key_info[key_idx].pull == GPIO_PULLDOWN)
		{
			is_pressed = true;
		}

		emt_key_core_key_pressed_record(key_idx, is_pressed);
	}
}

/**
 *****************************************************************************************
 * @brief App key timing timeout handler.
 *****************************************************************************************
 */
static void emt_key_timeout_handler(void *p_arg)
{
	emt_key_press_state_polling();
	emt_key_core_polling_10ms();
}

/**
 *****************************************************************************************
 * @brief Start app key timer.
 *****************************************************************************************
 */
static void emt_key_timer_start(void)
{
	osTimerStart(emt_key_timer_handle, EMT_KEY_TIMER_INTERVAL);

	s_is_timer_enabled = true;
}

/**
 *****************************************************************************************
 * @brief Stop app key timer.
 *****************************************************************************************
 */
void emt_key_timer_stop(void)
{
	osTimerStop(emt_key_timer_handle);

	s_is_timer_enabled = false;
}

/**
 *****************************************************************************************
 * @brief key core event handler.
 *****************************************************************************************
 */
static void emt_key_core_evt_handler(uint8_t key_idx, emt_key_click_type_t key_click_type)
{
	if (emt_key_core_is_all_release())
	{
		emt_key_timer_stop();
	}

	if(s_emt_key_evt_cb)
	{
		s_emt_key_evt_cb(s_emt_key_info[key_idx].key_id, key_click_type);
	}
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void emt_key_isr_handler(uint16_t GPIO_Pin)
{
#if 0
	if (!s_is_timer_enabled)
	{
		emt_key_timer_start();
	}
	
	for (uint8_t key_idx = 0; key_idx < s_emt_key_reg_num; key_idx++)
	{
		if (GPIO_Pin & s_emt_key_info[key_idx].gpio_pin)
		{
			emt_key_core_key_wait_polling_record(key_idx);
			return;
		}
	}
#else
	GPIO_PinState	pin_state;
	bool			is_pressed = false;

	for (uint8_t key_idx = 0; key_idx < s_emt_key_reg_num; key_idx++)
	{
		if (GPIO_Pin & s_emt_key_info[key_idx].gpio_pin)
		{
			pin_state = HAL_GPIO_ReadPin(s_emt_key_info[key_idx].gpio_port, s_emt_key_info[key_idx].gpio_pin);

			if (GPIO_PIN_RESET == pin_state && s_emt_key_info[key_idx].pull == GPIO_PULLUP)
			{
				is_pressed = true;
			}
			else if (GPIO_PIN_SET == pin_state && s_emt_key_info[key_idx].pull == GPIO_PULLDOWN)
			{
				is_pressed = true;
			}

			if(is_pressed)
			{
				if (!s_is_timer_enabled)
				{
					emt_key_timer_start();
				}

				emt_key_core_key_wait_polling_record(key_idx, s_emt_key_info[key_idx].is_skip_release);
			}
		}
	}
#endif
}

bool emt_key_init(emt_key_gpio_t key_inst[], uint8_t key_num, emt_key_evt_cb_t key_evt_cb)
{
	if (APP_KEY_REG_COUNT_MAX < key_num || NULL == key_evt_cb)
	{
		return false;
	}

	for (uint8_t key_idx = 0; key_idx < key_num; key_idx++)
	{
		if (!IS_EXTI_GPIO_PORT(GPIO_GET_INDEX(key_inst[key_idx].port)))
		{
			continue;
		}

		s_emt_key_info[key_idx].key_id = key_inst[key_idx].key_id;
		s_emt_key_info[key_idx].gpio_port = key_inst[key_idx].port;
		s_emt_key_info[key_idx].gpio_pin = key_inst[key_idx].gpio.Pin;
		s_emt_key_info[key_idx].trigger_mode = key_inst[key_idx].gpio.Mode;
		s_emt_key_info[key_idx].pull = key_inst[key_idx].gpio.Pull;
		s_emt_key_info[key_idx].is_skip_release = key_inst[key_idx].is_skip_release;
	}

	s_emt_key_reg_num = key_num;
	s_emt_key_evt_cb  = key_evt_cb;
	emt_key_core_cb_register(emt_key_core_evt_handler);

	/* If useing FreeRTOS, xTimer need to be create and do not create in IRQ handler */
	if(emt_key_timer_handle == NULL)
	{
		emt_key_timer_handle = osTimerNew(emt_key_timeout_handler, osTimerPeriodic, NULL, NULL);
	}
	return true;
}


