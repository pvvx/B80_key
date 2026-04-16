/*
 * app_battery.c
 *
 *  Created on: 9 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_data.h"
#include "app_battery.h"
#include "sws_printf.h"

#define MAX_VBAT_MV			3000 // mV
#define MIN_VBAT_WORK_MV	2000 // mV
#define MIN_VBAT_START_MV	2200 // mV
#define VBAT_LEVEL_MAX		100 // % BLE, = 200 Zigbee
#define VBAT2LEVEL_SHL		(32-8) // max level = 256
#define VBAT2LEVEL_COEF	    ((VBAT_LEVEL_MAX << VBAT2LEVEL_SHL)/(MAX_VBAT_MV - MIN_VBAT_WORK_MV))
#define BAT_AVERAGE_COUNT_SHL	5 // average 1<<BAT_AVERAGE_COUNT_SHL


#ifndef ADC_BAT_VREF_MV
#define ADC_BAT_VREF_MV		1175 // default ADC ref voltage (unit:mV)
#endif

#define ADC_BUF_COUNT	8

RAM battery_t battery;

#if (!APP_BATT_CHECK_ENABLE)
_attribute_data_reload_ u8 adc_hw_initialized = 0;
#else
extern u8 adc_hw_initialized;
#endif

#if 0 // Replace adc_init()
// Process takes about 120 μs at CPU CLK 24Mhz.
// _attribute_ram_code_
static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
	adc_power_on_sar_adc(0); // power off sar adc
	adc_reset_adc_module(); // reset whole digital adc module
	adc_enable_clk_24m_to_sar_adc(1); // enable signal of 24M clock to sar adc
	adc_set_sample_clk(5); // set adc clk as 4MHz
	dfifo_disable_dfifo2(); // disable misc channel data dfifo
	//enable the mic channel and set max_state_cnt
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	//adc_set_input_mode_chn_misc(DIFFERENTIAL_MODE); // differential mode
	//adc_set_resolution_chn_misc(RES14); // set resolution
	analog_write(anareg_adc_res_m, (analog_read(anareg_adc_res_m)&(~FLD_ADC_RES_M))
			 | (RES14 | FLD_ADC_EN_DIFF_CHN_M));

	// adc_set_ref_voltage(ADC_VREF_1P2V); //set channel Vref,
	analog_write(areg_adc_vref, ADC_VREF_1P2V);
	// adc_set_ain_pre_scaler(ADC_PRESCALER_1);
	analog_write(areg_ain_scale,0x3d);
	adc_set_state_length(240, 10);
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	// end adc_init();
    // analog_write(areg_ain_scale, 0x3d);
	if(p_ain == TEMPERATURE_SENSOR_P_EE) {
		// adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);
		analog_write(areg_adc_vref_vbat_div, (analog_read(areg_adc_vref_vbat_div)
				&(~FLD_ADC_VREF_VBAT_DIV)));
		adc_set_ain_chn_misc(TEMPERATURE_SENSOR_P_EE, TEMPERATURE_SENSOR_N_EE);
	} else {
		// adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_1F4);
		analog_write(areg_adc_vref_vbat_div, (analog_read(areg_adc_vref_vbat_div)
				&(~FLD_ADC_VREF_VBAT_DIV)) | (1<<2) );
		adc_set_ain_chn_misc(p_ain, GND);
	}
}
#endif

/* Process takes about 215 μs at CPU CLK 24Mhz if cache, else 373 μs */
// _attribute_ram_code_
u32 get_adc(ADC_InputPchTypeDef p_ain) { // ADC_InputPchTypeDef
	u32 adc_dat_buf[ADC_BUF_COUNT];
	u16 adc_sample[ADC_BUF_COUNT] = { 0 };
	int i, j;
	u16 temp;
	if (adc_hw_initialized != p_ain) {
		adc_hw_initialized = p_ain;
		adc_init();
		//	= adc_channel_init(p_ain);
		if(p_ain == TEMPERATURE_SENSOR_P_EE) {
			// adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);
			analog_write(areg_adc_vref_vbat_div, (analog_read(areg_adc_vref_vbat_div)
					&(~FLD_ADC_VREF_VBAT_DIV)));
			adc_set_ain_chn_misc(TEMPERATURE_SENSOR_P_EE, TEMPERATURE_SENSOR_N_EE);
		} else {
			// adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_1F4);
			analog_write(areg_adc_vref_vbat_div, (analog_read(areg_adc_vref_vbat_div)
					&(~FLD_ADC_VREF_VBAT_DIV)) | (1<<2) );
			adc_set_ain_chn_misc(p_ain, GND);
		}
	}
	if(p_ain == TEMPERATURE_SENSOR_P_EE) {
		//enable temperature sensor
		analog_write(0x00, (analog_read(0x00) & 0xef));
	}
	adc_power_on_sar_adc(1); // + 0.4 mA
	adc_reset_adc_module();
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	adc_config_misc_channel_buf((u16 *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	// cycles ~164 us: 8*20.5 us
	u16 rp = reg_dfifo2_wptr;
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while(rp == reg_dfifo2_wptr);
		rp = reg_dfifo2_wptr; // 0,4,8,c,10,14,18,1c
		if (adc_dat_buf[i] & BIT(13)) {
			/* 14 bit resolution, BIT(13) is sign bit,
			 * 1 means negative voltage in differential_mode  */
			adc_sample[i] = 0;
		} else {
			adc_sample[i] = ((u16) adc_dat_buf[i] & 0x1FFF);
		}
		// insert sort
		if (i) {
			if (adc_sample[i] < adc_sample[i - 1]) {
				temp = adc_sample[i];
				adc_sample[i] = adc_sample[i - 1];
				for (j = i - 1; j >= 0 && adc_sample[j] > temp; j--) {
					adc_sample[j + 1] = adc_sample[j];
				}
				adc_sample[j + 1] = temp;
			}
		}
	}
	dfifo_disable_dfifo2(); // misc channel data dfifo disable
	adc_power_on_sar_adc(0); // - 0.4 mA
	if(p_ain == TEMPERATURE_SENSOR_P_EE) {
		// disable temperature sensor
		analog_write(0x00, (analog_read(0x00) | 0x10));
	}
	return (adc_sample[2] + adc_sample[3] + adc_sample[4] + adc_sample[5]);
}

extern unsigned short g_adc_vbat_calib_vref;
extern signed char g_adc_vref_offset;
/* level: 2000..3000 mv - 0..100%
 * return = 1 - low_power */
int check_battery_level(int start) {
	u32 vbat = get_adc(VBAT);
	vbat *= g_adc_vbat_calib_vref;
	vbat >>= 13;
	vbat += g_adc_vref_offset;
	if (start && vbat < MIN_VBAT_START_MV) {
		sws_printf("Low power - %d mV!\n", vbat);
		// cpu_long_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, 180*CLOCK_32K_SYS_TIMER_CLK_1S);
		return 1;
	}
#if BATTERY_SERVICE_ENABLE
	battery.summ += vbat;
	battery.cnt++;
	if(battery.cnt >= (1<<BAT_AVERAGE_COUNT_SHL)) {
		services.battery_mv = battery.summ >> BAT_AVERAGE_COUNT_SHL;
		battery.summ -= services.battery_mv;
		battery.cnt--;
	} else {
		services.battery_mv = battery.summ / battery.cnt;
	}
	if (services.battery_mv < MAX_VBAT_MV) {
		if (services.battery_mv> MIN_VBAT_WORK_MV) {
			services.battery_level = (u8)(((services.battery_mv - MIN_VBAT_WORK_MV)
				* VBAT2LEVEL_COEF) >> VBAT2LEVEL_SHL);
		} else {
			services.battery_level = 0;
			sws_printf("Low power - %d mV!\n", vbat);
			// cpu_long_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, 180*CLOCK_32K_SYS_TIMER_CLK_1S);
			return 1;
		}
	} else {
		services.battery_level = VBAT_LEVEL_MAX;
	}
	services.flg_update |= BIT(MEAS_BIT_VBAT_ADV) | BIT(MEAS_BIT_VBAT_NTF);
	sws_printf("Vbat: %d#%d mV %d%%\n", services.battery_mv, vbat, services.battery_level);
#else
	battery.mv = (u16)vbat;
	battery.summ += vbat;
	battery.cnt++;
	if(battery.cnt >= (1<<BAT_AVERAGE_COUNT_SHL)) {
		battery.average_mv = battery.summ >> BAT_AVERAGE_COUNT_SHL;
		battery.summ -= battery.average_mv;
		battery.cnt--;
	} else {
		battery.average_mv = battery.summ / battery.cnt;
	}
	if (battery.average_mv < MAX_VBAT_MV) {
		if (battery.average_mv > MIN_VBAT_WORK_MV) {
			battery.level = (u8)(((battery.average_mv - MIN_VBAT_WORK_MV)
				* VBAT2LEVEL_COEF) >> VBAT2LEVEL_SHL);
		} else {
			battery.level = 0;
			sws_printf("Low power - %d mV!\n", vbat);
			// cpu_long_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, 180*CLOCK_32K_SYS_TIMER_CLK_1S);
			return 1;
		}
	} else {
		battery.level = VBAT_LEVEL_MAX;
	}
	battery.flg = 0xff;
	sws_printf("Vbat: %d#%d mV %d%%\n", battery.average_mv, battery.mv, battery.level);
#endif
	return 0;
}
