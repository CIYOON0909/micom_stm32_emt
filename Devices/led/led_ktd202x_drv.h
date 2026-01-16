/**
  ****************************************************************************** 
  * @file      led_ktd202x_drv.h
  * @author     
  * @version    
  * @date       
  * @brief      
  *  
  *  
  *  
  ****************************************************************************** 
  */
#ifndef _LED_KTD202X_DRV_H_
#define _LED_KTD202X_DRV_H_

#include "emt_types.h"

/*******************************************************************************
* define
*******************************************************************************/
#define KTD202x_I2Cx_TIMEOUT_MAX    1000

#define KTD2026EWE_ADDR         		0x30
#define KTD2026BEWE_ADDR        		0x31
#define KTD2026CEWE_ADDR        		0x32
#define KTD2027EWE_ADDR         		0x30
#define KTD2027BEWE_ADDR        		0x31

#define KTD202X_DEVICE_I2C_ADDR    		(KTD2026EWE_ADDR << 1)


#define KTD2026_NUM_LEDS 				3
#define KTD2027_NUM_LEDS 				4
#define KTD202X_MAX_LEDS 				KTD2026_NUM_LEDS

/* Register bank */
#define KTD202X_REG_RESET_CONTROL	0x00
#define KTD202X_REG_FLASH_PERIOD	0x01
#define KTD202X_REG_PWM1_TIMER		0x02
#define KTD202X_REG_PWM2_TIMER		0x03
#define KTD202X_REG_CHANNEL_CTRL	0x04
#define KTD202X_REG_TRISE_FALL		0x05
#define KTD202X_REG_LED_IOUT(x)		(0x06 + (x))

/* Register 0 */
#define KTD202X_TIMER_SLOT_CONTROL_TSLOT1	0x00
#define KTD202X_TIMER_SLOT_CONTROL_TSLOT2	0x01
#define KTD202X_TIMER_SLOT_CONTROL_TSLOT3	0x02
#define KTD202X_TIMER_SLOT_CONTROL_TSLOT4	0x03
#define KTD202X_RSTR_RESET					0x07
#define KTD202X_ENABLE_CTRL_WAKE			0x00 /* SCL High & SDA High */
#define KTD202X_ENABLE_CTRL_SLEEP			0x08 /* SCL High & SDA Toggling */
#define KTD202X_ENABLE_CTRL_ALWAYS_ON		0x18 /* Device always ON */

#define KTD202X_TRISE_FALL_SCALE_NORMAL		0x00
#define KTD202X_TRISE_FALL_SCALE_SLOW_X2	0x20
#define KTD202X_TRISE_FALL_SCALE_SLOW_X4	0x40
#define KTD202X_TRISE_FALL_SCALE_FAST_X8	0x60

/* Register 1 */
#define KTD202X_FLASH_PERIOD_256_MS_LOG_RAMP	0x00

/* Register 2-3 */
#define KTD202X_FLASH_ON_TIME_0_4_PERCENT	0x01

/* Register 4 */
#define KTD202X_CHANNEL_CTRL_MASK(x) 		(BIT(2 * (x)) | BIT(2 * (x) + 1))
#define KTD202X_CHANNEL_CTRL_OFF 			(0x00)
#define KTD202X_CHANNEL_CTRL_ON(x) 			(0x01)
#define KTD202X_CHANNEL_CTRL_PWM1(x) 		(0x02)
#define KTD202X_CHANNEL_CTRL_PWM2(x) 		(0x03)

/* Register 5 */
#define KTD202X_RAMP_TIMES_2_MS			0x00

/* Register 6-9 */
#define KTD202X_LED_CURRENT_10_mA			0x4f
#define KTD202X_FLASH_PERIOD_MIN_MS 		256
#define KTD202X_FLASH_PERIOD_STEP_MS 		128
#define KTD202X_FLASH_PERIOD_MAX_STEPS 		126
#define KTD202X_FLASH_ON_MAX 				256
#define KTD202X_MAX_BRIGHTNESS 				192


#define REG0_DO_NOTHING                    	0x04 // [2:0], 100
#define REG0_RESET_REGISTERS_ONLY        	0x05 // [2:0], 101
#define REG0_RESET_MAIN_DIGITAL_ONLY    	0x06 // [2:0], 110
#define REG0_RESET_COMPLETE_CHIP        	0x07 // [2:0], 111


/*******************************************************************************
* typedef
*******************************************************************************/
enum cust_led_id
{
    CUST_LED_CH1    = 0x01,
    CUST_LED_CH2    = 0x02,
    CUST_LED_CH3    = 0x04,
    CUST_LED_CH4    = 0x08,

#if KTD2028
    CUST_LED_TOTAL    = CUST_LED_CH1|CUST_LED_CH2|CUST_LED_CH3,CUST_LED_CH4
#else
    CUST_LED_TOTAL    = CUST_LED_CH1|CUST_LED_CH2|CUST_LED_CH3
#endif    
};

int led_ktd202x_drv_init(EMT_DEV_I2C_CB_T *p_dev_i2c_callback);
int led_ktd202x_drv_deinit(void);

void ktd_led_rgb_set_off(void);
void ktd_led_rgb_set(uint8_t red, uint8_t green, uint8_t blue);
void ktd_led_rgb_blink_set(uint8_t red, uint8_t green, uint8_t blue, uint32_t delay_on, uint32_t delay_off);
#endif //_LED_KTD202X_DRV_H_