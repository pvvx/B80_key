/*
 * app_command.c
 *
 *  Created on: 13 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app_main.h"
#include "app_data.h"
#include "app_att.h"
#include "app_ble.h"
#include "app_battery.h"
#include "app_soc_temp.h"
#include "app_cmd.h"
#include "bthome_beacon.h"
#include "flash_eep.h"
#include "sws_printf.h"

//#define send_buf services.RxTxData
u8 send_buf[20];

/**
 * @brief      write callback of Attribute of RxTx service
 * @param[in]  para - rf_packet_att_write_t
 * @return     0
 */
int app_onWriteData(void *para)
{
	u32 olen = 0;
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)para;
	u32 len = req->l2capLen - 3;

	if(services.RxTxValueInCCC && len > 0)	{
		sws_puts("Rec: ");
		sws_print_hex_dump(req->dat, len);
		sws_putchar('\n');
		len--;
		u8 cmd = req->dat[0];
		send_buf[0] = cmd;
		send_buf[1] = 0; // no err
		switch(cmd) {
		case CMD_ID_DEV_ID: // Get DEV_ID
			memcpy(send_buf, &dev_id, sizeof(send_buf));
			olen = sizeof(dev_id);
			break;
		case CMD_ID_DNAME: // Get/Set device name, "\0" - set default
			if (len) {
				if (len > MAX_DEV_NAME_LEN)
					len = MAX_DEV_NAME_LEN;
				save_dev_name(&req->dat[1], len);
				// wrk.bleConnected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			}
			memcpy(&send_buf[1], tbl_scanRsp.name, tbl_scanRsp.len - 1);
			olen = tbl_scanRsp.len;
			break;
		case CMD_ID_CFG: // Get/set config
			if (len) {
				if (len > sizeof(dev_cfg))
					len = sizeof(dev_cfg);
				memcpy(&dev_cfg, &req->dat[1], len);
				save_dev_cfg();
			}
			memcpy(&send_buf[2], &dev_cfg, sizeof(dev_cfg));
			olen = sizeof(dev_cfg) + 1;
			break;
		case CMD_ID_CFG_DEF: // Set default device config
			memcpy(&dev_cfg, &dev_cfg_def, sizeof(dev_cfg));
			test_dev_cfg();
			olen = sizeof(dev_cfg) + 1;
			break;
		case CMD_ID_DEV_MAC: // Get/Set mac
			if (len == 1 && req->dat[1] == 0) { // default MAC
				flash_erase_sector(flash_sector_mac_address);
				blc_initMacAddress(flash_sector_mac_address, &send_buf[2], &send_buf[2+6]);
				wrk.bleConnected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			} else if (len == sizeof(mac_public)+1 && req->dat[1] == sizeof(mac_public)) {
				flash_read_page(flash_sector_mac_address, 8, &send_buf[2]);
				if (memcmp(&send_buf[2], &req->dat[2], 6)) {
					memcpy(&send_buf[2], &req->dat[2], 6);
					generateRandomNum(2, &send_buf[2+6]);
					flash_erase_sector(flash_sector_mac_address);
					flash_write_page(flash_sector_mac_address, 8, &send_buf[2]);
					wrk.bleConnected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			} else if (len == sizeof(mac_public)+2+1 && req->dat[1] == sizeof(mac_public)+2) {
				flash_read_page(flash_sector_mac_address, 8, &send_buf[2]);
				if (memcmp(&send_buf[2], &req->dat[2], 8)) {
					memcpy(&send_buf[2], &req->dat[2], 8);
					flash_erase_sector(flash_sector_mac_address);
					flash_write_page(flash_sector_mac_address, 8, &send_buf[2]);
					wrk.bleConnected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			}
			flash_read_page(flash_sector_mac_address, 8, &send_buf[2]);
			send_buf[1] = 8;
			olen = 8 + 2;
			break;
		case CMD_ID_CFS: // Get/Set sensor config
			if (len) {
				if (len > sizeof(temperature.cfg))
					len = sizeof(temperature.cfg);
				memcpy(&temperature.cfg, &req->dat[1], len);
				save_temp_cfg();
			}
			memcpy(&send_buf[2], &temperature.cfg, sizeof(temperature.cfg));
			olen = sizeof(temperature.cfg) + 1;
			break;
		case CMD_ID_CFS_DEF: // Set default sensor config
			memcpy(&temperature.cfg, &temperature_cfg_def, sizeof(temperature.cfg));
			memcpy(&send_buf[2], &temperature.cfg, sizeof(temperature.cfg));
			olen = sizeof(temperature.cfg) + 1;
			break;
#if USE_PINCODE
		case CMD_ID_PINCODE: // Set new PinCode 0..999999
			if(len == 4) {
				u32 new_pincode = req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16) | (req->dat[4]<<24);
				if (pincode != new_pincode && new_pincode <= 999999) {
					pincode = new_pincode;
#if USE_EEP
					flash_write_cfg(&pincode, EEP_ID_PINCODE, sizeof(pincode));
#endif
					wrk.bleConnected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
					send_buf[1] = 2; // new pincode write
				} else {
					send_buf[1] = 3; // already (new pincode = old pincode)
				}
				bls_smp_eraseAllPairingInformation();
			} else if(pincode) {
				send_buf[1] = 1; // pincode presents (!= 0)
			}
			olen = 2;
			break;
#endif
#if USE_BINDKEY
		case CMD_ID_BKEY: // Get/Set beacon bindkey in EEP
			if (len == sizeof(bindkey)) {
				memcpy(bindkey, &req->dat[1], sizeof(bindkey));
#if USE_EEP
				flash_write_cfg(&bindkey, EEP_ID_BKEY, sizeof(bindkey));
#endif
				bthome_beacon_init();
			}
			memcpy(&send_buf[1], bindkey, sizeof(bindkey));
			olen = sizeof(bindkey) + 1;
			break;
#endif
		default:
			send_buf[1] = 0xff; // Error cmd
			olen = 2;
		}
		if (olen)
			blc_gatt_pushHandleValueNotify(BLS_CONN_HANDLE, RxTx_CMD_OUT_DP_H, send_buf, olen);
	}
	return 0;
}
