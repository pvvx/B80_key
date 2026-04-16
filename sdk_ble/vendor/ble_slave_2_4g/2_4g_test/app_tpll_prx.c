/********************************************************************************************************
 * @file    app_tpll_prx.c
 *
 * @brief   This is the source file for B80
 *
 * @author  2.4G Group
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
#include "stack/ble/ble.h"

#include "../app_config.h"
#include "driver.h"
#include "2p4g_common.h"

#include "tpll/tpll.h"

#if (TEST_2P4G_MODE==TPLL_PRX)

#define ACK_PAYLOAD_LEN         32

static volatile unsigned int rx_interval_us, rx_timestamp, rx_rssi = 0;
volatile unsigned char rx_flag, rx_dr_flag, tx_flag, ds_flag, invalid_pid_flag = 0;
volatile unsigned char rx_payload[128] = {0};
unsigned short length_pip_ret = 0;
static unsigned char ack_payload[ACK_PAYLOAD_LEN] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                                      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

volatile unsigned int rx_irq_cnt_rx_dr = 0;
volatile unsigned int rx_irq_cnt_invalid_pid = 0;
volatile unsigned int rx_irq_cnt_rx = 0;
volatile unsigned int rx_irq_cnt_tx = 0;
volatile unsigned int rx_irq_cnt_tx_ds = 0;
volatile unsigned int rx_irq_cnt_crc_ok = 0;

extern volatile unsigned char rx_flag, rx_dr_flag, tx_flag, ds_flag, invalid_pid_flag;
extern volatile unsigned char rx_data[];

__attribute__((section(".ram_code"))) __attribute__((optimize("-Os")))
void concurrent_irq_handler(void)
{
    unsigned short src_rf = rf_irq_src_get();

    if (src_rf & FLD_RF_IRQ_RX_DR)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX_DR);
        rx_irq_cnt_rx_dr++;
        rx_dr_flag = 1;
    }

    if (src_rf & FLD_RF_IRQ_RX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX);
        rx_irq_cnt_rx++;
        if( TPLL_RF_PACKET_CRC_OK(TPLL_GetRxPacket()) ){
            rx_irq_cnt_crc_ok++;
            rx_flag = 1;
        }
    }

    if (src_rf & FLD_RF_IRQ_INVALID_PID)
    {
        rf_irq_clr_src(FLD_RF_IRQ_INVALID_PID);
        rx_irq_cnt_invalid_pid++;
        invalid_pid_flag = 1;
    }

    if (src_rf & FLD_RF_IRQ_TX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        rx_irq_cnt_tx++;
        tx_flag = 1;
    }

    if (src_rf & FLD_RF_IRQ_TX_DS)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX_DS);
        rx_irq_cnt_tx_ds++;
        ds_flag = 1;
    }

    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
}

void tpll_config_init(signed short chnn)
{
	rf_2_4g_state_reset();
    irq_bleModeFlag = 0;
    unsigned char rx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7};
    /*
     * rf configuration
     * notes:b80 rx does not support multiple pipes
     */
    TPLL_Init(TPLL_BITRATE_2MBPS);

    TPLL_SetOutputPower(TPLL_RF_POWER_N0p22dBm);

    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);

    TPLL_ClosePipe(TPLL_PIPE_ALL);

    TPLL_SetAddress(TPLL_PIPE0, rx_address);

    TPLL_OpenPipe(TPLL_PIPE0);

    TPLL_ModeSet(TPLL_MODE_PRX);

    TPLL_SetRFChannel(chnn);

    TPLL_TxSettleSet(149);

    TPLL_RxSettleSet(80);

    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
    irq_enable_type(FLD_IRQ_ZB_RT_EN);
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_RX_DR);
    irq_enable();
}

int app_mainloop_2p4g(unsigned int  wakeup_tick)
{
	u32 base_tick = clock_time();
	unsigned int span = (unsigned int)(wakeup_tick - CONCURRENT_THRESHOLD_TIME * sys_tick_per_us - base_tick);
	if (span > 0xE0000000)  //BIT(31)+BIT(30)+BIT(29)   7/8 cycle of 32bit, 268.44*7/8 = 234.88 S
	{
		return  0;
	}

    u32 r = irq_disable();  //must disable irq.
    tpll_config_init(4);

    TPLL_PRXTrig();
    rx_timestamp = clock_time();

    while (1)
    {
        if (1 == rx_flag)
        {
            rx_flag = 0;
            gpio_toggle(GPIO_LED_GREEN);
            length_pip_ret = TPLL_ReadRxPayload((unsigned char *)&rx_payload);
    		while(!TPLL_TxFifoEmpty(0));
    		TPLL_WriteAckPayload(TPLL_PIPE0, ack_payload, ACK_PAYLOAD_LEN);

        }
    	app_idle_loop_2p4g();
        if(clock_time_exceed(base_tick, span/sys_tick_per_us))
        {

            app_exit_2p4gMode();
            break;
        }
    }

    irq_restore(r);

    return 0;
}

#endif
