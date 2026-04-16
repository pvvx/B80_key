/*
 * app_ble.c
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app_main.h"
#include "app_buffer.h"
#include "app_data.h"
#include "app_att.h"
#include "app_ble.h"
#include "bthome_beacon.h"
#include "sws_printf.h"

RAM u8 mac_public[6];
RAM adv_buf_t adv_buf;
#if USE_PINCODE
RAM u32 pincode;
#endif

#if (BLE_OTA_SERVER_ENABLE)
/**
 * @brief		callback function of ota start
 * @param[in]	none
 * @return      none
 */
void entry_ota_mode(void)
{
	blc_ota_setOtaProcessTimeout(240); //set OTA timeout 240 seconds
	wrk.otaIsWorking = 1;
}

/**
 * @brief		callback function of ota result
 * @param[in]	none
 * @return      none
 */
void show_ota_result(int result)
{
	if(result == OTA_SUCCESS) {  //OTA success

	}
}
#endif

#if SWS_PRINTF // debug
int app_advertise_prepare_handler(rf_packet_adv_t * p) {
	wrk.advCnt++;
	sws_printf("ADV: count %d\n", wrk.advCnt)
}
#endif


void app_start_adv(u8 adv_type, u32 interval, u32 duration_us) {
	adv_buf.adv_event = adv_type;
	bls_ll_setAdvEnable(BLC_ADV_DISABLE); // adv disable
// TODO: ADV_TYPE_NONCONNECTABLE_UNDIRECTED if adv != connection?
	bls_ll_setAdvParam(interval, interval + 5,
		ADV_TYPE_CONNECTABLE_UNDIRECTED,
		BLE_DEVICE_ADDRESS_TYPE,
		0,  NULL,
		MY_APP_ADV_CHANNEL,
		ADV_FP_NONE);

	adv_buf.flag[0] = 2;
	adv_buf.flag[1] = DT_FLAGS;
	adv_buf.flag[2] = 6;

	adv_buf.send_count++;
#if	USE_BINDKEY
	if(dev_cfg.flags & 1)
		bthome_encrypt_beacon(adv_type);
	else
#endif
		bthome_beacon(adv_type);
	bls_ll_setAdvData(adv_buf.flag, adv_buf.data_size);
	bls_ll_setScanRspData((u8 *)&tbl_scanRsp, tbl_scanRsp.len + 1);
	if(duration_us) {
		bls_ll_setAdvDuration(duration_us, 1);
	}
	bls_ll_setAdvEnable(BLC_ADV_ENABLE);  // adv enable
}


/**
 * @brief      callback function of Host Event
 * @param[in]  h - Host Event type
 * @param[in]  para - data pointer of event
 * @param[in]  n - data length of event
 * @return     0
 */
int app_host_event_callback (u32 h, u8 *para, int n) {
	switch(h & 0xff) {
		case GAP_EVT_SMP_PAIRING_BEGIN:
			{
				sws_puts("Pairing begin\n");
			}
			break;
		case GAP_EVT_SMP_PAIRING_SUCCESS:
			{
				gap_smp_pairingSuccessEvt_t* p = (gap_smp_pairingSuccessEvt_t*)para;
				sws_printf("Pairing success:bond flg %s\n", p->bonding? "true":"false");
				if(p->bonding_result) {
					sws_puts("save smp key succ\n");
				} else {
					sws_puts("save smp key failed\n");
				}
			}
			break;
		case GAP_EVT_SMP_PAIRING_FAIL:
			{
//#if SWS_PRINTF
				sws_printf("Pairing failed:rsn:0x%x\n", ((gap_smp_pairingFailEvt_t*)para)->reason);
//#endif
			}
			break;
		case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
			{
#if SWS_PRINTF
				gap_smp_connEncDoneEvt_t* p = (gap_smp_connEncDoneEvt_t*)para;
				sws_puts("Connection encryption done\n");
				if(p->re_connect == SMP_STANDARD_PAIR){  //first pairing
				} else if(p->re_connect == SMP_FAST_CONNECT) {  //auto connect
				}
#endif
			}
			break;
		case GAP_EVT_SMP_TK_DISPLAY:
			{
				sws_printf("TK display: %d\n", *(u32*)para);
			}
			break;
		case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
			{
				sws_puts("TK Request passkey\n");
			}
			break;
		case GAP_EVT_SMP_TK_REQUEST_OOB:
			{
				sws_puts("TK Request OOB\n");
			}
			break;
		case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
			{
				sws_printf("TK numeric comparison: %d\n", *(u32*)para);
			}
			break;
	}
	return 0;
}

void app_ble_init(void) {
	u8 mac_random_static[6];
	//* BLE stack Initialization  Begin
	blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
#if USE_BINDKEY
	bthome_beacon_init();
#endif
#if(BLE_DEVICE_ADDRESS_TYPE == OWN_ADDRESS_PUBLIC)
//	app_own_address_type = OWN_ADDRESS_PUBLIC;
	sws_printf("MAC: %06p\n", mac_public);
#elif(BLE_DEVICE_ADDRESS_TYPE == OWN_ADDRESS_RANDOM)
//	app_own_address_type = OWN_ADDRESS_RANDOM;
	blc_ll_setRandomAddr(mac_random_static);
	sws_printf("MAC: %06p\n", mac_random_static);
#endif

	//* LinkLayer Initialization  Begin
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(mac_public);		//mandatory
	blc_ll_initAdvertising_module();//adv module: mandatory for BLE slave,
	blc_ll_initSlaveRole_module();//slave module: mandatory for BLE slave,

	blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CONN_MAX_TX_OCTETS);
	blc_ll_initAclConnTxFifo(app_acl_txfifo, ACL_TX_FIFO_SIZE, ACL_TX_FIFO_NUM);
	blc_ll_initAclConnRxFifo(app_acl_rxfifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);
	//* LinkLayer Initialization  End
	//* HCI Initialization  Begin
	//* HCI Initialization  End
	//* Host Initialization  Begin
	/* GAP initialization must be done before any other host feature initialization !!! */
	blc_gap_peripheral_init();    //gap initialization
	/* GATT Initialization */
	my_gatt_init();
	/* L2CAP Initialization */
	blc_l2cap_register_handler(blc_l2cap_packet_receive);
	blc_l2cap_initRxDataBuffer(app_l2cap_rx_fifo, L2CAP_RX_BUFF_SIZE);
#if USE_PINCODE
	/* SMP Initialization */
	/* SMP Initialization may involve flash write/erase(when one sector stores too much information,
	 *   is about to exceed the sector threshold, this sector must be erased, and all useful information
	 *   should re_stored) , so it must be done after battery check */
	if (pincode && pincode <= 999999) {
		bls_smp_configPairingSecurityInfoStorageAddr(flash_sector_smp_storage);
		blc_smp_param_setBondingDeviceMaxNumber(4);	// default is 4, can not bigger than this value
		//set security level: "LE_Security_Mode_1_Level_3"
		blc_smp_setSecurityLevel(Authenticated_Pairing_with_Encryption);  //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Pairing_with_Encryption)
		blc_smp_enableAuthMITM(1);
		//blc_smp_setBondingMode(Bondable_Mode);	// if not set, default is : Bondable_Mode
		blc_smp_setIoCapability(IO_CAPABILITY_DISPLAY_ONLY);	// if not set, default is : IO_CAPABILITY_NO_INPUT_NO_OUTPUT
		//Smp Initialization may involve flash write/erase(when one sector stores too much information,
		//   is about to exceed the sector threshold, this sector must be erased, and all useful information
		//   should re_stored) , so it must be done after battery check
		//Notice:if user set smp parameters: it should be called after usr smp settings
		blc_smp_peripheral_init();

		blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )

		//host(GAP/SMP/GATT/ATT) event process: register host event callback and set event mask
		blc_gap_registerHostEventHandler(app_host_event_callback);
		blc_gap_setEventMask(GAP_EVT_MASK_SMP_TK_DISPLAY
#if 0
			| GAP_EVT_MASK_SMP_PAIRING_BEGIN
			| GAP_EVT_MASK_SMP_PAIRING_SUCCESS
			| GAP_EVT_MASK_SMP_PAIRING_FAIL
			| GAP_EVT_MASK_SMP_TK_DISPLAY
			| GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE
			| GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE
#endif
			);
		blc_smp_manualSetPinCode_for_debug(BLS_CONN_HANDLE, pincode);
	}
#else // ! USE_PINCODE
	blc_smp_setSecurityLevel(No_Security);
#endif // USE_PINCODE
	//* Host Initialization  End
	//* Service Initialization  Begin ////////////////////
#if (BLE_OTA_SERVER_ENABLE)
	// OTA init
	blc_ota_initOtaServer_module(); //must
	blc_ota_setOtaProcessTimeout(60); //set OTA whole process timeout
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);
#endif
	//* Service Initialization  End

	/* set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend */
	rf_set_power_level_index(dev_cfg.rfPower);

	//	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	//	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	// bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &task_suspend_exit);

	blc_hci_registerControllerEventHandler(app_event_handler); //register event callback
	bls_hci_mod_setEventMask_cmd(0xfffff); //enable all 18 events,event list see ll.h

	//* BLE stack Initialization  End
	/* Check if any Stack(Controller & Host) Initialization error after all BLE initialization done.
	 * attention that code will stuck in "while(1)" if any error detected in initialization, user need find what error happens and then fix it */
	// blc_app_checkControllerHostInitialization();

#if SWS_PRINTF // debug
	advertise_prepare_handler = app_advertise_prepare_handler;
#endif
}

/**
 * @brief      callback function of LinkLayer Event
 * @param[in]  h     - LinkLayer Event type
 * @param[in]  param - data pointer of event
 * @param[in]  n     - data length of event
 * @return     none
 */
int app_event_handler(u32 h, u8 *para, int n) {
	if((h & HCI_FLAG_EVENT_TLK_MODULE)!= 0)	{		//module event
		switch(h & 0xff) {
		case BLT_EV_FLAG_ADV_DURATION_TIMEOUT:
			//sws_puts("EVNT: ADV duration\n");
			if(adv_buf.adv_event) // && wrk.keyPressed)
				app_set_connect_adv();
			else
				app_set_work_adv();
			break;
		case BLT_EV_FLAG_SCAN_RSP: // scan request from Master
			sws_puts("EVNT: Scan resp\n");
			break;
		case BLT_EV_FLAG_CONNECT: // responds to connect reqeust from Master and enters Conn state Slave role
			// tlk_contr_evt_connect_t *p = (tlk_contr_evt_connect_t *) para;
			sws_puts("EVNT: Connect\n");
			wrk.bleConnected = BIT(CONNECTED_FLG_ENABLE);
			bls_l2cap_requestConnParamUpdate(CONN_INTERVAL_10MS, CONN_INTERVAL_15MS, 99, CONN_TIMEOUT_4S);
			break;
		case BLT_EV_FLAG_TERMINATE: // reason for disconnection
			// para[0] = tlk_contr_evt_terminate_t: ble_sts_t (HCI_ERR_CONN_TIMEOUT, ...)
			sws_printf("EVNT: Terminate %02x\n", para[0]);
			if(wrk.bleConnected & BIT(CONNECTED_FLG_RESET_OF_DISCONNECT)) {
				sws_puts("Go to reboot..\n");
				sws_buffer_flush();
				start_reboot();
			}
			wrk.bleConnected = 0;
			app_set_work_adv();
			break;
		case BLT_EV_FLAG_LL_REJECT_IND: // encryption request
			// para[0] = LL_REJECT_IND or LL_REJECT_EXT_IND
			sws_puts("EVNT: encryption request\n");
			break;
		case BLT_EV_FLAG_RX_DATA_ABANDON: // RX fifo overflows
			//sws_puts("EVNT: RX fifo overflows\n");
			break;
		case BLT_EV_FLAG_PHY_UPDATE: // PHY is updated
			// para[0] = le_phy_type_t: BLE_PHY_1M = BIT(0), BLE_PHY_2M  BIT(1)
			sws_printf("EVNT: PHY update %d\n", para[0] & 3);
			break;
		case BLT_EV_FLAG_DATA_LENGTH_EXCHANGE: // exchange max data length
			wrk.bleConnected |= BIT(CONNECTED_FLG_DATA_LENGTH_EXCHANGE);
			sws_printf("EVNT: exchange len %d:%d\n",
					((tlk_contr_evt_dataLenExg_t *)para)->connEffectiveMaxRxOctets,
					((tlk_contr_evt_dataLenExg_t *)para)->connEffectiveMaxTxOctets);
			break;
		case BLT_EV_FLAG_GPIO_EARLY_WAKEUP: // woke up from sleep by GPIO
			// para[0] = WAKEUP_STATUS_PAD, WAKEUP_STATUS_CORE, ..
			break;
		case BLT_EV_FLAG_CHN_MAP_REQ: // update current connection channel list
			// tlk_contr_evt_chnMapRequest_t  *p = (tlk_contr_evt_chnMapRequest_t *) para;
			break;
		case BLT_EV_FLAG_CONN_PARA_REQ: // update current connection parameters
			// Slave received Master's LL_Connect_Update_Req pkt.
			sws_printf("EVNT: Conn update %d,%d,%d\n",
					((tlk_contr_evt_connParaReq_t *)para)->interval,
					((tlk_contr_evt_connParaReq_t *)para)->latency,
					((tlk_contr_evt_connParaReq_t *)para)->timeout);
			break;
		case BLT_EV_FLAG_CHN_MAP_UPDATE: // updated channel map
			// tlk_contr_evt_chnMapRequest_t  *p = (tlk_contr_evt_chnMapRequest_t *) para;
			//sws_puts("EVNT: chg map update req\n");
			break;
		case BLT_EV_FLAG_CONN_PARA_UPDATE:// updated connection parameters
			//Master send SIG_Connection_Param_Update_Rsp pkt,and the reply result is 0x0000. When connection event counter value is equal
			//to the instant, a callback event BLT_EV_FLAG_CONN_PARA_UPDATE will generate. The connection interval at this time should be the
			//currently updated and valid connection interval!
			wrk.bleConnected |= BIT(CONNECTED_FLG_PAR_UPDATE);
			sws_puts("EVNT: Conn update rsp\n");
			break;
		case BLT_EV_FLAG_SUSPEND_ENTER: // event before Slave enters suspend
			// app_suspend_enter();
			break;
		case BLT_EV_FLAG_SUSPEND_EXIT:
			/* set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend */
			rf_set_power_level_index(dev_cfg.rfPower);
			// app_suspend_exit();
			break;
		default:
			break;
		}
	}
	return 0;
}
