/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: c54x.h,v 1.6 2003/12/04 08:25:44 honda Exp $
 */

#ifndef _C54X_H_
#define _C54X_H_

/******************************************
 *  ＣＰＵ メモリ・マップド・レジスタ
 ******************************************/
#define IMR	0x0
#define IFR	0x1
#define ST0	0x6
#define ST1	0x7
#define AL	0x8
#define AH	0x9
#define AG	0xA
#define BL	0xB
#define BH	0xC
#define BG	0xD
#define T	0xE
#define TRN	0xF
#define AR0	0x10
#define AR1	0x11
#define AR2	0x12
#define AR3	0x13
#define AR4	0x14
#define AR5	0x15
#define AR6	0x16
#define AR7	0x17
#define SP	0x18
#define BK	0x19
#define BRC	0x1A
#define RSA	0x1B
#define REA	0x1C
#define PMST	0x1D
#define XPC	0x1E

/********************************************
 *   ペリフェラル メモリ・マップド・レジスタ
 *******************************************/
/*
 * タイマーレジスタ
 */
#define TMU0_TIM	0x24
#define TMU0_PRD	0x25
#define TMU0_TCR	0x26

#define TMU1_TIM	0x30
#define TMU1_PRD	0x31
#define TMU1_TCR	0x32

#define TCR_TSS 0x0010
#define TCR_TRB 0x0020

/*
 *   割り込みマスクレジスタ(IMR)ビットフィールド
 */
#define INT0           0
#define INT1           1
#define INT2           2
#define TINT0          3

#define RINT0          4
#define XINT0          5

#define TINT1          7

#define INT3           8
#define HPINT          9
#define RINT1         10
#define XINT1         11

#define DMAC0         6
#define DMAC1         7
#define DMAC2         10
#define DMAC3         11
#define DMAC4         12
#define DMAC5         13


/*
 *  割り込みハンドラ番号 INHNO
 */
#define RESET_VEC             0
#define NMI_VEC	              1
#define SINT17_VEC            2
#define SINT18_VEC            3
#define SINT19_VEC           4
#define SINT20_VEC        5
#define SINT21_VEC        6
#define SINT22_VEC        7
#define SINT23_VEC        8
#define SINT24_VEC        9
#define SINT25_VEC        10
#define SINT26_VEC        11
#define SINT27_VEC        12
#define SINT28_VEC        13
#define SINT29_VEC        14
#define SINT30_VEC        15
#define INT0_VEC        16
#define INT1_VEC        17
#define INT2_VEC        18
#define TINT0_VEC        19
#define RINT0_VEC        20
#define XINT0_VEC        21
#define DMAC0_VEC        22
#define TINT1_VEC        23
#define INT3_VEC        24
#define HPI_VEC            25
#define RINT1_VEC        26
#define XINT1_VEC        27
#define DMAC2_VEC        26
#define DMAC3_VEC        27
#define DMAC4_VEC        28
#define DMAC5_VEC        29

/*
 *  TRAP番号
 */
#define RS_TRAP           0
#define NMI_TRAP       1
#define INT0_TRAP       16
#define INT1_TRAP       17
#define INT2_TRAP       18
#define TINT_TRAP       19
#define RINT0_TRAP       20
#define XINT0_TRAP       21
#define DMAC0_TRAP       22
#define DMAC1_TRAP       23
#define INT3_TRAP       24
#define HPINT_TRAP       25
#define RINT1_TRAP       26
#define DMAC2_TRAP       26
#define XINT1_TRAP       27
#define DMAC3_TRAP       27
#define DMAC4_TRAP       28
#define DMAC5_TRAP       29

#define SINTR        0
#define SINT16        1
#define SINT17        2
#define SINT18        3
#define SINT19        4
#define SINT20        5
#define SINT21        6
#define SINT22        7
#define SINT23        8
#define SINT24        9
#define SINT25        10
#define SINT26        11
#define SINT27        12
#define SINT28        13
#define SINT29        14
#define SINT30        15
#define SINT0        16
#define SINT1        17
#define SINT2        18
#define SINT3        19
#define SINT4        20
#define SINT5        21
#define SINT6        22
#define SINT7        23
#define SINT8        24
#define SINT9        25
#define SINT10        26
#define SINT11        27
#define SINT12          28
#define SINT13          29

#endif /* _C54X_H_ */
