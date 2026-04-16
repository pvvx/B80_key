/*
 * otp.c
 *
 *  Created on: 13 апр. 2026 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#if 0
#define OTP_PCE_AUTO_MODE_EN				 1/**< 0: otp_pce_auto_mode is close,set to 0 only for internal debugging*/
											  /**< 1:otp_pce_auto_mode is open */

//delay times in spec
#define Tms			1        //ptm  -->  pce = 1,   Tms>1(ns)
#define Tmh			1        //ptm  -->  pce = 0,   Tmh>1(ns)
#define Tcsp		20       //pce = 1 ---> pprog=1,  10 < Tcsp < 100(us)
#define Tpps		10       //pprog=1 ---> pwe=1,  5 < Tpps < 20(us)
#define Tpw			15       //pwe high time,  10 < Tpw < 20(us)
#define Tpwi		4        //pwe high time,  1 < Tpwi < 5(us)
#define Tpph		10       //pwe=0 ---> pprog=0,  5 < Tpph < 20(us)
#define Tcs			15       //pwe=1 ---> enable,  Tcs > 10(us)
#define Tpls        15       //ldo setup time,Tpls > 10(us)
#define Tsas        5        //deep standby to active mode setup time, Tsas >2(us)

typedef enum{
    OTP_PTM_PROG = 0x02,
    OTP_PTM_READ = 0x00,
    OTP_PTM_INIT_MARGIN_READ = 0x01,
    OTP_PTM_PGM_MARGIN_READ = 0x04,
}OTP_PtmTypeDef;
#endif

#if (MCU_CORE_B80)
/* =0 Indicates the program is not an OTP program */
extern unsigned char otp_program_flag;
extern u8 g_irq_en;
extern u8 g_pce_timeout;
/**
 * @brief     This function servers to waiting for pce timeout.
 * @param[in] none
 * @return	  none.
 */
void otp_pce_timeout_exceed(void) {
	g_irq_en = reg_irq_en;
	reg_irq_en = 0;
	int timeout = g_pce_timeout; // = 6
	timeout <<= 4; // * 16
	u32 tt = clock_time();
	while(clock_time() - tt < timeout && (reg_otp_ctrl0 & FLD_OTP_PCE) == 0);
}
/**
 * @brief      This function serves to otp set deep standby mode,if code run in flash,otp is idle,can enter deep to save current.
 * @param[in]  none
 * @return     none
 */
void otp_set_deep_standby_mode(void) {
	reg_otp_ctrl0 &=(~FLD_OTP_PCE);
//	reg_otp_ctrl1 &=(~FLD_OTP_PDSTD);
//	reg_otp_ctrl1 &=(~FLD_OTP_PLDO);
	reg_otp_ctrl1 &=(~FLD_OTP_PLDO | FLD_OTP_PDSTD);
}
/**
 * @brief      This function serves to read data from OTP memory.
 * @param[in]  ptm_mode - read mode.
 * @param[in]  addr     - the address of the data,the otp memory that can access is from 0x0000-0x7ffc,can't access other address.
 * @param[in]  word_len - the length of the data,the unit is word(4 bytes).
 * @param[in]  buff     - data buff.
 * @return     none
 */
void otp_read_cycle(OTP_PtmTypeDef ptm_mode,unsigned int addr, unsigned int word_len, unsigned int *buff) {
	otp_start(ptm_mode);
	sleep_us(15);
	u8 state = ~reg_otp_dat;
	while(word_len) {
		reg_otp_pa = addr;
		(void)reg_otp_rd_dat;
		while(reg_otp_dat & state);
		buff[0] = reg_otp_rd_dat;
		buff++;
		word_len--;
		while(reg_otp_dat & state);
	}
	reg_otp_ctrl0 &= ~FLD_OTP_PCE;
}
/**
 * @brief     This function is a common sequence used by these interfaces:otp_write32/otp_read_cycle/otp_set_active_mode.
 * @param[in] ptm_mode - ptm type.
 * @return    none
 */
void otp_start(OTP_PtmTypeDef ptm_mode) {
	if(!otp_program_flag)
		reg_otp_ctrl0 &= ~FLD_OTP_PCE;
	reg_otp_ctrl1 = (reg_otp_ctrl1 & (~FLD_OTP_PTM)) | ptm_mode;
	reg_otp_ctrl0 |= FLD_OTP_PCE;
	if(otp_program_flag)
		reg_otp_ctrl0 |= FLD_OTP_PCLK_EN;
}
/**
 * @brief      This function serves to preparations after otp software operation.
 * @param[in]  pce_flag - pce auto mode flag,from the return value of otp_auto_pce_disable function.
 * @return     none
 */
void otp_auto_pce_restore() {
	if(otp_program_flag) {
		reg_otp_ctrl1 &= (~FLD_OTP_PTM)
		reg_otp_ctrl1 = reg_otp_ctrl1; // ???
		reg_otp_ctrl0 |= FLD_OTP_PCE;
		sleep_us(15);
	}
	reg_irq_en = g_irq_en;
}
/**
 * @brief      This function serves to otp set active mode,if otp is in deep mode,need to operate on the otp,set active mode.
 * @param[in]  none
 * @return     none
 */
void otp_set_active_mode(void) {
	reg_otp_ctrl1 |= FLD_OTP_PLDO; //enable LDO to generate 2.5V power
	int i = 100;
	while(i--) _ASM_NOP_;
	reg_otp_ctrl1 |= FLD_OTP_PDSTD; // pin for deep standby mode,low active.
	i = 35;
	while(i--) _ASM_NOP_;
	if(otp_program_flag) {
		reg_otp_ctrl0  |= FLD_OTP_PCE; // IP enabling
		i = 100;
		while(i--) _ASM_NOP_;
	}
}
/**
 * @brief      This function serves to read data from OTP memory,belong to otp normal read.
 *             otp has three kinds of read mode,in general,just use OTP_READ normal read operation, when the execution of burning operation,
 *             need to use margin read(otp_pgm_margin_read,otp_initial_margin_read),check whether the write is successful.
 * @param[in]  addr     - the otp address of the data,it has to be a multiple of 4,the otp memory that can access is from 0x0000-0x3ffc,can't access other address.
 * @param[in]  word_len - the length of the data,the unit is word(4 bytes).
 * @param[in]  buff     - data buff.
 * @return     none
 *
 * Attention:
 * -# When the vbat voltage is greater than 3.3V, otp supply is 3.3V, if the vabt voltage is lower than 3.3V,
 *    then the otp supply voltage will follow the vbat voltage value, write to otp, according to the datasheet,
 *    the voltage value is at least 1.62V, if below the voltage value, you can not operate,and prompt adc sampling voltage has certain error,add by shuaixing.zhai, confirmed by baoyi 20211015.
 * -# Chip version A0/A1/A2/A3 can't read/write OTP in RF RX state, A4 chip doesn't have this limitation.
 *    The chip version number can be read in the following way: after calling cpu_wakeup_init(), read the value of the g_chip_version variable,
 *    which corresponds to the chip version as follows:
 *    A0: g_chip_version = 0xff
 *    A1: g_chip_version = 0xfe
 *    A2: g_chip_version = 0xfc
 *    A3: g_chip_version = 0xf8
 *    A4: g_chip_version = 0xfa
 */
void otp_read(unsigned int addr, unsigned int word_len, unsigned int *buff) {
	otp_pce_timeout_exceed();
	otp_read_cycle(OTP_PTM_READ, addr, word_len, buff);
	otp_auto_pce_restore();
}
#endif // MCU_CORE_B80
