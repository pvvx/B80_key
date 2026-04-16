/*
 * app_battery.h
 *
 *  Created on: 9 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _APP_BATTERY_H_
#define _APP_BATTERY_H_

u32 get_adc(ADC_InputPchTypeDef p_ain);

typedef struct {
	u32 summ;
	u16 cnt;
#if !BATTERY_SERVICE_ENABLE
	u16 average_mv;
	u16 mv;
	u8  level;
	u8  flg;
#endif
} battery_t;

extern battery_t battery;

int check_battery_level(int start); /* return = 1 - low_power */

#endif /* _APP_BATTERY_H_ */
