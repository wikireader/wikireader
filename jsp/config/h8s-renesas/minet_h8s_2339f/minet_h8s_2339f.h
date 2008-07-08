/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#ifndef _MINET_H8S_2339F_H_
#define _MINET_H8S_2339F_H_

/*
 *  MiSPO MiNET-H8S/2339F用定義
 */

#define MINET_H8S_2339F

/*
 *  SYSCRの初期値
 *  　・INTM1=1,INTM0=0：割込み制御モード２
 *  　・NMIEG=0：NMI入力の立下りエッジで割り込み要求を発生
 *  　・IRQPAS：IRQ入力端子セレクト
 *  　・RAME=1：内蔵RAM有効
 */
#define	SYS_SYSCR	(INT_MODE2 | RAME)

/*
 *  CPU クロック
 */
#define CPU_CLOCK	24576			/* kHz単位 （24.576MHz）*/

/*
 *  タイマに供給されるクロック (内部クロックφ/4でカウント)
 */
#define TIMER_CLOCK	((CPU_CLOCK)/4)		/* kHz単位 */

/*
 *  シリアルポートのBRRレジスタ値の定義
 *    計算式を用いる方法もあるが、誤差の判定が必要なので、
 *　　個別にマクロ定義している。
 *　　BRR_RATEマクロはマニュアル上のNに対応する。
 *　　いずれもn=0である。
 *　　BAUD_RATEはuser_config.hで定義する。
 */
#if CPU_CLOCK != 24576
#error CPU_CLOCK is invalid value.
#else /*  CPU_CLOCK != 24576  */

/*  SCI0  */
#if BAUD_RATE0 == 9600		/* ビットレート： 9600[bps] */
	#define BRR0_RATE	79
#elif BAUD_RATE0 == 19200	/* ビットレート：19200[bps] */
	#define BRR0_RATE	39
#elif BAUD_RATE0 == 38400	/* ビットレート：38400[bps] */
	#define BRR0_RATE	19
#endif /* BAUD_RATE0 == 9600  */

/*  SCI1  */
#if BAUD_RATE1 == 9600		/* ビットレート： 9600[bps] */
	#define BRR1_RATE	79
#elif BAUD_RATE1 == 19200	/* ビットレート：19200[bps] */
	#define BRR1_RATE	39
#elif BAUD_RATE1 == 38400	/* ビットレート：38400[bps] */
	#define BRR1_RATE	19
#endif /* BAUD_RATE1 == 9600  */

/*  SCI1  */
#if BAUD_RATE2 == 9600		/* ビットレート： 9600[bps] */
	#define BRR2_RATE	79
#elif BAUD_RATE2 == 19200	/* ビットレート：19200[bps] */
	#define BRR2_RATE	39
#elif BAUD_RATE2 == 38400	/* ビットレート：38400[bps] */
	#define BRR2_RATE	19
#endif /* BAUD_RATE1 == 9600  */

/*
 *  SCRレジスタのCKS0,CKS1ビット
 *　　ボーレート計算式のn=0に対応
 *　　ボーレート毎に値が異なる場合があるので注意
 */
#define SCR0_CKS		0	/*  SCI0  */
#define SCR1_CKS		0	/*  SCI1  */
#define SCR2_CKS		0	/*  SCI2  */

#endif /* CPU_CLOCK != 24576 */

/*
 *  DDRレジスタの初期値の定義
 */
#define P1DDR0		0xf0
#define P2DDR0		DUMMY
#define P3DDR0		DUMMY
/*  ポート４はDDRがないため、欠番  */
#define P5DDR0		DUMMY
#define P6DDR0		0xc3
#define P7DDR0		DUMMY
#define P8DDR0		DUMMY
#define P9DDR0		0xe0
#define PADDR0		0xff
#define PBDDR0		0xff
#define PCDDR0		0xff
#define PDDDR0		DUMMY
#define PEDDR0		DUMMY
#define PFDDR0		0xff
#define PGDDR0		0x1f




#endif /* _MINET_H8S_2339F_H_ */
