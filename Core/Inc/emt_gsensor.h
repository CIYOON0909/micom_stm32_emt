/*
 * emt_gsensor.h
 *
 *  Created on: Jun 10, 2021
 *      Author: kimjj
 */

#ifndef INC_EMT_GSENSOR_H_
#define INC_EMT_GSENSOR_H_

void emt_gsensor_init(void);
void emt_gsensor_deinit(void);
void emt_gsensor_rx_isr_handler(void);
void emt_gsensor_proc_task(void *argument);
void AIDetect_proc_task(void *argument);

#endif /* INC_EMT_GSENSOR_H_ */
