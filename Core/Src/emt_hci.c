/**
  ******************************************************************************
  * @file           : emt_hci.c
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


#include "string.h"
#include "stdio.h"
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os.h"
#endif
#include "main.h"

#include "stm32g0xx_hal.h"

#include "emt_types.h"
#include "emt_app.h"
#include "emt_uart_rx.h"
#include "emt_hci.h"

/* Private macro -------------------------------------------------------------*/
#if defined(__MICOM_BOOTLOADER__)
#define HCI_TX_LOCK
#define HCI_TX_UNLOCK
#else
#define HCI_TX_LOCK             osMutexAcquire(hciTxMutexHandle, osWaitForever);
#define HCI_TX_UNLOCK           osMutexRelease(hciTxMutexHandle);

/* External variables --------------------------------------------------------*/
extern osMutexId_t              hciTxMutexHandle;
extern osMessageQueueId_t       appQueueHandle;
#endif




/* Private variables ---------------------------------------------------------*/
static uint8_t                  g_hci_tx_buffer[EMT_HCI_PACKET_SIZE_MAX] = {0};
static uint8_t                  g_hci_rx_buffer[EMT_HCI_BUFFER_SIZE_MAX] = {0};

static EMT_HCI_PACKETS_INFO     g_hci_packets_info;

static emt_uart_transmit_cb_t   uart_transmit_cb = NULL;


/**
* @brief Initialize uart rx
* @param argument: None
* @retval None
*/
void emt_hci_init(emt_uart_transmit_cb_t uart_send_callback)
{
	memset(&g_hci_packets_info, 0x00, sizeof(EMT_HCI_PACKETS_INFO));
	memset(g_hci_tx_buffer, 0x00, EMT_HCI_PACKET_SIZE_MAX);
	memset(g_hci_rx_buffer, 0x00, EMT_HCI_BUFFER_SIZE_MAX);

	uart_transmit_cb = uart_send_callback;
}

void emt_hci_deinit(void)
{

}

int emt_hci_send(uint8_t dest, uint8_t type, uint8_t gid, uint8_t cid, uint8_t *p_data, uint16_t data_len)
{
	int res = -1;
	uint16_t index = 0;
	uint16_t length = 0;
	uint16_t checksum = 0;

	HCI_TX_LOCK

	g_hci_tx_buffer[length++] = EMT_HCI_HEAD_0;
	g_hci_tx_buffer[length++] = EMT_HCI_HEAD_1;
	g_hci_tx_buffer[length++] = EMT_HCI_DEV_ID_MAIN_MICOM;
	g_hci_tx_buffer[length++] = dest;
	g_hci_tx_buffer[length++] = type;
	g_hci_tx_buffer[length++] = gid;
	g_hci_tx_buffer[length++] = cid;

	g_hci_tx_buffer[length++] = (data_len & 0xFF);
	g_hci_tx_buffer[length++] = (data_len >> 8) & 0xFF;

	if(data_len > 0)
	{
		memcpy(&g_hci_tx_buffer[length], p_data, data_len);
		length += data_len;
	}

	for (index = 0; index < length; index++ )
	{
		checksum += g_hci_tx_buffer[index];
	}

	g_hci_tx_buffer[length++] = checksum & 0xFF;
	g_hci_tx_buffer[length++] = (checksum >> 8) & 0xFF;

	if(uart_transmit_cb)
	{
		res = uart_transmit_cb(g_hci_tx_buffer, length);
	}

	HCI_TX_UNLOCK

	return res;
}


/**
* @brief parser for command of CPU
* @param argument: None
* @retval None
*/
void emt_hci_packet_receive(EMT_HCI_PACKET_FORMAT *p_hci_packet)
{
	if(p_hci_packet->dest == EMT_HCI_DEV_ID_MAIN_MICOM)
	{
		if((p_hci_packet->gid == EMT_HCI_GID_SYSTEM) && (p_hci_packet->cid == EMT_HCI_CID_LOOPBACK))
		{
			emt_hci_send(p_hci_packet->src, EMT_HCI_TYPE_NOTI, p_hci_packet->gid, p_hci_packet->cid, p_hci_packet->payload, p_hci_packet->payload_length);
		}
		else
		{
#if defined(__MICOM_BOOTLOADER__)
			extern void emt_app_cpu_hci_cmd_proc(EMT_HCI_PACKET_MSG *hci_msg);
			EMT_HCI_PACKET_MSG hci_msg;

			hci_msg.src = p_hci_packet->src;
			hci_msg.type = p_hci_packet->type;
			hci_msg.gid = p_hci_packet->gid;
			hci_msg.cid = p_hci_packet->cid;
			hci_msg.payload_length = (p_hci_packet->payload_length > HCI_MSG_DATA_LEN)?HCI_MSG_DATA_LEN:p_hci_packet->payload_length;
			memcpy(hci_msg.payload, p_hci_packet->payload, hci_msg.payload_length);

			emt_app_cpu_hci_cmd_proc(&hci_msg);
#else
			EMT_MSGQUEUE_T msg;
			EMT_HCI_PACKET_MSG *p_hci_msg = (EMT_HCI_PACKET_MSG*)&msg.data[0];

			p_hci_msg->src = p_hci_packet->src;
			p_hci_msg->type = p_hci_packet->type;
			p_hci_msg->gid = p_hci_packet->gid;
			p_hci_msg->cid = p_hci_packet->cid;
			p_hci_msg->payload_length = (p_hci_packet->payload_length > HCI_MSG_DATA_LEN)?HCI_MSG_DATA_LEN:p_hci_packet->payload_length;
			memcpy(p_hci_msg->payload, p_hci_packet->payload, p_hci_msg->payload_length);
			msg.command = EMT_MSG_RECV_NEW_HCI_CPU_CMD;
			msg.data_len = sizeof(EMT_HCI_PACKET_MSG);

			osMessageQueuePut (appQueueHandle, &msg, 0, 0);
#endif
		}
	}
	else
	{
		if(p_hci_packet->dest == EMT_HCI_DEV_ID_FCAM_MICOM)
		{

		}
		else if(p_hci_packet->dest == EMT_HCI_DEV_ID_DIAGNOSTIC)
		{

		}
	}
}


void emt_hci_rx_process_data(const void* data, size_t len)
{
	const uint8_t* d = data;

	if(len >= (EMT_HCI_BUFFER_SIZE_MAX - g_hci_packets_info.hci_packet_index))
	{
		g_hci_packets_info.hci_packet_index = 0;
	}

	for (; len > 0; --len, ++d)
	{
		if(g_hci_packets_info.hci_status == EMT_ENUM_HCI_STATUS_NOT_READY)
		{
			if(*d == EMT_HCI_HEAD_0)
			{
				g_hci_packets_info.hci_packet_index = 0;
				g_hci_packets_info.checksum = 0;
				g_hci_packets_info.hci_status = EMT_ENUM_HCI_STATUS_NEW_HCI_HEAD_CHECK;
			}
		}
		else if((g_hci_packets_info.hci_status == EMT_ENUM_HCI_STATUS_NEW_HCI_HEAD_CHECK) && (*d == EMT_HCI_HEAD_1) && (g_hci_packets_info.hci_packet_index == 1))
		{
			g_hci_packets_info.hci_status = EMT_ENUM_HCI_STATUS_NEW_HCI_READY;
		}

		g_hci_rx_buffer[g_hci_packets_info.hci_packet_index++] = *d;
		if((g_hci_packets_info.hci_status & EMT_ENUM_HCI_STATUS_NEW_HCI) && (g_hci_packets_info.hci_status != EMT_ENUM_HCI_STATUS_NEW_HCI_CHECKSUM))
		{
			g_hci_packets_info.checksum += *d;
		}

		if((g_hci_packets_info.hci_status & EMT_ENUM_HCI_STATUS_NEW_HCI) && (g_hci_packets_info.hci_packet_index >= EMT_HCI_HEAD_SIZE))
		{
			EMT_HCI_PACKET_FORMAT *p_hci_packet = (EMT_HCI_PACKET_FORMAT*)&g_hci_rx_buffer[0];
			if ( g_hci_packets_info.hci_packet_index == EMT_HCI_HEAD_SIZE)
			{
				uint16_t max_payload_length;
				/* Check payload length validity based on GID */
				if(p_hci_packet->gid == EMT_HCI_GID_FW_UPDATE)
				{
					/* FW update packets can be up to 1040 bytes */
					max_payload_length = EMT_HCI_PAYLOAD_SIZE_MAX;
				}
				else
				{
					/* Normal packets are limited to 64 bytes */
					max_payload_length = EMT_HCI_PAYLOAD_SIZE_NORMAL;
				}

				/* Validate payload length */
				if(p_hci_packet->payload_length > max_payload_length)
				{
					/* Invalid packet - reset packet processing state */				
					memset(&g_hci_packets_info, 0x00, sizeof(EMT_HCI_PACKETS_INFO));						
					//test codes
					#if 0
					emt_hci_send(EMT_HCI_DEV_ID_CPU, EMT_HCI_TYPE_NOTI, EMT_HCI_GID_SYSTEM, EMT_HCI_CID_PACKET_CHECK, (uint8_t*)&p_hci_packet->payload_length, sizeof(uint16_t));
					emt_led_set_front_led(EMT_APP_FRONT_LED_2HZ_BLINK);
					#endif
					return;
				}
			}
			if(g_hci_packets_info.hci_packet_index == (p_hci_packet->payload_length + EMT_HCI_HEAD_SIZE))
			{
				g_hci_packets_info.hci_status = EMT_ENUM_HCI_STATUS_NEW_HCI_CHECKSUM;
			}
			else if(g_hci_packets_info.hci_packet_index == (p_hci_packet->payload_length + EMT_HCI_PACKET_SIZE_MIN))
			{
				uint16_t checksum = g_hci_rx_buffer[g_hci_packets_info.hci_packet_index - 2] | (g_hci_rx_buffer[g_hci_packets_info.hci_packet_index - 1] << 8);
				if(g_hci_packets_info.checksum == checksum)
				{
					emt_hci_packet_receive((EMT_HCI_PACKET_FORMAT*)&g_hci_rx_buffer[0]);
				}

				memset(&g_hci_packets_info, 0x00, sizeof(EMT_HCI_PACKETS_INFO));
			}
		}
	}
}
