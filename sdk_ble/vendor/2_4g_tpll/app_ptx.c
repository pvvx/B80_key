/********************************************************************************************************
 * @file    app_ptx.c
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
#include "app_config.h"
#include "app.h"
#include "stack/ble/ble.h"
#include "battery_check.h"
#include "stack/2_4g/tpll/tpll.h"

#if (TPLL_MODE == TPLL_PTX)
#define PTX_PIPE                     0//warning:B80 only support pipe0,B80B support pipe0~5
#define PTX_CHANNEL                  7
#define FAST_CALIBRATION_ENABLE      0

#if(FAST_CALIBRATION_ENABLE)
#define FAST_CALIB_VALUE_ADDRESS     0x44000//0x40000+16kbyte
static volatile unsigned short hpmc[160] = {0};
void TPLL_Fast_SettleCalib(void);
#endif

volatile unsigned char maxretry_flag  = 0;
volatile unsigned char rx_dr_flag     = 0;
volatile unsigned char rx_flag        = 0;
unsigned char          tx_payload_len = 32;
unsigned char          preamble_len   = 0;
volatile unsigned char ds_flag        = 0;
int                    ret            = 0;
static unsigned char tx_payload[32] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                                 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                                 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20};

volatile unsigned int        irq_cnt_invalid_pid          = 0;
volatile unsigned int        irq_cnt_max_retry            = 0;
volatile unsigned int        irq_cnt_tx_ds                = 0;
volatile unsigned int        irq_cnt_tx                   = 0;
volatile unsigned int        irq_cnt_rx                   = 0;
volatile unsigned int        irq_cnt_rx_dr                = 0;

_attribute_ram_code_ void irq_2p4g_sdk_handler(void)
{
    unsigned short src_rf = rf_irq_src_get();
    unsigned char pipe = TPLL_GetTXPipe();
    if (src_rf & FLD_RF_IRQ_TX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        irq_cnt_tx++;
    }
    if (src_rf & FLD_RF_IRQ_INVALID_PID)
    {
        rf_irq_clr_src(FLD_RF_IRQ_INVALID_PID);
        irq_cnt_invalid_pid++;
    }
    if (src_rf & FLD_RF_IRQ_TX_RETRYCNT)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX_RETRYCNT);
        irq_cnt_max_retry++;
        maxretry_flag = 1;
        //adjust rptr
        TPLL_UpdateTXFifoRptr(pipe);
    }
    if (src_rf & FLD_RF_IRQ_TX_DS)
    {
        rf_irq_clr_src(FLD_RF_IRQ_TX_DS);
        irq_cnt_tx_ds++;
        ds_flag = 1;
    }
    if (src_rf & FLD_RF_IRQ_RX_DR)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX_DR);
        irq_cnt_rx_dr++;
        rx_dr_flag = 1;
    }
    if (src_rf & FLD_RF_IRQ_RX)
    {
        rf_irq_clr_src(FLD_RF_IRQ_RX);
        irq_cnt_rx++;
        rx_flag = 1;
    }
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
}

/**
 * @brief      power management code for application
 * @param	   none
 * @return     none
 */
void blt_pm_proc(void)
{


}

#if (APP_BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation

_attribute_data_retention_	u32	lowBattDet_tick   = 0;

/**
 * @brief		this function is used to process battery power.
 * 				The low voltage protection threshold 2.0V is an example and reference value. Customers should
 * 				evaluate and modify these thresholds according to the actual situation. If users have unreasonable designs
 * 				in the hardware circuit, which leads to a decrease in the stability of the power supply network, the
 * 				safety thresholds must be increased as appropriate.
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void user_battery_power_check(u16 alarm_vol_mv)
{
	/*For battery-powered products, as the battery power will gradually drop, when the voltage is low to a certain
	  value, it will cause many problems.
		a) When the voltage is lower than operating voltage range of chip, chip can no longer guarantee stable operation.
		b) When the battery voltage is low, due to the unstable power supply, the write and erase operations
			of Flash may have the risk of error, causing the program firmware and user data to be modified abnormally,
			and eventually causing the product to fail. */
	u8 battery_check_returnValue = 0;
	if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
		battery_check_returnValue = app_battery_power_check(alarm_vol_mv+200);
	}
	else{
		battery_check_returnValue = app_battery_power_check(alarm_vol_mv);
	}
	if(battery_check_returnValue)
	{
		analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG) & (~LOW_BATT_FLG));  //clr
	}
	else
	{
		#if (UI_LED_ENABLE)  //led indicate
			for(int k=0;k<3;k++){
				gpio_write(GPIO_LED_BLUE, LED_ON_LEVEL);
				sleep_us(200000);
				gpio_write(GPIO_LED_BLUE, !LED_ON_LEVEL);
				sleep_us(200000);
			}
		#endif

		if(analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG){
			printf("[APP][BAT] The battery voltage is lower than %dmV, shut down!!!\n", (alarm_vol_mv + 200));
		} else {
			printf("[APP][BAT] The battery voltage is lower than %dmV, shut down!!!\n", alarm_vol_mv);
		}


		analog_write(USED_DEEP_ANA_REG,  analog_read(USED_DEEP_ANA_REG) | LOW_BATT_FLG);  //mark

	}
}

#endif

#if (APP_FLASH_PROTECTION_ENABLE)
/**
 * @brief      flash protection operation, including all locking & unlocking for application
 * 			   handle all flash write & erase action for this demo code. use should add more more if they have more flash operation.
 * @param[in]  flash_op_evt - flash operation event, including application layer action and stack layer action event(OTA write & erase)
 * 			   attention 1: if you have more flash write or erase action, you should should add more type and process them
 * 			   attention 2: for "end" event, no need to pay attention on op_addr_begin & op_addr_end, we set them to 0 for
 * 			   			    stack event, such as stack OTA write new firmware end event
 * @param[in]  op_addr_begin - operating flash address range begin value
 * @param[in]  op_addr_end - operating flash address range end value
 * 			   attention that, we use: [op_addr_begin, op_addr_end)
 * 			   e.g. if we write flash sector from 0x10000 to 0x20000, actual operating flash address is 0x10000 ~ 0x1FFFF
 * 			   		but we use [0x10000, 0x20000):  op_addr_begin = 0x10000, op_addr_end = 0x20000
 * @return     none
 */
_attribute_data_retention_ u16  flash_lockBlock_cmd = 0;
void app_flash_protection_operation(u8 flash_op_evt, u32 op_addr_begin, u32 op_addr_end)
{
	if(flash_op_evt == FLASH_OP_EVT_APP_INITIALIZATION)
	{
		/* ignore "op addr_begin" and "op addr_end" for initialization event
		 * must call "flash protection_init" first, will choose correct flash protection relative API according to current internal flash type in MCU */
		flash_protection_init();

		/* just sample code here, protect all flash area for old firmware and OTA new firmware.
		 * user can change this design if have other consideration */
		u32  app_lockBlock = 0xffffffff;
		#if (OTA_SERVER_ENABLE)
			u32 multiBootAddress = blc_ota_getCurrentUsedMultipleBootAddress();
			if(multiBootAddress == MULTI_BOOT_ADDR_0x10000){
				app_lockBlock = FLASH_LOCK_FW_LOW_128K;
			}
			else if(multiBootAddress == MULTI_BOOT_ADDR_0x20000){
				app_lockBlock = FLASH_LOCK_FW_LOW_256K;
			}
			else if(multiBootAddress == MULTI_BOOT_ADDR_0x40000){
				/* attention that 512K capacity flash can not lock all 512K area, should leave some upper sector
				 * for system data(SMP storage data & calibration data & MAC address) and user data
				 * will use a approximate value */
				app_lockBlock = FLASH_LOCK_FW_LOW_512K;
			}
		#else
			app_lockBlock = FLASH_LOCK_FW_LOW_256K; //just demo value, user can change this value according to application
		#endif


		flash_lockBlock_cmd = flash_change_app_lock_block_to_flash_lock_block(app_lockBlock);

		if(blc_flashProt.init_err){
			printf("[FLASH][PROT] flash protection initialization error!!!\n");
		}

		printf("[FLASH][PROT] initialization, lock flash\n");
		flash_lock(flash_lockBlock_cmd);
	}
	#if (OTA_SERVER_ENABLE)
		else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_BEGIN)
		{
			/* OTA clear old firmware begin event is triggered by stack, in "blc ota_initOtaServer_module", rebooting from a successful OTA.
			 * Software will erase whole old firmware for potential next new OTA, need unlock flash if any part of flash address from
			 * "op addr_begin" to "op addr_end" is in locking block area.
			 * In this sample code, we protect whole flash area for old and new firmware, so here we do not need judge "op addr_begin" and "op addr_end",
			 * must unlock flash */
			printf("[FLASH][PROT] OTA clear old FW begin, unlock flash\n");
			flash_unlock();
		}
		else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_END)
		{
			/* ignore "op addr_begin" and "op addr_end" for END event
			 * OTA clear old firmware end event is triggered by stack, in "blc ota_initOtaServer_module", erasing old firmware data finished.
			 * In this sample code, we need lock flash again, because we have unlocked it at the begin event of clear old firmware */
			printf("[FLASH][PROT] OTA clear old FW end, restore flash locking\n");
			flash_lock(flash_lockBlock_cmd);
		}
		else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_BEGIN)
		{
			/* OTA write new firmware begin event is triggered by stack, when receive first OTA data PDU.
			 * Software will write data to flash on new firmware area,  need unlock flash if any part of flash address from
			 * "op addr_begin" to "op addr_end" is in locking block area.
			 * In this sample code, we protect whole flash area for old and new firmware, so here we do not need judge "op addr_begin" and "op addr_end",
			 * must unlock flash */
			printf( "[FLASH][PROT] OTA write new FW begin, unlock flash\n");
			flash_unlock();
		}
		else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_END)
		{
			/* ignore "op addr_begin" and "op addr_end" for END event
			 * OTA write new firmware end event is triggered by stack, after OTA end or an OTA error happens, writing new firmware data finished.
			 * In this sample code, we need lock flash again, because we have unlocked it at the begin event of write new firmware */
			printf("[FLASH][PROT] OTA write new FW end, restore flash locking\n");
			flash_lock(flash_lockBlock_cmd);
		}
	#endif
	/* add more flash protection operation for your application if needed */
}
#endif

/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
_attribute_no_inline_ void user_init_normal(void)
{
//////////////////////////// basic hardware Initialization  Begin //////////////////////////////////

    /* random number generator must be initiated before any BLE stack initialization.
     * When deepSleep retention wakeUp, no need initialize again */
    random_generator_init();

    blc_readFlashSize_autoConfigCustomFlashSector();

    /* attention that this function must be called after "blc_readFlashSize_autoConfigCustomFlashSector" !!!*/
    blc_app_loadCustomizedParameters_normal();


    /* attention that this function must be called after "blc_app_loadCustomizedParameters_normal" !!!
       The reason is that the low battery check need the ADC calibration parameter, and this parameter
       is loaded in blc_app_loadCustomizedParameters_normal.
     */
    #if (APP_BATT_CHECK_ENABLE)
    /*The SDK must do a quick low battery detect during user initialization instead of waiting
      until the main_loop. The reason for this process is to avoid application errors that the device
      has already working at low power.
      Considering the working voltage of MCU and the working voltage of flash, if the Demo is set below 2.0V,
      the chip will alarm and deep sleep (Due to PM does not work in the current version of B92, it does not go
      into deepsleep), and once the chip is detected to be lower than 2.0V, it needs to wait until the voltage rises to 2.2V,
      the chip will resume normal operation. Consider the following points in this design:
        At 2.0V, when other modules are operated, the voltage may be pulled down and the flash will not
        work normally. Therefore, it is necessary to enter deepsleep below 2.0V to ensure that the chip no
        longer runs related modules;
        When there is a low voltage situation, need to restore to 2.2V in order to make other functions normal,
        this is to ensure that the power supply voltage is confirmed in the charge and has a certain amount of
        power, then start to restore the function can be safer.*/
        user_battery_power_check(VBAT_ALARM_THRES_MV);
    #endif


    #if (APP_FLASH_PROTECTION_ENABLE)
        app_flash_protection_operation(FLASH_OP_EVT_APP_INITIALIZATION, 0, 0);
        blc_appRegisterStackFlashOperationCallback(app_flash_protection_operation); //register flash operation callback for stack
    #endif

//////////////////////////// basic hardware Initialization  End //////////////////////////////////


//////////////////////////// 2P4G stack Initialization  Begin //////////////////////////////////
    //rf configuration
    TPLL_Init(TPLL_BITRATE_2MBPS);
    TPLL_SetOutputPower(TPLL_RF_POWER_N0p22dBm);
    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);
    TPLL_ClosePipe(TPLL_PIPE_ALL);

    #if PTX_PIPE == 0
    unsigned char tx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7}; //{0xaa,0xbb,0xcc,0xdd,0xee};
    TPLL_SetAddress(TPLL_PIPE0, tx_address);
    TPLL_OpenPipe(TPLL_PIPE0);
    TPLL_SetTXPipe(TPLL_PIPE0);
    #endif

    #if PTX_PIPE == 1
    unsigned char tx_address1[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    //    unsigned char tx_address1[5] = {0xe7, 0xe7, 0xe7, 0xe7, 0xe7};
    TPLL_SetAddress(TPLL_PIPE1, tx_address1);
    TPLL_OpenPipe(TPLL_PIPE1);
    TPLL_SetTXPipe(TPLL_PIPE1);
    #endif

    #if PTX_PIPE == 2
    unsigned char tx_address2[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address2);
    tx_address2[0] = 0x22;
    TPLL_SetAddress(TPLL_PIPE2, &tx_address2[0]);
    TPLL_OpenPipe(TPLL_PIPE2);
    TPLL_SetTXPipe(TPLL_PIPE2);
    #endif

    #if PTX_PIPE == 3
    unsigned char tx_address3[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address3);
    tx_address3[0] = 0x33;
    TPLL_SetAddress(TPLL_PIPE3, &tx_address3[0]);
    TPLL_OpenPipe(TPLL_PIPE3);
    TPLL_SetTXPipe(TPLL_PIPE3);
    #endif

    TPLL_ModeSet(TPLL_MODE_PTX);
    TPLL_SetRFChannel(7);
    TPLL_SetAutoRetry(0, 150);  //5,150
    TPLL_RxTimeoutSet(500);//if the mode is 250k ,the rx_time_out need more time, as so 1000us is ok!
    TPLL_Preamble_Set(8);
    TPLL_RxSettleSet(80);
    TPLL_TxSettleSet(149);

    #if(FAST_CALIBRATION_ENABLE)
    TPLL_Fast_SettleCalib();
    TPLL_SetHpmcCalVal(hpmc[PTX_CHANNEL]);
    TPLL_Fast_RxSettleInit(TPLL_RX_SETTLE_TIME_44US);
    TPLL_Fast_TxSettleInit(TPLL_TX_SETTLE_TIME_53US);
    TPLL_Fast_RxSettleEnable();
    TPLL_Fast_TxSettleEnable();
    TPLL_TxWaitSet(0);
    TPLL_RxWaitSet(0);
    TPLL_Fast_TxSettleSet(50);
    TPLL_Fast_RxSettleSet(50);
    TPLL_ResetBuf();
    TPLL_SetRFChannel(7);
    #endif

    //configure irq
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);

    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_TX | FLD_RF_IRQ_TX_DS | FLD_RF_IRQ_TX_RETRYCNT | FLD_RF_IRQ_RX_DR | FLD_RF_IRQ_INVALID_PID);
    irq_enable(); //enable general irq
    //////////////////////////// 2P4G stack Initialization  End //////////////////////////////////


//////////////////////////// User Configuration for 2P4G application ////////////////////////////
	///////////////////// Power Management initialization///////////////////
	#if(APP_PM_ENABLE)

	#else

	#endif
////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////// User 2P4G application /////////////////////////////////////
    preamble_len = TPLL_Preamble_Read();
    ds_flag = 1; // for first start

////////////////////////////////////////////////////////////////////////////////////////////////
}

#if (APP_PM_DEEPSLEEP_RETENTION_ENABLE)
/**
 * @brief       user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]   none
 * @return      none
 */
_attribute_ram_code_
void user_init_deepRetn(void)
{

    blc_app_loadCustomizedParameters_deepRetn();
    //////////// LinkLayer Initialization  Begin /////////////////////////

//////////////////////////// User Configuration for 2.4G application ////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif

/////////////////////////////////////////////////////////////////////s
// main loop flow
_attribute_ram_code_ void sdk_2p4g_main_loop(void)
{
    if (1 == ds_flag || 1 == maxretry_flag)
    {
        if (1 == ds_flag)
        {
            #if (UI_LED_ENABLE)
            gpio_toggle(GPIO_LED_GREEN);
            #endif
        }
        ds_flag = 0;
        maxretry_flag = 0;
        WaitMs(500);
        tx_payload[1]++;
        ret = TPLL_WriteTxPayload(PTX_PIPE, tx_payload, tx_payload_len);
        if (ret)
        {
            TPLL_PTXTrig();
        }
    }
}
/////////////////////////////////////////////////////////////////////

/**
 * @brief		This is main_loop function
 * @param[in]	none
 * @return      none
 */
_attribute_no_inline_ void main_loop(void)
{
	////////////////////////////////////// 2P4G entry /////////////////////////////////
    sdk_2p4g_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
	///////////////////////////////////// Battery Check ////////////////////////////////
	#if (APP_BATT_CHECK_ENABLE)
		/*The frequency of low battery detect is controlled by the variable lowBattDet_tick, which is executed every
		 500ms in the demo. Users can modify this time according to their needs.*/
		if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 500000) ){
			lowBattDet_tick = clock_time();
			user_battery_power_check(VBAT_ALARM_THRES_MV);
		}
	#endif
	////////////////////////////////////// PM Process /////////////////////////////////
	blt_pm_proc();
}
#endif
