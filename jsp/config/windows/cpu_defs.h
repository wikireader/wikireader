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
 *  @(#) $Id: cpu_defs.h,v 1.11 2003/12/15 07:19:22 takayuki Exp $
 */


/*
 *	プロセッサに依存する定義（Windows用）
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

#define OMIT_SIL_ACCESS

#define WINDOWS

#define INT_NUMINTERRUPTS	7	/* 割込み本数 */

#ifndef _MACRO_ONLY

#include <itron.h>
#include <vwindows.h>

typedef	UINT		INHNO;		/* 割込みハンドラ番号 */
typedef	UINT		EXCNO;		/* CPU例外ハンドラ番号 */

/*
 * 割込みエミュレーション関連
 */

#define INT_STAT_MASK		0x0000ffffu
#define INT_STAT_PENDING	0x00000001u
#define INT_STAT_RUNNING	0x00000002u
#define INT_STAT_ENABLED	0x00000004u

#define INT_MOD_MASK		0xff000000u
#define INT_MOD_LOGMASK		0x01000000u

struct tagInterruptLevel
{
	HANDLE			ThreadHandle;
	DWORD			ThreadID;
	void *			Routine;
	unsigned int	Flags;
};

extern struct tagInterruptLevel InterruptLevel[INT_NUMINTERRUPTS];
extern unsigned int             CurrentInterruptLevel;

/*  例外関連の定義 */

#define EXC_MAXITEMS 32

struct tagExceptionLevel
{
	DWORD ExceptionCode;
	void * Routine;
};

extern struct tagExceptionLevel ExceptionLevel[EXC_MAXITEMS];

/*
 * 割込みエミュレーション用操作関数
 */

extern BOOL			def_int( unsigned int ims, void * rtn);	/* ハンドラ定義 */
extern BOOL			ini_int( void );				/* 初期化 */
extern void			fin_int( void );
extern BOOL			ras_int(unsigned int ims);		/* 割込み要求 */
extern unsigned int sns_int( void );				/* Sense the highest pending request */
extern HANDLE		sch_int( void );				/* 割り込みのスケジュール */

extern BOOL			set_intlogmask( unsigned int ims, BOOL mask );

 /* 許可と禁止は引数に関係なく全許可&全禁止となる */
extern ER	ena_int(unsigned int ims);			/* 許可 */
extern ER	dis_int(unsigned int ims);			/* 禁止 */

extern ER	chg_ims(unsigned int ims);			/* 割込みレベルの設定 */
extern ER	get_ims(unsigned int *p_ims);		/* 割込みレベルの取得 */
extern ER	vget_ims(unsigned int *p_ims);		/* 割込みレベルの取得 */

extern BOOL def_exc(DWORD exc, void * routine);
extern BOOL ini_exc(void);
extern void fin_exc(void);

	/*
	 * システムクリティカルセクション
	 *   事実上のCPUロックで、ここに入っている間ディスパッチは発生しない
	 */
extern void enter_system_critical_section(BOOL *);
extern void leave_system_critiacl_section(BOOL *);

	/* 指定されたタスクをシステムクリティカルセクション外で停止させる */
extern ER   wait_for_thread_suspension_completion(HANDLE thread);

    /* シミュレーションタイマ制御 */
extern unsigned int current_timer_clock_unit;

    /* シミュレーション時刻のティックを修正する
     *   clk : 一回のタイマ割込み毎に加えるシミュレーション時間
     *     (clk = 0 : 自然な値 (TIMER_CLOCK_WINDOWS))
     */
extern ER set_clk(unsigned int clk);

#endif /* _MACRO_ONLY */

/*
 * タイマー関連
 */

#include <hw_timer.h>

/*
 *  タイムティックの定義
 */
#define	TIC_NUME	current_timer_clock_unit	/* タイムティックの周期の分子 */
#define	TIC_DENO	1						    /* タイムティックの周期の分母 */

#ifndef _MACRO_ONLY

/*
 * 拡張タイマー (Pentium Timer)
 */

typedef UD SYSUTIM;

Inline ER
vxget_tim(SYSUTIM * pk_sysutim)
{
	QueryPerformanceCounter((LARGE_INTEGER *)pk_sysutim);
	return E_OK;
}

#endif /* _MACRO_ONLY */

#endif /* _CPU_DEFS_H_ */
