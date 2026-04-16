/********************************************************************************************************
 * @file    main.c
 *
 * @brief   This is the source file for 2.4G SDK
 *
 * @author  2.4G GROUP
 * @date    02,2025
 *
 * @par     Copyright (c) 2025, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "app.h"
#include "stack/ble/ble.h"
#include "ota.h"

#if (FEATURE_TEST_MODE == OTA)

void irq_2p4g_sdk_handler(void);

/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
_attribute_ram_code_ void irq_handler(void)
{
	irq_2p4g_sdk_handler();
}

#if(RF_DEBUG_IO_ENABLE)
void rf_debug_config(void)
{
    gpio_set_func(GPIO_PA0, DBG);  //TX EN
    gpio_set_func(GPIO_PA4, DBG);  //TX ON
    gpio_set_func(GPIO_PA5, DBG);  //RX EB
    gpio_set_func(GPIO_PA6, DBG);  //RX DATE
    gpio_set_func(GPIO_PA7, DBG);  //RX ACCESS DET
    gpio_set_func(GPIO_PB0, DBG);  //RX DATA CLK
    gpio_set_func(GPIO_PB2, DBG);  //TX DATA
    gpio_set_func(GPIO_PB4, DBG);  //CLK DIG

    write_reg8(0x574, read_reg8(0x574)|BIT(4)|BIT(5));
}
#endif

/**
 * @brief       This is main function
 * @param[in]   none
 * @return      none
 */
#if (APP_PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_sec_noinline_
#endif
int main(void)
{
    #if (APP_PM_ENABLE)
        blc_pm_select_internal_32k_crystal();
    #endif

    #if (OTA_SERVER_ENABLE && (OTA_MASTER_BIN_ADDR == OTA_BIN_ADDR_0x40000 || OTA_SLAVE_BIN_ADDR == OTA_BIN_ADDR_0x40000))
        blc_ota_setFirmwareSizeAndBootAddress(252, MULTI_BOOT_ADDR_0x40000);
    #endif

    cpu_wakeup_init(INTERNAL_CAP_XTAL24M);

    int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

    clock_init(SYS_CLK_TYPE);

    gpio_init( !deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again

    #if(RF_DEBUG_IO_ENABLE)
        rf_debug_config();
    #endif

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
#endif
