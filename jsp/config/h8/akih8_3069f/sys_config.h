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
 *  Copyright (C) 2001-2004 by Kunihiko Ohnaka
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
#include <h8_3069f.h>

/*
 *  起動メッセージのターゲット名
 */

#define	TARGET_NAME	"AKI-H8/3069F"


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
#ifdef REDBOOT
#define STACKTOP    	(0x50fff0)
#else	/*  REDBOOT  */
#define STACKTOP    	(H8IN_RAM_BASE + H8IN_RAM_SIZE)
#endif	/*  #ifdef REDBOOT  */

#define str_STACKTOP	_TO_STRING(STACKTOP)

/* RedbootがRAM上に持つベクタテーブルの先頭アドレス */
#define VECTOR_TABLE_ADDR 	0x00fffd20

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
 *  サポートするシリアルディバイスの数は最大 3。
 *  
 *  サンプルプログラムを動かす場合は
 *  sys_defs.hにあるTASK_PORTIDの定義にも注意
 */
#define TNUM_PORT	2

#define	CONSOLE_PORTID		SYSTEM_PORTID	/* コンソールに用いるシリアルポート番号     */
#define	LOGTASK_PORTID		SYSTEM_PORTID	/* システムログを出力するシリアルポート番号 */

#define H8_MIN_BAUD_RATE	9600		/* SCI をクローズする前の待ち時間の計算に使用する。*/

/* エラー割り込みを、入力割り込みと別に操作する場合はコメントをはずす。*/
#if 0
#define H8_CFG_SCI_ERR_HANDLER
#endif

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

#define SYSTEM_BAUD_RATE	38400			/* bps	*/

#define	SYSTEM_PORTID		1

#define INHNO_SERIAL_IN		IRQ_RXI1
#define INHNO_SERIAL_OUT	IRQ_TXI1
#define INHNO_SERIAL_ERR	IRQ_ERI1

#if TNUM_PORT >= 2

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

#define USER_BAUD_RATE		38400			/* bps	*/

#define	USER_PORTID		2

#define INHNO_SERIAL2_IN		IRQ_RXI0
#define INHNO_SERIAL2_OUT	IRQ_TXI0
#define INHNO_SERIAL2_ERR	IRQ_ERI0

#endif	/* of #if TNUM_PORT >= 2 */


#if TNUM_PORT >= 3

#define USER2_SCI		H8SCI2

#define USER2_SCI_IPR		H8IPRB
#define USER2_SCI_IP_BIT		H8IPR_SCI2_BIT

#define USER2_SCI_SMR		0
			/* 送受信フォーマット			*/
	     		/* 調歩同期式				*/
	     		/* キャラクタレングス：8ビット		*/
	     		/* パリティなし				*/
	     		/* ストップビットレングス：1		*/
	     		/* クロックセレクト（分周比）:1		*/

#define USER2_BAUD_RATE		38400			/* bps	*/

#define	USER2_PORTID		3

#define INHNO_SERIAL3_IN		IRQ_RXI2
#define INHNO_SERIAL3_OUT	IRQ_TXI2
#define INHNO_SERIAL3_ERR	IRQ_ERI2

#endif	/* of #if TNUM_PORT >= 3 */



/*  プライオリティレベル設定用のデータ  */

/*  割込み要求のレベル設定  */
#define SYSTEM_SCI_IPM			IPM_LEVEL0
#define USER_SCI_IPM			IPM_LEVEL0
#define USER2_SCI_IPM			IPM_LEVEL0

/*  
 * 割込みハンドラ実行中の割込みマスクの値
 * 　　他の割込みをマスクするための設定  
 * 　　自分と同じレベルの割込み要求をブロックするため、
 * 　　上記の割込み要求レベルより１つ高いレベルに設定する。
 */

/*  ポート1：システム・ポート  */
#if SYSTEM_SCI_IPM == IPM_LEVEL0
#define sio_in_handler_intmask		IPM_LEVEL1
#elif SYSTEM_SCI_IPM == IPM_LEVEL1
#define sio_in_handler_intmask		IPM_LEVEL2
#endif 	/* SYSTEM_SCI_IPM == IPM_LEVEL0 */


/*  ポート2：ユーザー・ポート  */
#if TNUM_PORT >= 2
#if USER_SCI_IPM == IPM_LEVEL0
#define sio_in2_handler_intmask		IPM_LEVEL1
#elif USER_SCI_IPM == IPM_LEVEL1
#define sio_in2_handler_intmask		IPM_LEVEL2
#endif 	/* USER_SCI_IPM == IPM_LEVEL0 */
#endif	/* of #if TNUM_PORT >= 2 */

/*  ポート3：ユーザー・ポート2  */
#if TNUM_PORT >= 3
#if USER2_SCI_IPM == IPM_LEVEL0
#define sio_in3_handler_intmask		IPM_LEVEL1
#elif USER2_SCI_IPM == IPM_LEVEL1
#define sio_in3_handler_intmask		IPM_LEVEL2
#endif 	/* USER2_SCI_IPM == IPM_LEVEL0 */
#endif	/* of #if TNUM_PORT >= 3 */


#define sio_out_handler_intmask		sio_in_handler_intmask
#define sio_err_handler_intmask		sio_in_handler_intmask
#define sio_out2_handler_intmask	sio_in2_handler_intmask
#define sio_err2_handler_intmask	sio_in2_handler_intmask
#define sio_out3_handler_intmask	sio_in3_handler_intmask
#define sio_err3_handler_intmask	sio_in3_handler_intmask


/*
 *  タイマの設定
 */

#define SYSTEM_TIMER		H816TU0

#define SYSTEM_TIMER_CNT	(SYSTEM_TIMER + H8TCNT)
#define SYSTEM_TIMER_TCR	(SYSTEM_TIMER + H8TCR)
#define SYSTEM_TIMER_TIOR	(SYSTEM_TIMER + H8TIOR)
#define SYSTEM_TIMER_IER	H816TU_TISRA
#define SYSTEM_TIMER_IFR	H816TU_TISRA
#define SYSTEM_TIMER_TSTR	H816TU_TSTR
#define SYSTEM_TIMER_GR		(SYSTEM_TIMER + H8GRA)

#define SYSTEM_TIMER_STR	H8TSTR_STR0
#define SYSTEM_TIMER_STR_BIT	H8TSTR_STR0_BIT
#define SYSTEM_TIMER_IE		H8TISRA_IMIEA0		/* interrupt mask */
#define SYSTEM_TIMER_IE_BIT	H8TISRA_IMIEA0_BIT
#define SYSTEM_TIMER_IF		H8TISRA_IMFA0		/* match flag */
#define SYSTEM_TIMER_IF_BIT	H8TISRA_IMFA0_BIT

#define INHNO_TIMER		IRQ_IMIA0

#define SYSTEM_TIMER_TCR_BIT	(H8TCR_CCLR0 | H8TCR_TPSC1 | H8TCR_TPSC0)
#define SYSTEM_TIMER_TIOR_BIT	(0)

#define TIMER_CLOCK		((CPU_CLOCK)/8000)	
				/* 20MHz / 8 = 2.5MHz = 2500KHz */

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

#define	ENABLE_P8_CS	(H8P8DDR_CS0|H8P8DDR_CS1|H8P8DDR_CS2|H8P8DDR_CS3)

#if 0

#define	ENABLE_LOWER_DATA
#define	ENABLE_PB_CS	(H8PADDR_CS4|H8PADDR_CS5|H8PADDR_CS6|H8PBDDR_CS7)

#endif	/* of #if 0 */


/*
 *  微少時間待ちのための定義
 */

#if defined(ROM)

/*  ROM化（外部RAM使用）  */
#define	SIL_DLY_TIM1	 6880
#define	SIL_DLY_TIM2	  546

#elif defined(INMEM_ONLY)

/*  ROM化（内蔵メモリのみ使用） */
#define	SIL_DLY_TIM1	 2800
#define	SIL_DLY_TIM2	  521

#else	/* of #if defined(ROM) */

/*  簡易モニタ（ユーザープログラムは外部RAM上にダウンロード）  */
#define	SIL_DLY_TIM1	16150
#define	SIL_DLY_TIM2	 3161

#endif	/* of #if defined(ROM) */


/*
 *  DDRの初期値の定義
 */
#ifdef INMEM_ONLY	/*  ROM化かつ外部DRAMなし  */

/*  モードによってはDDRの初期値が異なるので、注意  */
#define	H8P1DDR0        0x00				/*  ポート1  */
#define	H8P2DDR0        0x00				/*  ポート2  */
#define	H8P3DDR0        0x00				/*  ポート3  */
#define	H8P4DDR0        0x00				/*  ポート4  */
#define	H8P5DDR0        0x00				/*  ポート5  */
#define	H8P6DDR0        0x00				/*  ポート6  */

/*  ポート7は入力専用でDDRレジスタがないため、省略している。  */

#ifdef SUPPORT_ETHER
#define	H8P8DDR0        (H8P8DDR_CS1 | H8P8DDR_CS2)	/*  ポート8  */
#else 	/* SUPPORT_ETHER */
#define	H8P8DDR0        0x00				/*  ポート8  */
#endif 	/* SUPPORT_ETHER */

#define	H8P9DDR0        0x00				/*  ポート9  */
#define	H8PADDR0        0x00				/*  ポートA  */
#define	H8PBDDR0        0x00				/*  ポートB  */

#else 	/*  of #ifdef INMEM_ONLY  */

#define	H8P1DDR0        0xff				/*  ポート1  */
#define	H8P2DDR0        0xff				/*  ポート2  */
#define	H8P3DDR0        0xff				/*  ポート3  */
#define	H8P4DDR0        DUMMY				/*  ポート4  */
#define	H8P5DDR0        H8P5DDR_A16			/*  ポート5  */
#define	H8P6DDR0        DUMMY				/*  ポート6  */

/*  ポート7は入力専用でDDRレジスタがないため、省略している。  */

#define	H8P8DDR0        (H8P8DDR_CS1 | H8P8DDR_CS2)	/*  ポート8  */
#define	H8P9DDR0        DUMMY				/*  ポート9  */
#define	H8PADDR0        DUMMY				/*  ポートA  */
#define	H8PBDDR0        H8PBDDR_UCAS			/*  ポートB  */

#endif 	/*  of #ifdef INMEM_ONLY  */


#ifdef REDBOOT
/*
 *  割込みプライオリティの初期値の定義
 *　　Redbootの場合、IRQ5を使用する。
 */
#define H8IPRA_INI	H8IPR_IRQ5_BIT

/*
 *  Redbootが使用する割込みベクタの退避と復元
 */

/* ベクタテーブルの内、trapa命令用に確保しているチャネル数 */
#define TRAP_VECTOR_SIZE	4

#ifndef _MACRO_ONLY
/*  退避スペースのデータ型定義  */
typedef struct {
	UW trap_vector[TRAP_VECTOR_SIZE];
	UW irq5;
} TMP_VECTOR;

/*  割込みベクタの退避  */
extern void load_vector(TMP_VECTOR *p);
/*  割込みベクタの復元  */
extern void save_vector(TMP_VECTOR *p);

#endif	/*  _MACRO_ONLY  */

#endif	/* of #ifdef REDBOOT */


#endif /* _SYS_CONFIG_H_ */
