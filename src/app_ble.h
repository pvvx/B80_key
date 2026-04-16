/*
 * app_ble.h
 *
 *  Created on: 11 апр. 2026 г.
 *      Author: pvvx
 */

#ifndef _APP_BLE_H_
#define _APP_BLE_H_

typedef struct {
	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;
}rf_adv_head_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	rf_adv_head_t  header;
	u8  rf_len;				//LEN(6)_RFU(2)
	u8	advA[6];			//address
	u8	data[31];			//0-31 byte
} rf_packet_adv_t;

#define	BLE_DEVICE_ADDRESS_TYPE 	OWN_ADDRESS_PUBLIC
#define MY_APP_ADV_CHANNEL			BLT_ENABLE_ADV_ALL
#define MY_CONNECT_ADV_TIME			(80*1000000) // 80 sec
#define MY_EVENT_ADV_TIME		((MY_ADV_EVENT_INTERVAL)*10000) // 16*0.625=10

#define ADV_BUFFER_SIZE		(31-3)

typedef struct {
	u32 send_count; // count & id advertise, = beacon_nonce.cnt32
	u8 adv_event;	// work Advertise event
	u8 data_size;	// Advertise data size
	u8 flag[3];		// Advertise type flags
	u8 data[ADV_BUFFER_SIZE];
} adv_buf_t;

extern adv_buf_t adv_buf;

#if USE_PINCODE
extern u32 pincode;
#endif
extern u8 mac_public[6];

typedef int (*advertise_prepare_handler_t) (rf_packet_adv_t * p);
extern advertise_prepare_handler_t advertise_prepare_handler;

void app_ble_init(void);
int app_event_handler(u32 h, u8 *para, int n);
void app_start_adv(u8 adv_type, u32 interval, u32 duration_us);

#define app_adv_init()	app_start_adv(0, MY_ADV_CONNECT_INTERVAL, MY_CONNECT_ADV_TIME)
#define app_set_work_adv() app_start_adv(0, dev_cfg.advInterval, 0)
#define app_set_connect_adv() app_start_adv(0, MY_ADV_CONNECT_INTERVAL, MY_CONNECT_ADV_TIME)
#define app_set_event_adv() app_start_adv(1, MY_ADV_EVENT_INTERVAL, MY_EVENT_ADV_TIME)

#endif /* SRC_APP_BLE_H_ */
