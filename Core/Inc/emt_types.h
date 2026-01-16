/**
  ******************************************************************************
  * @file           : emt_types.h
  * @brief          :
  ******************************************************************************
  * Created on      : 21 July, 2023
  *      Author     : kimjj
  *
  ******************************************************************************
  */

#ifndef __EMT_TYPES_H__
#define __EMT_TYPES_H__

#include "stdbool.h"
#include <stdint.h>
#include "stm32g0xx_hal.h"
#include "emt_defines.h"
#if defined(__MICOM_BOOTLOADER__)
#else
#include "cmsis_os2.h"
#include "main.h"
#endif


#if defined(__MICOM_BOOTLOADER__)
#define MSG_DATA_MAX_LEN									(2048)
#else
#define MSG_DATA_MAX_LEN									48
#endif

#define ADC_VALUE_COUNT										10

#define ARRAY_LEN(x)										(sizeof(x) / sizeof((x)[0]))

#define EMT_SUCCESS											0
#define	EMT_FAILURE											-1

typedef int32_t EMT_RESULT;

#define HEX2ASCII( a ) ((a) >= 0x0A ? ((a) - 0x0A + 'A'): ((a)+'0'))
#define ASCII2HEX( a ) ((a) >= 'A' ? ((a) -'A' + 0x0A): ((a)-'0'))

/**
  * @brief macro used to enter the critical section
  */
#define UTILS_ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
  __disable_irq()

/**
  * @brief macro used to exit the critical section
  */
#define UTILS_EXIT_CRITICAL_SECTION()  __set_PRIMASK(primask_bit)

enum
{
	EMT_OFF,
	EMT_ON
};


enum {
	OS_TIMER_50MS = 0,
	OS_TIMER_100MS,
	OS_TIMER_200MS,
	OS_TIMER_250MS,
	OS_TIMER_500MS,
	OS_TIMER_1000MS,
	OS_TIMER_MAX
};

typedef enum
{
	EMT_MSG_KEY_PROCESS,
	EMT_MSG_PERIODIC_50MSEC,
	EMT_MSG_PERIODIC_100MSEC,
	EMT_MSG_PERIODIC_200MSEC,
	EMT_MSG_PERIODIC_250MSEC,
	EMT_MSG_PERIODIC_500MSEC,
	EMT_MSG_PERIODIC_1000MSEC,
	EMT_MSG_RECV_NEW_HCI_CPU_CMD,
	EMT_MSG_RECV_GSENSOR_DATA,

	EMT_MSG_SET_CPU_POWER_UP,
	EMT_MSG_SET_CPU_POWER_DOWN,
	EMT_MSG_SET_CPU_RESET,
	EMT_MSG_ENTER_FW_DOWNLOAD_MODE,

	EMT_MSG_BP_OFF,

	EMT_MSG_GSENSOR_INIT,
	EMT_MSG_GSENSOR_DEINIT,
	EMT_MSG_GSENSOR_OP_MODE,
	EMT_MSG_GSENSOR_SET_SENSITIVITY,
	EMT_MSG_GSENSOR_RX_DATA,
	EMT_MSG_GSENSOR_WAKE_UP,

	EMT_MSG_AI_PARKING_INIT,
	EMT_MSG_AI_PARKING_DATA,

	EMT_MSG_BATTERY_CHECK,
	EMT_MSG_BATTERY_ACC_STATUS,
	EMT_MSG_BATTERY_INFO_DATA,
	EMT_MSG_BATTERY_ACC_ON
} EMT_MSG_ID;

typedef enum
{
	EMT_GSENSOR_OP_POWERDOWN,
	EMT_GSENSOR_OP_NORMAL,				// Normal and Parking Mode
	EMT_GSENSOR_OP_PARKING_AI,			// DIFINE AI Mode
	EMT_GSENSOR_OP_PARKING_LOW_POWER,	// Low Power Parking Mode, AQS Mode
} EMT_GSENSOR_OP_MODE;

typedef enum {
	EMT_GSENSOR_SENS_LOW,
	EMT_GSENSOR_SENS_MID,
	EMT_GSENSOR_SENS_HIGH,
	EMT_GSENSOR_SENS_MAX
} EMT_GSENSOR_SENS_LEVEL; 	//PARKING_SENSITIVITY

typedef enum {
    EMT_MSG_LED_INIT,
    EMT_MSG_LED_DEINIT,
    EMT_MSG_LED_STATUS,
    EMT_MSG_LED_TIMER_UPDATE
} EMT_LED_MSG_CMD;

typedef enum
{
	EMT_CPU_POWER_OFF,
	EMT_CPU_POWER_OFF_WITH_MICOM,
	EMT_CPU_POWER_ON,
	EMT_CPU_POWER_RESET
} EMT_SYSTEM_POWER_MODE;

/*												  FINE	     COMTEC
 	//Common
	EMT_BBX_OP_READY_MODE,						 	O			O
	EMT_BBX_OP_NORMAL_MODE, 					 	O			O
	EMT_BBX_OP_PARKING_AQS_MODE,					O			O
	EMT_BBX_OP_CUT_OFF_MODE,						O			O
	EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE,				O			O

	//FINE Only
	EMT_BBX_OP_PARKING_AI_MODE, 					O			X
	EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE, 			O			X

	//COMTEC Only
	EMT_BBX_OP_PARKING_NORMAL_MODE,					X			O
	EMT_BBX_OP_SYSTEM_OFF_MODE,						X			O
*/

typedef enum
{
	EMT_BBX_OP_READY_MODE,							// MICOM ON,  CPU START
	EMT_BBX_OP_NORMAL_MODE, 						// MICOM ON,  CPU ON(Response CMD from CPU)
	EMT_BBX_OP_PARKING_AQS_MODE,					// MICOM LOW CLK, check G-Sensor INT or ACC (Comtec : Easy Key)
	EMT_BBX_OP_CUT_OFF_MODE,						// MICOM LOW CLK, only check ACC detection
	EMT_BBX_OP_CUT_OFF_HIGH_TEMP_MODE,				// MICOM LOW CLK, check High Temperature
	//FINE Only
	EMT_BBX_OP_PARKING_AI_MODE, 					// MICOM ON,  CPU ON(Motion/TimeLabs)
	EMT_BBX_OP_PARKING_AI_LOW_PWR_MODE, 			// MICOM ON,  CPU OFF
	//COMTEC Only
	EMT_BBX_OP_PARKING_NORMAL_MODE,					// MICOM ON,  CPU ON
	EMT_BBX_OP_SYSTEM_OFF_MODE,						// MICOM LOW CLK, CPU OFF, ACC Check (Comtec : Easy Key)
	EMT_BBX_OP_CUT_OFF_HIGH_TEMP_AI_MODE,			// MICOM LOW CLK, check High Temperature
} EMT_BBX_OP_MODE;

typedef enum {
	FWUP_STATUS_OK,
	FWUP_STATUS_ERROR,
	FWUP_STATUS_ERROR_CHECKSUM,
	FWUP_STATUS_ERROR_FLASH_WRITE,
	FWUP_STATUS_ERROR_VERIFY,
	FWUP_STATUS_ERROR_ERASE,
	FWUP_STATUS_MAX
} EMT_MICOM_FWUP_STATUS_T;

typedef enum
{
	EMT_BP_TYPE_V12,
	EMT_BP_TYPE_V24
} EMT_BP_TYPE;


typedef enum
{
	EMT_APP_FRONT_LED_OFF,
	EMT_APP_FRONT_LED_2HZ_BLINK,
	EMT_APP_FRONT_LED_5HZ_BLINK,
#if (__TARGET_PROJECT__ != __TRINITY__) && (__TARGET_PROJECT__ != __CYCLOPS__)
	EMT_APP_FRONT_LED_DIMMING,
	EMT_APP_FRONT_LED_DIMMING_X5000,
#endif
	EMT_APP_FRONT_LED_ALWAYS_ON,
	EMT_APP_FRONT_LED_MAX
} EMT_FRONT_LED_TYPE;

#if (__TYPE_BB_SCENARIO__ == __BB_FINE__)
/*
 * Status LED Operation
 * OP								DN_LED(POWER),   	UP_LED(EVENT)
 * 주행녹화							ON					ON
 * 주차녹화(대기)					1Hz Blink			OFF
 * 이벤트(주행, 긴급)				ON					1Hz Blink
 * 이벤트(주차, 모션)				1Hz Blink			1Hz Blink
 * 메뉴 진입						ON					OFF
 * SD Format						1Hz Blink			OFF
 * 업그레이드						1Hz Blink			1Hz Blink
 * SD Error(No SD)					OFF					1Hz Blink
 */
typedef enum
{
	EMT_APP_STATUS_LED_NORMAL,
	EMT_APP_STATUS_LED_PARKING,
	EMT_APP_STATUS_LED_EVENT_NORMAL,
	EMT_APP_STATUS_LED_EVENT_PARKING,
	EMT_APP_STATUS_LED_MENU,
	EMT_APP_STATUS_LED_SD_FORMAT,
	EMT_APP_STATUS_LED_FW_UPDATE,
	EMT_APP_STATUS_LED_SD_ERROR,
	EMT_APP_STATUS_LED_OFF,
	EMT_APP_STATUS_LED_BLINK_TEST,
	EMT_APP_STATUS_LED_BLINK_TEST2
} EMT_STATUS_LED_TYPE;

#elif (__TYPE_BB_SCENARIO__ == __BB_COMTEC__) || (__TYPE_BB_SCENARIO__ == __BB_DOMESTIC__)
#if (__TARGET_PROJECT__ == __PRIME__)
/* EMT_LED_TYPE
 * (Sky, Orange, Yellow, Red) OFF
 * (O) 점등											: 기동 모드(녹화 개시까지) / 설정모드 / 재생 모드 / 고온동작주의(76도 이상) / 저온동작주의(-31도 이하)
 * (S) 점등											: 녹화 모드(항상/충격 녹화) / 녹화모드(항상녹화만) / 고온동작주의(71도 이상) / 저온동작주의(-21도 이하)
 * (Y) 점등											: 녹화 모드(충격 녹화만) 
 * (R) 점등											: 녹화 모드/녹음 OFF(일반녹화)
 * (O) 점멸 1s ON <--> 1s OFF	 					: 긴급 녹화 정지 작동시 / 이벤트 녹화 정지 / ALL 녹화 정지 / 에러 검출(에러 화면 표시 중)
 * (Y) 점멸 1s ON <--> 1s OFF						: 녹화 모드(수동 녹화) / 녹화 모드(충격 녹화)
 * (R) 점멸 1s ON <--> 1s OFF						: 주차 감시 모드 녹화 / 주차 감시 충격 녹화	
 * (R) 점멸 (0.3s ON <--> 0.1s OFF) x 3 <--> 1s OFF : 주차 감시 모드(모션 검출) / 주차 감시 모드(충격 빠른 녹화 시) //Comtec에 수정 검토 요청 중
 * (G + O) 교차 점멸 1s G ON <--> 1s O ON			: FW Upgrade
 * (G + O) 교차 점멸 0.5s G ON <--> 0.5s O GN		: DIAG - GPS Menu
 * 	미지정
 */
typedef enum
{
	EMT_APP_STATUS_LED_OFF,
	EMT_APP_STATUS_LED_O_ON,
	EMT_APP_STATUS_LED_S_ON,
	EMT_APP_STATUS_LED_Y_ON,
	EMT_APP_STATUS_LED_R_ON,
	EMT_APP_STATUS_LED_O_NORMAL_BLINK,
	EMT_APP_STATUS_LED_Y_NORMAL_BLINK,
	EMT_APP_STATUS_LED_R_NORMAL_BLINK,
	EMT_APP_STATUS_LED_R_PATTERN,
	EMT_APP_STATUS_LED_G_O_TOGGLE,
	EMT_APP_STATUS_LED_G_O_FAST_TOGGLE,
	EMT_APP_STATUS_LED_MAX
} EMT_STATUS_LED_TYPE;

#define EMT_APP_STATUS_LED_FW_UPDATE	EMT_APP_STATUS_LED_G_O_TOGGLE

typedef enum
{
    EMT_ENUM_LED_OP_MODE_OFF                = 0x00,
    EMT_ENUM_LED_OP_MODE_ON,
    EMT_ENUM_LED_OP_MODE_BLINK
} EMT_ENUM_LED_MODE_TYPE;

#elif (__TARGET_PROJECT__ == __TRINITY__) || (__TARGET_PROJECT__ == __CYCLOPS__)
#define EMT_STATUS_LED_TYPE uint16_t

#define RECORDING_STATUS_LED_MASK (0xFF00)
#define DEVICE_STATUS_LED_MASK (0x00FF)
#define RECORDING_STATUS_LED_SHIFT (8)
#define DEVICE_STATUS_LED_SHIFT (0)
#define SAMPLING_COUNT (6)
#define DEVICE_LED_CONFIG_SIZE (2)
#define RECORDING_LED_CONFIG_SIZE (1)

#define EMT_APP_STATUS_LED_NONE 0xFFFFFFFF

#define RECORDING_STATUS_LED_VALUE(x) \
(((x) & RECORDING_STATUS_LED_MASK) >> RECORDING_STATUS_LED_SHIFT)

#define DEVICE_STATUS_LED_VALUE(x) \
(((x) & DEVICE_STATUS_LED_MASK) >> DEVICE_STATUS_LED_SHIFT)

#define MAKE_STATUS_LED_VALUE(rec, dev) \
(0xFFFF & ((rec) << RECORDING_STATUS_LED_SHIFT | (dev) << DEVICE_STATUS_LED_SHIFT))


#define LED_OFF 0
#define LED_ON 1
typedef enum
{
	LED_TYPE_R,
	LED_TYPE_G,
	LED_TYPE_B,
	LED_TYPE_W,	
	LED_TYPE_MAX,
} led_type_t;

typedef enum
{
	LED_ENABLE_OFF,
	LED_ENABLE_ON,
	LED_ENABLE_PATTERN,
} led_enable_t;

typedef enum
{
	EMT_RECORDING_STATUS_LED_OFF           = 0,
	EMT_RECORDING_STATUS_LED_ON            = 1,
	EMT_RECORDING_STATUS_LED_FAST_BLINK    = 2,
	EMT_RECORDING_STATUS_LED_BLINK         = 3,	
	EMT_RECORDING_STATUS_LED_MAX           = 4
} EMT_RECORDING_STATUS_LED_TYPE;
typedef enum
{
	EMT_DEVICE_STATUS_LED_OFF           = 0,	
	EMT_DEVICE_STATUS_B_R_TOGGLE        = 1,
	EMT_DEVICE_STATUS_R_FAST_BLINK      = 2,
	EMT_DEVICE_STATUS_B_ON              = 3,
	EMT_DEVICE_STATUS_B_BLINK           = 4,
	EMT_DEVICE_STATUS_R_ON              = 5,
	EMT_DEVICE_STATUS_LED_MAX           = 6
} EMT_DEVICE_STATUS_LED_TYPE;

/**
 * onoff_time 은 켜짐 꺼짐 시간을 배열로 저장한다. 이 배열의 최대 갯수는 SAMPLING_COUNT 이다. 결국 On&off 패턴을 0부터 최대 5번 인덱스를 순차적으로 각 배열에 적힌 시간동안 유지하며 반복한다.
 * period_count 는 반복 횟수를 저장한다. 0일 경우 무한 반복이다.
 * scale_ms 는 배열에 적힌 시간을 배율로 곱해준다. 여기서는 timer interrupt 가 50m 주기로 동작하므로 50m 주기로 곱해준다.
 */

#if 1
typedef struct
{
	uint8_t onoff;
	uint16_t time;
} led_sequence_t;
#endif
typedef struct
{	
#if 1	
	led_sequence_t      sequence[SAMPLING_COUNT];
#else
    uint16_t            onoff_sequence[SAMPLING_COUNT];
#endif	
	uint16_t            period_count;
	const uint16_t      scale_ms;
	uint32_t            total_ms;
	//const uint8_t       start_value;	//0: OFF 부터 시작, 1: ON 부터 시작
	uint8_t             current_value;
} led_pattern_info_t;

typedef struct
{
	const uint8_t            enable;
	led_type_t               led_type;
	led_pattern_info_t       pattern_info;
	uint8_t                  pattern_idx;
} led_config_t;

typedef struct
{
	led_config_t led_configs[LED_TYPE_MAX-1];
	uint8_t led_config_real_size;
} led_config_group_t;


#define EMT_APP_STATUS_BOOT_ON          ((EMT_RECORDING_STATUS_LED_OFF<<RECORDING_STATUS_LED_SHIFT) | \
                                        (EMT_DEVICE_STATUS_B_R_TOGGLE<<DEVICE_STATUS_LED_SHIFT))
#define EMT_APP_STATUS_LED_OFF          (0)  // Both REC and DEV LED OFF
#define EMT_APP_STATUS_LED_FW_UPDATE    ((EMT_RECORDING_STATUS_LED_BLINK << RECORDING_STATUS_LED_SHIFT) | \
                                        (EMT_DEVICE_STATUS_B_BLINK << DEVICE_STATUS_LED_SHIFT))  // REC BLINK(3) and DEV BLINK(4)

#define EMT_APP_STATUS_LED_PARKING_AQS_MODE (EMT_RECORDING_STATUS_LED_OFF << RECORDING_STATUS_LED_SHIFT) | \
											(EMT_DEVICE_STATUS_B_BLINK << DEVICE_STATUS_LED_SHIFT)
#define EMT_APP_STATUS_LED_O_FAST_BLINK (0) //temmpory dummy value
#define EMT_APP_STATUS_LED_G_FAST_BLINK (0) //temmpory dummy value
#else
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
typedef enum
{
	EMT_APP_STATUS_LED_OFF,
	EMT_APP_STATUS_LED_G_ON,
	EMT_APP_STATUS_LED_O_ON,
	EMT_APP_STATUS_LED_G_NORMAL_BLINK,
	EMT_APP_STATUS_LED_O_NORMAL_BLINK,
	EMT_APP_STATUS_LED_G_FAST_BLINK,
	EMT_APP_STATUS_LED_O_FAST_BLINK,
	EMT_APP_STATUS_LED_G_PATTERN,
	EMT_APP_STATUS_LED_O_PATTERN,
	EMT_APP_STATUS_LED_G_O_TOGGLE,
	EMT_APP_STATUS_LED_G_O_FAST_TOGGLE,
	EMT_APP_STATUS_LED_MAX
} EMT_STATUS_LED_TYPE;

#define EMT_APP_STATUS_LED_FW_UPDATE	EMT_APP_STATUS_LED_G_O_FAST_TOGGLE
#endif
#endif

typedef enum
{
	EMT_CPU_WATCHDOG_OFF,
	EMT_CPU_WATCHDOG_BOOT_CHECK,
	EMT_CPU_WATCHDOG_WORKING_CHECK,
	EMT_CPU_WATCHDOG_MAX,
} EMT_CPU_WATCHDOG_MODE;


typedef struct {	// object data type
	uint8_t command;
	int8_t 	data_len;
	uint8_t data[MSG_DATA_MAX_LEN];
} EMT_MSGQUEUE_T;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} EMT_GSENSOR_VALUE_T;

typedef struct {
	EMT_GSENSOR_SENS_LEVEL level;
	EMT_GSENSOR_VALUE_T data;
} EMT_GSENSOR_SENSITIVITY_T;

typedef struct {
	EMT_GSENSOR_OP_MODE mode;
	EMT_GSENSOR_SENSITIVITY_T sensitivity;
} EMT_GSENSOR_OP_STRUCT_T;


typedef struct {
    uint8_t command;
    uint8_t data[3];  // Adjust size as needed
} EMT_LED_MSG_T;

typedef struct {
	uint32_t magic_number;
	uint32_t image_size;
	uint32_t version;
	uint32_t number_of_packets;
} __attribute__((packed)) EMT_FW_UPGRADE_INFO_T;

typedef struct {
	uint32_t  address;
	uint32_t  fw_packet_index;
} EMT_FW_DL_STATUS_T;

typedef struct
{
#if defined(__SUPPORT_24V_ADC_CHANNEL__)
	uint16_t bp_v24;	//v24bp_adc_mcu;
#endif
	uint16_t bp_v12;	//v12bp_adc_mcu; // 도매스틱에서 12V 24V 공용으로 사용할듯 변수명 별도로 가져갈 필요있어보임.(중요한 내용은 아님)
	uint16_t temperature;
	uint16_t edlc;
#if defined(__STM32_TEMPERATURE_ADC__)
	uint16_t internal_reference_voltage;
	uint16_t internal_temperature;
#endif
} EMT_ADC_STRUCT_T;

typedef struct
{
	bool	 charge_process_enable;
	bool	 temperature_warning_on;
	bool	 charging_on;
	int32_t	 low_level_count;
	uint32_t high_level_count;
	uint32_t charging_start_delay_count;
	uint16_t last_edlc;
} EMT_EDLC_CHARGE_STRUCT_T;

typedef struct
{
	bool 	cpu_off_wait;			/* CPU 에 EMT_HCI_CID_BP_DETACHED 전송 후 3초 안에 system_power_mode 바뀌지 않는 경우에 EMT_CPU_POWER_OFF 로 변경해주기 위함*/
	int32_t cpu_off_wait_count;
	int32_t	remove_check_count;
} EMT_BP_DETECT_STRUCT_T;

typedef struct
{
	int32_t delay_time;
	int32_t delay_count;
} EMT_BBX_AQS_STRUCT_T;

typedef struct
{
	bool	on;
	int32_t check_adc;
	int32_t check_count;
} EMT_BBX_CUT_OFF_STRUCT_T;

typedef struct
{
	bool	waiting_acc_on;
	int32_t	value;
	int32_t	check_count;
} EMT_TEMPERATURE_STRUCT_T;

#define RGB(r, g, b)	                ((r) << 16 | (g) << 8 | (b))
#define LED_RGB_COLOR_OFF			    RGB(0, 0, 0)
#define LED_RGB_COLOR_WHITE			    RGB(255, 255, 255)
#define LED_RGB_COLOR_BLUE			    RGB(0, 0, 255)
#define LED_RGB_COLOR_GREEN			    RGB(0, 255, 0)
#define LED_RGB_COLOR_YELLOW		    RGB(255, 255, 0)
#define LED_RGB_COLOR_RED			    RGB(255, 0, 0)


#define LED_PWM_DUTY_MIN				(2)
#define LED_PWM_DUTY_MAX				(27)	//(30) 30*50 = 1500
#define LED_PWM_DUTY_MIN_X5000			(4)
#define LED_PWM_DUTY_MAX_X5000			(30)	//10
#define LED_PWM_CTRL_CYCLE				(50)	//(100 - LED_PWM_DUTY_MAX)
#define FRONT_LED_BLINK_TIME_COUNT		(59)	// blink time 3sec, using triggered on 50ms timer

typedef int32_t (*dev_i2c_write_cb_t)(uint16_t reg_addr, uint8_t *p_buf, uint16_t size);
typedef int32_t (*dev_i2c_read_cb_t)(uint16_t reg_addr, uint8_t *p_buf, uint16_t size);

typedef struct
{
    dev_i2c_write_cb_t        dev_write_cb;
    dev_i2c_read_cb_t         dev_read_cb;
} EMT_DEV_I2C_CB_T;

typedef struct
{
	EMT_FRONT_LED_TYPE	previous_mode;
	EMT_FRONT_LED_TYPE	mode;
	bool				dimming_up;
	uint32_t			dimming_freq;
	uint32_t			dimming_duty;
	uint32_t			blink_time;
	uint32_t			blink_wait_count;
} EMT_FRONT_LED_STRUCT_T;

#if !defined(__MICOM_BOOTLOADER__)
typedef struct __PACKED
{
	uint8_t   r;
	uint8_t   g;
	uint8_t   b;
	uint16_t  delay_on;
	uint16_t  delay_off;
} EMT_STATUS_LED_RGB_INFO;
#endif

typedef struct
{
	EMT_CPU_WATCHDOG_MODE	mode;
	int32_t					discount;
	int32_t					boot_check_count;
	int32_t					working_check_count;
}EMT_CPU_WATCHDOG_T;

typedef struct
{
	int16_t					on_check_count;
	int16_t					off_check_count;
	bool					on;			//true;ACC ON  false;ACC OFF
}EMT_ACC_STRUCT_T;

typedef struct
{
	EMT_SYSTEM_POWER_MODE		system_power_mode;
	EMT_BBX_OP_MODE				bbx_op_mode;
	EMT_ADC_STRUCT_T			adc_value;
	EMT_BP_TYPE					bp_type;
	EMT_ACC_STRUCT_T			acc;
	EMT_EDLC_CHARGE_STRUCT_T	edlc;
	EMT_BP_DETECT_STRUCT_T		bp;
	EMT_BBX_AQS_STRUCT_T		aqs;
	EMT_TEMPERATURE_STRUCT_T	temperature;
	bool						bbx_communication_on;
	uint32_t					bbx_receive_cmd_count;
	EMT_CPU_WATCHDOG_T			cpu_watchdog;
	EMT_BBX_CUT_OFF_STRUCT_T	cpu_cutoff;
	bool	 					is_rtc_lost;
	bool						is_key_easy_btn_on;
	bool 						is_parking_rec_enable;
	bool						need_cpu_acc_check;
//	bool						usb_detect;
	uint8_t						periodic_data_mode;
	uint8_t						cpu_reset_type;
	EMT_GSENSOR_SENSITIVITY_T	gsensor_sensitivity;
//	structAI					ai_parking;
//	uint8_t						battery_acc_status;
//	uint8_t						battery_acc_status_count;
//	bool						battery_connected;
	EMT_FW_UPGRADE_INFO_T		fw_dl_info;
	uint32_t					cpu_error_reset_count;
	bool 						is_parking_manual_enable;
	bool 						is_parking_manual_set;
}EMT_APP_INFO;


#endif /* INC_EMT_TYPES_H_ */
