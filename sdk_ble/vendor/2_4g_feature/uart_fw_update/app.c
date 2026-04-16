/********************************************************************************************************
 * @file    app.c
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
#include "battery_check.h"
#include "timer_event.h"
#include "stack/ble/ble.h"
#include "fw_update_phy.h"
#include "fw_update.h"
#if (FEATURE_TEST_MODE == UART_FW_UPDATE)
#if (UART_FW_UPDATE_ROLE == MASTER)
#define FW_UPDATE_FW_VERSION        0x0001
#elif(FEATURE_TEST_MODE == SLAVE)
#define FW_UPDATE_FW_VERSION         0x0000
#define Flash_Addr              0x08
#define Flash_Buff_Len          1
volatile unsigned char Flash_Read_Buff[Flash_Buff_Len]={0};
#endif

#define OTA_CHANNEL       70
volatile unsigned char    FW_UPDATE_Trig = 0;

volatile unsigned char uart_dmairq_tx_cnt;
volatile unsigned char uart_dmairq_rx_cnt;
volatile unsigned char uart_dmairq_err_cnt;
volatile unsigned char uart_dma_send_flag = 1;

#if(INTERNAL_TEST)
#define FW_UPDATE_TEST_TIMES      100
_attribute_data_retention_  volatile unsigned int master_test_success_cnt = 0;
_attribute_data_retention_  volatile unsigned int master_test_fail_cnt = 0;
_attribute_data_retention_  volatile unsigned int fw_update_test_cnt = 0;
#endif
_attribute_ram_code_  void irq_2p4g_sdk_handler(void)
{
    #if(MCU_CORE_B80B)
    //1. UART irq
    if(dma_chn_irq_status_get(FLD_DMA_CHN_UART_RX))
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
        FW_UPDATE_PHY_RxIrqHandler();
    }
    if(dma_chn_irq_status_get(FLD_DMA_CHN_UART_TX))
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);

        FW_UPDATE_PHY_TxIrqHandler();
    }
    #elif(MCU_CORE_B80)
    unsigned char uart_dma_irqsrc= dma_chn_irq_status_get();
    //1. UART irq
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_RX)
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
        FW_UPDATE_PHY_RxIrqHandler();
    }
    if(uart_dma_irqsrc & FLD_DMA_CHN_UART_TX)
    {
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);

        FW_UPDATE_PHY_TxIrqHandler();
    }
    #endif

    irq_clr_src();
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

_attribute_data_retention_  u32 lowBattDet_tick   = 0;

/**
 * @brief       this function is used to process battery power.
 *              The low voltage protection threshold 2.0V is an example and reference value. Customers should
 *              evaluate and modify these thresholds according to the actual situation. If users have unreasonable designs
 *              in the hardware circuit, which leads to a decrease in the stability of the power supply network, the
 *              safety thresholds must be increased as appropriate.
 * @param[in]   none
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
 *             handle all flash write & erase action for this demo code. use should add more more if they have more flash operation.
 * @param[in]  flash_op_evt - flash operation event, including application layer action and stack layer action event(OTA write & erase)
 *             attention 1: if you have more flash write or erase action, you should should add more type and process them
 *             attention 2: for "end" event, no need to pay attention on op_addr_begin & op_addr_end, we set them to 0 for
 *                          stack event, such as stack OTA write new firmware end event
 * @param[in]  op_addr_begin - operating flash address range begin value
 * @param[in]  op_addr_end - operating flash address range end value
 *             attention that, we use: [op_addr_begin, op_addr_end)
 *             e.g. if we write flash sector from 0x10000 to 0x20000, actual operating flash address is 0x10000 ~ 0x1FFFF
 *                  but we use [0x10000, 0x20000):  op_addr_begin = 0x10000, op_addr_end = 0x20000
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

//      printf("[FLASH][PROT] initialization, lock flash\n");
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



//////////////////////////// 2P4G stack Initialization  End //////////////////////////////////


//////////////////////////// User Configuration for 2P4G application ////////////////////////////

    ////////////////// OTA relative ////////////////////////
    #if(UART_FW_UPDATE_ROLE == SLAVE)
        flash_read_page(Flash_Addr,Flash_Buff_Len, (unsigned char *)Flash_Read_Buff);
          if (Flash_Read_Buff[0] == 0x4b)
          {
            printf("[OTA][FLW] slave start at 0 address\n");
          }
          else
          {
            printf("[OTA][FLW] slave start at %4x address\n",FW_UPDATE_SLAVE_BIN_ADDR);
          }
    #endif

    #if(INTERNAL_TEST)
          gpio_set_func(GPIO_PC7 | GPIO_PC6,AS_GPIO);
      #if(UART_FW_UPDATE_ROLE == MASTER)
          gpio_set_output_en(GPIO_PC7 | GPIO_PC6, 1);        //enable output
          gpio_set_input_en(GPIO_PC7 | GPIO_PC6 ,0);         //disable input
          gpio_write(GPIO_PC7,1);
      #else
          gpio_set_output_en(GPIO_PC7 | GPIO_PC6, 0);        //enable output
          gpio_set_input_en(GPIO_PC7 | GPIO_PC6 ,1);         //disable input
          gpio_write(GPIO_PC7,0);
      #endif
          gpio_write(GPIO_PC6,0);
    #endif
	///////////////////// Power Management initialization///////////////////
	#if(APP_PM_ENABLE)

	#else

	#endif

////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// User 2P4G application /////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
}

/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void user_init_deepRetn(void)
{

  blc_app_loadCustomizedParameters_deepRetn();
  //////////// LinkLayer Initialization  Begin /////////////////////////

//////////////////////////// User Configuration for application ////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////

    #if(INTERNAL_TEST)
    gpio_set_func(GPIO_PC7 | GPIO_PC6,AS_GPIO);
    #if(UART_FW_UPDATE_ROLE == MASTER)
    gpio_set_output_en(GPIO_PC7 | GPIO_PC6, 1);        //enable output
    gpio_set_input_en(GPIO_PC7 | GPIO_PC6 ,0);         //disable input
    gpio_write(GPIO_PC7,1);
    #else
    gpio_set_output_en(GPIO_PC7 | GPIO_PC6, 0);        //enable output
    gpio_set_input_en(GPIO_PC7 | GPIO_PC6 ,1);         //disable input
    gpio_write(GPIO_PC7,0);
    #endif
    gpio_write(GPIO_PC6,0);
    #if(UART_FW_UPDATE_ROLE == MASTER)
    if(fw_update_test_cnt >= FW_UPDATE_TEST_TIMES){
      printf("[OTA][FLW] master fw update %d times,success %d times,fail %d times\n",fw_update_test_cnt,master_test_success_cnt,master_test_fail_cnt);
      printf("[OTA][FLW] master fw update test end!\n");
      gpio_write(GPIO_PC6,1);
      while(1);
    }
    #endif
    #endif
}

/////////////////////////////////////////////////////////////////////
// main loop flow
_attribute_ram_code_ void sdk_2p4g_main_loop(void)
{
#if(INTERNAL_TEST)
  #if(UART_FW_UPDATE_ROLE == MASTER)
    if ((fw_update_test_cnt < FW_UPDATE_TEST_TIMES))
    {
        printf("[OTA][FLW] master fw update %d times,success %d times,fail %d times\n",fw_update_test_cnt,master_test_success_cnt,master_test_fail_cnt);
        fw_update_test_cnt++;
        gpio_write(GPIO_PC7,1);
        WaitMs(1000);
  #else
    if((gpio_read(GPIO_PC6) == 1)){
        printf("[OTA][FLW] slave fw update test end!\n");
        while(1);
    }else if(gpio_read(GPIO_PC7) == 1){
  #endif

#else
  if (FW_UPDATE_Trig)
  {
#endif

#if (APP_BATT_CHECK_ENABLE)
    user_battery_power_check(VBAT_ALARM_THRES_MV);
#endif

#if (APP_FLASH_PROTECTION_ENABLE)
    flash_prot_op_cb(FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_BEGIN, 0, 0);
#endif
    FW_UPDATE_Trig = 0;
    FW_UPDATE_PHY_Init(FW_UPDATE_RxIrq);
#if (UART_FW_UPDATE_ROLE == MASTER)
    printf("[OTA][FLW] master FW UPDATE START!\n");
    FW_UPDATE_MasterInit(FW_UPDATE_MASTER_BIN_ADDR, FW_UPDATE_FW_VERSION);
#elif(UART_FW_UPDATE_ROLE == SLAVE)
    printf("[OTA][FLW] slave FW UPDATE START!\n");
      flash_read_page(Flash_Addr,Flash_Buff_Len, (unsigned char *)Flash_Read_Buff);
      if (Flash_Read_Buff[0] == 0x4b)
      {
          FW_UPDATE_SlaveInit(FW_UPDATE_SLAVE_BIN_ADDR, FW_UPDATE_FW_VERSION);
      }
      else
      {
          FW_UPDATE_SlaveInit(0, FW_UPDATE_FW_VERSION);
      }
#endif
#if (UI_LED_ENABLE)
      gpio_write(GPIO_LED_BLUE, 1);
      WaitMs(80);
      gpio_write(GPIO_LED_BLUE, 0);
      WaitMs(80);
      gpio_write(GPIO_LED_BLUE, 1);
      WaitMs(80);
      gpio_write(GPIO_LED_BLUE, 0);
#endif
      while (1)
      {
#if (UART_FW_UPDATE_ROLE == MASTER)
          FW_UPDATE_MasterStart();
#elif(UART_FW_UPDATE_ROLE == SLAVE)
          FW_UPDATE_SlaveStart();
#endif
          ev_process_timer();
      }
  }
#if (UI_LED_ENABLE)
#if (UART_FW_UPDATE_ROLE == MASTER)
  printf("[OTA][FLW] master mainloop\n");
#else
  printf("[OTA][FLW] slave mainloop\n");
#endif
  gpio_toggle(GPIO_LED_GREEN);
#endif
  WaitMs(200);

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

	///////////////////////////////////// Battery Check ////////////////////////////////
	#if (APP_BATT_CHECK_ENABLE)
		/*The frequency of low battery detect is controlled by the variable lowBattDet_tick, which is executed every
		 500ms in the demo. Users can modify this time according to their needs.*/
		if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 500000) ){
			lowBattDet_tick = clock_time();
			user_battery_power_check(VBAT_ALARM_THRES_MV);
		}
	#endif
    ////////////////////////////////////// UI entry /////////////////////////////////
    #if (UI_BUTTON_ENABLE)
        static u8 button_detect_en = 0;
        /* process button 1 second later after power on, to avoid power unstable */
        if(!button_detect_en && clock_time_exceed(0, 1000000)){
            button_detect_en = 1;
        }
        static u32 button_detect_tick = 0;
        if(button_detect_en && clock_time_exceed(button_detect_tick, 5000))
        {
            button_detect_tick = clock_time();
            proc_button(0, 0, 0);  //button triggers OTA
        }
    #endif

    ////////////////////////////////////// PM Process /////////////////////////////////
        blt_pm_proc();

}

#endif
