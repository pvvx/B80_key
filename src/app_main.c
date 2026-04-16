/********************************************************************************************************
 * @file    app.c
 *
 * @brief   This is the source file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    12,2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app_main.h"
#include "app_data.h"
#include "app_att.h"
#include "app_ble.h"
#include "app_battery.h"
#include "app_soc_temp.h"
#include "bthome_beacon.h"
#include "flash_eep.h"
#if APP_FLASH_PROTECTION_ENABLE
#include "app_flash_protect.h"
#endif
#include "sws_printf.h"

RAM work_t wrk;
RAM services_t services;

#ifdef GPIO_LED
void led_on(void) {
	gpio_set_output_en(GPIO_LED, 1);
	gpio_setup_up_down_resistor(GPIO_LED,
#if LED_ON
		PM_PIN_PULLUP_10K
#else
		PM_PIN_PULLDOWN_100K
#endif
		);
}
void led_off(void) {
	gpio_set_output_en(GPIO_LED, 0);
	gpio_setup_up_down_resistor(GPIO_LED,
#if LED_ON
		PM_PIN_PULLDOWN_100K
#else
		PM_PIN_PULLUP_10K
#endif
	);
}
#else
#define led_on()
#define led_off()
#endif

/**
 * @brief		enter suspend mode
 * @param[in]	none
 * @return      0 - forbidden enter suspend mode
 *              1 - allow enter suspend mode
 */
int app_suspend_enter(void) {
/*
	if(?) {
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		return 0;
	}
*/
	return 1;
}

void go_sleep(void) {
	cpu_long_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, 180*CLOCK_32K_SYS_TIMER_CLK_1S);
}

/* Test read OTP:
OTP_VDD_1V2_CALIB_ADDR      0x3fc0
OTP_CAP_VALUE_ADDR			0x3fc8
OTP_ADC_VREF_FT_CALIB_ADDR	0x3fcc
OTP_ADC_VREF_CP_CALIB_ADDR	0x3fd0

gain 0xAE+1000=1174, offset = 0x27-20 = 19
gain 0xB4+1000=1180, offset = 0x1C-20 = 8

3FC0: FFFFFFFF E465FFFE FFFFFFFF FFFFFFFF
3FD0: AE27B41C BD546412 F2BCBB50 DC323938
3FE0: 45515501 FFFFFFFF FFFFFFFF FFFFFFFF
3FF0: FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF */

void test_otp(void) {
	int len = 16;
	u32 addr = 0x3fc0;
	u32 buf[16];
	u32 *p = buf;
	sws_puts("OTP dump:\n");
	otp_set_active_mode();
	otp_read(addr, len, p);
	otp_set_deep_standby_mode();
	while(len) {
		sws_printf("%03x: %16p\n", addr, p);
		addr += 0x10;
		p += 4;
		len -= 4;
	}
}


/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
void user_init_normal(void)
{
	sws_init();
	/* random number generator must be initiated here( in the beginning of user_init_normal).
	 * When deepSleep retention wakeUp, no need initialize again */
	random_generator_init();  //this is must

#if FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_128K
	extern void blc_flash_read_mid_get_vendor_set_capacity(void);
	blc_flash_read_mid_get_vendor_set_capacity();
	flash_sector_mac_address = CFG_ADR_MAC_128K_FLASH;
	flash_sector_calibration = CFG_ADR_CALIBRATION_128K_FLASH;
	flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_128K_FLASH;
	flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_128K;
#else
	blc_readFlashSize_autoConfigCustomFlashSector();
#endif
	/* attention that this function must be called after "blc_readFlashSize_autoConfigCustomFlashSector" !!!*/
	blc_app_loadCustomizedParameters_normal();

	if(check_battery_level(1)) {
		go_sleep();
	}
	sws_puts("\nInit...\n");

	//test_otp();

#if (APP_FLASH_PROTECTION_ENABLE)
	app_flash_protection_operation(FLASH_OP_EVT_APP_INITIALIZATION, 0, 0);
	blc_appRegisterStackFlashOperationCallback(app_flash_protection_operation); //register flash operation callback for stack
#endif

	load_config();

	app_ble_init();

#if TEMP_SERVICE_ENABLE
	calk_temp();
#endif

#if (BLE_MODULE_PM_ENABLE)
 #if (SAVE_RAM_CODE_ENABLE)
	blc_ll_initPowerManagement_module_save_ram();        //pm module:      	 optional
 #else
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
 #endif
 #if (PM_DEEPSLEEP_RETENTION_ENABLE)
	blc_ll_initDeepsleepRetention_module();//Remove it if need save ramcode, and add DeepsleepRetentionEarlyWakeupTiming to 1ms
	bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	blc_pm_setDeepsleepRetentionThreshold(95, 95);
  #if (MCU_CORE_B80)
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(650);
  #elif (MCU_CORE_B80B)
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(550);
  #endif
 #else
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
 #endif
	bls_pm_registerFuncBeforeSuspend(&app_suspend_enter);
#else
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
#endif // BLE_MODULE_PM_ENABLE
	app_adv_init();
#ifdef GPIO_KEY
	gpio_setup_up_down_resistor(GPIO_KEY,
#if GPIO_KEY_ON
		PM_PIN_PULLDOWN_100K
#else
		PM_PIN_PULLUP_10K
#endif
		);
	wrk.keyPressed = gpio_read(GPIO_KEY)? GPIO_KEY_ON : !GPIO_KEY_ON;
#endif // GPIO_KEY
}

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_
void user_init_deepRetn(void)
{
	blc_app_loadCustomizedParameters_deepRetn();
	//* LinkLayer Initialization  Begin
	blc_ll_initBasicMCU(); //mandatory
	/* set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend */
	rf_set_power_level_index(dev_cfg.rfPower);
	blc_ll_recoverDeepRetention();
}
#endif

/**
 * @brief     BLE main loop
 * @param[in]  none.
 * @return     none.
 */
void main_loop(void) {
	u32 tt = clock_time();
	if(tt - wrk.measureTick >= wrk.measureInterval) {
		wrk.measureTick = tt;
		wrk.startMeasure = 1;
	}
	if(wrk.bleConnected) {
		blc_sdk_main_loop();
		// Notify
		//if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) {
#if BATTERY_SERVICE_ENABLE
			if((services.flg_update & BIT(MEAS_BIT_VBAT_NTF)) && services.batteryValueInCCC) {
				services.flg_update &= ~BIT(MEAS_BIT_VBAT_NTF);
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE,
					BATT_LEVEL_INPUT_DP_H, (u8 *)&services.battery_level,
					sizeof(services.battery_level));
			}
#endif
#if TEMP_SERVICE_ENABLE
			if((services.flg_update & BIT(MEAS_BIT_TEMP_NTF)) && services.tempValueInCCC) {
				services.flg_update &= ~BIT(MEAS_BIT_TEMP_NTF);
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE,
					TEMP_LEVEL_INPUT_DP_H, (u8 *)&services.temp,
					sizeof(services.temp));
			}
#endif
#if HUMI_SERVICE_ENABLE
			if((services.flg_update & BIT(MEAS_BIT_HUMI_NTF)) && services.humiValueInCCC) {
				services.flg_update &= ~BIT(MEAS_BIT_HUMI_NTF);
				blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE,
					TEMP_LEVEL_INPUT_DP_H, (u8 *)&services.humi,
					sizeof(services.humi));
			}
#endif
		//}
	}
	if(wrk.startMeasure) {
		// Measure
		wrk.startMeasure = 0;
		if(services.flg_update & BIT(MEAS_BIT_VBAT_ADV)) {
			calk_temp();
			if((!wrk.bleConnected) && (!adv_buf.adv_event)) {
				adv_buf.send_count++;
#if	USE_BINDKEY
				if(dev_cfg.flags & 1)
					bthome_encrypt_beacon(0);
				else
#endif
					bthome_beacon(0);

				bls_ll_setAdvData(adv_buf.flag, adv_buf.data_size);
			}
			services.flg_update &= ~(BIT(MEAS_BIT_VBAT_ADV)
					| BIT(MEAS_BIT_TEMP_ADV));
		} else {
			if(check_battery_level(0)) {
				go_sleep();
			}
		}
		wrk.measureTick = tt;
	}
//	if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) {
	if(!wrk.bleConnected) {
		blc_sdk_main_loop();
	}
	/*
		bls_pm_registerAppWakeupLowPowerCb(blt_soft_timer_process);
		bls_pm_setAppWakeupLowPower(blt_timer.timer[0].t,  1);
	*/

#ifdef GPIO_KEY
	u8 button_on = gpio_read(GPIO_KEY)? GPIO_KEY_ON : !GPIO_KEY_ON;
	if(button_on) {
		led_on();
		// button on
		if(!wrk.keyPressed) {
			// event button on
			wrk.timerWakeUpTick = (tt + 15 * SYSTEM_TIMER_TICK_1S) | 1; // 15 sec
			bls_pm_setAppWakeupLowPower(wrk.timerWakeUpTick, 1);
			wrk.keyCounter++;
			if(!wrk.bleConnected) {
			//if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) {
				app_set_event_adv();
			}
		} else {
			if(wrk.timerWakeUpTick
				&& ((u32)(clock_time() + BLT_TIMER_SAFE_MARGIN_PRE - wrk.timerWakeUpTick) < 4*CLOCK_16M_SYS_TIMER_CLK_1S)) {
				wrk.timerWakeUpTick = 0;
				led_off();
				bls_pm_setAppWakeupLowPower(0, 0); // clear callback
#if USE_EEP
				flash_erase_all_cfg();
#endif
				sws_puts("Go to reboot..\n");
				sws_buffer_flush();
				start_reboot();
			}
		}
	} else {
		// button off
		if(wrk.keyPressed) {
			// event button off
			led_off();
			//if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) {
			if(!wrk.bleConnected) {
				app_set_event_adv();
			}
			wrk.timerWakeUpTick = 0;
			bls_pm_setAppWakeupLowPower(0, 0); // clear callback
		}
	}
	wrk.keyPressed = button_on;

#endif // GPIO_KEY
	/* power management -----------------------------------------------*/
#if (BLE_OTA_SERVER_ENABLE)
	if(wrk.otaIsWorking) {
		bls_pm_setSuspendMask(SUSPEND_DISABLE); // (SUSPEND_ADV | SUSPEND_CONN)
	} else
#endif
	{
#ifdef GPIO_KEY
#if	GPIO_KEY_ON
		cpu_set_gpio_wakeup(GPIO_KEY, !wrk.keyPressed, 1);
#else
		cpu_set_gpio_wakeup(GPIO_KEY, wrk.keyPressed, 1);
#endif
		bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
#endif
		sws_buffer_flush(); // TODO: move to app_suspend_enter() ?
#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
#else
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
#endif
	}
}

