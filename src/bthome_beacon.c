/*
 * bthome_beacon.c
 *
 *  Created on: 17.10.23
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_main.h"
#include "app_data.h"
//#include "app_att.h"
#include "app_ble.h"
//#include "app_battery.h"
//#include "app_soc_temp.h"
#include "sws_printf.h"
#include "bthome_beacon.h"

static u32 set_bthome_data(pseg_bthome_data_t p) {
#if (BATTERY_SERVICE_ENABLE)
	p->b_id = BtHomeID_battery;
	p->battery_level = services.battery_level;
#endif
#if (TEMP_SERVICE_ENABLE)
	p->t_id = BtHomeID_temperature;
	p->temperature = services.temp; // x0.01 C
#endif
#if (HUMI_SERVICE_ENABLE)
	p->h_id = BtHomeID_humidity;
	p->humidity = measured_data.humi; // x0.01 %
#endif
#if (BATTERY_SERVICE_ENABLE)
	p->v_id = BtHomeID_voltage;
	p->battery_mv = services.battery_mv; // x mV
#endif
	return sizeof(seg_bthome_data_t);
}

static u32 set_bthome_event(pseg_bthome_event_t p) {
	p->o_id = BtHomeID_opened;
	p->opened = !wrk.keyPressed;
	p->c_id = BtHomeID_count32;
	p->counter = wrk.keyCounter;
	return sizeof(seg_bthome_event_t);
}

void bthome_beacon(bthome_beacon_number_e n) {
	padv_bthome_data_nc_t p = (padv_bthome_data_nc_t)&adv_buf.data;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_BTHOME_UUID16;
	p->info = BtHomeID_Info;
	p->p_id = BtHomeID_PacketId;
	p->pid = (u8)adv_buf.send_count;
	if(n) {
		set_bthome_event((pseg_bthome_event_t)&p->data);
		p->head.size = sizeof(adv_bthome_event_nc_t) - sizeof(p->head.size);
		adv_buf.data_size = sizeof(adv_bthome_event_nc_t) + sizeof(adv_buf.flag);
	} else {
		set_bthome_data((pseg_bthome_data_t)&p->data);
		p->head.size = sizeof(adv_bthome_data_nc_t) - sizeof(p->head.size);
		adv_buf.data_size = sizeof(adv_bthome_data_nc_t) + sizeof(adv_buf.flag);
	}
}

#if USE_BINDKEY

#include "ccm.h"

RAM u8 bindkey[16]
#if	!USE_EEP
			   = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#endif
;

RAM bthome_beacon_nonce_t bthome_nonce;

void bthome_beacon_init(void) {
	swapX(mac_public, bthome_nonce.mac, 6);
	bthome_nonce.uuid16 = ADV_BTHOME_UUID16;
	bthome_nonce.info = BtHomeID_Info_Encrypt;
}

// BTHOME adv security
typedef struct __attribute__((packed)) {
	adv_head_bth_t head; // 4 bytes
	u8 info;			 // 1 byte
	u8 data[23];		 // max 31-3-5 = 23 bytes
} adv_bthome_encrypt_t, * padv_bthome_encrypt_t;

/* Encrypt bthome data beacon packet */
__attribute__((optimize("-Os")))
static void bthome_encrypt(u8 *pdata, u32 data_size) {
	padv_bthome_encrypt_t p = (padv_bthome_encrypt_t)&adv_buf.data;
	p->head.size = data_size + sizeof(p->head) - sizeof(p->head.size) + sizeof(p->info) + 4 + 4; // + mic + count
	adv_buf.data_size = p->head.size + 1 + sizeof(adv_buf.flag);
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = bthome_nonce.uuid16;
	p->info = bthome_nonce.info;
	u8 *pmic = &adv_buf.data[data_size + sizeof(p->head) + sizeof(p->info)];
	*pmic++ = (u8)adv_buf.send_count;
	*pmic++ = (u8)(adv_buf.send_count>>8);
	*pmic++ = (u8)(adv_buf.send_count>>16);
	*pmic++ = (u8)(adv_buf.send_count>>24);
	bthome_nonce.cnt32 = adv_buf.send_count;
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (u8*)&bthome_nonce, sizeof(bthome_nonce),
					   NULL, 0,
					   pdata, data_size,
					   p->data,
					   pmic, 4);
}

/* Create encrypted bthome data beacon packet */
void bthome_encrypt_beacon(bthome_beacon_number_e n) {
	u8 buf[16];
	if(n) {
		bthome_encrypt(buf, set_bthome_event((pseg_bthome_event_t)&buf));
	} else {
		bthome_encrypt(buf, set_bthome_data((pseg_bthome_data_t)&buf));
	}
}

#endif // USE_BINDKEY

