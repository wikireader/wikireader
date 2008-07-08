/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006 by Monami Software Limited Partnership, JAPAN	
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: cpu_defs.h,v 1.8 2007/05/30 03:56:47 honda Exp $
 */

/*
 *	プロセッサに依存する定義（M32R用）
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

#define M32R

/* 例外要因番号 */
#define NUM_EXCEPTION	20

#define EXC_RESET	0
#define EXC_SBI		1
#define EXC_RIE		2
#define EXC_AE		3
#define EXC_TRAP00	4
#define EXC_TRAP01	5
#define EXC_TRAP02	6
#define EXC_TRAP03	7
#define EXC_TRAP04	8
#define EXC_TRAP05	9
#define EXC_TRAP06	10
#define EXC_TRAP07	11
#define EXC_TRAP08	12
#define EXC_TRAP09	13
#define EXC_TRAP10	14
#define EXC_TRAP11	15
#define EXC_TRAP12	16
#define EXC_TRAP13	17
#define EXC_TRAP14	18
#define EXC_TRAP15	19

/* 割込み要因番号 */
#define INT_INT0	1
#define INT_INT1	2
#define INT_INT2	3
#define INT_INT3	4
#define INT_INT4	5
#define INT_INT5	6
#define INT_INT6	7
#define INT_MFT0	16
#define INT_MFT1	17
#define INT_MFT2	18
#define INT_MFT3	19
#define INT_MFT4	20
#define INT_MFT5	21
#define INT_DMA0	32
#define INT_SIO0RCV	48
#define INT_SIO0XMT	49
#define INT_SIO1RCV	50
#define INT_SIO1XMT	51
#define INT_SIO2RCV	52
#define INT_SIO2XMT	53
#define INT_SIO3RCV	54
#define INT_SIO3XMT	55
#define INT_SIO4RCV	56
#define INT_SIO4XMT	57


#ifndef _MACRO_ONLY

typedef unsigned int   INHNO;		/* 割込みハンドラ番号  */
typedef unsigned char  IPR;			/* 割込み優先度        */
typedef unsigned int   EXCNO;		/* CPU例外ハンドラ番号 */

/* カーネル起動時のメッセージ */
#define COPYRIGHT_CPU \
"Copyright (C) 2006 by Monami Software Limited Partnership, JAPAN\n" \
"Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory\n" \
"            Graduate School of Information Science, Nagoya Univ., JAPAN\n"

#endif /* _MACRO_ONLY */
#endif /* _CPU_DEFS_H_ */
