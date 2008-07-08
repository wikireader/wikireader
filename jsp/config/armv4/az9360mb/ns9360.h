/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: ns9360.h,v 1.1 2007/05/21 01:33:50 honda Exp $
 */

#ifndef _NS9360_H_
#define _NS9360_H_


#ifndef _MACRO_ONLY
#include <itron.h>
#include <sil.h>
#endif /* _MACRO_ONLY */


#include <armv4.h>

/************************************************/
/*			CPU(Compiler) value type			*/
/************************************************/

#define	ARM9_INPUT_FREQUENCY             29491200
#define	MPMC_REFRESH_RATE 7812 /* nano-seconds */

#define	STACKTOP	stack_end   /* 非タスクコンテキスト用のスタックの初期値 */



/*
 *  タイマーの割り込みレベル
 */
#define INTLV_TIM0        4


/*
 *  タイマ値の内部表現とミリ秒単位との変換
 */
#define TIMER_CLOCK          177000			/* Base clock = 177MHz */


/*
 *  キャッシュ関連の設定
 */

/* 
 * キャッシュの設定 CP5 No.1 に書き込む値 
 * ICache のみON
 */
#define CP5_NO1_VAL         0x1078

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN  SIL_ENDIAN_BIG  /* Big */

/*
 *System Configuration Registers
 */
#define SYS_CONT_MODULE_BASE		0xa0900000					/*              1 M         */
#define	AHB_ABT_CFG_REG		(SYS_CONT_MODULE_BASE+0x0000)		/* AHB Arbiter Gen Configuration									*/
#define	BRC0_REG			(SYS_CONT_MODULE_BASE+0x0004)		/* BRC0																*/
#define	BRC1_REG			(SYS_CONT_MODULE_BASE+0x0008)		/* BRC1																*/
#define	BRC2_REG			(SYS_CONT_MODULE_BASE+0x000C)		/* BRC2																*/
#define	BRC3_REG			(SYS_CONT_MODULE_BASE+0x0010)		/* BRC3																*/
#define	BAT_BMT_REG			(SYS_CONT_MODULE_BASE+0x0014)		/* AHB Bus Arbiter Timeout Period AHB Bus Monitor Timeout Period	*/
#define	AHB_ERR1_REG		(SYS_CONT_MODULE_BASE+0x0018)		/* AHB Error Detect Status 1										*/
#define	AHB_ERR2_REG		(SYS_CONT_MODULE_BASE+0x001C)		/* AHB Error Detect Status 2										*/
#define	AHB_ERRMON_REG		(SYS_CONT_MODULE_BASE+0x0020)		/* AHB Error Monitoring Configuration								*/
#define	SWDT_CFG_REG		(SYS_CONT_MODULE_BASE+0x0174)		/* Software Watchdog Configuration									*/
#define	SWDT_REG			(SYS_CONT_MODULE_BASE+0x0178)		/* Software Watchdog Timer											*/
#define	CLK_CFG_REG			(SYS_CONT_MODULE_BASE+0x017C)		/* Clock Configuration register										*/
#define	RS_CNT_REG			(SYS_CONT_MODULE_BASE+0x0180)		/* Reset and Sleep Control register									*/
#define	MS_CFG_REG			(SYS_CONT_MODULE_BASE+0x0184)		/* Miscellaneous System Configuration register						*/
#define	PLL_CFG_REG			(SYS_CONT_MODULE_BASE+0x0188)		/* PLL Configuration register										*/
#define	GEMID_REG			(SYS_CONT_MODULE_BASE+0x0210)		/* GenID General purpose, user-defined ID register					*/
#define	EXINT0_CR			(SYS_CONT_MODULE_BASE+0x0214)		/* External Interrupt 0 Control register							*/
#define	EXINT1_CR			(SYS_CONT_MODULE_BASE+0x0218)		/* External Interrupt 1 Control register							*/
#define	EXINT2_CR			(SYS_CONT_MODULE_BASE+0x021C)		/* External Interrupt 2 Control register							*/
#define	EXINT3_CR			(SYS_CONT_MODULE_BASE+0x0220)		/* External Interrupt 3 Control register							*/

#define	IVARV_REG			INT_VECT0_REG						/* Interrupt Vector Address Register base							*/
#define	INT_CONFIG_REG		INT_CFG0_3_REG						/* Interrupt Configuration Register base							*/
#define AHB_ABT_CFG_REG_debug    0x00000002						/* debuger flag 													*/
#define AHB_ABT_CFG_REG_restart  0x00000004						/* Software Restart flag											*/
#define SCM_PLL_REG_FSStatus     0x01800000						/* PLL FS Status													*/
#define SCM_PLL_REG_NDStatus     0x001f0000						/* PLL ND Status													*/

/*
 * Interrupt Controller Registers
 */
#define	INT_VECT0_REG		(SYS_CONT_MODULE_BASE+0x00C4)		/* Interrupt Vector Address Register Level 0						*/
#define	INT_VECT1_REG		(SYS_CONT_MODULE_BASE+0x00C8)		/* Interrupt Vector Address Register Level 1						*/
#define	INT_VECT2_REG		(SYS_CONT_MODULE_BASE+0x00CC)		/* Interrupt Vector Address Register Level 2						*/
#define	INT_VECT3_REG		(SYS_CONT_MODULE_BASE+0x00D0)		/* Interrupt Vector Address Register Level 3						*/
#define	INT_VECT4_REG		(SYS_CONT_MODULE_BASE+0x00D4)		/* Interrupt Vector Address Register Level 4						*/
#define	INT_VECT5_REG		(SYS_CONT_MODULE_BASE+0x00D8)		/* Interrupt Vector Address Register Level 5						*/
#define	INT_VECT6_REG		(SYS_CONT_MODULE_BASE+0x00DC)		/* Interrupt Vector Address Register Level 6						*/
#define	INT_VECT7_REG		(SYS_CONT_MODULE_BASE+0x00E0)		/* Interrupt Vector Address Register Level 7						*/
#define	INT_VECT8_REG		(SYS_CONT_MODULE_BASE+0x00E4)		/* Interrupt Vector Address Register Level 8						*/
#define	INT_VECT9_REG		(SYS_CONT_MODULE_BASE+0x00E8)		/* Interrupt Vector Address Register Level 9						*/
#define	INT_VECT10_REG		(SYS_CONT_MODULE_BASE+0x00EC)		/* Interrupt Vector Address Register Level 10						*/
#define	INT_VECT11_REG		(SYS_CONT_MODULE_BASE+0x00F0)		/* Interrupt Vector Address Register Level 11						*/
#define	INT_VECT12_REG		(SYS_CONT_MODULE_BASE+0x00F4)		/* Interrupt Vector Address Register Level 12						*/
#define	INT_VECT13_REG		(SYS_CONT_MODULE_BASE+0x00F8)		/* Interrupt Vector Address Register Level 13						*/
#define	INT_VECT14_REG		(SYS_CONT_MODULE_BASE+0x00FC)		/* Interrupt Vector Address Register Level 14						*/
#define	INT_VECT15_REG		(SYS_CONT_MODULE_BASE+0x0100)		/* Interrupt Vector Address Register Level 15						*/
#define	INT_VECT16_REG		(SYS_CONT_MODULE_BASE+0x0104)		/* Interrupt Vector Address Register Level 16						*/
#define	INT_VECT17_REG		(SYS_CONT_MODULE_BASE+0x0108)		/* Interrupt Vector Address Register Level 17						*/
#define	INT_VECT18_REG		(SYS_CONT_MODULE_BASE+0x010C)		/* Interrupt Vector Address Register Level 18						*/
#define	INT_VECT19_REG		(SYS_CONT_MODULE_BASE+0x0110)		/* Interrupt Vector Address Register Level 19						*/
#define	INT_VECT20_REG		(SYS_CONT_MODULE_BASE+0x0114)		/* Interrupt Vector Address Register Level 20						*/
#define	INT_VECT21_REG		(SYS_CONT_MODULE_BASE+0x0118)		/* Interrupt Vector Address Register Level 21						*/
#define	INT_VECT22_REG		(SYS_CONT_MODULE_BASE+0x011C)		/* Interrupt Vector Address Register Level 22						*/
#define	INT_VECT23_REG		(SYS_CONT_MODULE_BASE+0x0120)		/* Interrupt Vector Address Register Level 23						*/
#define	INT_VECT24_REG		(SYS_CONT_MODULE_BASE+0x0124)		/* Interrupt Vector Address Register Level 24						*/
#define	INT_VECT25_REG		(SYS_CONT_MODULE_BASE+0x0128)		/* Interrupt Vector Address Register Level 25						*/
#define	INT_VECT26_REG		(SYS_CONT_MODULE_BASE+0x012C)		/* Interrupt Vector Address Register Level 26						*/
#define	INT_VECT27_REG		(SYS_CONT_MODULE_BASE+0x0130)		/* Interrupt Vector Address Register Level 27						*/
#define	INT_VECT28_REG		(SYS_CONT_MODULE_BASE+0x0134)		/* Interrupt Vector Address Register Level 28						*/
#define	INT_VECT29_REG		(SYS_CONT_MODULE_BASE+0x0138)		/* Interrupt Vector Address Register Level 29						*/
#define	INT_VECT30_REG		(SYS_CONT_MODULE_BASE+0x013C)		/* Interrupt Vector Address Register Level 30						*/
#define	INT_VECT31_REG		(SYS_CONT_MODULE_BASE+0x0140)		/* Interrupt Vector Address Register Level 31						*/
#define	INT_CFG0_3_REG		(SYS_CONT_MODULE_BASE+0x0144)		/* Int Config 0 Int Config 1 Int Config 2 Int Config 3				*/
#define	INT_CFG4_7_REG		(SYS_CONT_MODULE_BASE+0x0148)		/* Int Config 4 Int Config 5 Int Config 6 Int Config 7				*/
#define	INT_CFG8_11_REG		(SYS_CONT_MODULE_BASE+0x014C)		/* Int Config 8 Int Config 9 Int Config 10 Int Config 11			*/
#define	INT_CFG12_15_REG	(SYS_CONT_MODULE_BASE+0x0150)		/* Int Config 12 Int Config 13 Int Config 14 Int Config 15			*/
#define	INT_CFG16_19_REG	(SYS_CONT_MODULE_BASE+0x0154)		/* Int Config 16 Int Config 17 Int Config 18 Int Config 19			*/
#define	INT_CFG20_23_REG	(SYS_CONT_MODULE_BASE+0x0158)		/* Int Config 20 Int Config 21 Int Config 22 Int Config 23			*/
#define	INT_CFG24_27_REG	(SYS_CONT_MODULE_BASE+0x015C)		/* Int Config 24 Int Config 25 Int Config 26 Int Config 27			*/
#define	INT_CFG28_31_REG	(SYS_CONT_MODULE_BASE+0x0160)		/* Int Config 28 Int Config 29 Int Config 30 Int Config 31			*/
#define	ISRADDR_REG			(SYS_CONT_MODULE_BASE+0x0164)		/* ISRADDR															*/
#define	ISA_REG				(SYS_CONT_MODULE_BASE+0x0168)		/* Interrupt Status Active											*/
#define	ISRAW_REG			(SYS_CONT_MODULE_BASE+0x016C)		/* Interrupt Status Raw												*/
#define	INT_ID_REG			(SYS_CONT_MODULE_BASE+0x018C)		/* Active Interrupt Level register									*/

#define	INT_CFG_BIT_IE		0x80								/* IntConfigReg IE bit */
#define	INT_CFG_BIT_INV		0x40								/* IntConfigReg INV bit */
/* #define	INT_CFG_BIT_IT		0x20					*/			/* IntConfigReg IT bit */
#define	INT_CFG_BIT_IT		0x0								/* IntConfigReg IT bit */

#define	BBBIE_REG			0xa0401004							/* BBus Bridge Interrupt Enable register		*/
#define	BBBIS_REG			0xa0401000							/* BBus Bridge Interrupt Status register		*/
/* BBus Bridge Interrupt Enable/Status register bit define */
#define	BBBI_GLBL			0x80000000
#define	BBBI_DMA2			0x02000000
#define	BBBI_DMA1			0x01000000
#define	BBBI_1284			0x00000800
#define	BBBI_I2C			0x00000400
#define	BBBI_S4TX			0x00000200
#define	BBBI_S4RX			0x00000100
#define	BBBI_S3TX			0x00000080
#define	BBBI_S3RX			0x00000040
#define	BBBI_S1TX			0x00000020
#define	BBBI_S1RX			0x00000010
#define	BBBI_S2TX			0x00000008
#define	BBBI_S2RX			0x00000004
#define	BBBI_USB			0x00000002
#define	BBBI_DMA			0x00000001

#define	BBBIS_MASK			0x03000fff;							/* BBus Interrupt Status register Mask patern */


/*
 * Chip Select Registers
 */
#define	CS4B_REG			(SYS_CONT_MODULE_BASE+0x01D0)		/* System Memory Chip Select 4 Dynamic Memory Base					*/
#define	CS4M_REG			(SYS_CONT_MODULE_BASE+0x01D4)		/* System Memory Chip Select 4 Dynamic Memory Mask					*/
#define	CS5B_REG			(SYS_CONT_MODULE_BASE+0x01D8)		/* System Memory Chip Select 5 Dynamic Memory Base					*/
#define	CS5M_REG			(SYS_CONT_MODULE_BASE+0x01DC)		/* System Memory Chip Select 5 Dynamic Memory Mask					*/
#define	CS6B_REG			(SYS_CONT_MODULE_BASE+0x01E0)		/* System Memory Chip Select 6 Dynamic Memory Base					*/
#define	CS6M_REG			(SYS_CONT_MODULE_BASE+0x01E4)		/* System Memory Chip Select 6 Dynamic Memory Mask					*/
#define	CD7B_REG			(SYS_CONT_MODULE_BASE+0x01E8)		/* System Memory Chip Select 7 Dynamic Memory Base					*/
#define	CS7M_REG			(SYS_CONT_MODULE_BASE+0x01EC)		/* System Memory Chip Select 7 Dynamic Memory Mask					*/
#define	CS0B_REG			(SYS_CONT_MODULE_BASE+0x01F0)		/* System Memory Chip Select 0 Static Memory Base					*/
#define	CS0M_REG			(SYS_CONT_MODULE_BASE+0x01F4)		/* System Memory Chip Select 0 Static Memory Mask					*/
#define	CS1B_REG			(SYS_CONT_MODULE_BASE+0x01F8)		/* System Memory Chip Select 1 Static Memory Base					*/
#define	CS1M_REG			(SYS_CONT_MODULE_BASE+0x01FC)		/* System Memory Chip Select 1 Static Memory Mask					*/
#define	CS2B_REG			(SYS_CONT_MODULE_BASE+0x0200)		/* System Memory Chip Select 2 Static Memory Base					*/
#define	CS2M_REG			(SYS_CONT_MODULE_BASE+0x0204)		/* System Memory Chip Select 2 Static Memory Mask					*/
#define	CS3B_REG			(SYS_CONT_MODULE_BASE+0x0208)		/* System Memory Chip Select 3 Static Memory Base					*/
#define	CS3M_REG			(SYS_CONT_MODULE_BASE+0x020C)		/* System Memory Chip Select 3 Static Memory Mask					*/

/*
 *Timer Registers
 */
#define	TIMER0_RLC_REG		(SYS_CONT_MODULE_BASE+0x0044)		/* Timer 0 Reload Count register									*/
#define	TIMER1_RLC_REG		(SYS_CONT_MODULE_BASE+0x0048)		/* Timer 1 Reload Count register									*/
#define	TIMER2_RLC_REG		(SYS_CONT_MODULE_BASE+0x004C)		/* Timer 2 Reload Count register									*/
#define	TIMER3_RLC_REG		(SYS_CONT_MODULE_BASE+0x0050)		/* Timer 3 Reload Count register									*/
#define	TIMER4_RLC_REG		(SYS_CONT_MODULE_BASE+0x0054)		/* Timer 4 Reload Count register									*/
#define	TIMER5_RLC_REG		(SYS_CONT_MODULE_BASE+0x0058)		/* Timer 5 Reload Count register									*/
#define	TIMER6_RLC_REG		(SYS_CONT_MODULE_BASE+0x005C)		/* Timer 6 Reload Count register									*/
#define	TIMER7_RLC_REG		(SYS_CONT_MODULE_BASE+0x0060)		/* Timer 7 Reload Count register									*/
#define	TIMER8_RLC_REG		(SYS_CONT_MODULE_BASE+0x0064)		/* Timer 8 Reload Count register									*/
#define	TIMER9_RLC_REG		(SYS_CONT_MODULE_BASE+0x0068)		/* Timer 9 Reload Count register									*/
#define	TIMER10_RLC_REG		(SYS_CONT_MODULE_BASE+0x006C)		/* Timer 10 Reload Count register									*/
#define	TIMER11_RLC_REG		(SYS_CONT_MODULE_BASE+0x0070)		/* Timer 11 Reload Count register									*/
#define	TIMER12_RLC_REG		(SYS_CONT_MODULE_BASE+0x0074)		/* Timer 12 Reload Count register									*/
#define	TIMER13_RLC_REG		(SYS_CONT_MODULE_BASE+0x0078)		/* Timer 13 Reload Count register									*/
#define	TIMER14_RLC_REG		(SYS_CONT_MODULE_BASE+0x007C)		/* Timer 14 Reload Count register									*/
#define	TIMER15_RLC_REG		(SYS_CONT_MODULE_BASE+0x0080)		/* Timer 15 Reload Count register									*/
#define	TIMER0_RR			(SYS_CONT_MODULE_BASE+0x0084)		/* Timer 0 Read register											*/
#define	TIMER1_RR			(SYS_CONT_MODULE_BASE+0x0088)		/* Timer 1 Read register											*/
#define	TIMER2_RR			(SYS_CONT_MODULE_BASE+0x008C)		/* Timer 2 Read register											*/
#define	TIMER3_RR			(SYS_CONT_MODULE_BASE+0x0090)		/* Timer 3 Read register											*/
#define	TIMER4_RR			(SYS_CONT_MODULE_BASE+0x0094)		/* Timer 4 Read register											*/
#define	TIMER5_RR			(SYS_CONT_MODULE_BASE+0x0098)		/* Timer 5 Read register											*/
#define	TIMER6_RR			(SYS_CONT_MODULE_BASE+0x009C)		/* Timer 6 Read register											*/
#define	TIMER7_RR			(SYS_CONT_MODULE_BASE+0x00A0)		/* Timer 7 Read register											*/
#define	TIMER8_RR			(SYS_CONT_MODULE_BASE+0x00A4)		/* Timer 8 Read register											*/
#define	TIMER9_RR			(SYS_CONT_MODULE_BASE+0x00A8)		/* Timer 9 Read register											*/
#define	TIMER10_RR			(SYS_CONT_MODULE_BASE+0x00AC)		/* Timer 10 Read register											*/
#define	TIMER11_RR			(SYS_CONT_MODULE_BASE+0x00B0)		/* Timer 11 Read register											*/
#define	TIMER12_RR			(SYS_CONT_MODULE_BASE+0x00B4)		/* Timer 12 Read register											*/
#define	TIMER13_RR			(SYS_CONT_MODULE_BASE+0x00B8)		/* Timer 13 Read register											*/
#define	TIMER14_RR			(SYS_CONT_MODULE_BASE+0x00BC)		/* Timer 14 Read register											*/
#define	TIMER15_RR			(SYS_CONT_MODULE_BASE+0x00C0)		/* Timer 15 Read register											*/
#define	TIS_REG				(SYS_CONT_MODULE_BASE+0x0170)		/* Timer Interrupt Status register									*/
#define	TIMER0_CR			(SYS_CONT_MODULE_BASE+0x0190)		/* Timer 0 Control register											*/
#define	TIMER1_CR			(SYS_CONT_MODULE_BASE+0x0194)		/* Timer 1 Control register											*/
#define	TIMER2_CR			(SYS_CONT_MODULE_BASE+0x0198)		/* Timer 2 Control register											*/
#define	TIMER3_CR			(SYS_CONT_MODULE_BASE+0x019C)		/* Timer 3 Control register											*/
#define	TIMER4_CR			(SYS_CONT_MODULE_BASE+0x01A0)		/* Timer 4 Control register											*/
#define	TIMER5_CR			(SYS_CONT_MODULE_BASE+0x01A4)		/* Timer 5 Control register											*/
#define	TIMER6_CR			(SYS_CONT_MODULE_BASE+0x01A8)		/* Timer 6 Control register											*/
#define	TIMER7_CR			(SYS_CONT_MODULE_BASE+0x01AC)		/* Timer 7 Control register											*/
#define	TIMER8_CR			(SYS_CONT_MODULE_BASE+0x01B0)		/* Timer 8 Control register											*/
#define	TIMER9_CR			(SYS_CONT_MODULE_BASE+0x01B4)		/* Timer 9 Control register											*/
#define	TIMER10_CR			(SYS_CONT_MODULE_BASE+0x01B8)		/* Timer 10 Control register										*/
#define	TIMER11_CR			(SYS_CONT_MODULE_BASE+0x01BC)		/* Timer 11 Control register										*/
#define	TIMER12_CR			(SYS_CONT_MODULE_BASE+0x01C0)		/* Timer 12 Control register										*/
#define	TIMER13_CR			(SYS_CONT_MODULE_BASE+0x01C4)		/* Timer 13 Control register										*/
#define	TIMER14_CR			(SYS_CONT_MODULE_BASE+0x01C8)		/* Timer 14 Control register										*/
#define	TIMER15_CR			(SYS_CONT_MODULE_BASE+0x01CC)		/* Timer 15 Control register										*/

/* Defines for the SCM modules dynamic (RAM) chip selects */
#define SCM_CS4_BASE_REG	(SYS_CONT_MODULE_BASE+0x01D0)
#define SCM_CS4_MASK_REG	(SYS_CONT_MODULE_BASE+0x01D4)
#define SCM_CS5_BASE_REG	(SYS_CONT_MODULE_BASE+0x01D8)
#define SCM_CS5_MASK_REG	(SYS_CONT_MODULE_BASE+0x01DC)
#define SCM_CS6_BASE_REG	(SYS_CONT_MODULE_BASE+0x01E0)
#define SCM_CS6_MASK_REG	(SYS_CONT_MODULE_BASE+0x01E4)
#define SCM_CS7_BASE_REG	(SYS_CONT_MODULE_BASE+0x01E8)
#define SCM_CS7_MASK_REG	(SYS_CONT_MODULE_BASE+0x01EC)

/* Defines for the SCM modules static chip selects */
#define SCM_CS0_BASE_REG	(SYS_CONT_MODULE_BASE+0x01F0)
#define SCM_CS0_MASK_REG	(SYS_CONT_MODULE_BASE+0x01F4)
#define SCM_CS1_BASE_REG	(SYS_CONT_MODULE_BASE+0x01F8)
#define SCM_CS1_MASK_REG	(SYS_CONT_MODULE_BASE+0x01FC)
#define SCM_CS2_BASE_REG	(SYS_CONT_MODULE_BASE+0x0200)
#define SCM_CS2_MASK_REG	(SYS_CONT_MODULE_BASE+0x0204)
#define SCM_CS3_BASE_REG	(SYS_CONT_MODULE_BASE+0x0208)
#define SCM_CS3_MASK_REG	(SYS_CONT_MODULE_BASE+0x020C)

/*
 *Interrupt ID
 */
#define	INT_ID_WDT			0									/* WDT Interrupt ID													*/
#define	INT_ID_AHB_ER		1									/* AHB Bus Error Interrupt ID										*/
#define	INT_ID_BBUS_INT		2									/* BBus Aggregate Interrupt ID										*/
#define	INT_ID_ETH_RX		4									/* Ethernet Module Receive Interrupt ID								*/
#define	INT_ID_ETH_TX		5									/* Ethernet Module Transmit Interrupt ID							*/
#define	INT_ID_ETH_PHY		6									/* Ethernet Phy Interrupt ID										*/
#define	INT_ID_LCD			7									/* LCD Module interrupt ID											*/
#define	INT_ID_PCI_BRG		8									/* PCI Bridge Module Interrupt ID									*/
#define	INT_ID_PCI_ABT		9									/* PCI Arbiter Module Interrupt ID									*/
#define	INT_ID_PCI_EX0		10									/* PCI External Interrupt 0 ID										*/
#define	INT_ID_PCI_EX1		11									/* PCI External Interrupt 1 ID										*/
#define	INT_ID_PCI_EX2		12									/* PCI External Interrupt 2 ID										*/
#define	INT_ID_PCI_EX3		13									/* PCI External Interrupt 3 ID										*/
#define	INT_ID_I2C			14									/* I2C Interrupt ID													*/
#define	INT_ID_BBUS_DMA		15									/* BBus DMA Interrupt ID											*/
#define	INT_ID_TIMER0		16									/* Timer Interrupt 0 ID												*/
#define	INT_ID_TIMER1		17									/* Timer Interrupt 1 ID												*/
#define	INT_ID_TIMER2		18									/* Timer Interrupt 2 ID												*/
#define	INT_ID_TIMER3		19									/* Timer Interrupt 3 ID												*/
#define	INT_ID_TIMER4		20									/* Timer Interrupt 4 ID												*/
#define	INT_ID_TIMER5		21									/* Timer Interrupt 5 ID												*/
#define	INT_ID_TIMER6		22									/* Timer Interrupt 6 ID												*/
#define	INT_ID_TIMER7		23									/* Timer Interrupt 7 ID												*/
#define	INT_ID_TIMER89		24									/* Timer Interrupt 8 and 9 ID										*/
#define	INT_ID_USB_HOST		25									/* USB HOST Interrupt ID											*/
#define	INT_ID_USB_DEV		26									/* USB DEVIDE Interrupt ID											*/
#define	INT_ID_TIMER1415	27									/* Timer Interrupt 14 and 15 ID										*/
#define	INT_ID_EXT_INT0		28									/* External Interrupt 0 ID											*/
#define	INT_ID_EXT_INT1		29									/* External Interrupt 1 ID											*/
#define	INT_ID_EXT_INT2		30									/* External Interrupt 2 ID											*/
#define	INT_ID_EXT_INT3		31									/* External Interrupt 3 ID											*/
/* これ以降はBBusの割込みID */          
#define	INT_ID_BBUS_F		32									/* BBus Interrupt First ID											*/
#define	INT_ID_BBUS_RFU30	33									/* BBus Interrupt RFU (bit30)										*/
#define	INT_ID_BBUS_RFU29	34									/* BBus Interrupt RFU (bit29)										*/
#define	INT_ID_BBUS_RFU28	35									/* BBus Interrupt RFU (bit28)										*/
#define	INT_ID_BBUS_RFU27	36									/* BBus Interrupt RFU (bit27)										*/
#define	INT_ID_BBUS_RFU26	37									/* BBus Interrupt RFU (bit26)										*/
#define	INT_ID_AHB_DMA2		38									/* AHB_DMA2 Interrupt (BBus bit25)									*/
#define	INT_ID_AHB_DMA1		39									/* AHB_DMA1 Interrupt (BBus bit24)									*/
#define	INT_ID_BBUS_RFU23	40									/* BBus Interrupt RFU (bit23)										*/
#define	INT_ID_BBUS_RFU22	41									/* BBus Interrupt RFU (bit22)										*/
#define	INT_ID_BBUS_RFU21	42									/* BBus Interrupt RFU (bit21)										*/
#define	INT_ID_BBUS_RFU20	43									/* BBus Interrupt RFU (bit20)										*/
#define	INT_ID_BBUS_RFU19	44									/* BBus Interrupt RFU (bit19)										*/
#define	INT_ID_BBUS_RFU18	45									/* BBus Interrupt RFU (bit18)										*/
#define	INT_ID_BBUS_RFU17	46									/* BBus Interrupt RFU (bit17)										*/
#define	INT_ID_BBUS_RFU16	47									/* BBus Interrupt RFU (bit16)										*/
#define	INT_ID_BBUS_RFU15	48									/* BBus Interrupt RFU (bit15)										*/
#define	INT_ID_BBUS_RFU14	49									/* BBus Interrupt RFU (bit14)										*/
#define	INT_ID_BBUS_RFU13	50									/* BBus Interrupt RFU (bit13)										*/
#define	INT_ID_BBUS_RFU12	51									/* BBus Interrupt RFU (bit12)										*/
#define	INT_ID_1284			52									/* IEEE 1284 Module Interrupt (BBus bit11)							*/
#define	INT_ID_BI2C			53									/* I2C Interrupt (BBus bit10)										*/
#define	INT_ID_SERD_TX		54									/* SER D Tx Interrupt (BBus bit9)									*/
#define	INT_ID_SERD_RX		55									/* SER D Rx Interrupt (BBus bit8)									*/
#define	INT_ID_SERC_TX		56									/* SER C Tx Interrupt (BBus bit7)									*/
#define	INT_ID_SERC_RX		57									/* SER C Rx Interrupt (BBus bit6)									*/
#define	INT_ID_SERA_TX		58									/* SER A Tx Interrupt (BBus bit5)									*/
#define	INT_ID_SERA_RX		59									/* SER A Rx Interrupt (BBus bit4)									*/
#define	INT_ID_SERB_TX		60									/* SER B Tx Interrupt (BBus bit3)									*/
#define	INT_ID_SERB_RX		61									/* SER B Rx Interrupt (BBus bit2)									*/
#define	INT_ID_USB			62									/* USB module Interrupt (BBus bit1)									*/
#define	INT_ID_DMAE			63									/* BBus DMA aggregate Interrupt (BBus bit0)							*/

#define	INT_ID_NONE			0xff								/* unuse ID */
#define	BBUS_IRQ_NUM		32									/* BBus IRQ start number											*/


/*
 * Serial Controller Registers
 */
#define	SC2CRA_REG		0x90200000		/* Channel B Control Register A */
#define	SC2CRB_REG		0x90200004		/* Channel B Control Register B */
#define	SC2SRA_REG		0x90200008		/* Channel B Status Register A */
#define	SC2BRG_REG		0x9020000C		/* Channel B Bit-Rate register */
#define	SC2FIFO_REG		0x90200010		/* Channel B FIFO Data register */
#define	SC2RBT_REG		0x90200014		/* Channel B Receive Buffer Gap Timer */
#define	SC2RCT_REG		0x90200018		/* Channel B Receive Character Gap Timer */
#define	SC2RMR_REG		0x9020001C		/* Channel B Receive Match register */
#define	SC2RMM_REG		0x90200020		/* Channel B Receive Match Mask register */
#define	SC2FCR_REG		0x90200034		/* Channel B Flow Control register */
#define	SC2FCF_REG		0x90200038		/* Channel B Flow Control Force register */

#define	SC1CRA_REG		0x90200040		/* Channel A Control Register A */
#define	SC1CRB_REG		0x90200044		/* Channel A Control Register B */
#define	SC1SRA_REG		0x90200048		/* Channel A Status Register A */
#define	SC1BRG_REG		0x9020004C		/* Channel A Bit-Rate register */
#define	SC1FIFO_REG		0x90200050		/* Channel A FIFO Data register */
#define	SC1RBT_REG		0x90200054		/* Channel A Receive Buffer Gap Timer */
#define	SC1RCT_REG		0x90200058		/* Channel A Receive Character Gap Timer */
#define	SC1RMR_REG		0x9020005C		/* Channel A Receive Match register */
#define	SC1RMM_REG		0x90200060		/* Channel A Receive Match Mask register */
#define	SC1FCR_REG		0x90200074		/* Channel A Flow Control register */
#define	SC1FCF_REG		0x90200078		/* Channel A Flow Control Force register */

#define	SC3CRA_REG		0x90300000		/* Channel C Control Register A */
#define	SC3CRB_REG		0x90300004		/* Channel C Control Register B */
#define	SC3SRA_REG		0x90300008		/* Channel C Status Register A */
#define	SC3BRG_REG		0x9030000C		/* Channel C Bit-Rate register */
#define	SC3FIFO_REG		0x90300010		/* Channel C FIFO Data register */
#define	SC3RBT_REG		0x90300014		/* Channel C Receive Buffer Gap Timer */
#define	SC3RCT_REG		0x90300018		/* Channel C Receive Character Gap Timer */
#define	SC3RMR_REG		0x9030001C		/* Channel C Receive Match register */
#define	SC3RMM_REG		0x90300020		/* Channel C Receive Match Mask register */
#define	SC3FCR_REG		0x90300034		/* Channel C Flow Control register */
#define	SC3FCF_REG		0x90300038		/* Channel C Flow Control Force register */

#define	SC4CRA_REG		0x90300040		/* Channel D Control Register A */
#define	SC4CRB_REG		0x90300044		/* Channel D Control Register B */
#define	SC4SRA_REG		0x90300048		/* Channel D Status Register A */
#define	SC4BRG_REG		0x9030004C		/* Channel D Bit-Rate register */
#define	SC4FIFO_REG		0x90300050		/* Channel D FIFO Data register */
#define	SC4RBT_REG		0x90300054		/* Channel D Receive Buffer Gap Timer */
#define	SC4RCT_REG		0x90300058		/* Channel D Receive Character Gap Timer */
#define	SC4RMR_REG		0x9030005C		/* Channel D Receive Match register */
#define	SC4RMM_REG		0x90300060		/* Channel D Receive Match Mask register */
#define	SC4FCR_REG		0x90300074		/* Channel D Flow Control register */
#define	SC4FCF_REG		0x90300078		/* Channel D Flow Control Force register */

/* Serial Channel Control Register bit define */
#define	SCCRA_CE		0x80000000		/* 1... .... .... .... .... .... .... .... */
#define	SCCRA_BRK		0x40000000		/* .1.. .... .... .... .... .... .... .... */
#define	SCCRA_STICK		0x20000000		/* ..1. .... .... .... .... .... .... .... */
#define	SCCRA_EPS		0x10000000		/* ...1 .... .... .... .... .... .... .... */
#define	SCCRA_PE		0x08000000		/* .... 1... .... .... .... .... .... .... */
#define	SCCRA_STOP		0x04000000		/* .... .1.. .... .... .... .... .... .... */
#define	SCCRA_WLS		0x03000000		/* .... ..11 .... .... .... .... .... .... */
#define	SCCRA_CTSTX		0x00800000		/* .... .... 1... .... .... .... .... .... */
#define	SCCRA_RTSRX		0x00400000		/* .... .... .1.. .... .... .... .... .... */
#define	SCCRA_RL		0x00200000		/* .... .... ..1. .... .... .... .... .... */
#define	SCCRA_LL		0x00100000		/* .... .... ...1 .... .... .... .... .... */
#define	SCCRA_DTR		0x00020000		/* .... .... .... ..1. .... .... .... .... */
#define	SCCRA_RTS		0x00010000		/* .... .... .... ...1 .... .... .... .... */
#define	SCCRA_RIE		0x00000e00		/* .... .... .... .... .... 111. .... .... */
#define	SCCRA_ERXDMA	0x00000100		/* .... .... .... .... .... ...1 .... .... */
#define	SCCRA_RIC		0x000000e0		/* .... .... .... .... .... .... 111. .... */
#define	SCCRA_TIC		0x0000001e		/* .... .... .... .... .... .... ...1 111. */
#define	SCCRA_ETXDMA	0x00000001		/* .... .... .... .... .... .... .... ...1 */
#define	SCCRB_RDM		0xf0000000		/* 1111 .... .... .... .... .... .... .... */
#define	SCCRB_RBGT		0x08000000		/* .... 1... .... .... .... .... .... .... */
#define	SCCRB_RCGT		0x04000000		/* .... .1.. .... .... .... .... .... .... */
#define	SCCRB_MODE		0x00300000		/* .... .... ..11 .... .... .... .... .... */
#define	SCCRB_BITORDR	0x00080000		/* .... .... .... 1... .... .... .... .... */
#define	SCCRB_RTSTX		0x00008000		/* .... .... .... .... 1... .... .... .... */

/* Serial Channel Status Register bit define */
#define	SCSRA_MATCH		0xf0000000		/* 1111 .... .... .... .... .... .... .... */
#define	SCSRA_BGAP		0x08000000		/* .... 1... .... .... .... .... .... .... */
#define	SCSRA_CGAP		0x04000000		/* .... .1.. .... .... .... .... .... .... */
#define	SCSRA_RXFDB		0x00300000		/* .... .... ..11 .... .... .... .... .... */
#define	SCSRA_DCD		0x00080000		/* .... .... .... 1... .... .... .... .... */
#define	SCSRA_RI		0x00040000		/* .... .... .... .1.. .... .... .... .... */
#define	SCSRA_DSR		0x00020000		/* .... .... .... ..1. .... .... .... .... */
#define	SCSRA_CTS		0x00010000		/* .... .... .... ...1 .... .... .... .... */
#define	SCSRA_RBRK		0x00008000		/* .... .... .... .... 1... .... .... .... */
#define	SCSRA_RFE		0x00004000		/* .... .... .... .... .1.. .... .... .... */
#define	SCSRA_RPE		0x00002000		/* .... .... .... .... ..1. .... .... .... */
#define	SCSRA_ROVER		0x00001000		/* .... .... .... .... ...1 .... .... .... */
#define	SCSRA_RRDY		0x00000800		/* .... .... .... .... .... 1... .... .... */
#define	SCSRA_RHALF		0x00000400		/* .... .... .... .... .... .1.. .... .... */
#define	SCSRA_RBC		0x00000200		/* .... .... .... .... .... ..1. .... .... */
#define	SCSRA_RFS		0x00000100		/* .... .... .... .... .... ...1 .... .... */
#define	SCSRA_DCDI		0x00000080		/* .... .... .... .... .... .... 1... .... */
#define	SCSRA_RII		0x00000040		/* .... .... .... .... .... .... .1.. .... */
#define	SCSRA_DSRI		0x00000020		/* .... .... .... .... .... .... ..1. .... */
#define	SCSRA_CTSI		0x00000010		/* .... .... .... .... .... .... ...1 .... */
#define	SCSRA_TRDY		0x00000008		/* .... .... .... .... .... .... .... 1... */
#define	SCSRA_THALF		0x00000004		/* .... .... .... .... .... .... .... .1.. */
#define	SCSRA_TEMPTY	0x00000001		/* .... .... .... .... .... .... .... ...1 */


/*
 *Memory Controller Registers
 */
#define MEM_CONTROLLER_BASE      0xA0700000 /* ARM MPMC Memory Controller Registers  Base Address */
#define HIGHEST_HW     0xFFFEFFF0 /* Highest *hardware only* address, use for termination */
#define CS_BASE_SHIFT_BITS                      12
#define CS_MASK_SHIFT_BITS                      12


#define MPMCControl_REG            ((UW volatile *) (MEM_CONTROLLER_BASE + 0x0))
#define MPMCStatus_REG             ((UW volatile *) (MEM_CONTROLLER_BASE + 0x4))
#define MPMCConfig_REG             ((UW volatile *) (MEM_CONTROLLER_BASE + 0x8))
#define MPMCDynamicControl_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x20))    
#define MPMCDynamicRefresh_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x24))
#define MPMCDynamicReadConfig_REG  ((UW volatile *) (MEM_CONTROLLER_BASE + 0x28))
#define MPMCDynamictRP_REG         ((UW volatile *) (MEM_CONTROLLER_BASE + 0x30))
#define MPMCDynamictRAS_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x34))
#define MPMCDynamictSREX_REG       ((UW volatile *) (MEM_CONTROLLER_BASE + 0x38))
#define MPMCDynamictAPR_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x3C))
#define MPMCDynamictDAL_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x40))
#define MPMCDynamictWR_REG         ((UW volatile *) (MEM_CONTROLLER_BASE + 0x44))
#define MPMCDynamictRC_REG         ((UW volatile *) (MEM_CONTROLLER_BASE + 0x48))
#define MPMCDynamictRFC_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x4C))
#define MPMCDynamictXSR_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x50))
#define MPMCDynamictRRD_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x54))
#define MPMCDynamictMRD_REG        ((UW volatile *) (MEM_CONTROLLER_BASE + 0x58))
#define MPMCStaticExtendedWait_REG ((UW volatile *) (MEM_CONTROLLER_BASE + 0x80))
#define MPMCDynamicConfig0_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x100))
#define MPMCDynamicRasCas0_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x104))
#define MPMCDynamicConfig1_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x120))
#define MPMCDynamicRasCas1_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x124))
#define MPMCDynamicConfig2_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x140))
#define MPMCDynamicRasCas2_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x144))
#define MPMCDynamicConfig3_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x160))
#define MPMCDynamicRasCas3_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x164))
#define MPMCStaticConfig0_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x200))
#define MPMCStaticWaitWen0_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x204))
#define MPMCStaticWaitOen0_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x208))
#define MPMCStaticWaitRd0_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x20C))
#define MPMCStaticWaitPage0_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x210))
#define MPMCStaticWaitWr0_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x214))
#define MPMCStaticWaitTurn0_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x218))
#define MPMCStaticConfig1_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x220))
#define MPMCStaticWaitWen1_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x224))
#define MPMCStaticWaitOen1_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x228))
#define MPMCStaticWaitRd1_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x22C))
#define MPMCStaticWaitPage1_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x230))
#define MPMCStaticWaitWr1_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x234))
#define MPMCStaticWaitTurn1_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x238))
#define MPMCStaticConfig2_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x240))
#define MPMCStaticWaitWen2_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x244))
#define MPMCStaticWaitOen2_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x248))
#define MPMCStaticWaitRd2_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x24C))
#define MPMCStaticWaitPage2_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x250))
#define MPMCStaticWaitWr2_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x254))
#define MPMCStaticWaitTurn2_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x258))
#define MPMCStaticConfig3_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x260))
#define MPMCStaticWaitWen3_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x264))
#define MPMCStaticWaitOen3_REG    ((UW volatile *) (MEM_CONTROLLER_BASE + 0x268))
#define MPMCStaticWaitRd3_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x26C))
#define MPMCStaticWaitPage3_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x270))
#define MPMCStaticWaitWr3_REG     ((UW volatile *) (MEM_CONTROLLER_BASE + 0x274))
#define MPMCStaticWaitTurn3_REG   ((UW volatile *) (MEM_CONTROLLER_BASE + 0x278))


/* PrimeCell MultiPort Memory Controller defines */
    #define MPMC_DYNAMIC_REFRESH_refresh 10, 0
    #define MPMC_DYNAMIC_CONFIG0_config    31, 0
    #define MPMC_DYNAMIC_RASCAS0_rascas    31, 0
    #define MPMC_DYNAMIC_CONFIG1_config   31, 0
    #define MPMC_DYNAMIC_RASCAS1_rascas    31, 0
    #define MPMC_DYNAMIC_CONFIG2_config  31, 0
    #define MPMC_DYNAMIC_RASCAS2_rascas  31, 0
    #define MPMC_DYNAMIC_CONFIG3_config   31, 0
    #define MPMC_DYNAMIC_RASCAS3_rascas   31, 0

    /* Defines for static memory */
    #define MPMC_STATIC_CONFIG0_config     0x82
    #define MPMC_STATIC_WAIT_WEN0_wen      0x02
    #define MPMC_STATIC_WAIT_OEN0_oen      0x02
    #define MPMC_STATIC_WAIT_RD0_rd        0x09
    #define MPMC_STATIC_WAIT_PAGE0_page    0x02
    #define MPMC_STATIC_WAIT_WR0_wr        0x09
    #define MPMC_STATIC_WAIT_TURN0_turn    0x02

    #define MPMC_STATIC_CONFIG1_config     0x81
    #define MPMC_STATIC_WAIT_WEN1_wen      0x00
    #define MPMC_STATIC_WAIT_OEN1_oen      0x00
    #define MPMC_STATIC_WAIT_RD1_rd        0x08
    #define MPMC_STATIC_WAIT_PAGE1_page    0x00
    #define MPMC_STATIC_WAIT_WR1_wr        0x04
    #define MPMC_STATIC_WAIT_TURN1_turn    0x00

    #define MPMC_STATIC_CONFIG2_config     0x82
    #define MPMC_STATIC_WAIT_WEN2_wen      0x02
    #define MPMC_STATIC_WAIT_OEN2_oen      0x02
    #define MPMC_STATIC_WAIT_RD2_rd        0x09
    #define MPMC_STATIC_WAIT_PAGE2_page    0x02
    #define MPMC_STATIC_WAIT_WR2_wr        0x09
    #define MPMC_STATIC_WAIT_TURN2_turn    0x02

    #define MPMC_STATIC_CONFIG3_config     0x82
    #define MPMC_STATIC_WAIT_WEN3_wen      0x02
    #define MPMC_STATIC_WAIT_OEN3_oen      0x02
    #define MPMC_STATIC_WAIT_RD3_rd        0x09
    #define MPMC_STATIC_WAIT_PAGE3_page    0x02
    #define MPMC_STATIC_WAIT_WR3_wr        0x09
    #define MPMC_STATIC_WAIT_TURN3_turn    0x02


/*
 * BBus Utility Control ans Status Registers
 */

/* address */
#define BBUS_UTILITY_BASE              0x90600000      /* BBus Utility Control ans Status Registers  Base Address */

#define BBUS_MAST_RESET_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x0))
#define BBUS_INTERRUPT_STATUS_REG      ((UW volatile *) (BBUS_UTILITY_BASE + 0x4))
#define BBUS_GPIO_CONFIG1_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x10))
#define BBUS_GPIO_CONFIG2_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x14))
#define BBUS_GPIO_CONFIG3_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x18))
#define BBUS_GPIO_CONFIG4_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x1c))
#define BBUS_GPIO_CONFIG5_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x20))
#define BBUS_GPIO_CONFIG6_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x24))
#define BBUS_GPIO_CONFIG7_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x28))
#define BBUS_GPIO_CONT1_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x30))
#define BBUS_GPIO_CONT2_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x34))
#define BBUS_GPIO_STATUS1_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x40))
#define BBUS_GPIO_STATUS2_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x44))
#define BBUS_TIMEOUT_REG               ((UW volatile *) (BBUS_UTILITY_BASE + 0x50))
#define BBUS_DMA_INT_STATUS_REG        ((UW volatile *) (BBUS_UTILITY_BASE + 0x60))
#define BBUS_DMA_INT_ENABLE_REG        ((UW volatile *) (BBUS_UTILITY_BASE + 0x64))
#define BBUS_USB_CONFIG_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x70))
#define BBUS_ENDIAN_CONFIG_REG         ((UW volatile *) (BBUS_UTILITY_BASE + 0x80))
#define BBUS_ARM_WAKEUP_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x90))
#define BBUS_GPIO_CONFIG8_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x100))
#define BBUS_GPIO_CONFIG9_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x104))
#define BBUS_GPIO_CONFIG10_REG         ((UW volatile *) (BBUS_UTILITY_BASE + 0x108))
#define BBUS_GPIO_CONT3_REG            ((UW volatile *) (BBUS_UTILITY_BASE + 0x120))
#define BBUS_GPIO_STATUS3_REG          ((UW volatile *) (BBUS_UTILITY_BASE + 0x130))

/* registers value */
#define	BBUS_MAST_RESET_USBDEV		0x00001000					/* UDBDEV bit ON                                   */
#define	BBUS_MAST_RESET_USBHST		0x00000800					/* UDBHST bit ON                                   */
#define	BBUS_MAST_RESET_RTC2		0x00000400					/* RTC2 bit ON                                     */
#define	BBUS_MAST_RESET_RTC1		0x00000200					/* RTC1 bit ON                                     */
#define	BBUS_MAST_RESET_I2C 		0x00000080					/* I2C bit ON                                      */
#define	BBUS_MAST_RESET_1284		0x00000040					/* IEEE 1284 bit ON                                */
#define	BBUS_MAST_RESET_SerD		0x00000020					/* SerD bit ON                                     */
#define	BBUS_MAST_RESET_SerC		0x00000010					/* SerC bit ON                                     */
#define	BBUS_MAST_RESET_SerA		0x00000008					/* SerA bit ON                                     */
#define	BBUS_MAST_RESET_SerB		0x00000004					/* SerB bit ON                                     */
#define	BBUS_MAST_RESET_DMA			0x00000001					/* DMA bit ON 	                                  */

#define	BBUS_INTERRUPT_RESET		0x00000001					/* Interrupt Reset                                 */

#define	BBUS_GPIO_CONFIG1_GPIO0		0x00000000					/* GPIO[0] Configration   D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG1_GPIO1		0x00000000					/* GPIO[1] Configration   D07:04 : Func0           */
#define	BBUS_GPIO_CONFIG1_GPIO2		0x00000b00					/* GPIO[2] Configration   D11:08 : Func3           */
#define	BBUS_GPIO_CONFIG1_GPIO3		0x0000b000					/* GPIO[3] Configration   D15:12 : Func3           */
#define	BBUS_GPIO_CONFIG1_GPIO4		0x000b0000					/* GPIO[4] Configration   D19:16 : Func3           */
#define	BBUS_GPIO_CONFIG1_GPIO5		0x00b00000					/* GPIO[5] Configration   D23:20 : Func3           */
#define	BBUS_GPIO_CONFIG1_GPIO6		0x00000000					/* GPIO[6] Configration   D27:24 : Func0           */
#define	BBUS_GPIO_CONFIG1_GPIO7		0xb0000000					/* GPIO[7] Configration   D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG2_GPIO8		0x00000000					/* GPIO[8] Configration   D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO9		0x00000000					/* GPIO[9] Configration   D07:04 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO10	0x00000400					/* GPIO[10] Configration  D11:08 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO11	0x00004000					/* GPIO[11] Configration  D15:12 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO12	0x00040000					/* GPIO[12] Configration  D19:16 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO13	0x00400000					/* GPIO[13] Configration  D23:20 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO14	0x04000000					/* GPIO[14] Configration  D27:24 : Func0           */
#define	BBUS_GPIO_CONFIG2_GPIO15	0x40000000					/* GPIO[15] Configration  D31:28 : Func0           */

#define	BBUS_GPIO_CONFIG3_GPIO16	0x0000000b					/* GPIO[16] Configration  D03:00 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO17	0x000000b0					/* GPIO[17] Configration  D07:04 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO18	0x00000b00					/* GPIO[18] Configration  D11:08 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO19	0x0000b000					/* GPIO[19] Configration  D15:12 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO20	0x000b0000					/* GPIO[20] Configration  D19:16 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO21	0x00b00000				    /* GPIO[21] Configration  D23:20 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO22	0x0b000000					/* GPIO[22] Configration  D27:24 : Func3           */
#define	BBUS_GPIO_CONFIG3_GPIO23	0xb0000000					/* GPIO[23] Configration  D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG4_GPIO24	0x0000000b					/* GPIO[24] Configration  D03:00 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO25	0x000000b0					/* GPIO[25] Configration  D07:04 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO26	0x00000b00					/* GPIO[26] Configration  D11:08 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO27	0x0000b000					/* GPIO[27] Configration  D15:12 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO28	0x000b0000					/* GPIO[28] Configration  D19:16 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO29	0x00b00000					/* GPIO[29] Configration  D23:20 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO30	0x0b000000					/* GPIO[30] Configration  D27:24 : Func3           */
#define	BBUS_GPIO_CONFIG4_GPIO31	0xb0000000					/* GPIO[31] Configration  D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG5_GPIO32	0x00000003					/* GPIO[32] Configration  D03:00 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO33	0x00000030					/* GPIO[33] Configration  D07:04 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO34	0x00000300					/* GPIO[34] Configration  D11:08 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO35	0x00003000					/* GPIO[35] Configration  D15:12 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO36	0x000b0000					/* GPIO[36] Configration  D19:16 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO37	0x00b00000					/* GPIO[37] Configration  D23:20 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO38	0x0b000000					/* GPIO[38] Configration  D27:24 : Func3           */
#define	BBUS_GPIO_CONFIG5_GPIO39	0xb0000000					/* GPIO[39] Configration  D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG6_GPIO40	0x00000000					/* GPIO[40] Configration  D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG6_GPIO41	0x00000000					/* GPIO[41] Configration  D07:04 : Func0           */
#define	BBUS_GPIO_CONFIG6_GPIO42	0x00000200					/* GPIO[42] Configration  D11:08 : Func2           */
#define	BBUS_GPIO_CONFIG6_GPIO43	0x00002000					/* GPIO[43] Configration  D15:12 : Func2           */
#define	BBUS_GPIO_CONFIG6_GPIO44	0x00060000					/* GPIO[44] Configration  D19:16 : Func2           */
#define	BBUS_GPIO_CONFIG6_GPIO45	0x00200000					/* GPIO[45] Configration  D23:20 : Func2           */
#define	BBUS_GPIO_CONFIG6_GPIO46	0x0b000000					/* GPIO[46] Configration  D27:24 : Func3           */
#define	BBUS_GPIO_CONFIG6_GPIO47	0x30000000					/* GPIO[47] Configration  D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG7_GPIO48	0x00000000					/* GPIO[48] Configration  D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO49	0x00000000					/* GPIO[49] Configration  D07:04 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO50	0x00000000					/* GPIO[50] Configration  D11:08 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO51	0x00000000					/* GPIO[51] Configration  D15:12 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO52	0x00000000					/* GPIO[52] Configration  D19:16 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO53	0x00000000					/* GPIO[53] Configration  D23:20 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO54	0x00000000					/* GPIO[54] Configration  D27:24 : Func0           */
#define	BBUS_GPIO_CONFIG7_GPIO55	0x00000000					/* GPIO[55] Configration  D31:28 : Func0           */

#define	BBUS_GPIO_CONFIG8_GPIO56	0x00000000					/* GPIO[56] Configration  D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO57	0x00000000					/* GPIO[57] Configration  D07:04 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO58	0x00000000					/* GPIO[58] Configration  D11:08 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO59	0x00000000					/* GPIO[59] Configration  D15:12 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO60	0x00000000					/* GPIO[60] Configration  D19:16 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO61	0x00000000					/* GPIO[61] Configration  D23:20 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO62	0x00000000					/* GPIO[62] Configration  D27:24 : Func0           */
#define	BBUS_GPIO_CONFIG8_GPIO63	0x00000000					/* GPIO[63] Configration  D31:28 : Func0           */

#define	BBUS_GPIO_CONFIG9_GPIO64	0x00000000					/* GPIO[64] Configration  D03:00 : Func0           */
#define	BBUS_GPIO_CONFIG9_GPIO65	0x00000000					/* GPIO[65] Configration  D07:04 : Func3           */
#define	BBUS_GPIO_CONFIG9_GPIO66	0x00000000					/* GPIO[66] Configration  D11:08 : Func3           */
#define	BBUS_GPIO_CONFIG9_GPIO67	0x0000b000					/* GPIO[67] Configration  D15:12 : Func3           */
#define	BBUS_GPIO_CONFIG9_GPIO68	0x00020000					/* GPIO[68] Configration  D19:16 : Func1           */
#define	BBUS_GPIO_CONFIG9_GPIO69	0x00200000					/* GPIO[69] Configration  D23:20 : Func3           */
#define	BBUS_GPIO_CONFIG9_GPIO70	0x03000000					/* GPIO[70] Configration  D27:24 : Func3           */
#define	BBUS_GPIO_CONFIG9_GPIO71	0x30000000					/* GPIO[71] Configration  D31:28 : Func3           */

#define	BBUS_GPIO_CONFIG10_GPIO72	0x00000003					/* GPIO[72] Configration  D03:00 : Func3           */


#define	BBUS_GPIO_CONT_STS1_GPIO0		0x00000001				/* GPIO[0] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO1		0x00000002				/* GPIO[1] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO2		0x00000004				/* GPIO[2] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO3		0x00000008				/* GPIO[3] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO4		0x00000010				/* GPIO[4] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO5		0x00000020				/* GPIO[5] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO6		0x00000040				/* GPIO[6] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO7		0x00000080				/* GPIO[7] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO8		0x00000100				/* GPIO[8] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO9		0x00000200				/* GPIO[9] control/status bit                      */
#define	BBUS_GPIO_CONT_STS1_GPIO10		0x00000400				/* GPIO[10] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO11		0x00000800				/* GPIO[11] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO12		0x00001000				/* GPIO[12] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO13		0x00002000				/* GPIO[13] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO14		0x00004000				/* GPIO[14] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO15		0x00008000				/* GPIO[15] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO16		0x00010000				/* GPIO[16] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO17		0x00020000				/* GPIO[17] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO18		0x00040000				/* GPIO[18] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO19		0x00080000				/* GPIO[19] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO20		0x00100000				/* GPIO[20] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO21		0x00200000				/* GPIO[21] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO22		0x00400000				/* GPIO[22] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO23		0x00800000				/* GPIO[23] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO24		0x01000000				/* GPIO[24] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO25		0x02000000				/* GPIO[25] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO26		0x04000000				/* GPIO[26] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO27		0x08000000				/* GPIO[27] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO28		0x10000000				/* GPIO[28] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO29		0x20000000				/* GPIO[29] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO30		0x40000000				/* GPIO[30] control/status bit                     */
#define	BBUS_GPIO_CONT_STS1_GPIO31		0x80000000				/* GPIO[31] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO32		0x00000001				/* GPIO[32] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO33		0x00000002				/* GPIO[33] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO34		0x00000004				/* GPIO[34] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO35		0x00000008				/* GPIO[35] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO36		0x00000010				/* GPIO[36] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO37		0x00000020				/* GPIO[37] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO38		0x00000040				/* GPIO[38] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO39		0x00000080				/* GPIO[39] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO40		0x00000100				/* GPIO[40] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO41		0x00000200				/* GPIO[41] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO42		0x00000400				/* GPIO[42] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO43		0x00000800				/* GPIO[43] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO44		0x00001000				/* GPIO[44] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO45		0x00002000				/* GPIO[45] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO46		0x00004000				/* GPIO[46] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO47		0x00008000				/* GPIO[47] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO48		0x00010000				/* GPIO[48] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO49		0x00020000				/* GPIO[49] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO50		0x00040000				/* GPIO[50] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO51		0x00080000				/* GPIO[51] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO52		0x00100000				/* GPIO[52] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO53		0x00200000				/* GPIO[53] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO54		0x00400000				/* GPIO[54] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO55		0x00800000				/* GPIO[55] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO56		0x01000000				/* GPIO[56] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO57		0x02000000				/* GPIO[57] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO58		0x04000000				/* GPIO[58] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO59		0x08000000				/* GPIO[59] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO60		0x10000000				/* GPIO[60] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO61		0x20000000				/* GPIO[61] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO62		0x40000000				/* GPIO[62] control/status bit                     */
#define	BBUS_GPIO_CONT_STS2_GPIO63		0x80000000				/* GPIO[63] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO64		0x00000001				/* GPIO[64] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO65		0x00000002				/* GPIO[65] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO66		0x00000004				/* GPIO[66] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO67		0x00000008				/* GPIO[67] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO68		0x00000010				/* GPIO[68] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO69		0x00000020				/* GPIO[69] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO70		0x00000040				/* GPIO[70] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO71		0x00000080				/* GPIO[71] control/status bit                     */
#define	BBUS_GPIO_CONT_STS3_GPIO72		0x00000100				/* GPIO[72] control/status bit                     */

#define	BBUS_DMA_INT_STS_CH0			0x00000001				/* BBus DMA channel #1 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH1			0x00000002				/* BBus DMA channel #2 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH2			0x00000004				/* BBus DMA channel #3 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH3			0x00000008				/* BBus DMA channel #4 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH4			0x00000010				/* BBus DMA channel #5 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH5			0x00000020				/* BBus DMA channel #6 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH6			0x00000040				/* BBus DMA channel #7 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH7			0x00000080				/* BBus DMA channel #8 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH8			0x00000100				/* BBus DMA channel #9 interrupt status bit        */
#define	BBUS_DMA_INT_STS_CH9			0x00000200				/* BBus DMA channel #10 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH10			0x00000400				/* BBus DMA channel #11 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH11			0x00000800				/* BBus DMA channel #12 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH12			0x00001000				/* BBus DMA channel #13 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH13			0x00002000				/* BBus DMA channel #14 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH14			0x00004000				/* BBus DMA channel #14 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH15			0x00008000				/* BBus DMA channel #15 interrupt status bit       */
#define	BBUS_DMA_INT_STS_CH16			0x00010000				/* BBus DMA channel #16 interrupt status bit       */

#define	BBUS_DMA_INT_ENABLE_CH0			0x00000001				/* BBus DMA channel #1 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH1			0x00000002				/* BBus DMA channel #2 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH2			0x00000004				/* BBus DMA channel #3 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH3			0x00000008				/* BBus DMA channel #4 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH4			0x00000010				/* BBus DMA channel #5 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH5			0x00000020				/* BBus DMA channel #6 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH6			0x00000040				/* BBus DMA channel #7 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH7			0x00000080				/* BBus DMA channel #8 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH8			0x00000100				/* BBus DMA channel #9 interrupt enable bit        */
#define	BBUS_DMA_INT_ENABLE_CH9			0x00000200				/* BBus DMA channel #10 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH10		0x00000400				/* BBus DMA channel #11 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH11		0x00000800				/* BBus DMA channel #12 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH12		0x00001000				/* BBus DMA channel #13 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH13		0x00002000				/* BBus DMA channel #14 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH14		0x00004000				/* BBus DMA channel #14 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH15		0x00008000				/* BBus DMA channel #15 interrupt enable bit       */
#define	BBUS_DMA_INT_ENABLE_CH16		0x00010000				/* BBus DMA channel #16 interrupt enable bit       */


/*
 * Ethernet Control ans Status Registers
 */

/* address */
#define ETH_CONTROL_BASE    0xa0600000			     		 /* Ethernet Control ans Status Registers Base Address */

#define ETH_CONTROL_EGCR1	((UW volatile *) (ETH_CONTROL_BASE + 0x000))
#define ETH_CONTROL_EGCR2	((UW volatile *) (ETH_CONTROL_BASE + 0x004))
#define ETH_CONTROL_EGSR	((UW volatile *) (ETH_CONTROL_BASE + 0x008))
#define ETH_CONTROL_ETSR	((UW volatile *) (ETH_CONTROL_BASE + 0x018))
#define ETH_CONTROL_ERSR	((UW volatile *) (ETH_CONTROL_BASE + 0x01c))
#define ETH_CONTROL_MAC1	((UW volatile *) (ETH_CONTROL_BASE + 0x400))
#define ETH_CONTROL_MAC2	((UW volatile *) (ETH_CONTROL_BASE + 0x404))
#define ETH_CONTROL_IPGT	((UW volatile *) (ETH_CONTROL_BASE + 0x408))
#define ETH_CONTROL_IPGR	((UW volatile *) (ETH_CONTROL_BASE + 0x40c))
#define ETH_CONTROL_CLRT	((UW volatile *) (ETH_CONTROL_BASE + 0x410))
#define ETH_CONTROL_MAXF	((UW volatile *) (ETH_CONTROL_BASE + 0x414))
#define ETH_CONTROL_SUPP	((UW volatile *) (ETH_CONTROL_BASE + 0x418))
#define ETH_CONTROL_MCFG	((UW volatile *) (ETH_CONTROL_BASE + 0x420))
#define ETH_CONTROL_MCMD	((UW volatile *) (ETH_CONTROL_BASE + 0x424))
#define ETH_CONTROL_MADR	((UW volatile *) (ETH_CONTROL_BASE + 0x428))
#define ETH_CONTROL_MWTD	((UW volatile *) (ETH_CONTROL_BASE + 0x42c))
#define ETH_CONTROL_MRDD	((UW volatile *) (ETH_CONTROL_BASE + 0x430))
#define ETH_CONTROL_MIND	((UW volatile *) (ETH_CONTROL_BASE + 0x434))
#define ETH_CONTROL_SA1 	((UW volatile *) (ETH_CONTROL_BASE + 0x440))
#define ETH_CONTROL_SA2 	((UW volatile *) (ETH_CONTROL_BASE + 0x444))
#define ETH_CONTROL_SA3 	((UW volatile *) (ETH_CONTROL_BASE + 0x448))
#define ETH_CONTROL_SAFR	((UW volatile *) (ETH_CONTROL_BASE + 0x500))
#define ETH_CONTROL_HT1 	((UW volatile *) (ETH_CONTROL_BASE + 0x504))
#define ETH_CONTROL_HT2 	((UW volatile *) (ETH_CONTROL_BASE + 0x508))
#define ETH_CONTROL_STAT	((UW volatile *) (ETH_CONTROL_BASE + 0x680))
#define ETH_CONTROL_RXAPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa00))
#define ETH_CONTROL_RXBPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa04))
#define ETH_CONTROL_RXCPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa08))
#define ETH_CONTROL_RXDPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa0c))
#define ETH_CONTROL_EINTR 	((UW volatile *) (ETH_CONTROL_BASE + 0xa10))
#define ETH_CONTROL_EINTREN	((UW volatile *) (ETH_CONTROL_BASE + 0xa14))
#define ETH_CONTROL_TXPTR 	((UW volatile *) (ETH_CONTROL_BASE + 0xa18))
#define ETH_CONTROL_TXRPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa1c))
#define ETH_CONTROL_TXERBD	((UW volatile *) (ETH_CONTROL_BASE + 0xa20))
#define ETH_CONTROL_TXSPTR	((UW volatile *) (ETH_CONTROL_BASE + 0xa24))
#define ETH_CONTROL_RXAOFF	((UW volatile *) (ETH_CONTROL_BASE + 0xa28))
#define ETH_CONTROL_RXBOFF	((UW volatile *) (ETH_CONTROL_BASE + 0xa2c))
#define ETH_CONTROL_RXCOFF	((UW volatile *) (ETH_CONTROL_BASE + 0xa30))
#define ETH_CONTROL_RXDOFF	((UW volatile *) (ETH_CONTROL_BASE + 0xa34))
#define ETH_CONTROL_TXOFF 	((UW volatile *) (ETH_CONTROL_BASE + 0xa38))
#define ETH_CONTROL_RXFREE	((UW volatile *) (ETH_CONTROL_BASE + 0xa3c))
#define ETH_CONTROL_TXBD 	((UW volatile *) (ETH_CONTROL_BASE + 0x1000))


/*
 *  割込みハンドラのベクタ番号(割込み優先度)
 */
/*#define INHNO_SIO   INTLV_UART */
#define INHNO_SIO2_RX	57
#define INHNO_SIO2_TX	56
#define INHNO_SIO_RX	59
#define INHNO_SIO_TX	58
#define	INHNO_BBB		8
#define	INHNO_ETH_RX	5
#define	INHNO_ETH_TX	6
#define	INHNO_ETH_LINK	7
#define	INHNO_USB_HOST_D	25
#define	INHNO_USBDEV	26

#ifndef _MACRO_ONLY

/*
 * カーネル起動時用の初期化(sys_putcを使用するため)
 */
extern void uart_init(ID siopid);

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block {
	
	VP		pSraReg;           /* SRA Reg Address                 */
	VP		pFifoReg;          /* FIFO Reg Address                */
	VP		pCraReg;           /* CRA Reg Address                 */
	VP		pCrbReg;           /* CRB Reg Address                 */
	VP		pRctReg;           /* RCT Reg Address                 */
	VP		pRbtReg;           /* RBT Reg Address                 */
	VP		pBrgReg;           /* BRG Reg Address                 */
	UW		CraData;           /* CRA Reg Set Data                */
	UW		CrbData;           /* CRB Reg Set Data                */
	UW		RctData;           /* RCT Reg Set Data                */
	UW		RbtData;           /* RBT Reg Set Data                */
	UW		BrgData;           /* BRG Reg Set Data                */
	UW		BbbiTx;            /* BBUS Bridge TX INT Control      */
	UW		BbbiRx;            /* BBUS Bridge RX INT Control      */

} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block {
    const SIOPINIB  *siopinib; /* シリアルI/Oポート初期化ブロック */
    VP_INT          exinf;     /* 拡張情報 */
    BOOL    openflag;          /* オープン済みフラグ */
    BOOL    sendflag;          /* 送信割込みイネーブルフラグ */
    BOOL    getready;          /* 文字を受信した状態 */
    BOOL    putready;          /* 文字を送信できる状態 */
    UW		rxfdb;             /* rxbuf内の受信バイト数 */
    UW		rxbuf;             /* 受信バッファ */
	ID		siopid;
}SIOPCB;

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u      /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u      /* 受信通知コールバック */


/*
 *  オンチップのUARTからのポーリング出力
 */
extern void uart_putc(char c);

/*
 *  SIOドライバの初期化ルーチン
 */
extern void uart_initialize(void);


/*
 *  オープンしているポートがあるか？
 */
extern BOOL uart_openflag(ID siopid);


/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB   *uart_opn_por(ID siopid, VP_INT exinf);


/*
 *  シリアルI/Oポートのクローズ
 */
extern void uart_cls_por(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL uart_snd_chr(SIOPCB *siopcb, char c);


/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT  uart_rcv_chr(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  SIOの割込みサービスルーチン
 */
extern void uart_isr(void);


/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void uart_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void uart_ierdy_rcv(VP_INT exinf);

/*
 *  SIOの割込み許可ルーチン
 */
extern	void uart_TxRx_Enable(ID siopid);

/*
 *  SIOの割込み禁止ルーチン
 */
extern void uart_TxRx_Disable(ID siopid);

/*
 * 現在のシリアルポートの取得
 */
extern	ID uart_get_id(SIOPCB *siopcb);

#endif /* _MACRO_ONLY */



/*
 *  キャッシュ関連
 */
#ifndef _MACRO_ONLY

#define CACHE_TAG_RAM 0x11000000 /* W */

Inline void
cache_on(void)
{

}


Inline void
cache_off(void)
{

}


Inline void
cache_purge(void)
{

}


#endif /* _MACRO_ONLY */
#endif /* _NS9360_H_ */
