/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: hw_timer.h,v 1.9 2003/07/08 14:57:15 hiro Exp $
 */

/*
 *	タイマドライバ（DVE68K/40用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>
#include <dve68k_dga.h>

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	TVEC_TT0

#ifndef _MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef UW	CLOCK;

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *
 *  DVE68K/40 CPUボードでは，タイマは1μ秒毎にカウントアップする．
 */
#define	TIMER_CLOCK		1000
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffffff)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマ停止までの時間（nsec単位）
 *
 *  値に根拠はない．
 */
#define	TIMER_STOP_DELAY	200

/*
 *  レジスタの設定値
 */
#define	CSR12_START	0x80000000u	/* タイマ動作 */

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
	CLOCK	cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

	/*
	 *  タイマ周期を設定し，タイマの動作を開始する．
	 */
	assert(cyc <= MAX_CLOCK);
	dga_write((VP) TADR_DGA_CSR12, CSR12_START | cyc);

	/*
	 *  タイマ割込みの割込みレベルを設定し，要求をクリアした後，
	 *  マスクを解除する．
	 */
	dga_set_ilv((VP) TADR_DGA_CSR25, TBIT_TT0IL, TIRQ_LEVEL4);
	dga_write((VP) TADR_DGA_CSR23, TBIT_TT0);
	dga_bit_or((VP) TADR_DGA_CSR21, TBIT_TT0);
}

/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
	dga_write((VP) TADR_DGA_CSR23, TBIT_TT0);
}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
hw_timer_terminate()
{
	/*
	 *  タイマの動作を停止する．
	 */
	dga_bit_and((VP) TADR_DGA_CSR12, ~CSR12_START);

	/*
	 *  タイマ割込みをマスクし，要求をクリアする．
	 */
	dga_bit_and((VP) TADR_DGA_CSR21, ~TBIT_TT0);
	dga_write((VP) TADR_DGA_CSR23, TBIT_TT0);
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
hw_timer_get_current()
{
	CLOCK	clk;

	/*
	 *  タイマの動作を一時的に停止し，タイマ値を読み出す．
	 */
	dga_bit_and((VP) TADR_DGA_CSR12, ~CSR12_START);
	sil_dly_nse(TIMER_STOP_DELAY);
	clk = dga_read((VP) TADR_DGA_CSR13) & 0x00ffffffu;
	dga_bit_or((VP) TADR_DGA_CSR12, CSR12_START);
	return(clk);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt()
{
	return((dga_read((VP) TADR_DGA_CSR20) & TBIT_TT0) != 0);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
