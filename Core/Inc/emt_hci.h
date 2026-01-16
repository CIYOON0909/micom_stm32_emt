/**
  ******************************************************************************
  * @file           : emt_hci.c
  * @brief          : EMT UART Host Communication Interface header
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#ifndef EMT_HCI_H
#define EMT_HCI_H

#include "emt_types.h"

/*****************************************************************************
 * define
 *****************************************************************************/

#define HCI_MSG_DATA_LEN						MSG_DATA_MAX_LEN - 6

/*
	EMT HCI PACKET FORMAT
*/
#define EMT_HCI_HEAD_0							0XE8
#define EMT_HCI_HEAD_1							0X6D


#define EMT_HCI_HEAD_SIZE						9
#define EMT_HCI_PAYLOAD_SIZE_MAX				1040    /* firmware packet info : index, length, packet */
#define EMT_HCI_PAYLOAD_SIZE_NORMAL				64    /* normal packet max payload size */
#define EMT_HCI_CHECKSUM_SIZE					2

#define EMT_HCI_PACKET_SIZE_MIN					(EMT_HCI_HEAD_SIZE + EMT_HCI_CHECKSUM_SIZE)  /* head[2], srcID, destID, Type, GID, CID, payload length[2], , CheckSum[2] */
#define EMT_HCI_PACKET_SIZE_MAX					(EMT_HCI_PACKET_SIZE_MIN + EMT_HCI_PAYLOAD_SIZE_MAX)
#define EMT_HCI_BUFFER_SIZE_MAX					(EMT_HCI_PACKET_SIZE_MAX + 128)


/*
	EMT_HOST_IF_DEVICE_ID
*/
#define EMT_HCI_DEV_ID							0x80
#define EMT_HCI_DEV_ID_CPU						(EMT_HCI_DEV_ID|0x01)
#define EMT_HCI_DEV_ID_MAIN_MICOM				(EMT_HCI_DEV_ID|0x02)
#define EMT_HCI_DEV_ID_FCAM_MICOM				(EMT_HCI_DEV_ID|0x04)
#define EMT_HCI_DEV_ID_DIAGNOSTIC				(EMT_HCI_DEV_ID|0x08)

#define EMT_HCI_GID_SYSTEM						0x01
#define EMT_HCI_GID_PARKING						0x02
#define EMT_HCI_GID_GPS							0x03
#define EMT_HCI_GID_ADC							0x04
#define EMT_HCI_GID_GSENSOR						0x05
#define EMT_HCI_GID_LED							0x06
#define EMT_HCI_GID_RADAR						0x07
#define EMT_HCI_GID_FW_UPDATE					0xF0
#define EMT_HCI_GID_DIAGNOSTIC					0x90


/*
	EMT_HCI_GID_SYSTEM
*/
#define EMT_HCI_CID_FW_VERSION					0x01
#define EMT_HCI_CID_WATCHDOG_CLEAR				0x02
#define EMT_HCI_CID_WATCHDOG_CONFIG				0x03
#define EMT_HCI_CID_SYSTEM_READY				0x04
#define EMT_HCI_CID_TEMPERATURE_WARNING			0x05
#define EMT_HCI_CID_CPU_POWER_OFF				0x06
#define EMT_HCI_CID_CPU_POWER_RESET				0x07
#define EMT_HCI_CID_MICOM_RESET_CPU_DCDC_OFF	0x08
#define EMT_HCI_CID_BP_DETACHED					0x09
#define EMT_HCI_CID_ACC_STATE					0x0A
#define EMT_HCI_CID_PERIODIC_DATA_ENABLE		0x0B
#define EMT_HCI_CID_RTC_TIME_GET				0x0C
#define EMT_HCI_CID_RTC_TIME_SET				0x0D
#define EMT_HCI_CID_LOOPBACK					0xFB

/*
	EMT_HCI_GID_PARKING
*/
#define EMT_HCI_CID_PARKING_EVENT_IGNORE_TIME	0x01
#define EMT_HCI_CID_PARKING_QUICK_PRESSED		0x02
#define EMT_HCI_CID_PARKING_CUT_OFF_VOL			0x03
#define EMT_HCI_CID_PARKING_AI_EVENT			0x04
#define EMT_HCI_CID_PARKING_REC_ENABLE_SET		0x05
#define EMT_HCI_CID_PARKING_MANUAL_ENABLE_SET	0x06
#define EMT_HCI_CID_PARKING_MANUAL_ACC_SET		0x07

/*
	EMT_HCI_GID_GPS
*/
#define EMT_HCI_CID_GPS_RESET					0x01
#define EMT_HCI_CID_GPS_CONFIG					0x02
#define EMT_HCI_CID_GPS_MSG_PVT					0x03
#define EMT_HCI_CID_GPS_MSG_SAT					0x04
#define EMT_HCI_CID_GPS_MSG_RAW					0x05
#define EMT_HCI_CID_GPS_SET_MSG					0x06

/*
	EMT_HCI_GID_ADC
*/
#define EMT_HCI_CID_ADC_PERIODIC_DATA			0x01

/*
	EMT_HCI_GID_GSENSOR
*/
#define EMT_HCI_CID_GSENSOR_PERIODIC_DATA		0x01
#define EMT_HCI_CID_GSENSOR_SENSITIVIY_DATA		0x02
#define EMT_HCI_CID_GSENSOR_PERIOD				0x03

/*
	EMT_HCI_GID_LED
*/
#define EMT_HCI_CID_LED_PATTEN					0x01
#define EMT_HCI_CID_FRONT_LED_PWM				0x02
#define EMT_HCI_CID_LED_SET_RGB_INFO   			0x03

/*
	EMT_HCI_GID_RADAR
*/
#define EMT_HCI_CID_RADAR_RESERVED1				0x01

/*
	EMT_HCI_GID_FW_UPDATE
*/
#define EMT_HCI_CID_ENTER_BOOTLOADER			0x01
#define EMT_HCI_CID_NEW_FW_INFO					0x02
#define EMT_HCI_CID_NEW_FW_PACKET				0x03
#define EMT_HCI_CID_NEW_FW_AREA_ERASE			0x04

/*
	EMT_HCI_GID_DIAGNOSTIC
*/	
#define EMT_HCI_CID_DIAG_RESERVED1				0x01

/*
	EMT_HCI_TYPE
*/
#define EMT_HCI_TYPE_NONE						0x00
#define EMT_HCI_TYPE_CMD						0x01
#define EMT_HCI_TYPE_RESP						0x02
#define EMT_HCI_TYPE_NOTI						0x03

typedef enum {
	EMT_ENUM_HCI_STATUS_NOT_READY				= 0x00,

	EMT_ENUM_HCI_STATUS_NEW_HCI					= 0x20,
	EMT_ENUM_HCI_STATUS_NEW_HCI_HEAD_CHECK		= EMT_ENUM_HCI_STATUS_NEW_HCI | 0x01,
	EMT_ENUM_HCI_STATUS_NEW_HCI_READY			= EMT_ENUM_HCI_STATUS_NEW_HCI | 0x02,
	EMT_ENUM_HCI_STATUS_NEW_HCI_CHECKSUM		= EMT_ENUM_HCI_STATUS_NEW_HCI | 0x03
}EMT_ENUM_HCI_STATUS;


typedef enum {
	EMT_APP_CPU_POWER_OFF,
	EMT_APP_CPU_POWER_OFF_SUSPEND,
	EMT_APP_CPU_POWER_OFF_BDF,
	EMT_APP_CPU_POWER_OFF_TEMPOFF,
	EMT_APP_CPU_POWER_OFF_AI_TEMPOFF,
	EMT_APP_CPU_POWER_OFF_MAX,
	EMT_APP_CPU_POWER_OFF_AI_PARKING,
} EMT_CPU_POWER_OFF_TYPE;


typedef enum {
	EMT_APP_CPU_POWER_RESET,
	EMT_APP_CPU_POWER_RESET_NORMAL_TO_PARKING,
	EMT_APP_CPU_POWER_RESET_PARKING_TO_NORMAL,
	EMT_APP_CPU_POWER_RESET_MAX,
	EMT_APP_CPU_POWER_RESET_AI_PARKING,
} EMT_CPU_RESET_TYPE;


typedef enum
{
	EMT_PERIODIC_DATA_ALL_OFF			= 0x00,

	EMT_PERIODIC_DATA_ADC				= 0x01,
	EMT_PERIODIC_DATA_GSENSOR			= 0x02,
	EMT_PERIODIC_DATA_ACC_STATE			= 0x04,

	EMT_PERIODIC_DATA_ALL_ON			= EMT_PERIODIC_DATA_ADC | EMT_PERIODIC_DATA_GSENSOR | EMT_PERIODIC_DATA_ACC_STATE
} EMT_PERIODIC_DATA_TYPE;

typedef struct {
	uint16_t hci_packet_index;
	EMT_ENUM_HCI_STATUS hci_status;
	uint16_t checksum;
}EMT_HCI_PACKETS_INFO;

typedef struct __PACKED
{
	uint8_t status;
	uint8_t time[7];
} EMT_HCI_RTC_INFO;

typedef struct __PACKED
{
	uint8_t head1;
	uint8_t head2;
	uint8_t src;
	uint8_t dest;
	uint8_t type;
	uint8_t gid;
	uint8_t cid;
	uint16_t payload_length;
	uint8_t payload[4]; /* [0 ~ 1024] */
	//uint16_t cs;
} EMT_HCI_PACKET_FORMAT;

typedef struct __PACKED
{
	uint8_t bp_type;
	uint16_t bp_v24;
	uint16_t bp_v12;
	uint16_t edlc;
	int16_t temperature_value;
	uint16_t temperature_adc;
#if defined(__STM32_TEMPERATURE_ADC__)
	uint16_t internal_temperature_adc;
	uint16_t internal_reference_voltage;
	uint16_t internal_temperature;
#endif
} EMT_HCI_PERIODIC_ADC_DATA;

typedef struct __PACKED  
{
	uint32_t  version;
	uint32_t  fw_size;
	uint32_t  number_of_packets;
} EMT_HCI_FW_UPDATE_INFO;

typedef struct __PACKED
{
	uint32_t  address;
	uint16_t  packet_len;
	uint8_t  packets[4];
} EMT_HCI_FW_PACKET_INFO;

typedef struct 
{
	uint8_t src;
	uint8_t type;
	uint8_t gid;
	uint8_t cid;
	uint16_t payload_length;
	uint8_t payload[HCI_MSG_DATA_LEN];
} EMT_HCI_PACKET_MSG;

typedef int (*emt_uart_transmit_cb_t)(uint8_t *p_data, uint16_t size);

void emt_hci_init(emt_uart_transmit_cb_t uart_send_callback);
void emt_hci_deinit(void);
int emt_hci_send(uint8_t dest, uint8_t type, uint8_t gid, uint8_t cid, uint8_t *p_data, uint16_t data_len);
void emt_hci_packet_receive(EMT_HCI_PACKET_FORMAT *p_hci_packet);
void emt_hci_rx_process_data(const void* data, size_t len);

#endif /* EMT_HCI_H */
