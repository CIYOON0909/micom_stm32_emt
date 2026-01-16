/**
 ****************************************************************************************
 *
 * @file emt_key.h
 *
 * @brief Key API
 *
 ****************************************************************************************
 */

#ifndef __EMT_KEY_H__
#define __EMT_KEY_H__

#include "emt_key_core.h"
#include "emt_io.h"
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
	GPIO_TypeDef		*port;			/**< Key gpio port(GPIOA, GPIOB, GPIOC...) */
	GPIO_InitTypeDef	gpio;			/**< Key gpio structure  */
	uint8_t				key_id;			/**< Key register ID. */
	bool				is_skip_release;
} emt_key_gpio_t;


typedef struct
{
	GPIO_TypeDef		*gpio_port;			/**< Key gpio port(GPIOA, GPIOB, GPIOC...) */
	uint32_t			gpio_pin;			/**< Key gpio pin. */
	uint32_t			trigger_mode;		/**< Specifies the operating mode for the selected pin. */
	uint32_t			pull;				/**< Pull mode.*/
	uint8_t				key_id;				/**< Key register ID. */
	bool				is_skip_release;
} emt_key_info_t;


typedef void (*emt_key_evt_cb_t)(uint8_t key_id, emt_key_click_type_t key_click_type);


void emt_key_isr_handler(uint16_t GPIO_Pin);


bool emt_key_init(emt_key_gpio_t key_inst[], uint8_t key_num, emt_key_evt_cb_t key_click_cb);


#endif

