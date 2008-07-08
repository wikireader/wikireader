/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2006 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN  
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
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
 *  @(#) $Id: hw_timer.h,v 1.2 2006/08/03 04:14:02 honda Exp $
 */

/*
 *	タイマドライバ（M3029用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>
#include <sil.h>
#include <m3029.h>


/*
 *  タイマーの割り込みレベル
 */
#define INT_LEVEL_TIMER	0x05

/*
 * タイマ割込みハンドラのベクタ番号
 */
#ifdef USE_TA0
#define	INHNO_TIMER	       INT_TA0
#elif defined(USE_TA1)
#define	INHNO_TIMER	       INT_TA1
#endif


/*
 * タイマモードレジスタ
 */
#ifdef USE_TA0
#define TADR_SFR_TAMR TADR_SFR_TA0MR
#elif  defined(USE_TA1)
#define TADR_SFR_TAMR TADR_SFR_TA1MR
#endif

/*
 * アップダウンフラグ
 */
#ifdef USE_TA0
#define TBIT_UDF_TAUD TBIT_UDF_TA0UD
#elif  defined(USE_TA1)
#define TBIT_UDF_TAUD TBIT_UDF_TA1UD  
#endif

/*
 * タイマレジスタ
 */
#ifdef USE_TA0
#define TADR_SFR_TA TADR_SFR_TA0
#elif  defined(USE_TA1)
#define TADR_SFR_TA TADR_SFR_TA1
#endif

/*
 * カウント開始ビット
 */
#ifdef USE_TA0
#define TBIT_TABSR_TS  TBIT_TABSR_TA0S
#elif  defined(USE_TA1)
#define TBIT_TABSR_TS  TBIT_TABSR_TA1S
#endif

/*
 * 割込み制御レジスタのオフセット
 */
#ifdef USE_TA0
#define TADR_SFR_TAIC_OFFSET  TADR_SFR_TA0IC_OFFSET
#elif  defined(USE_TA1)
#define TADR_SFR_TAIC_OFFSET  TADR_SFR_TA1IC_OFFSET
#endif


#ifndef _MACRO_ONLY


/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;


/*
 *  タイマ値の内部表現とミリ秒単位との変換
 *
 *  1msのタイマー割り込みを使用するのでそのクロックを定義する．
 */
#define TIMER_CLOCK		20000
#define TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((unsigned long)(clock) * 1000 / TIMER_CLOCK)


/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffff)


/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値（単位は内部表現）
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)


/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
    /*
     * タイマ初期化
     */
    /* タイマカウント停止 */
	sil_wrb_mem((VP)TADR_SFR_TABSR,
                sil_reb_mem((VP)TADR_SFR_TABSR) & ~TBIT_TABSR_TS);
    /* ダウンカウント設定 */
	sil_wrb_mem((VP)TADR_SFR_UDF,
                sil_reb_mem((VP)TADR_SFR_UDF)   & ~TBIT_UDF_TAUD);
    /*  カウントソースf1, タイマモード  */
	sil_wrb_mem((VP)TADR_SFR_TAMR, 0x00);		
    
	/*
     *  タイマ値の初期化
	 *  タイマは 1msで TIMER_CLOCK カウント,
	 *  レジスタ設定値はマイナス1する.
	 */
	sil_wrh_mem((VP)TADR_SFR_TA, (TIMER_CLOCK - 1));
    
    /* 割り込みレベルの設定 */
    set_ic_ilvl((VP)(TADR_SFR_INT_BASE + TADR_SFR_TAIC_OFFSET), INT_LEVEL_TIMER);
    
    /* カウント開始	*/
	sil_wrb_mem((VP)TADR_SFR_TABSR,
                sil_reb_mem((VP)TADR_SFR_TABSR) | TBIT_TABSR_TS);
}


/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
	/*
	 *  この関数はタイマ割り込みハンドラが
	 *  isig_tim を呼び出す前に実行される.
	 *  割込みが受け付けられてから
	 *  割込みハンドラに処理が移った時点で
	 *  割込み要求ビットはクリアされるため
	 *  何もする必要がない. 
	 */
}


/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
hw_timer_terminate()
{
    /* カウント停止	*/
	sil_wrb_mem((VP)TADR_SFR_TABSR,
                sil_reb_mem((VP)TADR_SFR_TABSR) & ~TBIT_TABSR_TS);    
}


/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
hw_timer_get_current(void)
{
	return ((TIMER_CLOCK-1) - sil_reh_mem((VP)TADR_SFR_TA));
}


/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return (sil_reb_mem((VP)(TADR_SFR_INT_BASE + TADR_SFR_TAIC_OFFSET)) & TBIT_IC_IR) != 0;
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
