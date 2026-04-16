/********************************************************************************************************
 * @file    app_config.h
 *
 * @brief   This is the header file for BLE SDK
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
#pragma once

#include "app_cfg.h"

///////////////////////// Feature Configuration////////////////////////////////////////////////
/**
 *  @brief  Feature select in BLE Module project
 */
#define FLASH_SIZE_OPTION					FLASH_SIZE_OPTION_128K ///		FLASH_SIZE_OPTION_512K //very important, user need confirm !!!
#define APP_FLASH_PROTECTION_ENABLE			0
/* firmware signature check */
#define FIRMWARES_SIGNATURE_ENABLE     		0

/* Power Management */
#define BLE_MODULE_PM_ENABLE				1
#define PM_DEEPSLEEP_RETENTION_ENABLE       1
/* watchdog enable and timeout setting */
#define MODULE_WATCHDOG_ENABLE				0
#define WATCHDOG_INIT_TIMEOUT				500  //Unit:ms

#if INFO_SERVICE_ENABLE
#define APP_FIRMWARE_STR	{"github.com/pvvx"}
#define USE_FLASH_SERIAL_UID	1 //or #define APP_SERIAL_STR {"012345678.."}
#if MCU_CORE_B80
#define APP_HARDWARE_STR 	{'B','8','0','-','0'+(BOARD/10),'0'+(BOARD%10)}
#else
#define APP_HARDWARE_STR 	{'B','8','0','B','-','0'+(BOARD/10),'0'+(BOARD%10)}
#endif
#define APP_SOFTWARE_STR 	{'V','0'+((VERSION_BCD>>12)&0x0f),'.','0'+((VERSION_BCD>>8)&0x0f),'.','0'+((VERSION_BCD>>4)&0x0f),'.','0'+(VERSION_BCD&0x0f)}
#define APP_MAN_STR 		{"Telink-pvvx"}
#endif

#define USE_EEP	 		1

#define USE_BINDKEY 		(DEV_SERVICES & SERVICE_BINDKEY)
#define USE_PINCODE			(DEV_SERVICES & SERVICE_PINCODE)

///////////////////////// System Clock  Configuration /////////////////////////////////////////

#define CLOCK_SYS_CLOCK_HZ      					24000000

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	#define SYS_CLK_TYPE  							SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define SYS_CLK_TYPE  							SYS_CLK_24M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 32000000)
	#define SYS_CLK_TYPE  							SYS_CLK_32M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 48000000)
	#define SYS_CLK_TYPE  							SYS_CLK_48M_Crystal
#else
	#error "unsupported system clock !"
#endif

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

/////////////////// DEEP SAVE FLG //////////////////////////////////
//#define USED_DEEP_ANA_REG                   DEEP_ANA_REG0 //u8,can save 8 bit info when deep
//#define	LOW_BATT_FLG					    BIT(0)

#define RAM _attribute_data_retention_

/////////////////// set default   ////////////////

#include "../common/default_config.h"
