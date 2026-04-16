/********************************************************************************************************
 * @file    cstartup_flash.S
 *
 * @brief   This is the boot file for BLE SDK
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
#ifdef MCU_STARTUP_FLASH
@********************************************************************************************************
@                                   SRAM CONFIGURATION
@********************************************************************************************************
#define SRAM_BASE_ADDR		(0x840000)
#define SRAM_16K      		(0x4000)	//16KSRAM
#define SRAM_20K      		(0x5000)	//20KSRAM

#if MCU_STARTUP_B80
#if SWS_PRINTF_MODE
#define	SRAM_SIZE			(SRAM_BASE_ADDR + SRAM_16K - 256)
#else
#define	SRAM_SIZE			(SRAM_BASE_ADDR + SRAM_16K)
#endif
#elif MCU_STARTUP_B80B
#define	SRAM_SIZE			(SRAM_BASE_ADDR + SRAM_20K)
#endif
@********************************************************************************************************
@                                   BOOT FEATURE CONFIGURATION
@********************************************************************************************************
#define  FLL_STK_EN           			0
#define  GPIO_DEBUG_EN           		0
#define  ZERO_IC_TAG_EN		  			1
#define  IC_TAG_CACHE_ADDR_EQU_EN    	0
@The design of the B80 chip is to power off the flash when the chip goes to sleep,
@and then power on the flash again after waking up, so there is no need to perform the flash wake-up action.
#define  FLASH_WAKEUP_EN                0
#define  WD_SET_AND_START_EN            0 /* default 0; 1: Set and Start the watchdog in advance. */

#ifndef __LOAD_RAM_SIZE__
#define	__LOAD_RAM_SIZE__		0xc
#endif

#ifndef __IRQ_STK_SIZE__
#define	__IRQ_STK_SIZE__		0x180
#endif
	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************
					@ Mode, correspords to bits 0-5 in CPSR
	.equ MODE_BITS,		0x1F	@ Bit mask for mode bits in CPSR
	.equ IRQ_MODE, 		0x12	@ Interrupt Request mode
	.equ SVC_MODE, 		0x13	@ Supervisor mode 
	.equ IRQ_STK_SIZE,	__IRQ_STK_SIZE__
	.equ __LOAD_FLASH, 	1
	.equ __LOAD_DUT, 	0
	.equ __LOAD_RAM, 	0
	.equ __SRAM_SIZE,   SRAM_SIZE
@********************************************************************************************************
@                                            TC32 EXCEPTION VECTORS
@********************************************************************************************************
	.section	.vectors,"ax"
	.global		__reset
	.global	 	__irq
	.global 	__start
	.global		__LOAD_RAM
	.global		__LOAD_DUT
	.global		__LOAD_FLASH
	.global		__SRAM_SIZE
	.global	_ramcode_size_
	.global	_ram_use_end_

__start:					@ MUST,  referenced by boot.link

	.extern irq_handler

	.extern firmwareVersion

	.extern  _data_end_div_16_align_256_
	.extern  _start_text_
	.extern  _ictag_start_
	.extern  _ictag_end_
	.org 0x0
	tj	__reset
@	.word	(firmwareVersion)
	.org 0x8
	.word	(0x544c4e4b)
	@Boot will write the value here to the digital register 0x602, mainly the difference of 0x602[4],
	@write 1 to fetch instructions from otp, and write 0 to fetch instructions from flash;
	.word	(0x00880000 + _data_end_div_16_align_256_)

	.org 0x10
	tj		__irq
	.org 0x18
	.word	(_bin_size_)
	.extern  main

	.org 0x20

	.global	start_suspend
	.thumb_func
	.type	start_suspend, %function

start_suspend:						@ instruction cache address + 0x58: 0x3c16 * 4 = 0xf058
	tpush   {r2-r3}

    tmovs	r2, #129				@0x81
    tloadr	r3, __suspend_data      @0x80006f
    tstorerb	r2, [r3, #0]		@*(volatile unsigned char *)0x80006f = 0x81

    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8
    tmov	r8, r8

    tpop	{r2-r3}
    tjex	lr
	.align 4
__suspend_data:
	.word   (0x80006f)

__reset:
#if WD_SET_AND_START_EN
    @ reference wd_set_interval_ms
    tloadr      r1, WD_REG_BASE    @0x800620
    tloadr      r0, WD_REG_BASE+8  @  default 0x5b
    tshftl      r0, r0,#1          @  (WD_REG_BASE+8)<<1
    tstorerb    r0, [r1, #1]       @ *(unsigned char*)(0x800621)= (WD_REG_BASE+8)<<1
    tshftr      r0, r0,#8          @  (WD_REG_BASE+8)>>8
    tstorerb    r0, [r1, #2]       @ *(unsigned char*)(0x800622)=(WD_REG_BASE+8)>>8

    @ reference wd_start
    tloadr      r1, WD_REG_BASE    @0x800620
    tmov        r0, #0x40          @ 01000000
    tstorerb    r0, [r1, #0]       @ *(unsigned char*)(0x800620)=0x60,reg_tmr_ctrl8[6]=1

    tloadr      r1, WD_REG_BASE+4  @0x800622
    tloadrb     r2, [r1]
    tmov        r0, #0x80          @ 10000000
    tor         r0, r2
    tstorerb    r0, [r1, #0]       @ reg_tmr_ctrl8 |=reg_tmr_ctrl8|0x80
#endif

#if (GPIO_DEBUG_EN)
	#if (MCU_STARTUP_B80)
		@ add debug, PB4 output 1
		tloadr     	r1, DEBUG_GPIO    @0x80058a  PB oen
		tmov		r0, #139      @0b 11101111
		tstorerb	r0, [r1, #0]

		tmov		r0, #16			@0b 00010000
		tstorerb	r0, [r1, #1]	@0x800583  PB output

	#elif (MCU_STARTUP_B80B)
		@ add debug, PB4 output 1
		tloadr     	r1, DEBUG_GPIO_OEN    @0x80050a  PB oen
		tmov		r0, #239		@0b 11101111
		tstorerb	r0, [r1, #0]

		tloadr     	r1, DEBUG_GPIO_OUTPUT
		tmov		r0, #16			@0b 00010000
		tstorerb	r0, [r1, #0]	@0x8005c4  PB output set
	#endif
#endif

	@B80_B is handled in the boot program.
#if (MCU_STARTUP_B80)
	@when running the flash program, let the otp enter the deep standby mode to reduce the power consumption of the chip.
	tloadr		r3, OTP_CTRL	  @0x800010	otp ctrl
	tloadrb		r1, [r3, #0]
	tmovs		r0, #253		  @0b 11111101
	tand		r1, r0
 	tstorerb	r1, [r3, #0]	  @pce = 0

	tloadrb		r1, [r3, #1]
	tmovs		r0, #207		  @0b 11001111 mc icct1
	tand		r1, r0
 	tstorerb	r1, [r3, #1]	  @pdstb = 0  pldo = 0
#endif

@The design of the B80 chip is to power off the flash when the chip goes to sleep,
@and then power on the flash again after waking up, so there is no need to perform the flash wake-up action.
#if FLASH_WAKEUP_EN

FLASH_WAKEUP_BEGIN:
	tloadr      r0,DATA_I+24			@Adjust number from 44 to 24, by BLE SUNWEI
	tmov		r1,#0
	tstorerb    r1,[r0,#1]
	tmov        r1,#171						@Flash deep cmd: 0xAB
	tstorerb    r1,[r0,#0]
	tmov		r2,#0
	tmov        r3,#6
TNOP:
	tadd        r2,#1
	tcmp        r2,r3
	tjle        TNOP
	tmov		r1,#1
	tstorerb    r1,[r0,#1]
FLASH_WAKEUP_END:

#endif
@********************************************************************************************************
@                              		 UPDATE SP UNDER IRQ/SVC MODE
@********************************************************************************************************
	tloadr	r0, DAT0						@r0 = 0x12 IRQ
	tmcsr	r0								@CPSR=r0
	tloadr	r0, DAT0 + 8					@r0 = irq_stk + IRQ_STK_SIZE
	tmov	r13, r0  						@r13/SP= r0    	update SP under IRQ mode

	tloadr	r0, DAT0 + 4					@r0 = 0x13 SVC
	tmcsr	r0								@CPSR=r0
	tloadr	r0, DAT0 + 12					@r0 = 0x850000
	tmov	r13, r0  						@r13= r0		update SP under SVC mode

@********************************************************************************************************
@                                    IC TAG INITIALIZATION
@********************************************************************************************************
#if ZERO_IC_TAG_EN
ZERO_TAG:
	tmov    r0, #0
	tloadr	r1, DAT0 + 28					@r1 = _ictag_start_
	tloadr	r2, DAT0 + 32					@r2 = _ictag_end_
ZERO_TAG_BEGIN:
	tcmp	r1, r2
	tjge	ZERO_TAG_END					@r1>=r2 jump to ZERO_TAG_END
	tstorer	r0, [r1, #0]					@*(unsigned int*)(_ictag_start_)=r0=0
	tadd    r1, #4							@r1 = r1 + 4
	tj		ZERO_TAG_BEGIN					@jump to ZERO_TAG_BEGIN
ZERO_TAG_END:
#endif
@********************************************************************************************************
@                                    IC CACHE INITIALIZATION
@********************************************************************************************************
SET_IC:
	tloadr     	r1, DAT0 + 24				@ r1 = 0x80060c
	tloadr      r0, DAT0 + 36				@ r0 = _start_text_
	tshftr      r0,r0,#8                    @ r0 = _start_text_/256
	tstorerb	r0, [r1, #0]				@ *(unsigned int*)(0x80060c) = r0
#if IC_TAG_CACHE_ADDR_EQU_EN

#else
	tadd    	r0, #1						@ r0 = r0 + 1
#endif
	tstorerb	r0, [r1, #1]				@ *(unsigned int*)(0x80060d) = r0
SET_IC_END:
@********************************************************************************************************
@                                    DCDC INITIALIZATION FOR FLASH
@********************************************************************************************************
SET_BOOT:
	tmov        r2, #5
	tloadrb     r1, [r2]		@read form core_840005
	tmov		r0, #240		@0xf0
	tand		r1, r0			@(84005)&0xf0 == 0xA0,from deepret
	tmov     	r0, #160    	@A0
	tcmp        r0, r1			@(840004) == A0?
	tjne		SET_BOOT_END	@wakeup from not from retention or wakeup.

	tmov        r2, #4
	tloadrb     r1, [r2]		@read form core_840004
	tloadr     	r0, BOOT_SEL_D
	tstorerb	r1, [r0, #0]
	tmov        r2, #5
	tloadrb     r1, [r2]		@read form core_840005
@	tloadr     	r0, BOOT_SEL_D+4
	tstorerb	r1, [r0, #1]
	tj			COPY_DATA_RELOAD

@WHILE:				@not be necessary, delete it, by BLE SUNWEI
@	tj	WHILE			@not be necessary, delete it, by BLE SUNWEI

SET_BOOT_END:
@********************************************************************************************************
@                              		 FILL .DATA AND .BSS WITH 0xFF
@********************************************************************************************************
#if FLL_STK_EN

	tloadr	r0, FLL_D						@r0 = 0xffffffff
	tloadr	r1, FLL_D+4						@r1 = _start_bss_
	tloadr	r2, FLL_D+8						@r2 = 0x850000

FLL_STK:
	tcmp	r1, r2							@
	tjge	FLL_STK_END						@r1>=r2 jump to FLL_STK_END
	tstorer r0, [r1, #0]					@*(unsigned int*)(_start_data_)=0xffffffff
	tadd    r1, #4							@r1 = r1+4
	tj		FLL_STK							@jump to FLL_STK
FLL_STK_END:
#endif
@********************************************************************************************************
@                                    .BSS INITIALIZATION FOR 0
@********************************************************************************************************
	tmov	r0, #0							@r0 = 0
	tloadr	r1, DAT0 + 16					@r1 = _start_bss_
	tloadr	r2, DAT0 + 20					@r2 = _end_bss_

ZERO_BSS_BEGIN:
	tcmp	r1, r2
	tjge	ZERO_BSS_END					@r1>=r2 jump to ZERO_BSS_END
	tstorer	r0, [r1, #0]					@*(unsigned int*)(_start_bss_)=r0=0
	tadd    r1, #4							@r1 = r1 + 4
	tj		ZERO_BSS_BEGIN					@jump to ZERO_BSS_BEGIN
ZERO_BSS_END:
@********************************************************************************************************
@                                    DATA INITIALIZATION
@********************************************************************************************************
COPY_DATA_RELOAD:
	tloadr		r1, DATA_I+12				@ r1 = _rstored_
	tloadr		r2, DATA_I+16				@ r2 = _data_reload_start_
	tloadr		r3, DATA_I+20				@ r3 = _data_reload_end_
COPY_DATA_RELOAD_BEGIN:
	tcmp		r2, r3						@
	tjge		COPY_DATA_RELOAD_END		@ r2>=r3 jump to COPY_DATA_RELOAD_END
	tloadr		r0, [r1, #0]				@ r0 = *(unsigned int*)(_rstored_)
	tstorer 	r0, [r2, #0]				@ *(unsigned int*)(_data_reload_start_) = r0
	tadd    	r1, #4						@ r1 = r1 + 4
	tadd		r2, #4						@ r2 = r2 + 4
	tj			COPY_DATA_RELOAD_BEGIN		@ jump to COPY_DATA_RELOAD_BEGIN
COPY_DATA_RELOAD_END:

ENTER_MAIN:
	tjl	main
END:
	tj	END

	.balign	4
DAT0:
	.word	0x12			    		@0		IRQ
	.word	0x13			    		@4		SVC
	.word	(irq_stk + IRQ_STK_SIZE)	@8
	.word	(SRAM_SIZE)		    		@12  	stack end
	.word	(_start_bss_)               @16
	.word	(_end_bss_)                 @20
	.word	(0x80060c)                  @24
	.word	_ictag_start_               @28		IC tag start
	.word	_ictag_end_	            	@32		IC tag end
#if IC_TAG_CACHE_ADDR_EQU_EN
	.word	(0x00f000)    				@36
#endif
	.word	_start_text_    			@40

DATA_I:	
	.word   (_dstored_)                 @0
	.word	_start_data_				@4
	.word	_end_data_					@8
	.word   (_rstored_)                 @12
	.word	_data_reload_start_			@16
	.word	_data_reload_end_			@20
#if FLASH_WAKEUP_EN
	.word	(0x0080000c)                @24 not @44
#endif

#if (GPIO_DEBUG_EN)
#if (MCU_STARTUP_B80)
DEBUG_GPIO:
	.word	(0x80050a)                  @  PBx oen
#elif (MCU_STARTUP_B80B)
DEBUG_GPIO_OEN:
	.word	(0x80050a)                  @  PBx oen

DEBUG_GPIO_OUTPUT:
	.word	(0x8005c4)                  @  PBx output set
#endif
#endif

#if WD_SET_AND_START_EN
WD_REG_BASE:
    .word   (0x800620)                  @0  reg_tmr_ctrl
    .word   (0x800622)                  @4  reg_wd_ctrl1
@ capt tick=500ms=(12000000>>18)=0x2d, capt tick = 1s =(24000000>>18) =0x5b,
@ capt tick=  2s =(48000000>>18)=0xb7, capt tick = 3s =(72000000>>18) =0x112,
@ capt tick=  4s =(96000000>>18)=0x16e,capt tick = 5s =(120000000>>18)=0x1c9,
    .word   (0x5b)                      @8  capt tick
#endif

#if (MCU_STARTUP_B80)
OTP_CTRL:
	.word	(0x800010)                  @  otp control
#endif

#if FLL_STK_EN
FLL_D:				@Only be used when FLL_STK_EN is 1, by BLE SUNWEI
	.word	0xffffffff                	@0
	.word	(_ram_use_end_)    			@4
	.word	(SRAM_SIZE)		    		@8  	stack end
#endif

BOOT_SEL_D:
	.word	(0x80063e)
	@.word	(0x80063f)	//not be used, delete it, by BLE SUNWEI

	.align 4
__irq:
	tpush    	{r14}					@push R14/LR register
	tpush    	{r0-r7}					@push r0~r7 into stack
	tmrss    	r0						@save cpsr to r0
	
	tmov		r1, r8					@r8~r12 cannot use the way like {r0~r7} to push into stack
	tmov		r2, r9					@must move r8~r12 into r1~r5 to push
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}
	
	tjl      	irq_handler				@jump to irq_handler

	tpop		{r0-r5}					@pop r8~r12
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr    	r0						@return r0 to cpsr
	tpop		{r0-r7}					@pop r0~r7
	treti    	{r15}					@return to pc

ASMEND:
	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.end
#endif
