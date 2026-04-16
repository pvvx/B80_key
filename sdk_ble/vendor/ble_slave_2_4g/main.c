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
#include "2p4g_common.h"

volatile unsigned char irq_bleModeFlag = 1;
/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
_attribute_ram_code_ void irq_handler(void)
{
	#if (TEST_2P4G_MODE)
		if(!irq_bleModeFlag)
		{
			concurrent_irq_handler();
		}
		else
	#endif
		{
			blc_sdk_irq_handler();
		}
}


/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main(void)
{
	#if (BLE_OTA_SERVER_ENABLE && (FLASH_SIZE_OPTION == FLASH_SIZE_OPTION_128K))
		blc_ota_setFirmwareSizeAndBootAddress(48, MULTI_BOOT_ADDR_0x10000);
	#endif
	cpu_wakeup_init(INTERNAL_CAP_XTAL24M);

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_ble_init();

	clock_init(SYS_CLK_TYPE);

	gpio_init( !deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again


	#if FIRMWARES_SIGNATURE_ENABLE
		blt_firmware_signature_check();
	#endif
	user_init_normal ();
	#if (MODULE_WATCHDOG_ENABLE)
		wd_set_interval_ms(WATCHDOG_INIT_TIMEOUT,CLOCK_SYS_CLOCK_1MS);
		wd_start();
	#endif

    irq_enable();

	while (1) {
		#if (MODULE_WATCHDOG_ENABLE)
			wd_clear(); //clear watch dog
		#endif

		main_loop ();
	}
}

