/**
 *****************************************************************************************
 *
 * @file emt_key_core.c
 *
 * @brief Key Core Implementation.
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "emt_key_core.h"
#include <string.h>

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */
/**@brief  key core variable. */
struct emt_key_core_t
{
	bool is_pressed;
	bool is_in_polling;
	bool is_skip_release;
};
/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static emt_key_core_evt_cb_t	s_key_core_evt_cb = NULL;
static struct  emt_key_core_t	s_key_core_info[APP_KEY_REG_COUNT_MAX];

static emt_key_state_t	key_scan_state[APP_KEY_REG_COUNT_MAX] = {APP_KEY_STA_INIT};
static uint8_t			key_long_click_count[APP_KEY_REG_COUNT_MAX] = {0};

static emt_key_state_t	key_read_state[APP_KEY_REG_COUNT_MAX] = {APP_KEY_STA_INIT};
static uint8_t			key_double_click_count[APP_KEY_REG_COUNT_MAX] = {0};
static uint8_t			key_continue_click_count[APP_KEY_REG_COUNT_MAX] = {0};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void emt_key_core_key_polling_cplt(uint8_t key_idx)
{
	s_key_core_info[key_idx].is_pressed    = false;
	s_key_core_info[key_idx].is_in_polling = false;
}

/**
 *****************************************************************************************
 * @brief key click event handler.
 *
 * @param[in] key_idx:   Index of key register.
 * @param[in] key_state: Key state of read procedure.
 *****************************************************************************************
 */
static void emt_key_core_click_event_handler(uint8_t key_idx, emt_key_state_t key_state)
{
	emt_key_click_type_t  key_click_type = APP_KEY_NO_CLICK;

	switch (key_state)
	{
	case APP_KEY_STA_NO_CLICK:
		key_click_type = APP_KEY_NO_CLICK;
		emt_key_core_key_polling_cplt(key_idx);
		break;

	case APP_KEY_STA_SINGLE_CLICK:
		key_click_type = APP_KEY_SINGLE_CLICK;
		emt_key_core_key_polling_cplt(key_idx);
		break;

	case APP_KEY_STA_DOUBLE_CLICK:
		key_click_type = APP_KEY_DOUBLE_CLICK;
		emt_key_core_key_polling_cplt(key_idx);
		break;

	case APP_KEY_STA_LONG_CLICK:
		key_click_type = APP_KEY_LONG_CLICK;
		if(s_key_core_info[key_idx].is_skip_release)
			emt_key_core_key_polling_cplt(key_idx);
		break;

	case APP_KEY_STA_CONTINUE_CLICK:
		key_click_type = APP_KEY_CONTINUE_CLICK;
		if(s_key_core_info[key_idx].is_skip_release)
			emt_key_core_key_polling_cplt(key_idx);
		break;

	case APP_KEY_STA_RELEASE:
		key_click_type = APP_KEY_CONTINUE_RELEASE;
		break;

	default:
		break;
	}

	if(s_key_core_evt_cb)
	{
		s_key_core_evt_cb(key_idx, key_click_type);
	}
}

/**
 *****************************************************************************************
 * @brief key state scan.
 *
 * @param[in] key_idx: Index of key.
 *
 * @return Result of scan.
 *****************************************************************************************
 */
static emt_key_state_t emt_key_core_state_scan(uint8_t key_idx)
{
	emt_key_state_t key_state_return = APP_KEY_STA_INIT;

	switch (key_scan_state[key_idx])
	{
	case APP_KEY_STA_INIT:
		if (s_key_core_info[key_idx].is_pressed)
		{
			key_scan_state[key_idx] = APP_KEY_STA_DEBOUNCE;
		}
		break;

	case APP_KEY_STA_DEBOUNCE:
		if (s_key_core_info[key_idx].is_pressed)
		{
			key_long_click_count[key_idx] = 0;
			key_scan_state[key_idx]       = APP_KEY_STA_PRESS;
		}
		else
		{
			key_scan_state[key_idx] = APP_KEY_STA_INIT;
			key_state_return        = APP_KEY_STA_NO_CLICK;
		}
		break;

	case APP_KEY_STA_PRESS:
		if (!s_key_core_info[key_idx].is_pressed)
		{
			key_state_return        = APP_KEY_STA_SINGLE_CLICK;
			key_scan_state[key_idx] = APP_KEY_STA_INIT;
		}
		else
		{
			if ( APP_KEY_LONG_TIME_COUNT <= ++key_long_click_count[key_idx])
			{
				key_state_return        = APP_KEY_STA_LONG_CLICK;
				if(s_key_core_info[key_idx].is_skip_release)
				{
					key_scan_state[key_idx] = APP_KEY_STA_INIT;
				}
				else
				{
					key_scan_state[key_idx] = APP_KEY_STA_WAIT_RELEASE;
				}
			}
		}
		break;

	case APP_KEY_STA_WAIT_RELEASE:
		if (!s_key_core_info[key_idx].is_pressed)
		{
			key_scan_state[key_idx] = APP_KEY_STA_INIT;
			emt_key_core_key_polling_cplt(key_idx);
			key_state_return = APP_KEY_STA_RELEASE;
		}
		else
		{
			key_state_return = APP_KEY_STA_CONTINUE_CLICK;
		}
		break;

	default:
		break;
	}

	return key_state_return;
}

/**
 *****************************************************************************************
 * @brief key click type read.
 *
 * @param[in] key_idx: Index of key register.
 *****************************************************************************************
 */
static void emt_key_core_click_read(uint8_t key_idx)
{
	emt_key_state_t current_key_state;

	current_key_state = emt_key_core_state_scan(key_idx);

	switch (key_read_state[key_idx])
	{
	case APP_KEY_STA_INIT:
		if (APP_KEY_STA_SINGLE_CLICK == current_key_state)
		{
			key_double_click_count[key_idx] = 0;
			key_read_state[key_idx]         = APP_KEY_STA_SINGLE_CLICK;
		}
		else if (APP_KEY_STA_NO_CLICK == current_key_state || APP_KEY_STA_LONG_CLICK == current_key_state)
		{
			emt_key_core_click_event_handler(key_idx, current_key_state);
			if(s_key_core_info[key_idx].is_skip_release)
			{
				key_read_state[key_idx] = APP_KEY_STA_INIT;
			}
		}
		else if (APP_KEY_STA_CONTINUE_CLICK == current_key_state)
		{
			if (APP_KEY_CONTINUE_TIME_COUNT <= ++key_continue_click_count[key_idx])
			{
				key_continue_click_count[key_idx] = 0;
				key_read_state[key_idx]          = APP_KEY_STA_CONTINUE_CLICK;
				emt_key_core_click_event_handler(key_idx, APP_KEY_STA_CONTINUE_CLICK);
				if(s_key_core_info[key_idx].is_skip_release)
				{
					key_read_state[key_idx] = APP_KEY_STA_INIT;
				}
			}
		}
		break;

	case APP_KEY_STA_SINGLE_CLICK:
		if (APP_KEY_STA_SINGLE_CLICK == current_key_state)
		{
			key_read_state[key_idx] = APP_KEY_STA_INIT;
			emt_key_core_click_event_handler(key_idx, APP_KEY_STA_DOUBLE_CLICK);
		}
		else if (APP_KEY_DOUBLE_TIME_COUNT <= ++key_double_click_count[key_idx])
		{
			key_read_state[key_idx] = APP_KEY_STA_INIT;
			emt_key_core_click_event_handler(key_idx, APP_KEY_STA_SINGLE_CLICK);
		}
		break;

	case APP_KEY_STA_CONTINUE_CLICK:
		if (APP_KEY_STA_RELEASE == current_key_state)
		{
			key_read_state[key_idx] = APP_KEY_STA_INIT;
			emt_key_core_click_event_handler(key_idx, APP_KEY_STA_RELEASE);
		}
		else if (APP_KEY_STA_CONTINUE_CLICK == current_key_state)
		{
			if (APP_KEY_CONTINUE_TIME_COUNT <= ++key_continue_click_count[key_idx])
			{
				key_continue_click_count[key_idx] = 0;
				key_read_state[key_idx]          = APP_KEY_STA_CONTINUE_CLICK;
				emt_key_core_click_event_handler(key_idx, APP_KEY_STA_CONTINUE_CLICK);
				if(s_key_core_info[key_idx].is_skip_release)
				{
					key_read_state[key_idx] = APP_KEY_STA_INIT;
				}
			}
		}
		break;

	default:
		break;
	}
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
bool emt_key_core_cb_register(emt_key_core_evt_cb_t key_core_evt_cb)
{
	if (NULL == key_core_evt_cb)
	{
		return false;
	}

	s_key_core_evt_cb = key_core_evt_cb;

	return true;
}

void emt_key_core_polling_10ms()
{
	for (uint8_t key_idx = 0; key_idx < APP_KEY_REG_COUNT_MAX; key_idx++)
	{
		if (s_key_core_info[key_idx].is_in_polling)
		{
			emt_key_core_click_read(key_idx);
		}
	}
}

void emt_key_core_key_wait_polling_record(uint8_t key_idx, bool is_skip_release)
{
	s_key_core_info[key_idx].is_in_polling = true;
	s_key_core_info[key_idx].is_skip_release = is_skip_release;
}

void emt_key_core_key_pressed_record(uint8_t key_idx, bool is_pressed)
{
	s_key_core_info[key_idx].is_pressed = is_pressed;
}

bool emt_key_core_is_all_release(void)
{
	for (uint8_t key_idx = 0; key_idx < APP_KEY_REG_COUNT_MAX; key_idx++)
	{
		if ((s_key_core_info[key_idx].is_pressed) && (!s_key_core_info[key_idx].is_skip_release))
		{
			return false;
		}
	}

	return true;
}
