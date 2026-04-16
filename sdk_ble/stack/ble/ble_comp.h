/********************************************************************************************************
 * @file    ble_comp.h
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
#ifndef BLE_COMP_H
#define BLE_COMP_H


/* for some API name in old version SDK compatible with new version SDK */

/////////////////////////////////////L2CAP/////////////////////////////////////////////
#define blc_l2cap_reg_att_sig_hander 							blc_l2cap_reg_att_sig_handler

/////////////////////////////////////SMP/////////////////////////////////////////////
#define	blc_smp_setSecurityParamters							blc_smp_setSecurityParameters
#define	blm_smp_configParingSecurityInfoStorageAddr     blm_smp_configPairingSecurityInfoStorageAddr
#define	bls_smp_configpairingSecurityInfoStorageAddr      bls_smp_configPairingSecurityInfoStorageAddr
#define	bls_smp_eraseAllpairingInformation                      bls_smp_eraseAllPairingInformation
#define	blc_smp_setpairingMethods                                blc_smp_setPairingMethods

////////////////////////////////////APP///////////////////////////////////////
#define blc_app_loadCustomizedParameters 						blc_app_loadCustomizedParameters_normal

//////////////////////////////////////ATT////////////////////////////////////////////////
#define	blc_att_resetEffectiveMtuSize(BLS_CONN_HANDLE)							blc_att_resetEffectiveMtuSize(BLS_CONN_HANDLE)
#define	blt_att_setEffectiveMtuSize                                blc_att_setEffectiveMtuSize

//////////////////////////////////////OTA////////////////////////////////////////////////
#define bls_ota_setFirmwareSizeAndOffset					blc_ota_setFirmwareSizeAndBootAddress
#define bls_ota_clearNewFwDataArea							blc_ota_initOtaServer_module
#define bls_ota_registerStartCmdCb							blc_ota_registerOtaStartCmdCb
#define bls_ota_registerResultIndicateCb					blc_ota_registerOtaResultIndicationCb
#define bls_ota_registerVersionReqCb						blc_ota_registerOtaFirmwareVersionReqCb
#define bls_ota_setTimeout(timeout_us)						blc_ota_setOtaProcessTimeout(timeout_us/1000000)

/* for some macro name in old version SDK compatible with new version SDK */

///////////////////////////////////controller//////////////////////////////////////
#define BLT_EV_FLAG_RX_DATA_ABANDOM								BLT_EV_FLAG_RX_DATA_ABANDON
#define EVENT_MASK_RX_DATA_ABANDOM								EVENT_MASK_RX_DATA_ABANDON

//////////////////////////////////////HCI/////////////////////////////////////////
#define HCI_EVT_MASK_CONNECTION_COMPELETE 						HCI_EVT_MASK_CONNECTION_COMPLETE
#define HCI_EVT_MASK_CHANGE_CONECTION_LINK_KEY_COMPLETE 		HCI_EVT_MASK_CHANGE_CONNECTION_LINK_KEY_COMPLETE

//////////////////////////////////////GAP/////////////////////////////////////////
#define GAP_APPEARE_UNKNOWN 									GAP_APPEAR_UNKNOWN
#define GAP_EVT_SMP_TK_DISPALY 									GAP_EVT_SMP_TK_DISPLAY
#define GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM 						GAP_EVT_GATT_HANDLE_VALUE_CONFIRM
#define GAP_EVT_MASK_SMP_TK_DISPALY 							GAP_EVT_MASK_SMP_TK_DISPLAY
#define GAP_EVT_MASK_GATT_HANDLE_VLAUE_CONFIRM 					GAP_EVT_MASK_GATT_HANDLE_VALUE_CONFIRM
#define GAP_EVT_SMP_TK_DISPALY                                    GAP_EVT_SMP_TK_DISPLAY
#define DT_INCOMPLT_LIST_16BIT_SERVICE_UUID						DT_INCOMPLETE_LIST_16BIT_SERVICE_UUID

//////////////////////////////////////SMP////////////////////////////////////////////////
#define pairing_FAIL_REASON_PASSKEY_ENTRY						PAIRING_FAIL_REASON_PASSKEY_ENTRY
#define pairing_FAIL_REASON_OOB_NOT_AVAILABLE					PAIRING_FAIL_REASON_OOB_NOT_AVAILABLE
#define pairing_FAIL_REASON_AUTH_REQUIRE						PAIRING_FAIL_REASON_AUTH_REQUIRE
#define pairing_FAIL_REASON_CONFIRM_FAILED						PAIRING_FAIL_REASON_CONFIRM_FAILED
#define pairing_FAIL_REASON_pairing_NOT_SUPPORTED				PAIRING_FAIL_REASON_PAIRING_NOT_SUPPORTED
#define pairing_FAIL_REASON_ENCRYPT_KEY_SIZE					PAIRING_FAIL_REASON_ENCRYPT_KEY_SIZE
#define pairing_FAIL_REASON_CMD_NOT_SUPPORT						PAIRING_FAIL_REASON_CMD_NOT_SUPPORT
#define pairing_FAIL_REASON_UNSPECIFIED_REASON					PAIRING_FAIL_REASON_UNSPECIFIED_REASON
#define pairing_FAIL_REASON_REPEATED_ATTEMPT					PAIRING_FAIL_REASON_REPEATED_ATTEMPT
#define pairing_FAIL_REASON_INVAILD_PARAMETER					PAIRING_FAIL_REASON_INVALID_PARAMETER
#define pairing_FAIL_REASON_DHKEY_CHECK_FAIL					PAIRING_FAIL_REASON_DHKEY_CHECK_FAIL
#define pairing_FAIL_REASON_NUMUERIC_FAILED						PAIRING_FAIL_REASON_NUMERIC_FAILED
#define pairing_FAIL_REASON_BREDR_pairing						PAIRING_FAIL_REASON_BREDR_PAIRING
#define pairing_FAIL_REASON_CROSS_TRANSKEY_NOT_ALLOW			PAIRING_FAIL_REASON_CROSS_TRANSKEY_NOT_ALLOW
#define pairing_FAIL_REASON_pairing_TIEMOUT						PAIRING_FAIL_REASON_PAIRING_TIMEOUT
#define pairing_FAIL_REASON_CONN_DISCONNECT						PAIRING_FAIL_REASON_CONN_DISCONNECT
#define pairing_FAIL_REASON_SUPPORT_NC_ONLY						PAIRING_FAIL_REASON_SUPPORT_NC_ONLY

#define PAIRING_FAIL_REASON_INVAILD_PARAMETER 					PAIRING_FAIL_REASON_INVALID_PARAMETER
#define PAIRING_FAIL_REASON_NUMUERIC_FAILED 					PAIRING_FAIL_REASON_NUMERIC_FAILED
#define PAIRING_FAIL_REASON_PAIRING_TIEMOUT 					PAIRING_FAIL_REASON_PAIRING_TIMEOUT

#define IO_CAPABLITY_DISPLAY_ONLY 								IO_CAPABILITY_DISPLAY_ONLY
#define IO_CAPABLITY_DISPLAY_YESNO								IO_CAPABILITY_DISPLAY_YESNO
#define IO_CAPABLITY_KEYBOARD_ONLY								IO_CAPABILITY_KEYBOARD_ONLY
#define IO_CAPABLITY_NO_IN_NO_OUT								IO_CAPABILITY_NO_IN_NO_OUT
#define	IO_CAPABLITY_KEYBOARD_DISPLAY							IO_CAPABILITY_KEYBOARD_DISPLAY

#define Authenticated_pairing_with_Encryption							Authenticated_Pairing_with_Encryption
#define Authenticated_LE_Secure_Connection_pairing_with_Encryption		Authenticated_LE_Secure_Connection_Pairing_with_Encryption

///////////////////////////////////  APP  //////////////////////////////////////
#if FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_512K
	#define CFG_ADR_MAC											CFG_ADR_MAC_512K_FLASH
	#define FLASH_ADR_SMP_PAIRING								FLASH_ADR_SMP_PAIRING_512K_FLASH
#else
	#define CFG_ADR_MAC											CFG_ADR_MAC_128K_FLASH
	#define FLASH_ADR_SMP_PAIRING								FLASH_ADR_SMP_PAIRING_128K_FLASH
#endif

#endif /* BLE_COMP_H_ */

