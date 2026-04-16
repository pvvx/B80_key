/*
 * app_data.c
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app_main.h"
#include "app_data.h"
#include "app_ble.h"
#include "app_soc_temp.h"
#include "bthome_beacon.h"
#include "flash_eep.h"
#include "sws_printf.h"

RAM services_t services;

const u8 def_dev_name[] = {DEV_NAME_STR};
const dev_config_t dev_cfg_def = {
		.advInterval = MY_ADV_WORK_INTERVAL,
		.measuredIntervals = MY_MEASURE_INTERVAL,
		.rfPower = MY_RF_POWER_INDEX
};

#if USE_EEP
RAM tbl_scanRsp_t tbl_scanRsp;
RAM dev_config_t dev_cfg;
#else
RAM tbl_scanRsp_t tbl_scanRsp = {
		.len = sizeof(def_dev_name) + 1,
		.id = DT_COMPLETE_LOCAL_NAME,
		.name = {DEV_NAME_STR}
};

RAM dev_config_t dev_cfg = {
		.advInterval = MY_ADV_WORK_INTERVAL,
		.measuredIntervals = 2, // sec
		.rfPower = MY_RF_POWER_INDEX
};
#endif

//__attribute__((optimize("-Os")))
void test_dev_cfg(void) {
	if(dev_cfg.advInterval > ADV_INTERVAL_10_24S)
		dev_cfg.advInterval = ADV_INTERVAL_10_24S;
	else if(dev_cfg.advInterval < ADV_INTERVAL_50MS) {
		dev_cfg.advInterval = ADV_INTERVAL_50MS;
	}
	if(dev_cfg.measuredIntervals < 1) {
		dev_cfg.measuredIntervals = 1;
	}
	tbl_scanRsp.name[MAX_DEV_NAME_LEN] = 0;
	tbl_scanRsp.len = strlen((const char *)tbl_scanRsp.name);
	tbl_scanRsp.id = DT_COMPLETE_LOCAL_NAME;
	if(tbl_scanRsp.len <= 0 || tbl_scanRsp.len > MAX_DEV_NAME_LEN) {
		memcpy(tbl_scanRsp.name, def_dev_name, sizeof(def_dev_name));
		tbl_scanRsp.len = sizeof(def_dev_name);
	}
	if(!dev_cfg.rfPower)
		dev_cfg.rfPower = MY_RF_POWER_INDEX;
	wrk.measureInterval = dev_cfg.advInterval * dev_cfg.measuredIntervals * 10000;
}

void save_dev_name(u8 * pname, int len) {
	memcpy(tbl_scanRsp.name, pname, len);
	test_dev_cfg();
#if USE_EEP
	flash_write_cfg(&tbl_scanRsp.name, EEP_ID_DEV_NAME, tbl_scanRsp.len);
#endif
}

#if USE_EEP
void save_dev_cfg(void) {
	test_dev_cfg();
	flash_write_cfg(&dev_cfg, EEP_ID_DEV_CFG, sizeof(dev_cfg));
}
#endif

#if USE_EEP
void load_config(void) {
	flash_supported_eep_ver(BOARD, (VERSION_BCD<<16) | BOARD);
	if(flash_read_cfg(&tbl_scanRsp.name, EEP_ID_DEV_NAME, MAX_DEV_NAME_LEN) <= 0) {
#if 0
		memcpy(tbl_scanRsp.name, def_dev_name, sizeof(def_dev_name));
		tbl_scanRsp.len = sizeof(def_dev_name);
#else
		tbl_scanRsp.name[0] = 0;
#endif
	}
	load_temp_cfg();
	if(flash_read_cfg(&dev_cfg, EEP_ID_DEV_CFG, sizeof(dev_cfg)) != sizeof(dev_cfg)) {
		memcpy(&dev_cfg, &dev_cfg_def, sizeof(dev_cfg));
	}
#if USE_PINCODE
	if(flash_write_cfg(&pincode, EEP_ID_PINCODE, sizeof(pincode)) != sizeof(pincode)) {
		pincode = 0;
	}
#endif
#if USE_BINDKEY
	if(flash_read_cfg(&bindkey, EEP_ID_BKEY, sizeof(bindkey)) != sizeof(bindkey)) {
		generateRandomNum(sizeof(bindkey), bindkey);
		flash_write_cfg(&bindkey, EEP_ID_BKEY, sizeof(bindkey));
	}
#endif
	test_dev_cfg();
}
#endif
