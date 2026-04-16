/*
 * app_cfg.h
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#define VERSION_BCD			0x0100	// bcd "0.1.0.0"
#define BOARD				1

enum {
	ID_SENSOR_NONE = 0,
	ID_SENSOR_SHTC3,   // 1
	ID_SENSOR_SHT4x,   // 2
	ID_SENSOR_SHT30,	// 3
	ID_SENSOR_CHT8305,	// 4
	ID_SENSOR_AHT2x,	// 5
	ID_SENSOR_CHT8215,	// 6
	ID_SENSOR_INA226,	// 7
	ID_SENSOR_MY18B20,	// 8
	ID_SENSOR_MY18B20x2,	// 9
	ID_SENSOR_HX71X,	// 10
	ID_SENSOR_PWMRH,	// 11
	ID_SENSOR_NTC,		// 12
	ID_SENSOR_INA3221,	// 13
	ID_SENSOR_SCD41,	// 14
	ID_SENSOR_BME280,	// 15
	ID_SENSOR_BMP280,	// 16
	ID_SENSOR_PCF85163,	// 17
	ID_SENSOR_TEMP_MCU,	// 18
	ID_SENSOR_TYPE_MAX
} ID_SENSOR_TYPES_e;

// supported services by the device (bits)
#define SERVICE_OTA			0x00000001	// OTA all enable!
#define SERVICE_OTA_EXT		0x00000002	// Compatible BigOTA/ZigbeeOTA
#define SERVICE_PINCODE 	0x00000004	// support pin-code
#define SERVICE_BINDKEY 	0x00000008	// support encryption beacon (bindkey)
#define SERVICE_HISTORY 	0x00000010	// flash logger enable
#define SERVICE_SCREEN		0x00000020	// screen
#define SERVICE_LE_LR		0x00000040	// support extension advertise + LE Long Range
#define SERVICE_THS			0x00000080	// T & H sensor
#define SERVICE_RDS			0x00000100	// wake up when the reed switch + pulse counter
#define SERVICE_KEY			0x00000200	// key "connect"
#define SERVICE_OUTS		0x00000400	// GPIO output
#define SERVICE_INS			0x00000800	// GPIO input
#define SERVICE_TIME_ADJUST 0x00001000	// time correction enabled
#define SERVICE_HARD_CLOCK	0x00002000	// RTC enabled
#define SERVICE_TH_TRG		0x00004000	// use TH trigger out
#define SERVICE_LED			0x00008000	// use led
#define SERVICE_MI_KEYS		0x00010000	// use mi keys
#define SERVICE_PRESSURE	0x00020000	// pressure sensor
#define SERVICE_18B20		0x00040000	// use sensor(s) MY18B20
#define SERVICE_IUS			0x00080000	// use I and U sensor (INA226)
#define SERVICE_PLM			0x00100000	// use PWM-RH and NTC
#define SERVICE_BUTTON		0x00200000	// брелок-кнопка
#define SERVICE_FINDMY		0x00400000	// FindMy
#define SERVICE_SCANTIM		0x00800000	// Scan Time (develop, test only!)
#define SERVICE_ZIGBEE		0x01000000	// BZ-version
#define SERVICE_PIR			0x02000000	// use PIR sensor
/*
#define SERVICE_NC_1		0x04000000
#define SERVICE_NC_2		0x08000000
#define SERVICE_NC_3		0x10000000
#define SERVICE_NC_4		0x20000000
#define SERVICE_NC_5		0x40000000
*/
#define SERVICE_EXTENDED	0x80000000

#define DEV_SERVICES (SERVICE_OTA \
		| SERVICE_PINCODE \
		| SERVICE_LED \
		| SERVICE_KEY \
		| SERVICE_BINDKEY \
)

#define DEV_NAME_STR	 	"BEST"
#define APP_MODEL_STR 		{"TempSensor"}

#define ID_SENSORS		(ID_SENSOR_TEMP_MCU | (ID_SENSOR_NONE << 8))
/* UUID service */
#define TEMP_SERVICE_ENABLE		2	// =2 -> measured.temp in 0.01C, =1 -> measured.temp in 0.1C
#define HUMI_SERVICE_ENABLE		0	// =1 -> measured.humi in 0.01%
//
#define INFO_SERVICE_ENABLE		1	// =1 -> add info service
#define BATTERY_SERVICE_ENABLE	1	// =1 -> measured.battery_level 0..100%
#define ENV_SERVICE_ENABLE	(TEMP_SERVICE_ENABLE || HUMI_SERVICE_ENABLE)
#define RXTX_SERVICE_ENABLE		1	// =1 -> add command or rx-tx service
#define BLE_OTA_SERVER_ENABLE	(DEV_SERVICES & SERVICE_OTA)   // =1 -> add OTA service

#define MY_ADV_WORK_INTERVAL		(ADV_INTERVAL_1S*5) // 5 sec
#define MY_MEASURE_INTERVAL			2 // in MY_ADV_WORK_INTERVAL(ms), =  MY_ADV_WORK_INTERVAL*MY_MEASURE_INTERVAL
#define MY_ADV_CONNECT_INTERVAL		(ADV_INTERVAL_1S5) // 1.5 sec
#define MY_ADV_EVENT_INTERVAL		(ADV_INTERVAL_50MS) // 50 ms
#define MY_RF_POWER_INDEX			RF_POWER_P0p59dBm // RF_POWER_P2p87dBm // RF_POWER_P3p95dBm

#if (DEV_SERVICES & SERVICE_KEY)
#define GPIO_KEY_ON			0
#define GPIO_KEY			GPIO_PD2
#define PD2_INPUT_ENABLE    1
//#define PD2_DATA_OUT        0
//#define PD2_OUTPUT_ENABLE   0
#define PD2_FUNC            AS_GPIO
#if GPIO_KEY_ON
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLDOWN_100K
#else
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_10K
#endif
#endif // (DEV_SERVICES & SERVICE_KEY)

#if (DEV_SERVICES & SERVICE_LED)
#define GPIO_LED_ON			1
#define GPIO_LED			GPIO_PD6
//#define PD6_INPUT_ENABLE    0
//#define PD2_DATA_OUT        0
#define PD2_OUTPUT_ENABLE   0
#define PD6_FUNC            AS_GPIO
#if GPIO_LED_ON
#define PULL_WAKEUP_SRC_PD6 PM_PIN_PULLUP_10K
#else
#define PULL_WAKEUP_SRC_PD6 PM_PIN_PULLDOWN_100K
#endif
#endif // (DEV_SERVICES & SERVICE_LED)



/* DEBUG  Configuration */
// #define SWS_PRINTF_MODE		1 // disable/enable in makefile!

#endif /* _APP_CFG_H_ */
