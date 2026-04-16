/********************************************************************************************************
 * @file    app_att.c
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
//#include "app_main.h"
#include "app_data.h"
#include "app_cmd.h"
#include "app_att.h"

/**
 *  @brief  connect parameters structure for ATT
 */
typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

static const gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

static const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

static const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

static const u16 reportRefUUID = GATT_UUID_REPORT_REF;

static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

static const u16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC;

static const u16 serviceChangeUUID = GATT_UUID_SERVICE_CHANGE;

static const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

static const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

static const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

static const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;

static const u16 my_appearanceUUID = GATT_UUID_APPEARANCE;

static const u16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM;

static const u16 my_appearance = GAP_APPEAR_UNKNOWN;

static const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;

//------------------ Info service

#if INFO_SERVICE_ENABLE

//#define CHARACTERISTIC_UUID_SYSTEM_ID			0x2A23 // System ID
#define CHARACTERISTIC_UUID_MODEL_NUMBER		0x2A24 // Model Number String: LYWSD03MMC
#define CHARACTERISTIC_UUID_SERIAL_NUMBER		0x2A25 // Serial Number String: F1.0-CFMK-LB-ZCXTJ--
#define CHARACTERISTIC_UUID_FIRMWARE_REV		0x2A26 // Firmware Revision String: 1.0.0_0109
#define CHARACTERISTIC_UUID_HARDWARE_REV		0x2A27 // Hardware Revision String: B1.4
#define CHARACTERISTIC_UUID_SOFTWARE_REV		0x2A28 // Software Revision String: 0x109
#define CHARACTERISTIC_UUID_MANUFACTURER_NAME	0x2A29 // Manufacturer Name String: miaomiaoce.com

//// device Information  attribute values
//static const u16 my_UUID_SYSTEM_ID		    = CHARACTERISTIC_UUID_SYSTEM_ID;
static const u16 my_UUID_MODEL_NUMBER	    = CHARACTERISTIC_UUID_MODEL_NUMBER;
static const u16 my_UUID_SERIAL_NUMBER	    = CHARACTERISTIC_UUID_SERIAL_NUMBER;
static const u16 my_UUID_FIRMWARE_REV	    = CHARACTERISTIC_UUID_FIRMWARE_REV;
static const u16 my_UUID_HARDWARE_REV	    = CHARACTERISTIC_UUID_HARDWARE_REV;
static const u16 my_UUID_SOFTWARE_REV	    = CHARACTERISTIC_UUID_SOFTWARE_REV;
static const u16 my_UUID_MANUFACTURER_NAME  = CHARACTERISTIC_UUID_MANUFACTURER_NAME;

static const u8 my_ModCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SerialCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SERIAL_NUMBER), U16_HI(CHARACTERISTIC_UUID_SERIAL_NUMBER)
};
static const u8 my_FirmCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_FIRMWARE_REV), U16_HI(CHARACTERISTIC_UUID_FIRMWARE_REV)
};
static const u8 my_HardCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SoftCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_SoftRev_DP_H), U16_HI(DeviceInformation_SoftRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SOFTWARE_REV), U16_HI(CHARACTERISTIC_UUID_SOFTWARE_REV)
};
static const u8 my_ManCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_ManName_DP_H), U16_HI(DeviceInformation_ManName_DP_H),
	U16_LO(CHARACTERISTIC_UUID_MANUFACTURER_NAME), U16_HI(CHARACTERISTIC_UUID_MANUFACTURER_NAME)
};

#ifndef APP_FIRMWARE_STR
#define APP_FIRMWARE_STR {"guthub"}
#endif
#ifndef APP_SERIAL_STR
#define APP_SERIAL_STR {"012345678"}
#endif
#ifndef APP_MODEL_STR
#define APP_MODEL_STR {"test-model"}
#endif
#ifndef APP_HARDWARE_STR
#define APP_HARDWARE_STR {"V0.0"}
#endif
#ifndef APP_SOFTWARE_STR
#define APP_SOFTWARE_STR {"V0.0.0.0"}
#endif
#ifndef APP_MAN_STR
#define APP_MAN_STR {"DIY"}
#endif

static const u8 my_FirmStr[] = APP_FIRMWARE_STR;
#if USE_FLASH_SERIAL_UID
RAM u8 my_SerialStr[20]; // "556202-C86013-123456"
#else
static const u8 my_SerialStr[] = APP_SERIAL_STR;
#endif
static const u8 my_ModelStr[] = APP_MODEL_STR;
static const u8 my_HardStr[] = APP_HARDWARE_STR;
static const u8 my_SoftStr[] = APP_SOFTWARE_STR;
static const u8 my_ManStr[] = APP_MAN_STR;

#if 0
//------------------ PnP
static const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;
static const u8 my_PnPtrs[] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

//// device Information  attribute values
static const u8 my_PnCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
	U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
};
#endif

#endif // INFO_SERVICE_ENABLE

//------------------ Battery service

#if BATTERY_SERVICE_ENABLE
static const u16 my_batServiceUUID		= SERVICE_UUID_BATTERY;
static const u16 my_batCharUUID			= CHARACTERISTIC_UUID_BATTERY_LEVEL;
//RAM u16 batteryValueInCCC;
// Battery attribute values
static const u8 my_batCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(BATT_LEVEL_INPUT_DP_H), U16_HI(BATT_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
};
#endif // BATTERY_SERVICE_ENABLE

//------------------ Environmental sensing service
#if ENV_SERVICE_ENABLE
const u16 my_envServiceUUID       = 0x181A; // environmental_sensing
//------------------ Temperature service
#if TEMP_SERVICE_ENABLE
#if TEMP_SERVICE_ENABLE == 2
#define CHARACTERISTIC_UUID_TEMPERATYRE  0x2A6E // https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.temperature.xml
#else
#define CHARACTERISTIC_UUID_TEMPERATYRE  0x2A1F
#endif
//// Temp attribute values
static const u16 my_tempCharUUID       	 = CHARACTERISTIC_UUID_TEMPERATYRE;
//RAM u16 tempValueInCCC;
static const u8 my_tempCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(TEMP_LEVEL_INPUT_DP_H), U16_HI(TEMP_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_TEMPERATYRE), U16_HI(CHARACTERISTIC_UUID_TEMPERATYRE)
};
#endif
#if HUMI_SERVICE_ENABLE
//// Humi attribute values
#define CHARACTERISTIC_UUID_HUMIDITY     0x2A6F // https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.humidity.xml
static const u16 my_humiCharUUID       	 = CHARACTERISTIC_UUID_HUMIDITY;
//RAM u16 humiValueInCCC;
static const u8 my_humiCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(HUMI_LEVEL_INPUT_DP_H), U16_HI(HUMI_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HUMIDITY), U16_HI(CHARACTERISTIC_UUID_HUMIDITY)
};
#endif
#endif // ENV_SERVICE_ENABLE

//------------------ RX-TX service

#if RXTX_SERVICE_ENABLE
/////////////// RxTx/CMD Char ///////////////
#define SERVICE_BTHOME_UUID16		0xFCD2	// 16-bit UUID Service 0xFCD2 BTHOME

#define  COMMAND_UUID16_SERVICE 0xFCD2 // BTHome
#define  COMMAND_UUID16_CHARACTERISTIC 0xFFF4

static const  u16 my_RxTxUUID				= COMMAND_UUID16_CHARACTERISTIC;
static const  u16 my_RxTx_ServiceUUID		= COMMAND_UUID16_SERVICE;
//static const  u8 my_RxTxName[] = "CMD";
//// RxTx attribute values
static const u8 my_RxTxCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RxTx_CMD_OUT_DP_H), U16_HI(RxTx_CMD_OUT_DP_H),
	U16_LO(COMMAND_UUID16_CHARACTERISTIC), U16_HI(COMMAND_UUID16_CHARACTERISTIC)
};

const dev_id_t dev_id = {
	.pid = 0,						// packet identifier = CMD_ID_DEVID
	.revision = 1,					// protocol version/revision
	.hw_version = BOARD + 0x100,	// hardware version
	.sw_version = VERSION_BCD,		// software version (BCD)
	.dev_spec_data = ID_SENSORS,	// device-specific data (bit0..3: sensor_type)
	.services = DEV_SERVICES		// supported services by the device
};

#endif // RXTX_SERVICE_ENABLE

//------------------  over service

/* uint16 1..65535 "Start of Affected Attribute Handle Range",
   uint16 1..65535 "End of Affected Attribute Handle Range" */
static u16 serviceChangeVal[2] = {0};
static u8 serviceChangeCCC[2] = {0,0};

//------------------ OTA service
#if BLE_OTA_SERVER_ENABLE
// OTA
static const  u8 my_OtaUUID[16]			= WRAPPING_BRACES(TELINK_SPP_DATA_OTA);
static const  u8 my_OtaServiceUUID[16]	= WRAPPING_BRACES(TELINK_OTA_UUID_SERVICE);
static u8 my_OtaData 					= 0x00;
static const  u8 my_OtaName[] 			= {'O', 'T', 'A'};
// OTA attribute values
static const u8 my_OtaCharVal[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
	TELINK_SPP_DATA_OTA
};
#endif // BLE_OTA_SERVER_ENABLE

// GAP attribute values
static const u8 my_devNameCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const u8 my_appearanceCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const u8 my_periConnParamCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};

// GATT attribute values
static const u8 my_serviceChangeCharVal[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};

static const attribute_t my_Attributes[] = {
	{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute
	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(u8*)(&my_characterUUID), (u8*)(my_devNameCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,5, (u8*)(&my_devNameUUID), (u8*)(tbl_scanRsp.name), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID), (u8*)(my_appearanceCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUUID), (u8*)(&my_appearance), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID), (u8*)(my_periConnParamCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), (u8*)(&my_periConnParameters), 0},
	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(u8*)(&my_characterUUID), (u8*)(my_serviceChangeCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (u8*)(&serviceChangeUUID), (u8*)(&serviceChangeVal), 0},
	 {0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},
#if INFO_SERVICE_ENABLE
	// 000c - 0018 Device Information Service
	{13,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_devServiceUUID), 0},
#if 0
	 // PnP
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_PnCharVal),(u8*)(&my_characterUUID), (u8*)(my_PnCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},
#endif
     // model string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_ModCharVal),(u8*)(&my_characterUUID),(u8*)(my_ModCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_ModelStr),(u8*)(&my_UUID_MODEL_NUMBER),(u8*)(my_ModelStr), 0},
	 // seral string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialCharVal),(u8*)(&my_characterUUID),(u8*)(my_SerialCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialStr),(u8*)(&my_UUID_SERIAL_NUMBER),(u8*)(my_SerialStr), 0},
	 // firmware string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmCharVal),(u8*)(&my_characterUUID),(u8*)(my_FirmCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmStr),(u8*)(&my_UUID_FIRMWARE_REV),(u8*)(my_FirmStr), 0},
	 // hardware string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_HardCharVal),(u8*)(&my_characterUUID),(u8*)(my_HardCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_HardStr),(u8*)(&my_UUID_HARDWARE_REV),(u8*)(my_HardStr), 0},
	 // software string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftCharVal),(u8*)(&my_characterUUID),(u8*)(my_SoftCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftStr),(u8*)(&my_UUID_SOFTWARE_REV),(u8*)(my_SoftStr), 0},
	 // man name string
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_ManCharVal),(u8*)(&my_characterUUID),(u8*)(my_ManCharVal), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_ManStr),(u8*)(&my_UUID_MANUFACTURER_NAME),(u8*)(my_ManStr), 0},
#endif
#if BATTERY_SERVICE_ENABLE
	// Battery Service
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_batServiceUUID), 0},
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_batCharVal),(u8*)(&my_characterUUID), (u8*)(my_batCharVal), 0},				//prop
	 {0,ATT_PERMISSIONS_READ,2,sizeof(services.battery_level),(u8*)(&my_batCharUUID),(u8*)(&services.battery_level), 0},	//value
	 {0,ATT_PERMISSIONS_RDWR,2,sizeof(services.batteryValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&services.batteryValueInCCC), 0},	//value
#endif // BATTERY_SERVICE_ENABLE
#if ENV_SERVICE_ENABLE
#if	TEMP_SERVICE_ENABLE && HUMI_SERVICE_ENABLE
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_envServiceUUID), 0},
#else
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_envServiceUUID), 0},
#endif
#if TEMP_SERVICE_ENABLE
	 // Temperature in 0.1C or 0.01C
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_tempCharVal),(u8*)(&my_characterUUID),(u8*)(my_tempCharVal), 0},				//prop
	 {0,ATT_PERMISSIONS_READ,2,sizeof(services.temp),(u8*)(&my_tempCharUUID),(u8*)(&services.temp), 0},	//value
	 {0,ATT_PERMISSIONS_RDWR,2,sizeof(services.tempValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&services.tempValueInCCC), 0},	//value
#endif
#if HUMI_SERVICE_ENABLE
	 // Humidity in 0.01%
	 {0,ATT_PERMISSIONS_READ,2,sizeof(my_humiCharVal),(u8*)(&my_characterUUID), (u8*)(my_humiCharVal), 0},				//prop
	 {0,ATT_PERMISSIONS_READ,2,sizeof(services.humi),(u8*)(&my_humiCharUUID),(u8*)(&services.humi), 0},	//value
	 {0,ATT_PERMISSIONS_RDWR,2,sizeof(services.humiValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&humiValueInCCC), 0},	//value
#endif
#endif // TEMP_SERVICE_ENABLE
#if RXTX_SERVICE_ENABLE
	// (Command) RxTx Communication
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_RxTx_ServiceUUID), 0},
	 {0,ATT_PERMISSIONS_READ, 2,sizeof(my_RxTxCharVal),(u8*)(&my_characterUUID),	(u8*)(my_RxTxCharVal), 0},				//prop
//	 {0,ATT_PERMISSIONS_RDWR, 2,sizeof(services.RxTxData),(u8*)(&my_RxTxUUID), (u8*)&services.RxTxData, (att_readwrite_callback_t)&app_onWriteData, NULL},
	 {0,ATT_PERMISSIONS_AUTHOR_RDWR, 2,sizeof(dev_id),(u8*)(&my_RxTxUUID), (u8*)&dev_id, (att_readwrite_callback_t)&app_onWriteData, NULL},
	 {0,ATT_PERMISSIONS_RDWR, 2,sizeof(services.RxTxValueInCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(&services.RxTxValueInCCC), 0},	//value
//	 {0,ATT_PERMISSIONS_READ, 2,sizeof(my_RxTxName),(u8*)(&userdesc_UUID), (u8*)(my_RxTxName), 0},
#endif // RXTX_SERVICE_ENABLE
#if BLE_OTA_SERVER_ENABLE
	//  OTA
	{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
	 {0,ATT_PERMISSIONS_READ, 2, sizeof(my_OtaCharVal),(u8*)(&my_characterUUID), (u8*)(my_OtaCharVal), 0},				//prop
	 {0,ATT_PERMISSIONS_AUTHOR_RDWR,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, NULL},				//value
	 {0,ATT_PERMISSIONS_READ, 2,sizeof(my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},
#endif // OTA_SERVICE_ENABLE
};

#if SWS_PRINTF_MODE
extern const char tab_hex_ascii[];
#else
const char tab_hex_ascii[] = "0123456789ABCDEF";
#endif

unsigned char * str_bin2hex(unsigned char *d, unsigned char *s, int len) {
	while(len--) {
		*d++ = tab_hex_ascii[(*s >> 4) & 0xf];
		*d++ = tab_hex_ascii[(*s++ >> 0) & 0xf];
	}
	return d;
}

/**
 * @brief      Initialize the attribute table
 * @param[in]  none
 * @return     none
 */
void my_gatt_init(void) {
#if	USE_FLASH_SERIAL_UID
	/* my_SerialStr[6+1+6+1+6] = "SoC_ID-FlashID-FUUID"
	 * "012345-789012-456789*/
	union {
		u8 b[16];
		u32 d[4];
	} buf;
	u8 *p = my_SerialStr;
	// Read SoC ID, version
	buf.b[0] = REG_ADDR8(0x7f);
	buf.b[1] = REG_ADDR8(0x7e);
	buf.b[2] = REG_ADDR8(0x7d);
	p = str_bin2hex(p, buf.b, 3);
	*p++ = '-';
	// Read flash MID
#if 1
	p = str_bin2hex(p, (unsigned char *)&blc_flash_mid, 3);
#else
	buf.d[0] = flash_read_mid();
	p = str_bin2hex(p, buf.b, 3);
#endif
	*p++ = '-';
	// Read flash UID
	//flash_read_uid(buf);
	extern void flash_mspi_read_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char dummy_cnt, unsigned char *data, unsigned long data_len);
	flash_mspi_read_ram(FLASH_READ_UID_CMD_GD_PUYA_ZB_TH, 0x00, 1, 1, buf.b, 16);
	buf.d[0] ^= buf.d[1];
	buf.d[0] ^= buf.d[2];
	buf.d[0] ^= buf.d[3];
	str_bin2hex(p, buf.b, 3);
#endif
	bls_att_setAttributeTable ((u8 *)my_Attributes);
}
