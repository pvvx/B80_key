/*
 * app_soc_temp.c
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_data.h"
#include "app_battery.h"
#include "app_soc_temp.h"
#include "flash_eep.h"
#include "sws_printf.h"

#if TEMP_SERVICE_ENABLE

#define TEMP_AVERAGE_COUNT_SHL	4
#if TEMP_SERVICE_ENABLE == 2
#define TEMP_DEF_COEF	10500
#define TEMP_DEF_ZERO	57400
#else
#define TEMP_DEF_COEF	1050
#define TEMP_DEF_ZERO	5740
#endif

const temperature_cfg_t temperature_cfg_def = {
		.zero = TEMP_DEF_ZERO,
		.coef = TEMP_DEF_COEF
};

RAM temperature_t temperature = {
		.cfg.zero = TEMP_DEF_ZERO,
		.cfg.coef = TEMP_DEF_COEF
};

s16 calk_temp(void) {
	u32 adc = get_adc(TEMPERATURE_SENSOR_P_EE);
	temperature.summ += (s32)temperature.cfg.zero - (((s32)adc * (u32)temperature.cfg.coef) >> 12);
	temperature.cnt++;
#if	TEMP_SERVICE_ENABLE
	if(temperature.cnt >= (1 << TEMP_AVERAGE_COUNT_SHL)) {
		services.temp = temperature.summ >> TEMP_AVERAGE_COUNT_SHL;
		temperature.summ -= services.temp;
		temperature.cnt--;
	} else {
		services.temp = temperature.summ / temperature.cnt;
	}
	services.flg_update |= BIT(MEAS_BIT_TEMP_ADV) | BIT(MEAS_BIT_TEMP_NTF);
	sws_printf("Temp: %d C\n", services.temp);
	return (s16)services.temp;
#else
	if(temperature.cnt >= (1 << TEMP_AVERAGE_COUNT_SHL)) {
		temperature.average = temperature.summ >> TEMP_AVERAGE_COUNT_SHL;
		temperature.summ -= temperature.average;
		temperature.cnt--;
	} else {
		temperature.average = temperature.summ / temperature.cnt;
	}
	temperature.flg = 0xff;
	sws_printf("Temp: %d C\n", temperature.average);
	return (s16)temperature.average;
#endif
}


#if USE_EEP
void load_temp_cfg(void) {
	if(flash_read_cfg(&temperature.cfg, EEP_ID_TEMP_CFG, sizeof(temperature.cfg)) != sizeof(temperature.cfg)) {
		memcpy(&temperature.cfg, &temperature_cfg_def, sizeof(temperature.cfg));
	}
}
void save_temp_cfg(void) {
	flash_write_cfg(&temperature.cfg, EEP_ID_TEMP_CFG, sizeof(temperature.cfg));
}
#endif

#endif // TEMP_SERVICE_ENABLE

