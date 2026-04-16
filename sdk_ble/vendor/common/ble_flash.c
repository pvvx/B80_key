/********************************************************************************************************
 * @file    ble_flash.c
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
#include "drivers.h"
#include "ble_flash.h"
#include "stack/ble/ble.h"


/* global variable for some flash sector address, initial value use 512K flash.
 * attention that they will be changed in function "blc readFlashSize_autoConfigCustomFlashSector"  */
_attribute_data_retention_	unsigned int flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH;
_attribute_data_retention_	unsigned int flash_sector_calibration = CFG_ADR_CALIBRATION_512K_FLASH;
_attribute_data_retention_	unsigned int flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_512K_FLASH; //for slave only

_attribute_data_retention_	unsigned int flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_512K; //for master only

_attribute_data_retention_	unsigned int  blc_flash_mid = 0;
_attribute_data_retention_	unsigned int  blc_flash_vendor = 0;
_attribute_data_retention_	unsigned char blc_flash_capacity;

/**
 * @brief		This function is used to read flash mid, get flash vendor, and set flash capacity
 * @param[in]	none
 * @return      none
 */
void blc_flash_read_mid_get_vendor_set_capacity(void)
{
	/* attention: blc_flash_mid/blc_flash_vendor/blc_flash_capacity will be used by application and stack later
	 * so do not change code here */
	blc_flash_mid = flash_read_mid();
	blc_flash_vendor = flash_get_vendor(blc_flash_mid);
	blc_flash_capacity = ((blc_flash_mid & 0x00ff0000)>>16);

}


/**
 * @brief		This function can automatically recognize the flash size,
 * 				and the system selects different customized sector according
 * 				to different sizes.
 * @param[in]	none
 * @return      none
 */
void blc_readFlashSize_autoConfigCustomFlashSector(void)
{
	blc_flash_read_mid_get_vendor_set_capacity();

#if (FLASH_GD25LD10C_SUPPORT_EN || FLASH_P25D09U_SUPPORT_EN || FLASH_GD25WD10EGIG_SUPPORT_EN)	//128K
	if(blc_flash_capacity == FLASH_SIZE_128K){
		flash_sector_mac_address = CFG_ADR_MAC_128K_FLASH;
		flash_sector_calibration = CFG_ADR_CALIBRATION_128K_FLASH;
		flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_128K_FLASH;
		flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_128K;
		printf("[FLASH][INI] 128K Flash, MAC on 0x%x\n", flash_sector_mac_address);
	}
#endif
#if (FLASH_GD25LD40C_SUPPORT_EN || FLASH_P25Q40SU_SUPPORT_EN || FLASH_ZG25WD40B_SUPPORT_EN)		//512K
	else if(blc_flash_capacity == FLASH_SIZE_512K){
		flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH;
		flash_sector_calibration = CFG_ADR_CALIBRATION_512K_FLASH;
		flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_512K_FLASH;
		flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_512K;
		printf("[FLASH][INI] 512K Flash, MAC on 0x%x\n", flash_sector_mac_address);
	}
#endif
	else{
		/*This SDK do not support other flash size except what listed above
		  If code stop here, please check your Flash */
		printf("[FLASH][INI] flash size %x do not support !!!\n", blc_flash_capacity);
		while(1);
	}
}

/*
 * VVWWXX38C1A4YYZZ
 * public_mac: 		  VVWWXX 38C1A4
 * random_static_mac: VVWWXXYYZZ C0
 */
/**
 * @brief		This function is used to initialize the MAC address
 * @param[in]	flash_addr - flash address for MAC address
 * @param[in]	mac_public - public address
 * @param[in]	mac_random_static - random static MAC address
 * @return      none
 */
void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static) {
	u8 mac_read[8];
	u32 * p = (u32 *) &mac_read;
	flash_read_page(flash_addr, sizeof(mac_read), mac_read);

	if (p[0] == 0xffffffff && p[1] == 0xffffffff) {
		generateRandomNum(3, mac_read);
		mac_read[3] = 0x38;             //company id: 0xA4C138
		mac_read[4] = 0xC1;
		mac_read[5] = 0xA4;
		generateRandomNum(2, &mac_read[6]);
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}
	// copy public address
	memcpy(mac_public, mac_read, 6);
	// copy random address
	mac_random_static[0] = mac_public[0];
	mac_random_static[1] = mac_public[1];
	mac_random_static[2] = mac_public[2];
	mac_random_static[3] = mac_read[6];
	mac_random_static[4] = mac_read[7];
	mac_random_static[5] = 0xC0; 		//for random static
}

