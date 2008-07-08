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

#ifndef _H8S_2339F_H_
#define _H8S_2339F_H_

/*
 *  H8S/2339F 用定義
 */

/*
 *  モジュールストップコントロールレジスタ
 *  (様々なモジュールの ON、OFF に使っているので、ここで定義する。)
 */
#define MSTPCR		0xff3c
#define MSTPCR_TPU	BIT13
#define MSTPCR_SCI2	BIT7
#define MSTPCR_SCI1	BIT6
#define MSTPCR_SCI0	BIT5


/*
 *  割込みコントローラ
 */
/* レジスタのアドレス */
/* システムコントロールレジスタは、既に定義済み */
#define ISCRH		0xff2c		/* IRQセンスコントロールレジスタH */
#define ISCRL		0xff2d		/* IRQセンスコントロールレジスタL */
#define IER		0xff2e		/* IRQイネーブルレジスタ */
#define ISR		0xff2f		/* IRQステータスレジスタ */
#define IPRA		0xfec4		/* インタラプトプライオリティレジスタA */
#define IPRB 		0xfec5		/* インタラプトプライオリティレジスタB */
#define IPRC		0xfec6		/* インタラプトプライオリティレジスタC */
#define IPRD		0xfec7		/* インタラプトプライオリティレジスタD */
#define IPRE		0xfec8		/* インタラプトプライオリティレジスタE */
#define IPRF		0xfec9		/* インタラプトプライオリティレジスタF */
#define IPRG		0xfeca		/* インタラプトプライオリティレジスタG */
#define IPRH		0xfecb		/* インタラプトプライオリティレジスタH */
#define IPRI		0xfecc		/* インタラプトプライオリティレジスタI */
#define IPRJ		0xfecd		/* インタラプトプライオリティレジスタJ */
#define IPRK		0xfece		/* インタラプトプライオリティレジスタK */

/* 各レジスタのビットパターン */

/* IPRx (x = A-K) */
#define IPR6		BIT6
#define IPR5		BIT5
#define IPR4		BIT4
#define IPR2		BIT2
#define IPR1		BIT1
#define IPR0		BIT0

/* icu_set_ilv用 */
#define IPR_UPR		TRUE
#define IPR_LOW		FALSE
#define IPR_UPR_MASK	(IPR6|IPR5|IPR4)	/* IPR上位ビットマスク */
#define IPR_LOW_MASK	(IPR2|IPR1|IPR0)	/* IPR下位ビットマスク */

/* IER */
#define IRQ7E		BIT7
#define IRQ6E		BIT6
#define IRQ5E		BIT5
#define IRQ4E		BIT4
#define IRQ3E		BIT3
#define IRQ2E		BIT2
#define IRQ1E		BIT1
#define IRQ0E		BIT0

/* ISCRH */
#define IRQ7SCB		BIT7
#define IRQ7SCA		BIT6
#define IRQ6SCB		BIT5
#define IRQ6SCA		BIT4
#define IRQ5SCB		BIT3
#define IRQ5SCA		BIT2
#define IRQ4SCB		BIT1
#define IRQ4SCA		BIT0

/* ISCRL */
#define IRQ3SCB		BIT7
#define IRQ3SCA		BIT6
#define IRQ2SCB		BIT5
#define IRQ2SCA		BIT4
#define IRQ1SCB		BIT3
#define IRQ1SCA		BIT2
#define IRQ0SCB		BIT1
#define IRQ0SCA		BIT0

/* ISR */
#define IRQ7F		BIT7
#define IRQ6F		BIT6
#define IRQ5F		BIT5
#define IRQ4F		BIT4
#define IRQ3F		BIT3
#define IRQ2F		BIT2
#define IRQ1F		BIT1
#define IRQ0F		BIT0

/* 割込み例外処理ベクタテーブルのベクタ番号 */
					/* 要求発生元 */
#define IRQ_POWRESET	0		/* パワーオンリセット */
#define IRQ_MANRESET	1		/* マニュアルリセット */
/* 2-4 は、システム予約 */
#define IRQ_TRACE	5		/* トレース */
/* 6 は、システム予約 */
#define IRQ_NMI		7		/* 外部端子 */
#define IRQ_TRAP0	8		/* トラップ命令 */
#define IRQ_TRAP1	9		/* トラップ命令 */
#define IRQ_TRAP2	10		/* トラップ命令 */
#define IRQ_TRAP3	11		/* トラップ命令 */
/* 12-15 は、システム予約 */
#define IRQ_IRQ0	16		/* 外部端子 */
#define IRQ_IRQ1	17		/* 外部端子 */
#define IRQ_IRQ2	18		/* 外部端子 */
#define IRQ_IRQ3	19		/* 外部端子 */
#define IRQ_IRQ4	20		/* 外部端子 */
#define IRQ_IRQ5	21		/* 外部端子 */
#define IRQ_IRQ6	22		/* 外部端子 */
#define IRQ_IRQ7	23		/* 外部端子 */
#define IRQ_SWDTEND	24		/* DTC */
#define IRQ_WOVI	25		/* ウォッチドッグタイマ */
#define IRQ_CMI 	26		/* リフレッシュコントローラ */
/* 27 番は、リザーブ */
#define IRQ_ADI		28		/* A/D */
/* 29, 30, 31 番は、リザーブ */
#define IRQ_TGI0A	32		/* TPUチャネル0 */
#define IRQ_TGI0B	33		/* TPUチャネル0 */
#define IRQ_TGI0C	34		/* TPUチャネル0 */
#define IRQ_TGI0D	35		/* TPUチャネル0 */
#define IRQ_TCI0V	36		/* TPUチャネル0 */
/* 37, 38, 39 番は、リザーブ */
#define IRQ_TGI1A	40		/* TPUチャネル1 */
#define IRQ_TGI1B	41		/* TPUチャネル1 */
#define IRQ_TCI1V	42		/* TPUチャネル1 */
#define IRQ_TCI1U	43		/* TPUチャネル1 */
#define IRQ_TGI2A	44		/* TPUチャネル2 */
#define IRQ_TGI2B	45		/* TPUチャネル2 */
#define IRQ_TCI2V	46		/* TPUチャネル2 */
#define IRQ_TCI2U	47		/* TPUチャネル2 */
#define IRQ_TGI3A	48		/* TPUチャネル3 */
#define IRQ_TGI3B	49		/* TPUチャネル3 */
#define IRQ_TGI3C	50		/* TPUチャネル3 */
#define IRQ_TGI3D	51		/* TPUチャネル3 */
#define IRQ_TCI3V	52		/* TPUチャネル3 */
/* 53, 54, 55 番は、リザーブ */
#define IRQ_TGI4A	56		/* TPUチャネル4 */
#define IRQ_TGI4B	57		/* TPUチャネル4 */
#define IRQ_TCI4V	58		/* TPUチャネル4 */
#define IRQ_TCI4U	59		/* TPUチャネル4 */
#define IRQ_TGI5A	60		/* TPUチャネル5 */
#define IRQ_TGI5B	61		/* TPUチャネル5 */
#define IRQ_TCI5V	62		/* TPUチャネル5 */
#define IRQ_TCI5U	63		/* TPUチャネル5 */
/* 64, 65, 66, 67, 68, 69, 70, 71 番は、リザーブ */
#define IRQ_DEND0A	72		/* DMAC */
#define IRQ_DEND0B	73		/* DMAC */
#define IRQ_DEND1A	74		/* DMAC */
#define IRQ_DEND1B	75		/* DMAC */
/* 76, 77, 78, 79 番は、リザーブ */
#define IRQ_ERI0	80		/* SCIチャネル0 */
#define IRQ_RXI0	81		/* SCIチャネル0 */
#define IRQ_TXI0	82		/* SCIチャネル0 */
#define IRQ_TEI0	83		/* SCIチャネル0 */
#define IRQ_ERI1	84		/* SCIチャネル1 */
#define IRQ_RXI1	85		/* SCIチャネル1 */
#define IRQ_TXI1	86		/* SCIチャネル1 */
#define IRQ_TEI1	87		/* SCIチャネル1 */
#define IRQ_ERI2	88		/* SCIチャネル2 */
#define IRQ_RXI2	89		/* SCIチャネル2 */
#define IRQ_TXI2	90		/* SCIチャネル2 */
#define IRQ_TEI2	91		/* SCIチャネル2 */

/*
 *  バスコントローラ
 */
/* レジスタのアドレス */
#define ABWCR		0xfed0		/* バス幅コントロールレジスタ */
#define ASTCR		0xfed1		/* アクセスステートコントロールレジスタ */
#define WCRH		0xfed2		/* ウェイトコントロールレジスタH */
#define WCRL      	0xfed3		/* ウェイトコントロールレジスタL */
#define BCRH		0xfed4		/* バスコントロールレジスタH */
#define BCRL		0xfed5		/* バスコントロールレジスタL */
#define MCR		0xfed6		/* メモリコントロールレジスタ */
#define DRAMCR		0xfed7		/* DRAMコントロールレジスタ */
#define RTCNT		0xfed8		/* リフレッシュタイマカウンタ */
#define RTCOR		0xfed9		/* リフレッシュタイマコンスタントレジスタ */

/* 各レジスタのビットパターン */

/* ASTCR */
#define AST7 		BIT7
#define AST6 		BIT6
#define AST5 		BIT5
#define AST4 		BIT4
#define AST3 		BIT3
#define AST2 		BIT2
#define AST1 		BIT1
#define AST0 		BIT0

/* WCRH */
#define W71 		BIT7
#define W70 		BIT6
#define W61 		BIT5
#define W60 		BIT4
#define W51 		BIT3
#define W50 		BIT2
#define W41 		BIT1
#define W40 		BIT0

/* WCRL */
#define W31 		BIT7
#define W30 		BIT6
#define W21 		BIT5
#define W20 		BIT4
#define W11 		BIT3
#define W10 		BIT2
#define W01 		BIT1
#define W00 		BIT0

/* BCRH */
#define ICIS1 		BIT7
#define ICIS0 		BIT6
#define BRSTRM		BIT5
#define BRSTS1 		BIT4
#define BRSTS0		BIT3
#define RMTS2		BIT2
#define RMTS1		BIT1
#define RMTS0 		BIT0


/*
 *  I/O ポート
 */

#define TNUM_IO_PORT	15	/*  I/Oポートの本数  */

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

/* port7 */
#define P7DDR		0xfeb6

/* port8 */
#define P8DDR		0xfeb7

/* port9 */
#define P9DDR		0xfeb8

/*  portA  */
#define PADDR		0xfeb9
#define PADR		0xff69
#define PORTA		0xff59

/* portB */
#define PBDDR		0xfeba

/* portC */
#define PCDDR		0xfebb

/* portD */
#define PDDDR		0xfebc

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

#endif /* _H8S_2339F_H_ */
