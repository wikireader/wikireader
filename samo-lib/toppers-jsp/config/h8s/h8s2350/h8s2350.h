/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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

#ifndef _H8S_2350_H_
#define _H8S_2350_H_

/*
 *  H8S/2350 用定義
 */
/*
 *  モジュールストップコントロールレジスタ
 *  (様々なモジュールの ON、OFF に使っているので、ここで定義する。)
 */
#define MSTPCR		0xff3c	/* モジュールストップコントロールレジスタ */

/*
 *  I/O ポート
 */

/* 各ポートレジスタのアドレス */

/* port1 */
#define P1DDR		0xfeb0
#define P1DR		0xff60
#define PORT1		0xff50

/* port2 */
#define P2DDR		0xfeb1
#define P2DR		0xff61
#define PORT2		0xff51

/* port3 */
#define P3DDR		0xfeb2
#define P3DR		0xff62
#define PORT3		0xff52
#define P3ODR		0xff76

/* port4 */
#define PORT4		0xff53

/* port5 */
#define P5DDR		0xfeb4
#define P5DR		0xff64
#define PORT5		0xff54

/* port6 */
#define P6DDR		0xfeb5
#define P6DR		0xff65
#define PORT6 		0xff55

/*  portA  */
#define PADDR		0xfeb9
#define PADR		0xff69
#define PORTA		0xff59

/*  portE  */
#define PEDDR		0xfebd
#define PEDR		0xff6d
#define PORTE		0xff5d
#define PEPCR		0xff74

/*  portF  */
#define PFDDR		0xfebe
#define PFDR		0xff6e
#define PORTF		0xff5e

/*  portG  */
#define PGDDR		0xfebf	/* ポートＧデータディスクリプションレジスタ */
#define PGDR		0xff6f	/* ポートＧデータレジスタ */
#define PORTG		0xff5f	/* ポートＧレジスタ */

/* 各レジスタのビットパターン */

/* P2DDR */
#define P27DDR		BIT7
#define P26DDR		BIT6
#define P25DDR		BIT5
#define P24DDR		BIT4
#define P23DDR		BIT3
#define P22DDR		BIT2
#define P21DDR		BIT1
#define P20DDR		BIT0

/* P3DDR */
#define P37DDR		BIT7
#define P36DDR		BIT6
#define P35DDR		BIT5
#define P34DDR		BIT4
#define P33DDR		BIT3
#define P32DDR		BIT2
#define P31DDR		BIT1
#define P30DDR		BIT0

/* PGDDR */
#define PG4DDR		BIT4
#define PG3DDR		BIT3
#define PG2DDR		BIT2
#define PG1DDR		BIT1
#define PG0DDR		BIT0

#endif /* _H8S_2350_H_ */
