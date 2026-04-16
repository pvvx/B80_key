/*
 * app_soc_temp.h
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _APP_SOC_TEMP_H_
#define _APP_SOC_TEMP_H_

typedef struct {
	u16 coef;
	u16 zero;
} temperature_cfg_t;

typedef struct {
	temperature_cfg_t cfg;
	s32 summ;
	u16 cnt;
	s16 average;
	u8  flg;
} temperature_t;

extern const temperature_cfg_t temperature_cfg_def;
extern temperature_t temperature;

s16 calk_temp(void);

#if USE_EEP
void load_temp_cfg(void);
void save_temp_cfg(void);
#else
#define load_temp_cfg();
#define save_temp_cfg();
#endif

#endif /* _APP_SOC_TEMP_H_ */
