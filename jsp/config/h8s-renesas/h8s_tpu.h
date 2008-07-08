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

#ifndef _H8S_TPU_H_
#define _H8S_TPU_H_

#include <s_services.h>		/* デバイスドライバ用標準インクルードファイル */

/*
 *  H8S内蔵16ビットタイマパルスユニット(TPU)関連の定義
 */

/* TPU内レジスタのアドレス */

/* チャネル０ */
#define TCR0		0xffd0		/* タイマコントロールレジスタ０ */
#define TMDR0		0xffd1		/* タイマモードレジスタ０ */
#define TIOR0H		0xffd2		/* タイマI/Oコントロールレジスタ０Ｈ */
#define TIOR0L		0xffd3		/* タイマI/Oコントロールレジスタ０Ｌ */
#define TIER0		0xffd4		/* タイマインタラプトイネーブルレジスタ０ */
#define TSR0		0xffd5		/* タイマステータスレジスタ０ */
#define TCNT0		0xffd6		/* タイマカウンタ０ */
#define TGR0A		0xffd8		/* タイマジェネラルレジスタ０Ａ */
#define TGR0B		0xffda		/* タイマジェネラルレジスタ０Ｂ */
#define TGR0C		0xffdc		/* タイマジェネラルレジスタ０Ｃ */
#define TGR0D		0xffde		/* タイマジェネラルレジスタ０Ｄ */

/* チャネル１ */
#define TCR1		0xffe0		/* タイマコントロールレジスタ１ */
#define TMDR1		0xffe1		/* タイマモードレジスタ１ */
#define TIOR1		0xffe2		/* タイマI/Oコントロールレジスタ１ */
#define TIER1		0xffe4		/* タイマインタラプトイネーブルレジスタ１ */
#define TSR1		0xffe5		/* タイマステータスレジスタ１ */
#define TCNT1		0xffe6		/* タイマカウンタ１ */
#define TGR1A		0xffe8		/* タイマジェネラルレジスタ１Ａ */
#define TGR1B		0xffea		/* タイマジェネラルレジスタ１Ｂ */

/* チャネル２ */
#define TCR2		0xfff0		/* タイマコントロールレジスタ２ */
#define TMDR2		0xfff1		/* タイマモードレジスタ２ */
#define TIOR2		0xfff2		/* タイマI/Oコントロールレジスタ２ */
#define TIER2		0xfff4		/* タイマインタラプトイネーブルレジスタ２ */
#define TSR2		0xfff5		/* タイマステータスレジスタ２ */
#define TCNT2		0xfff6		/* タイマカウンタ２ */
#define TGR2A		0xfff8		/* タイマジェネラルレジスタ２Ａ */
#define TGR2B		0xfffa		/* タイマジェネラルレジスタ２Ｂ */

/* チャネル３ */
#define TCR3		0xfe80		/* タイマコントロールレジスタ３ */
#define TMDR3		0xfe81		/* タイマモードレジスタ３ */
#define TIOR3H		0xfe82		/* タイマI/Oコントロールレジスタ３Ｈ */
#define TIOR3L		0xfe83		/* タイマI/Oコントロールレジスタ３Ｌ */
#define TIER3		0xfe84		/* タイマインタラプトイネーブルレジスタ３ */
#define TSR3		0xfe85		/* タイマステータスレジスタ３ */
#define TCNT3		0xfe86		/* タイマカウンタ３ */
#define TGR3A		0xfe88		/* タイマジェネラルレジスタ３Ａ */
#define TGR3B		0xfe8a		/* タイマジェネラルレジスタ３Ｂ */
#define TGR3C		0xfe8c		/* タイマジェネラルレジスタ３Ｃ */
#define TGR3D		0xfe8e		/* タイマジェネラルレジスタ３Ｄ */

/* チャネル４ */
#define TCR4		0xfe90		/* タイマコントロールレジスタ４ */
#define TMDR4		0xfe91		/* タイマモードレジスタ４ */
#define TIOR4		0xfe92		/* タイマI/Oコントロールレジスタ４ */
#define TIER4		0xfe94		/* タイマインタラプトイネーブルレジスタ４ */
#define TSR4		0xfe95		/* タイマステータスレジスタ４ */
#define TCNT4		0xfe96		/* タイマカウンタ４ */
#define TGR4A		0xfe98		/* タイマジェネラルレジスタ４Ａ */
#define TGR4B		0xfe9a		/* タイマジェネラルレジスタ４Ｂ */

/* チャネル５ */
#define TCR5		0xfea0		/* タイマコントロールレジスタ５ */
#define TMDR5		0xfea1		/* タイマモードレジスタ５ */
#define TIOR5		0xfea2		/* タイマI/Oコントロールレジスタ５ */
#define TIER5		0xfea4		/* タイマインタラプトイネーブルレジスタ５ */
#define TSR5		0xfea5		/* タイマステータスレジスタ５ */
#define TCNT		0xfea6		/* タイマカウンタ５ */
#define TGR5A		0xfea8		/* タイマジェネラルレジスタ５Ａ */
#define TGR5B		0xfeaa		/* タイマジェネラルレジスタ５Ｂ */

/* 共通 */
#define TSTR		0xffc0		/* タイマステートレジスタ */
#define TSYR		0xffc1		/* タイマシンクロレジスタ */

/* 各レジスタのビットパターン */

/* TCR */
#define CCLR2		BIT7		/* チャネル０、３のみ */
#define CCLR1		BIT6
#define CCLR0		BIT5
#define CKEG1		BIT4
#define CKEG0		BIT3
#define TPSC2		BIT2
#define TPSC1		BIT1
#define TPSC0		BIT0

/* TMDR */
#define BFB		BIT5		/* チャネル０、３のみ */
#define BFA		BIT4		/* チャネル０、３のみ */
#define MD3		BIT3
#define MD2		BIT2
#define MD1		BIT1
#define MD0		BIT0

/* TIOR */
/* TIORx、または、TIORxH */
#define IOB3		BIT7
#define IOB2		BIT6
#define IOB1		BIT5
#define IOB0		BIT4
#define IOA3		BIT3
#define IOA2		BIT2
#define IOA1		BIT1
#define IOA0		BIT0

/* TIORxL */
#define IOD3		BIT7
#define IOD2		BIT6
#define IOD1		BIT5
#define IOD0		BIT4
#define IOC3		BIT3
#define IOC2		BIT2
#define IOC1		BIT1
#define IOC0		BIT0

/* TIER */
#define TTGE		BIT7
#define TCIEU		BIT5		/* チャネル１，２，４，５のみ */
#define TCIEV		BIT4
#define TGIED		BIT3		/* チャネル０，３のみ */
#define TGIEC		BIT2		/* チャネル０，３のみ */
#define TGIEB		BIT1
#define TGIEA		BIT0

/* TSR */
#define TCFD		BIT7
#define TCFU		BIT5		/* チャネル１，２，４，５のみ */
#define TCFV		BIT4
#define TGFD		BIT3		/* チャネル０，３のみ */
#define TGFC		BIT2		/* チャネル０，３のみ */
#define TGFB		BIT1
#define TGFA		BIT0

/* TCNT, TGR */
/* 定義すべきものは、特に無い */

/* TSTR */
#define CST5		BIT5
#define CST4		BIT4
#define CST3		BIT3
#define CST2		BIT2
#define CST1		BIT1
#define CST0		BIT0

/* TSYR */
#define SYNC5		BIT5
#define SYNC4		BIT4
#define SYNC3		BIT3
#define SYNC2		BIT2
#define SYNC1		BIT1
#define SYNC0		BIT0

#endif /* _H8S_TPU_H_ */
