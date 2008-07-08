/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000,2001 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002-2004 by Hokkaido Industrial Research Institute, JAPAN
 *
 *  上記著作権者は，Free Software Foundation によって公表されている
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の条件のいずれかを満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 *
 *  @(#) $Id: sh7615.h,v 1.5 2005/07/06 00:45:07 honda Exp $
 */

#ifndef _SH7615_H_
#define _SH7615_H_

#include <sil.h>

/*
 *  割込みのベクタ番号定義
 */
#define GII	 4					/*  一般不当命令:General Illegal Instruction  */
#define SII	 6					/*  スロット不当命令:Slot Illegal Instruction */
#define CAE	 9					/*  CPUアドレスエラー:CPU Address Error       */
#define DAE	10					/*  DMAアドレスエラー:DMA Address Error       */

#define NMI	11					/*  NMI         */
#define USBK	12				/*  ユーザーブレーク    */
#define HUDI	13				/*  ユーザーデバッグインターフェース    */
/* 外部ベクタ番号を指定する場合、再定義か別に定義する */
#define IRQ0	64				/*  外部割込み要求      */
#define IRQ1	65
#define IRQ2	66
#define IRQ3	67
#define IRL1	64
#define IRL2	65
#define IRL3	65
#define IRL4	66
#define IRL5	66
#define IRL6	67
#define IRL7	67
#define IRL8	68
#define IRL9	68
#define IRL10	69
#define IRL11	69
#define IRL12	70
#define IRL13	70
#define IRL14	71
#define IRL15	71

/* ベクタ番号を自由に指定する場合、再定義か別に定義する */
	/*  DMAC:ダイレクトメモリアクセスコントローラ  */
#define DMAC0	72				/*  DMAC0  */
#define DMAC1	73				/*  DMAC1  */

#define ITI	74					/*  WDT:ウォッチドックタイマ        */
#define CMI	75					/*  REF:DRAMリフレッシュ制御        */
#define EINT	76				/*  EINT:EtherC割り込み         */

	/*  FRT:フリーランニングタイマ  */
#define ICI	77					/*  FRT  */
#define OCI	78
#define OVI	79

	/*  TPU:タイマパルスユニット  */
#define TGI0A	80				/*  TPU0  */
#define TGI0B	81
#define TGI0C	82
#define TGI0D	83
#define TCI0V	84
#define TGI1A	85				/*  TPU1  */
#define TGI1B	86
#define TCI1V	87
#define TCI1U	88
#define TGI2A	89				/*  TPU2  */
#define TGI2B	90
#define TCI2V	91
#define TCI2U	92

	/*  SCIF:シリアルコミュニケーションインターフェース  */
#define ERI1	93				/*  SCI1  */
#define RXI1	94
#define BRI1	95
#define TXI1	96
#define ERI2	97				/*  SCI2  */
#define RXI2	98
#define BRI2	99
#define TXI2	100

	/*  SIO:シリアルI/O  */
#define EREI0	101				/*  SCI0  */
#define TERI0	102
#define RDFI0	103
#define TDEI0	104
#define EREI1	105				/*  SCI1  */
#define TERI1	106
#define RDFI1	107
#define TDEI1	108
#define EREI2	109				/*  SCI2  */
#define TERI2	110
#define RDFI2	111
#define TDEI2	112



#ifndef _MACRO_ONLY

/*
 *  SH2の内部レジスタ定義
 */

/* レジスタのアクセスは原則32bit幅 */
/*
 *   バスステートコントローラ
 */
/* バスコントロールレジスタ */
#define BCR1    ((VW *)0xffffffe0)
#define BCR2    ((VW *)0xffffffe4)
#define BCR3    ((VW *)0xfffffffc)
/* ウェイトコントロールレジスタ */
#define WCR1    ((VW *)0xffffffe8)
#define WCR2    ((VW *)0xffffffc0)
#define WCR3    ((VW *)0xffffffc4)
/* 個別メモリコントロールレジスタ */
#define MCR     ((VW *)0xffffffec)
/* リフレッシュタイマコントロール/ステータスレジスタ */
#define RTCSR   ((VW *)0xfffffff0)
/* リフレッシュタイマカウンタ */
#define RTCNT   ((VW *)0xfffffff4)
/* リフレッシュタイムコンスタントレジスタ */
#define RTCOR   ((VW *)0xfffffff8)
/*-----内部発振回路-----*/
#define FMR     ((VB *)0xfffffe90)

/*
 * ピンファンクションコントローラ
 */
#define PACR    ((VH *)0xfffffc80)	/*  ポートA コントロールレジスタ  */
#define PAIOR   ((VH *)0xfffffc82)	/*  ポートA I/Oレジスタ       */
#define PBCR    ((VH *)0xfffffc88)	/*  ポートB コントロールレジスタ  */
#define PBIOR   ((VH *)0xfffffc8a)	/*  ポートB I/Oレジスタ       */
#define PBCR2   ((VH *)0xfffffc8e)	/*  ポートB コントロールレジスタ2 */
#define PADR    ((VH *)0xfffffc84)	/*  ポートA データレジスタ        */
#define PBDR    ((VH *)0xfffffc8c)	/*  ポートB データレジスタ        */

/* 割り込み優先度レベル設定レジスタ */
#define IPRA    ((VH *)0xfffffee2)
#define IPRB    ((VH *)0xfffffe60)
#define IPRC    ((VH *)0xfffffee6)
#define IPRD    ((VH *)0xfffffe40)
#define IPRE    ((VH *)0xfffffec0)
/* ベクタ番号設定レジスタ */
#define VCRA    ((VH *)0xfffffe62)
#define VCRB    ((VH *)0xfffffe64)
#define VCRC    ((VH *)0xfffffe66)
#define VCRD    ((VH *)0xfffffe68)
#define VCRE    ((VH *)0xfffffe42)
#define VCRF    ((VH *)0xfffffe44)
#define VCRG    ((VH *)0xfffffe46)
#define VCRH    ((VH *)0xfffffe48)
#define VCRI    ((VH *)0xfffffe4a)
#define VCRJ    ((VH *)0xfffffe4c)
#define VCRK    ((VH *)0xfffffe4e)
#define VCRL    ((VH *)0xfffffe50)
#define VCRM    ((VH *)0xfffffe52)
#define VCRN    ((VH *)0xfffffe54)
#define VCRO    ((VH *)0xfffffe56)
#define VCRP    ((VH *)0xfffffec2)
#define VCRQ    ((VH *)0xfffffec4)
#define VCRR    ((VH *)0xfffffec6)
#define VCRS    ((VH *)0xfffffec8)
#define VCRT    ((VH *)0xfffffeca)
#define VCRU    ((VH *)0xfffffecc)
#define VCRWDT  ((VH *)0xfffffee4)
#define DMA_VCRDMA0     ((VW *)0xffffffa0)
#define DMA_VCRDMA1     ((VW *)0xffffffa8)
/* 割り込みコントロールレジスタ */
#define ICR     ((VH *)0xfffffee0)
/* 割り込みコントロール/ステータスレジスタ */
#define IRQCSR  ((VH *)0xfffffee8)

/*
 * 割り込みコントローラの初期化
 */
Inline void
sh2_init_intcontorller (void)
{
	sil_wrh_mem (IPRA, 0x0000);
	sil_wrh_mem (IPRB, 0x0000);
	sil_wrh_mem (IPRC, 0x0000);
	sil_wrh_mem (IPRD, 0x0000);
	sil_wrh_mem (IPRE, 0x0000);
}

#endif /* _MACRO_ONLY */

#endif /* _SH7615_H_ */
