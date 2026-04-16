/********************************************************************************************************
 * @file    custom_pair.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef BLM_PAIR_H_
#define BLM_PAIR_H_

#ifndef ACL_CENTRAL_CUSTOM_PAIR_ENABLE
#define ACL_CENTRAL_CUSTOM_PAIR_ENABLE					0
#endif


#ifndef FLASH_CUSTOM_PAIRING_MAX_SIZE
#define FLASH_CUSTOM_PAIRING_MAX_SIZE     				4096
#endif


#if (ACL_CENTRAL_CUSTOM_PAIR_ENABLE)

	typedef struct{
		u8 manual_pair;
		u8 mac_type;  //address type
		u8 mac[6];
		u32 pair_tick;
	}man_pair_t;

	extern man_pair_t blm_manPair;

	/**
	 * @brief		this function serves to init the master host paring management
	 * @param[in]	none
	 * @return      none
	 */
	void user_master_host_pairing_management_init(void);

	/**
	 * @brief		this function serves to unpair cmd proc
	 * 				when press unpair button on dongle or send unpair cmd from slave
	 * 				dongle will call this function to process current unpair cmd
	 * @param[in]	none
	 * @return      none
	 */
	void user_tbl_salve_mac_unpair_proc(void);

	/**
	 * @brief		this function serves to search mac address in the bond slave mac table:
	 *              when slave paired with dongle, add this addr to table
	 * 				re_poweron slave, dongle will search if this AdvA in slave adv pkt is in this table
	 * 				if in, it will connect slave directly
	 * 				this function must in ramcode
	 * @param[in]	adr_type
	 * @param[in]	adr - pointer to point the address data
	 * @return      none
	 */
	int user_tbl_slave_mac_search(u8 adr_type, u8 * adr);

	/**
	 * @brief		this function serves to add new mac address to table
	 * @param[in]	adr_type
	 * @param[in]	adr - pointer to point the address data
	 * @return      none
	 */
	int user_tbl_slave_mac_add(u8 adr_type, u8 *adr);

	/**
	 * @brief		this function serves to remove adr from slave mac table by adr
	 * 				when rc trigger unpair, use this function to delete the slave mac
	 * @param[in]	adr_type
	 * @param[in]	adr - pointer to point the address data
	 * @return      none
	 */
	int user_tbl_slave_mac_delete_by_adr(u8 adr_type, u8 *adr);

#endif


/**
 * @brief       this function serves to set current ReadRequest attribute handle
 * @param[in]	handle - connect handle
 * @return      none
 */
void app_setCurrentReadReq_attHandle(u16 handle);

/**
 * @brief		host pair or upair proc in main loop
 * @param[in]	none
 * @return      none
 */
void host_pair_unpair_proc(void);



extern int	central_pairing_enable;
extern int central_unpair_enable;



#endif /* APP_PAIR_H_ */
