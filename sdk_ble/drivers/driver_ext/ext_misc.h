/********************************************************************************************************
 * @file    ext_misc.h
 *
 * @brief   This is the header file for BLE SDK
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
#ifndef DRIVERS_8208_EXT_MISC_H_
#define DRIVERS_8208_EXT_MISC_H_
#include "../register.h"
#include "compiler.h"
#include "../driver.h"
/******************************* rf start **********************************************************************/
/**
 * @brief     This function serves to set BLE mode of RF.
 * @return	  none.
 */
typedef void (*rf_1m_init_t)(void);
extern rf_1m_init_t rf_ble_1m_param_init; ///This function serves to set RF  ble 1m Register initialization.
#define rf_drv_ble_init rf_ble_1m_param_init

void rf_ble_state_reset(void);

void rf_2_4g_state_reset(void);
/******************************* rf end  **********************************************************************/


/******************************* watchdog_start ***********************************************************************/

#define WATCHDOG_TIMEOUT_COEFF	18		//  check register definition, 0x622

#define WATCHDOG_DISABLE	( reg_tmr_ctrl &= ~FLD_TMR_WD_EN )

/******************************** watchdog_end  **********************************************************************/


/******************************* stimer_start ******************************************************************/

/**
 * @brief   system Timer : 16Mhz, Constant
 */
enum{
	SYSTEM_TIMER_TICK_1US 		= 16,
	SYSTEM_TIMER_TICK_1MS 		= 16000,
	SYSTEM_TIMER_TICK_1S 		= 16000000,

	SYSTEM_TIMER_TICK_625US  	= 10000,  //625*16
	SYSTEM_TIMER_TICK_1250US 	= 20000,  //1250*16
};


#define reg_system_tick_irq reg_system_tick_irq_level

/******************************* stimer_end ********************************************************************/



/******************************* pm_start ******************************************************************/

/**
 * @brief      This function serves to set the working mode of MCU based on 32k rc,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * 				  This saves size of RAM code but increases operating time and average power consumption.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int  cpu_sleep_wakeup_32k_rc_text(SleepMode_TypeDef sleep_mode,  SleepWakeupSrc_TypeDef wakeup_src, pm_wakeup_tick_type_e wakeup_tick_type, unsigned int  wakeup_tick);

/**
 * @brief      This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 *  			  This saves size of RAM code but increases operating time and average power consumption.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int  cpu_sleep_wakeup_32k_xtal_text(SleepMode_TypeDef sleep_mode,  SleepWakeupSrc_TypeDef wakeup_src, pm_wakeup_tick_type_e wakeup_tick_type, unsigned int  wakeup_tick);


/**
 * @brief      This function serves to determine whether wake up source is internal 32k RC.This saves size of RAM code but increases operating time and average power consumption.
 * @param[in]  none.
 * @return     none.
 */
static inline void blc_pm_select_internal_32k_crystal_save_ram(void)
{
	cpu_sleep_wakeup_and_longsleep 	= cpu_sleep_wakeup_32k_rc_text;
	pm_tim_recover  	 			= pm_tim_recover_32k_rc;
	blt_miscParam.pm_enter_en 		= 1; // allow enter pm, 32k rc does not need to wait for 32k clk to be stable
}

/**
 * @brief      This function serves to determine whether wake up source is external 32k crystal.This saves size of RAM code but increases operating time and average power consumption.
 * @param[in]  none.
 * @return     none.
 */
static inline void blc_pm_select_external_32k_crystal_save_ram(void)
{
	cpu_sleep_wakeup_and_longsleep 	= cpu_sleep_wakeup_32k_xtal_text;
	pm_check_32k_clk_stable 		= check_32k_clk_stable;
	pm_tim_recover		 			= pm_tim_recover_32k_xtal;
	blt_miscParam.pad32k_en 		= 1; // set '1': 32k clk src use external 32k crystal
}
/******************************* pm_end ******************************************************************/



/******************************* dma_start ******************************************************************/

/**
 * @brief	ACL RX Data buffer length = maxRxOct + 21, then 16 Byte align
 *			maxRxOct + 21 = 4(DMA_len) + 2(BLE header) + maxRxOct + 4(MIC) + 3(CRC) + 8(ExtraInfo)
			RX buffer size must be be 16*n, due to MCU design
 */
//actually +21.The purpose of +22 is to deal with extreme situations. Due to DMA design,at least one byte buffer can not be unusable.

#define		TLK_RF_RX_EXT_LEN		(22)	//4(DMA_len) + 2(BLE header) + ISORxOct + 4(MIC) + 3(CRC) + 8(ExtraInfo)
#define 	CAL_LL_ACL_RX_FIFO_SIZE(maxRxOct)	(((maxRxOct + TLK_RF_RX_EXT_LEN) + 15) / 16 *16)


/**
 * @brief	ACL TX Data buffer length = maxTxOct + 10, then 4 Byte align
 *			maxTxOct + 10 = 4(DMA_len) + 2(BLE header) + maxTxOct + 4(MIC)
			TX buffer size must be be 4*n, due to MCU design
 */

#define     TLK_RF_TX_EXT_LEN		(10)	//10 = 4(DMA_len) + 2(BLE header) + 4(MIC)
#define 	CAL_LL_ACL_TX_FIFO_SIZE(maxTxOct)	(((maxTxOct + TLK_RF_TX_EXT_LEN) + 3) / 4 *4)

/******************************* dma_end ******************************************************************/

/******************************* random start ******************************************************************/
/**
 * @brief      This function performs to get a serial of random number.
 * @param[in]  len- the length of random number
 * @param[in]  data - the first address of buffer store random number in
 * @return     the result of a serial of random number..
 */
void generateRandomNum(int len, unsigned char *data);
/******************************* random end ******************************************************************/
#endif
