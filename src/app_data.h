/*
 * app_data.h
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _APP_DATA_H_
#define _APP_DATA_H_

typedef enum {
	MEAS_BIT_VBAT_ADV   = 0,
	MEAS_BIT_VBAT_NTF, // 1
	MEAS_BIT_TEMP_ADV, // 3
	MEAS_BIT_TEMP_NTF, // 4
	MEAS_BIT_HUMI_ADV, // 5
	MEAS_BIT_HUMI_NTF  // 6
} measure_bits_e;

// used att UUID
typedef struct {
#if BATTERY_SERVICE_ENABLE
	u16 batteryValueInCCC;
	u16 battery_mv;
	u8 battery_level;
#endif
	u8 flg_update;
#if TEMP_SERVICE_ENABLE
	u16 tempValueInCCC;
#if TEMP_SERVICE_ENABLE == 2
	s16 temp;
#else
	s8 temp;
#endif
#endif
#if HUMI_SERVICE_ENABLE
	u16 humiValueInCCC;
	u16 humi;
#endif
#if	RXTX_SERVICE_ENABLE
	u16 RxTxValueInCCC;
//	u8 RxTxData[20];
#endif
} services_t;

extern services_t services;

/**
 * @brief	BLE Scan Response Packet data
 */
#define MAX_DEV_NAME_LEN		20

typedef struct {
	u8 len;
	u8 id;
	u8 name[MAX_DEV_NAME_LEN+1];
} tbl_scanRsp_t;

extern tbl_scanRsp_t tbl_scanRsp;

typedef struct{
	u16 advInterval; // MY_ADV_WORK_INTERVAL
	u8 measuredIntervals; // Interval = advInterval(ms) * measuredIntervals
	u8 rfPower;	  // MY_RF_POWER_INDEX
	u8 flags; // bit0: adv encrypt,
} dev_config_t;

extern dev_config_t dev_cfg;
extern const dev_config_t dev_cfg_def;

void test_dev_cfg(void);
void save_dev_name(u8 * pname, int len);

#if USE_EEP
void load_config(void);
void save_dev_cfg(void);
#else
#define load_config()
#define save_dev_cfg() test_dev_cfg()
#endif

#endif /* _APP_DATA_H_ */
