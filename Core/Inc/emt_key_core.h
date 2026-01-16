/**
 ****************************************************************************************
 *
 * @file emt_key_core.h
 *
 * @brief Key Core API
 *
 *****************************************************************************************
 */

#ifndef __EMT_KEY_CORE_H__
#define __EMT_KEY_CORE_H__

#include <stdint.h>
#include <stdbool.h>


#define APP_KEY_REG_COUNT_MAX         10   	/**< Maximum number of key instance can register, which can be configurable. */
#define APP_KEY_DOUBLE_TIME_COUNT     10   	/**< Double click time count(in unit of 10ms), which can be configurable. */
#define APP_KEY_LONG_TIME_COUNT       24  	/**< Long click time count(in unit of 10ms), which can be configurable. */
#define APP_KEY_CONTINUE_TIME_COUNT   10   	/**< Continue click time count(in unit of 10ms), which can be configurable. */


/**@brief App key polling state. */
typedef enum
{
	APP_KEY_STA_INIT,                /**< Key initialization state for key scan procedure. */
	APP_KEY_STA_DEBOUNCE,            /**< Key debounce state for key scan procedure. */
	APP_KEY_STA_PRESS,               /**< Key press state for key scan procedure. */
	APP_KEY_STA_WAIT_RELEASE,        /**< Wait key release state for key scan procedure. */
	APP_KEY_STA_NO_CLICK,            /**< Key no click state for key read procedure. */
	APP_KEY_STA_SINGLE_CLICK,        /**< Key single click state for key read procedure. */
	APP_KEY_STA_DOUBLE_CLICK,        /**< Key double click state for key read procedure. */
	APP_KEY_STA_LONG_CLICK,          /**< Key long click state for key read procedure. */
	APP_KEY_STA_CONTINUE_CLICK,      /**< Key continue click state for key read procedure. */
	APP_KEY_STA_RELEASE,             /**< key continue click release state. */
} emt_key_state_t;

/**@brief App key click event type. */
typedef enum
{
	APP_KEY_NO_CLICK,                /**< Key no click event. */
	APP_KEY_SINGLE_CLICK,            /**< Key single click event. */
	APP_KEY_DOUBLE_CLICK,            /**< Key double click event. */
	APP_KEY_LONG_CLICK,              /**< Key long click event. */
	APP_KEY_CONTINUE_CLICK,          /**< Key continue click event. */
	APP_KEY_CONTINUE_RELEASE         /**< Key continue click release. */
} emt_key_click_type_t;
/** @} */

/**
 * @defgroup APP_KEY_CORE_TYPEDEF Typedefs
 * @{
 */
/**@brief APP Key core event callback.*/
typedef void (*emt_key_core_evt_cb_t)(uint8_t key_idx, emt_key_click_type_t key_click_type);
/** @} */

/**
 * @defgroup APP_KEY_CORE_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief Register app key click event callback.
 *
 * @param[in] key_core_evt_cb: Key core event callback.
 *
 * @return Result of key core event callback is NULL or not.
 *****************************************************************************************
 */
bool emt_key_core_cb_register(emt_key_core_evt_cb_t key_core_evt_cb);

/**
 *****************************************************************************************
 * @brief Record app key is pressed down.
 *
 * @param[in] key_idx:    Index of key register.
 * @param[in] is_pressed: True or false
 *****************************************************************************************
 */
void emt_key_core_key_pressed_record(uint8_t key_idx, bool is_pressed);

/**
 *****************************************************************************************
 * @brief Notificaiton which key is waiting for polling.
 *
 * @param[in] key_idx: Index of key register.
 *****************************************************************************************
 */
void emt_key_core_key_wait_polling_record(uint8_t key_idx, bool is_skip_release);

/**
 *****************************************************************************************
 * @brief Check all keys are released or not.
 *
 * @return Result of checking.
 *****************************************************************************************
 */
bool emt_key_core_is_all_release(void);

/**
 *****************************************************************************************
 * @brief App key state polling.
 *****************************************************************************************
 */
void emt_key_core_polling_10ms(void);
/** @} */

#endif
/** @} */
/** @} */

