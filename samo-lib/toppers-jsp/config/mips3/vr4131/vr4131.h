/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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

#ifndef _VR4131_H_
#define _VR4131_H_

/*
 *  VR4131 ハードウェア定義
 */

/* CP0ハザードのための時間稼ぎ NOP_FOR_CP0_HAZARD  */
/* VR4131ではmtc0命令の直後から割込み禁止になっているのでnop命令を挿入しなくても
   大丈夫 */
#define NOP_FOR_CP0_HAZARD

/* Configレジスタ  */
#define BP		BIT16	/* 分岐予測の設定 */

/* TLB のエントリの最大値  */
#define TMAX_TLB	31

/* TLBマスク値 */
#define TLB_VPN2	0x800

/* キャッシュ命令 */
#define Index_Invalidate_I	(0x0+0x0)
#define Index_Store_Tag_I       (0x8+0x0)
#define Index_Store_Tag_D       (0x8+0x1)

/* キャッシュのラインサイズ(バイト単位)  */
#define D_CACHE_LINE_SIZE 	0x10	/* 16バイト */
#define I_CACHE_LINE_SIZE 	0x10	/* 16バイト */

/*============================================================================*/

#ifndef _MACRO_ONLY

Inline void
vr4131_exit() {
	while(1);
	/* GDB_STUB 使用時には、STUBの頭に飛ぶ処理が必要かも。 */
}

#endif /* _MACRO_ONLY */

/*============================================================================*/
/* 以下の定義は、各ユニットごとに、vr4131_xxx.hとしてまとめることも必要かも。 */

/*
 * クロックマスクユニット(CMU)関係の定義
 */
#define CMUCLKMSK	0x0f000060

#define MSKSIU		BIT1	/* SIU, DSIUへのTClock供給の制御 */
#define MSKSSIU		BIT8	/* SIUへの18.432MHzクロック供給の制御 */
#define MSKDSIU		BIT11	/* DSIUへの18.432MHzクロック供給の制御 */

/*
 * パワーマネージメントユニット(PMU)関係の定義
 */
#define PMUINTREG	0x0f0000c0	/* PMU 割込み／ステータスレジスタ */
#define PMUCNTREG	0x0f0000c2	/* PMU コントロールレジスタ */
#define PMUTCLKDIVREG	0x0f0000cc

/* for PMUINTREG */
#define TIMOUTRST	BIT5		/* HALTimerリセット検出 */

/* for PMUCNTREG */
#define HALTIMERRST	BIT2		/* HALTimerリセット*/

/* for PMUTCLKDIVREG */
#define VTDIV_3		(BIT1 | BIT0)

/*
 * バスコントロールユニット(BCU)関係の定義
 */
#define BCUCNTREG1	0x0f000000
#define ROMSIZEREG	0x0f000004
#define ROMSPEEDREG	0x0f000006
#define IO0SPEEDREG	0x0f000008
#define IO1SPEEDREG	0x0f00000a
#define BCUCNTREG3	0x0f000016

/* for ROMSIZEREG */
#define SIZE3_4			BIT12
#define SIZE2_4			BIT8
#define SIZE1_4			BIT4
#define SIZE0_4			BIT0

/* for ROMSPEEDREG */
#define ROM4_WAIT_5VTClock	(BIT13 | BIT12)
#define ROM2_WAIT_8VTClock	(BIT2 | BIT0)

/* for IO0SPEEDREG */
#define IO0_1_WAIT_4		(BIT1 | BIT0)

/* for IO1SPEEDREG */
#define IO1_3_WAIT_11		BIT11

/* for BCUCNTREG3 */
#define EXT_ROMCS_3ROM_2ROM	(BIT13 | BIT12)
#define IO32			BIT7
#define LCDSEL1_BUFFER		BIT1
#define LCDSEL0_BUFFER		BIT0

/*
 * SDRAMコントロールユニット(SDRAMU)関係の定義
 */
#define SDRAMMODEREG	0x0f000400
#define SDRAMCNTREG	0x0f000402
#define BCURFCNTREG	0x0f000404
#define BCURFCOUNTREG	0x0f000406
#define RAMSIZEREG	0x0f000408

/* for SDRAMMODEREG */
#define SCLK		BIT15
#define LTMODE_2	BIT5

/* for SDRAMCNTREG */
#define TRC_3VTClock	(BIT9 | BIT8)
#define TDAL_2VTClock	BIT5
#define TRCD_2VTClock	BIT1

/* for RAMSIZEREG */
#define SIZE3_64	(BIT14 | BIT12)
#define SIZE2_64	(BIT10 | BIT8)
#define SIZE1_64	(BIT6 | BIT4)
#define SIZE0_64	(BIT2 | BIT0)

/*
 * シリアルインタフェースユニット(SIU)関係の定義
 */
#define SIURESET	0x0f000809

/* for SIURESET */
#define DSIURESET	BIT1	/*  DSIUをリセットするフラグ  */

#endif /* _VR4131_H_ */
