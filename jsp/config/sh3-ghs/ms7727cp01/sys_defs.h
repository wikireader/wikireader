/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                     2003         by  Advanced Data Controls, Corp
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
 *  @(#) $Id: sys_defs.h,v 1.2 2003/12/19 11:48:37 honda Exp $
 */

/*
 *  ターゲットシステムに依存する定義（MS7717CP01用）
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

/*
 *  システムに依存する定義
 */

#ifndef _SYS_DEFS_H_
#define _SYS_DEFS_H_

/*
 *  起動メッセージのターゲット名
 */
#define	TARGET_NAME	"MS7727CP01(SH7727 T-Engine)"

/*
 * プロセッサの種類(DSPには対応していない)
 */
#define SH3
#define SH7727

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN  SIL_ENDIAN_LITTLE  /* リトルエンディアン */


/*
 *  JSPカーネル動作時のメモリマップ
 *      0x8c000000 - 0x8c000000 ベクタテーブル
 *      0x8c000500 - 0x8c001fff T-Monitor使用領域
 *      0x8c002000 - 0x8cffffff コード領域(1MB)
 *      0x8c100000 -            データ領域(3MB)
 *                 - 0x8c3fffff タスク独立部用スタック
 *                 - 0x8e000000 メモリ終了
 *
 */

/*
 *   スタック領域の定義
 */
#define STACKTOP   0x8c400000    /* タスク独立部用スタックの初期値 */

/*
 *  タイムティックの定義
 */
#define	TIC_NUME	1		/* タイムティックの周期の分子 */
#define	TIC_DENO	1		/* タイムティックの周期の分母 */

/*
 *  タイマーの割り込みレベル
 */
#define KTIM_INT_LV   4

/*
 *  タイマ値の内部表現と msec 単位との変換
 *  MS7727CP01では、Pクロックが24MHz
 *  分周比 /4,/16,/64/256 のいずれかを選択
 *  /4:  6Mhz     tick 6000   0.16 μsec
 *  /16: 1.5Mhz   tick 1500   0.6 μsec
 *  /64: 0.375Mhz tick 375    2.6 μsec
 *  /256:0.097Mhz tick 97     10 μsec
 *
 *  /16 を選択
 *  TMUのクロックの上限は2Mhz
 */

#define TCR0_TPSC  0x0001


/*
 *  タイマ値の内部表現とミリ秒単位との変換
 */
#define TIMER_CLOCK     1600

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	 90
#define	SIL_DLY_TIM2	 50


#ifndef _MACRO_ONLY

asm void ___set_sr(s)
{
%reg s

   ldc s,sr
}



Inline void
kernel_abort()
{
    UW sr = 0x10000000;
    
    /*
     * 強制的にリセットを発生させる
     */
    /*
     * BLビットを立てる
     */
    ___set_sr(sr);
    /*
     * 書込み例外
     */
    *((volatile int *) 0xFFFFFEC1) = 0;
}
#endif /* _MACRO_ONLY */


#endif /* _SYS_DEFS_H_ */













