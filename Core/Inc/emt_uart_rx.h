/**
	******************************************************************************
	* @file           : emt_uart_rx.h
	* @brief          :
	******************************************************************************
	* Created on      : 21 July, 2023
	*      Author     : kimjj
	*
	******************************************************************************
*/

#ifndef INC_EMT_UART_RX_H_
#define INC_EMT_UART_RX_H_

#include "emt_types.h"

void emt_uart_init(void);
void emt_uart_deinit(void);
int emt_uart_send(uint8_t *p_data, uint16_t size);
void emt_uart_rx_check(void);
#if defined(__MICOM_BOOTLOADER__)
#else
void emt_uart_rx_proc_task(void *argument);
#endif
#endif /* INC_EMT_UART_RX_H_ */
