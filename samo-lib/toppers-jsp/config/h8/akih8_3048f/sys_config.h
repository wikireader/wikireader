/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: sys_config.h,v 1.16 2007/03/23 07:22:15 honda Exp $
 */

/*
 *　ターゲットシステム依存モジュール
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */

#include <sys_rename.h>

/*
 *  ターゲットシステムのハードウェア資源の定義
 */
#include <h8.h>
#include <h8_3048f.h>

/*
 *  起動メッセージのターゲット名
 */

#define	TARGET_NAME	"AKI-H8/3048F"

/*
 *  chg_ipmをサポートするかどうかの定義
 */
#define SUPPORT_CHG_IPM

/*
 *  vxget_tim をサポートするかどうかの定義
 */
#define	SUPPORT_VXGET_TIM

/*
 *   スタック領域の定義
 */

			/* 非タスクコンテキスト用スタックの初期値 */
#define STACKTOP    	(H8IN_RAM_BASE + H8IN_RAM_SIZE)	
#define str_STACKTOP	_TO_STRING(STACKTOP)

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */

extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ呼出しで実現することを想定し
 *  ている．
 */

extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ呼出しで実現するこ
 *  とを想定している．
 */

extern void cpu_putc(char c);

Inline void
sys_putc(char c)
{
	cpu_putc(c);
};

#endif /* _MACRO_ONLY */

/*
 *  (1) シリアルポートの設定
 *  (2) タイマーの設定
 *  (3) 外部アドレス空間制御
 */

/*
 *  サポートするシリアルディバイスの数は最大 2
 *  
 *  サンプルプログラムを動かす場合は
 *  sys_defs.hにあるTASK_PORTIDの定義にも注意
 */
#define TNUM_PORT		2

/*
 *  SCI 共通定数、マクロ
 */

#define	CONSOLE_PORTID		SYSTEM_PORTID	/* コンソールに用いるシリアルポート番号		*/
#define	LOGTASK_PORTID		SYSTEM_PORTID	/* システムログを出力するシリアルポート番号	*/

#define H8_MIN_BAUD_RATE	9600		/* SCI をクローズする前の待ち時間の計算に使用する。*/

/* エラー割り込みを、入力割り込みと別に操作する場合はコメントをはずす。*/
/*#define H8_CFG_SCI_ERR_HANDLER*/

/*
 *  SYSTEM 用 SCI の設定値
 */

#define SYSTEM_SCI		H8SCI1

#define SYSTEM_SCI_IPR		H8IPRB
#define SYSTEM_SCI_IP_BIT	H8IPR_SCI1_BIT

#define SYSTEM_SCI_SMR		0
			/* 送受信フォーマット			*/
	     		/* 調歩同期式				*/
	     		/* キャラクタレングス：8ビット		*/
	     		/* パリティなし				*/
	     		/* ストップビットレングス：1		*/
	     		/* クロックセレクト（分周比）:1		*/

#if TNUM_PORT == 1

#define SYSTEM_BAUD_RATE	38400			/* bps	*/

#define	SYSTEM_PORTID		1

#define INHNO_SERIAL_IN		IRQ_RXI1
#define INHNO_SERIAL_OUT	IRQ_TXI1
#define INHNO_SERIAL_ERR	IRQ_ERI1

#elif TNUM_PORT == 2	/* of #if TNUM_PORT == 1 */

#define USER_SCI		H8SCI0

#define USER_SCI_IPR		H8IPRB
#define USER_SCI_IP_BIT		H8IPR_SCI0_BIT

#define USER_SCI_SMR		0
			/* 送受信フォーマット			*/
	     		/* 調歩同期式				*/
	     		/* キャラクタレングス：8ビット		*/
	     		/* パリティなし				*/
	     		/* ストップビットレングス：1		*/
	     		/* クロックセレクト（分周比）:1		*/

#ifdef SUPPORT_PPP

#define USER_BAUD_RATE		9600			/* bps	*/

#else	/* of #ifdef SUPPORT_PPP */

#define USER_BAUD_RATE		38400			/* bps	*/

#endif	/* of #ifdef SUPPORT_PPP */

#define SYSTEM_BAUD_RATE	38400			/* bps	*/

#define	USER_PORTID		1
#define	SYSTEM_PORTID		2

#define INHNO_SERIAL_IN		IRQ_RXI0
#define INHNO_SERIAL_OUT	IRQ_TXI0
#define INHNO_SERIAL_ERR	IRQ_ERI0

#define INHNO_SERIAL2_IN	IRQ_RXI1
#define INHNO_SERIAL2_OUT	IRQ_TXI1
#define INHNO_SERIAL2_ERR	IRQ_ERI1

#else	/* of #if TNUM_PORT == 1 */

#error TNUM_PORT <= 2

#endif	/* of #if TNUM_PORT == 1 */

/*  プライオリティレベル設定用のデータ  */

/*  割込み要求のレベル設定  */
#define SYSTEM_SCI_IPM			IPM_LEVEL0
#define USER_SCI_IPM			IPM_LEVEL0

/*  
 * 割込みハンドラ実行中の割込みマスクの値
 * 　　他の割込みをマスクするための設定  
 * 　　自分と同じレベルの割込み要求をブロックするため、
 * 　　上記の割込み要求レベルより１つ高いレベルに設定する。
 */
#if TNUM_PORT == 1	/*  ポート１：システム・ポート  */

/*  システム・ポート  */
#if SYSTEM_SCI_IPM == IPM_LEVEL0
#define sio_in_handler_intmask		IPM_LEVEL1
#elif SYSTEM_SCI_IPM == IPM_LEVEL1
#define sio_in_handler_intmask		IPM_LEVEL2
#endif 	/* SYSTEM_SCI_IPM == IPM_LEVEL0 */


#elif TNUM_PORT == 2	/* of #if TNUM_PORT == 1 */
			/*  ポート１：ユーザー・ポート  */
			/*  ポート２：システム・ポート  */
/*  ユーザー・ポート  */
#if USER_SCI_IPM == IPM_LEVEL0
#define sio_in_handler_intmask		IPM_LEVEL1
#elif USER_SCI_IPM == IPM_LEVEL1
#define sio_in_handler_intmask		IPM_LEVEL2
#endif 	/* USER_SCI_IPM == IPM_LEVEL0 */

/*  システム・ポート  */
#if SYSTEM_SCI_IPM == IPM_LEVEL0
#define sio_in2_handler_intmask		IPM_LEVEL1
#elif SYSTEM_SCI_IPM == IPM_LEVEL1
#define sio_in2_handler_intmask		IPM_LEVEL2
#endif 	/* SYSTEM_SCI_IPM == IPM_LEVEL0 */

#endif	/* of #if TNUM_PORT == 1 */

#define sio_out_handler_intmask		sio_in_handler_intmask
#define sio_err_handler_intmask		sio_in_handler_intmask
#define sio_out2_handler_intmask	sio_in2_handler_intmask
#define sio_err2_handler_intmask	sio_in2_handler_intmask


/*
 *  タイマの設定
 */

#define SYSTEM_TIMER		H8ITU0

#define SYSTEM_TIMER_CNT	(SYSTEM_TIMER + H8TCNT)
#define SYSTEM_TIMER_TCR	(SYSTEM_TIMER + H8TCR)
#define SYSTEM_TIMER_TIOR	(SYSTEM_TIMER + H8TIOR)
#define SYSTEM_TIMER_IER	(SYSTEM_TIMER + H8TIER)
#define SYSTEM_TIMER_IFR	(SYSTEM_TIMER + H8TSR)
#define SYSTEM_TIMER_TSTR	H8ITU_TSTR
#define SYSTEM_TIMER_GR		(SYSTEM_TIMER + H8GRA)

#define SYSTEM_TIMER_STR	H8TSTR_STR0
#define SYSTEM_TIMER_STR_BIT	H8TSTR_STR0_BIT
#define SYSTEM_TIMER_IE		H8TIER_IMIEA		/* interrupt mask */
#define SYSTEM_TIMER_IE_BIT	H8TIER_IMIEA_BIT
#define SYSTEM_TIMER_IF		H8TSR_IMIFA		/* match flag */
#define SYSTEM_TIMER_IF_BIT	H8TSR_IMIFA_BIT

#define INHNO_TIMER		IRQ_IMIA0

#define SYSTEM_TIMER_TCR_BIT	(H8TCR_CCLR0 | H8TCR_TPSC1 | H8TCR_TPSC0)
#define SYSTEM_TIMER_TIOR_BIT	(0)

#define TIMER_CLOCK		((CPU_CLOCK)/8000)
				/* 16MHz / 8 = 2MHz = 2000KHz */

/*  プライオリティレベル設定用のデータ  */

/*  割込み要求のレベル設定  */
#define SYSTEM_TIMER_IPR                H8IPRA
#define SYSTEM_TIMER_IP_BIT             H8IPR_ITU0_BIT
#define SYSTEM_TIMER_IPM                IPM_LEVEL1

/*  
 * 割込みハンドラ実行中の割込みマスクの値
 * 　　他の割込みをマスクするための設定  
 * 　　自分と同じレベルの割込み要求をブロックするため、
 * 　　上記の割込み要求レベルより１つ高いレベルに設定する。
 */
#if SYSTEM_TIMER_IPM == IPM_LEVEL0
#define timer_handler_intmask		IPM_LEVEL1
#elif SYSTEM_TIMER_IPM == IPM_LEVEL1
#define timer_handler_intmask		IPM_LEVEL2
#endif 	/* SYSTEM_TIMER_IPM == IPM_LEVEL0 */



/*
 *  外部アドレス空間制御
 */

#if 0
#define ENABLE_LOWER_DATA
#define ENABLE_P8_CS	(H8P8DDR_CS0|H8P8DDR_CS1|H8P8DDR_CS2|H8P8DDR_CS3)
#define ENABLE_PA_CS	(H8PADDR_CS4|H8PADDR_CS5|H8PADDR_CS6)
#define ENABLE_PB_CS 	H8PBDDR_CS7

#define ENABLE_PA_A21_A23	(H8BRCR_A23E|H8BRCR_A22E|H8BRCR_A21E)

#endif


/*
 *  微少時間待ちのための定義
 */

#ifdef ROM

#define	SIL_DLY_TIM1	 9400
#define	SIL_DLY_TIM2	  701

#else	/* of #ifdef ROM */

#define	SIL_DLY_TIM1	24900
#define	SIL_DLY_TIM2	 5260

#endif	/* of #ifdef ROM */

/*
 *  DDRの初期値の定義
 */
#define	H8P1DDR0        0xff				/*  ポート1  */
#define	H8P2DDR0        0xff				/*  ポート2  */
#define	H8P3DDR0        0xff				/*  ポート3  */

#ifdef ENABLE_LOWER_DATA
#define	H8P4DDR0        0xff				/*  ポート4  */
#else	/* #if ENABLE_LOWER_DATA */
#define	H8P4DDR0        DUMMY				/*  ポート4  */
#endif	/* #if ENABLE_LOWER_DATA */

#define	H8P5DDR0        0xff				/*  ポート5  */
#define	H8P6DDR0        DUMMY				/*  ポート6  */

/*  ポート7は入力専用でDDRレジスタがないため、省略している。  */

#ifdef ENABLE_P8_CS
#define	H8P8DDR0        ENABLE_P8_CS			/*  ポート8  */
#else	/* #ifdef ENABLE_P8_CS */
#define	H8P8DDR0        DUMMY				/*  ポート8  */
#endif	/* #ifdef ENABLE_P8_CS */

#define	H8P9DDR0        DUMMY				/*  ポート9  */

#ifdef ENABLE_PA_CS
#define	H8PADDR0        ENABLE_PA_CS			/*  ポートA  */
#elif defined(ENABLE_PA_A21_A23)
#define	H8PADDR0        ENABLE_PA_A21_A23		/*  ポートA  */
#else
#define	H8PADDR0        DUMMY				/*  ポートA  */
#endif	/* #ifdef ENABLE_PA_CS */

#ifdef ENABLE_PB_CS
#define	H8PBDDR0        ENABLE_PB_CS			/*  ポートB  */
#else	/* #ifdef ENABLE_PB_CS */
#define	H8PBDDR0        DUMMY				/*  ポートB  */
#endif	/* #ifdef ENABLE_PB_CS */

#endif /* _SYS_CONFIG_H_ */
