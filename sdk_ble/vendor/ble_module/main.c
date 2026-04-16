/********************************************************************************************************
 * @file    main.c
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
#include "stack/ble/ble.h"
#include "app.h"
#include "app_att.h"
#include "app_buffer.h"
#include "spp.h"
extern my_fifo_t spp_rx_fifo;

/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
_attribute_ram_code_ void irq_handler(void)
{
	blc_sdk_irq_handler();

#if(MCU_CORE_B80)
	if (reg_uart_status1 & FLD_UART_TX_DONE)
#elif(MCU_CORE_B80B)
	if (reg_uart_status1(UART_NUM) & FLD_UART_TX_DONE)
#endif
	{
		Tr_SetUartTxDone();
		uart_clr_tx_done(UART_NUM);
	}
#if(MCU_CORE_B80)
	if (dma_chn_irq_status_get() & FLD_DMA_CHN_UART_RX)
#elif(MCU_CORE_B80B)
	if (dma_chn_irq_status_get(FLD_DMA_CHN_UART_RX) & FLD_DMA_CHN_UART_RX)
#endif
	{
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
		u8* w = spp_rx_fifo.p + (spp_rx_fifo.wptr & (spp_rx_fifo.num - 1)) * spp_rx_fifo.size;
		if (w[0] != 0 || w[1] != 0) { //Length(u16) is not 0
			my_fifo_next(&spp_rx_fifo);
			u8* p = spp_rx_fifo.p + (spp_rx_fifo.wptr & (spp_rx_fifo.num - 1)) * spp_rx_fifo.size;
			reg_dma_uart_rx_addr = (u16) ((u32) p); //switch uart RX dma address
		}
	}
	if (uart_is_parity_error(UART_NUM))//when stop bit error or parity error.
	{
		uart_clear_parity_error(UART_NUM);
	}
}



/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
#if (PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_sec_noinline_
#endif
int main(void)
{
	#if (BLE_MODULE_PM_ENABLE)
		blc_pm_select_internal_32k_crystal();
	#endif

	#if (BLE_OTA_SERVER_ENABLE && (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_128K))
		blc_ota_setFirmwareSizeAndBootAddress(48, MULTI_BOOT_ADDR_0x10000);
	#endif

	cpu_wakeup_init(INTERNAL_CAP_XTAL24M);

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_ble_init();

	clock_init(SYS_CLK_TYPE);

	gpio_init( !deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again


	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		if( deepRetWakeUp ){
			user_init_deepRetn ();
		}
		else
	#endif
		{

		#if FIRMWARES_SIGNATURE_ENABLE
			blt_firmware_signature_check();
		#endif
			user_init_normal ();
		}

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}

