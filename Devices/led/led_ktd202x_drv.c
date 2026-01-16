/**
  ****************************************************************************** 
  * @file      led_ktd202x_drv.c
  * @author     
  * @version    
  * @date       
  * @brief      
  *             
  *  
  *  
  ****************************************************************************** 
  */

/*******************************************************************************
* include
*******************************************************************************/
#define APP_LOG_PRINTF_ENABLE           0

#include "emt_types.h"
#include "stm32g0xx_hal.h"
#include "led_ktd202x_drv.h"


static EMT_DEV_I2C_CB_T *p_dev_i2c_cb = NULL;



/*******************************************************************************
* variable
*******************************************************************************/




/*******************************************************************************
* public function prototype
*******************************************************************************/
static int8_t ktd202x_I2C_Write(uint16_t reg_addr, uint8_t data)
{
    if(p_dev_i2c_cb)
    {
        if(p_dev_i2c_cb->dev_write_cb(reg_addr, &data, 1) != 0)
            return -1;
    }

    return 0;
}

static int8_t ktd202x_I2C_Read(uint16_t reg_addr, uint8_t *p_data)
{
    if(p_dev_i2c_cb)
    {
        if(p_dev_i2c_cb->dev_read_cb(reg_addr, p_data, 1) != 0)
            return -1;
    }

    return 0;
}

// #define KTD_DEBUG
#ifdef KTD_DEBUG 
#define LOG_DBG(fmt, args...)   APP_LOG_INFO( "[%s]:: " fmt, __func__, ## args);
#else
#define LOG_DBG(fmt, args...)
#endif


#define TOTAL_TIME_START_COUNT              0x01
#define TOTAL_TIME_START                    348
#define TOTAL_TIME_STEP                     128

#define TIME_PERCENT_START_COUNT            0X00
#define TIME_PERCENT_STEP (4)


void ktd202x_ch1_led_off(void);
void ktd202x_ch2_led_off(void);
void ktd202x_ch3_led_off(void);
void ktd202x_ch4_led_off(void);
void ktd202x_ch_all_led_off(void);

uint8_t g_led_status = 0;



static int led_ktd202x_drv_reg_init(void)
{
    int err = 0;
    int count = 100;
    LOG_DBG("start probe!\n");
    // all registers default reset value

    do
    {
		err = ktd202x_I2C_Write(KTD202X_REG_RESET_CONTROL, KTD202X_RSTR_RESET);// "VERY IMPORTANT!!!!" -> NACK(skip error), refer to ds-page13.
		HAL_Delay(1);	//specified 200us delay in datasheet(page13)
		err = ktd202x_I2C_Write(KTD202X_REG_RESET_CONTROL, KTD202X_ENABLE_CTRL_ALWAYS_ON);

 		if(err == 0)
		{
			HAL_Delay(1);
			err = ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, KTD202X_CHANNEL_CTRL_OFF);//turn off leds
		}

		if(err == 0)
		{
			break;
		}
		count--;
    }while((err != 0) && (count>0));

    g_led_status = 0x00;
    return err;
}

static int led_ktd202x_drv_reg_deinit(void)
{
    int err = 0;

    ktd202x_ch_all_led_off();

    err = ktd202x_I2C_Write(KTD202X_REG_RESET_CONTROL, KTD202X_ENABLE_CTRL_SLEEP);

    g_led_status = 0x00;
    return err;
}

int  ktd202x_translate_timer1(unsigned long delay_on, unsigned long delay_off)
{
    int time_count = 0;
    int percent_count = 0;    
    if(delay_on == 0 && delay_off == 0)
        return -1;
    
    if((delay_on + delay_off)< 348)
    {
        time_count = 1;
    }
    else
    {
        time_count = 
                (delay_on + delay_off - TOTAL_TIME_START)/TOTAL_TIME_STEP + TOTAL_TIME_START_COUNT + 1;
    }

    percent_count = 
            (delay_on*100/(delay_on + delay_off))*10 /TIME_PERCENT_STEP + TIME_PERCENT_START_COUNT + 1;

    ktd202x_I2C_Write(KTD202X_REG_FLASH_PERIOD, time_count);
    ktd202x_I2C_Write(KTD202X_REG_PWM1_TIMER, percent_count);
    
    LOG_DBG("time_count = %x  percent_count = %x   \n",  time_count, percent_count);    
    
    return 0;
}

int  ktd202x_translate_timer2(unsigned long delay_on, unsigned long delay_off)
{
    int time_count = 0;
    int percent_count = 0;
    if(delay_on == 0 && delay_off == 0)
        return -1;

    if((delay_on + delay_off)< 348)
    {
        time_count = 1;
    }
    else
    {
        time_count =
                (delay_on + delay_off - TOTAL_TIME_START)/TOTAL_TIME_STEP + TOTAL_TIME_START_COUNT + 1;
    }

    percent_count =
            (delay_on*100/(delay_on + delay_off))*10 /TIME_PERCENT_STEP + TIME_PERCENT_START_COUNT + 1;

    ktd202x_I2C_Write(KTD202X_REG_FLASH_PERIOD, time_count);
    ktd202x_I2C_Write(KTD202X_REG_PWM2_TIMER, percent_count);

    LOG_DBG("time_count = %x  percent_count = %x   \n",  time_count, percent_count);

    return 0;
}

void ktd202x_ch1_led_blink(int brightness, unsigned long delay_on, unsigned long delay_off)
{
    int ret=0;
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_PWM1(1);
    g_led_status = g_led_status | 0x02;
	ret = ktd202x_translate_timer1(delay_on, delay_off);
    if(ret < 0)
    {
        ktd202x_ch1_led_off();
        return;
    }

    ktd202x_I2C_Write(KTD202X_REG_TRISE_FALL, KTD202X_RAMP_TIMES_2_MS);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(1), brightness);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
}

void ktd202x_ch2_led_blink(int brightness, unsigned long delay_on, unsigned long delay_off)
{
    int ret=0;
//    g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_PWM1(2);
	g_led_status = g_led_status | 0x08;
    ret = ktd202x_translate_timer1(delay_on, delay_off);
    if(ret < 0)
    {
        ktd202x_ch2_led_off();
        return;
    }

    ktd202x_I2C_Write(KTD202X_REG_TRISE_FALL, KTD202X_RAMP_TIMES_2_MS);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(2), brightness);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
}

void ktd202x_ch3_led_blink(int brightness, unsigned long delay_on, unsigned long delay_off)
{
    int ret=0;
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_PWM1(3);
    g_led_status = g_led_status | 0x20;
	ret = ktd202x_translate_timer1(delay_on, delay_off);
    if(ret < 0)
    {
        ktd202x_ch3_led_off();
        return;
    }

    ktd202x_I2C_Write(KTD202X_REG_TRISE_FALL, KTD202X_RAMP_TIMES_2_MS);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(3), brightness);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
}

void ktd202x_ch4_led_blink(int brightness, unsigned long delay_on, unsigned long delay_off)
{
    int ret = 0;
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_PWM1(4);
    g_led_status = g_led_status | 0x80;
	ret = ktd202x_translate_timer1(delay_on, delay_off);
    if(ret < 0)
    {
        ktd202x_ch4_led_off();
        return;
    }

    ktd202x_I2C_Write(KTD202X_REG_TRISE_FALL, KTD202X_RAMP_TIMES_2_MS);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(4), brightness);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);

}

void ktd202x_ch1_led_off(void)
{
    g_led_status = g_led_status & (~0x03); 
    
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(1), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x  \n",  g_led_status);
}

void ktd202x_ch2_led_off(void)
{
    g_led_status = g_led_status & (~0x0C);
    
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(2), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x  \n",  g_led_status);
}

void ktd202x_ch3_led_off(void)
{
    g_led_status = g_led_status & (~0x30);
    
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(3), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x  \n",  g_led_status);
}

void ktd202x_ch4_led_off(void)
{
    g_led_status = g_led_status & (~0xC0);

    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(4), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x\n",  g_led_status);
}

void ktd202x_ch_all_led_off(void)
{
    g_led_status = 0x00;
    
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(1), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(2), 0x00);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(3), 0x00);
#if KTD2028
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(4), 0x00);
#endif
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, KTD202X_CHANNEL_CTRL_OFF);
    LOG_DBG("g_led_status = 0x%x\n",  g_led_status);
}

void ktd202x_ch1_led_on(int level)
{
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_ON(1);
  g_led_status = g_led_status | 0x01; 
    
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(1), level);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x, led_current=<0x%x>  \n",  g_led_status, level);
}

void ktd202x_ch2_led_on(int level)
{
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_ON(2);
	g_led_status = g_led_status|0x04;

    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(2), level);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status = 0x%x, led_current=<0x%x>  \n", g_led_status, level);
    
}

void ktd202x_ch3_led_on(int level)
{
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_ON(3);
	g_led_status = g_led_status|0x10;

    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(3), level);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status  = %x , led_current =0x%x \n", g_led_status, level);
}

void ktd202x_ch4_led_on(int level)
{
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_ON(4);
 	g_led_status = g_led_status|0x40;

    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(4), level);
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status  = %x , led_current =0x%x \n", g_led_status, level);
}

void ktd202x_ch_all_led_on(int level)
{
    //g_led_status = g_led_status | KTD202X_CHANNEL_CTRL_ON(1) | KTD202X_CHANNEL_CTRL_ON(1) | KTD202X_CHANNEL_CTRL_ON(1) | KTD202X_CHANNEL_CTRL_ON(1);
	g_led_status = g_led_status|0x55;

    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(1), level);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(2), level);
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(3), level);
#if KTD2028
    ktd202x_I2C_Write(KTD202X_REG_LED_IOUT(4), level);
#endif
    ktd202x_I2C_Write(KTD202X_REG_CHANNEL_CTRL, g_led_status);
    LOG_DBG("g_led_status  = %x , led_current =0x%x \n", g_led_status, level);
}


void ktd_led_rgb_set_off(void)
{
	ktd202x_ch_all_led_off();
}

void ktd_led_rgb_set(uint8_t red, uint8_t green, uint8_t blue)
{
    if(red)
    {
        ktd202x_ch3_led_on(red);
    }
    
    if(green)
    {
        ktd202x_ch2_led_on(green);
    }
    
    if(blue)
    {
        ktd202x_ch1_led_on(blue);
    }
}

void ktd_led_rgb_blink_set(uint8_t red, uint8_t green, uint8_t blue, uint32_t delay_on, uint32_t delay_off)
{
    if(red)
    {
        ktd202x_ch3_led_blink(red, delay_on, delay_off);
    }
    
    if(green)
    {
        ktd202x_ch2_led_blink(green, delay_on, delay_off);
    }
    
    if(blue)
    {
		ktd202x_ch1_led_blink(blue, delay_on, delay_off);
    }
}

int led_ktd202x_drv_init(EMT_DEV_I2C_CB_T *p_dev_i2c_callback)
{
     int err;
    
    p_dev_i2c_cb = p_dev_i2c_callback;
    err = led_ktd202x_drv_reg_init();
    return err;
}

int led_ktd202x_drv_deinit( void )
{
      int err;
    
    err = led_ktd202x_drv_reg_deinit();
    p_dev_i2c_cb = NULL;    
    return err;
}
