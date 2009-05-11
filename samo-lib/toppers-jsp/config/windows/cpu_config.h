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
 *  @(#) $Id: cpu_config.h,v 1.15 2003/12/15 07:19:22 takayuki Exp $
 */


/*
 *	プロセッサ依存モジュール（Windows用）
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  Windows-HAL独自のインクルードファイル
 */


/*
 *  Windows-HAL独自の定義
 */

#define FPU_CONTEXTSIZE 108     /* 浮動小数点ユニットが退避するコンテキストの大きさ */

/*
 *  chg_ims/ref_ims をサポートするかどうかの定義
 */
#define	SUPPORT_CHG_IMS

/*
 *  TCB 関連の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */


#ifndef _MACRO_ONLY

#include <s_services.h>

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	HANDLE	        ThreadHandle;		//タスクに割り当てられたスレッドのハンドル
	DWORD		    ThreadID;			//タスクに割り当てられたスレッドのID
	unsigned int    InterruptLevel;		//このタスクが動いているときの割込みマスクレベル
} CTXB;

/*
 * CPU状態フラグ
 */
#define CPU_STAT_LOCK			0x01		//CPUロック状態です
#define CPU_STAT_EXC			0x02		//例外処理中です
#define CPU_STAT_DOUBLEFAULT	0x04		//ダブルフォルトが起こってます

extern volatile char CPUStatus;

/*
 *   CPU例外ハンドラ
 */

/*#define EXCHNO_ZDIV  EXCEPTION_INT_DIVIDE_BY_ZERO */

/*
 *  システム状態参照
 */

Inline BOOL
sense_context()
{
		/* 割込み全許可でなければ非タスクコンテキストです */
	return (CurrentInterruptLevel != 0) ? TRUE : FALSE;
}

Inline BOOL
sense_lock()
{
		/* 例外処理中 or ロック状態フラグが立っていれば CPUロック状態です */
	return (CPUStatus & (CPU_STAT_LOCK|CPU_STAT_EXC)) != 0 ? TRUE : FALSE;
}

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は，chg_ipm をサポートするための変数．chg_ipm をサポー
 *  トしない場合には，task_intmask が常に 0 になっていると考えればよい．
 */

#include "debugout.h"
#include "device.h"

unsigned long task_intmask;

Inline void
i_lock_cpu()
{   dis_int(0);   }

Inline void
i_unlock_cpu()
{	ena_int(0);   }

#define t_lock_cpu	i_lock_cpu
#define t_unlock_cpu	i_unlock_cpu

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ
 *
 *  dispatch は，タスクコンテキストから呼び出されたサービスコール処理
 *  内で，CPUロック状態で呼び出さなければならない．
 */
extern void	dispatch();
extern void idispatch();

/*
 *  現在のコンテキストを捨ててディスパッチ
 *
 *  exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 */

extern void    exit_and_dispatch(void);

/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */


/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */

Inline void
define_inh(INHNO inhno, FP inthdr)
{	def_int((unsigned int)inhno,(void *)inthdr);   }

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
Inline void
define_exc(EXCNO excno, FP exchdr)
{	def_exc( (DWORD)excno, (void *)exchdr);   }

/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 *  全ての割り込みハンドラはディスパッチャを起動するSIGUSR1をマスクして
 *  動作する。
 *  reqflg をチェックする前に割込みを禁止しないと，reqflg をチェック後
 *  に起動された割込みハンドラ内でディスパッチが要求された場合に，ディ
 *  スパッチされない．
 */

	/* Windows-HALにおいてこの処理は割込みエミュレータに一任している */

#define INT_ENTRY(hdr) hdr
#define INTHDR_ENTRY(inthdr) extern void inthdr(void)

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 *  CPU例外ハンドラは，非タスクコンテキストで実行する．そのため，CPU例
 *  外ハンドラを呼び出す前に割込みモードに移行し，リターンしてきた後に
 *  元のモードに戻す．元のモードに戻すために，割込みモードに移行する前
 *  の SR を割込みスタック上に保存する．CPU例外がタスクコンテキストで
 *  発生し，reqflg が TRUE になった時に，ret_exc へ分岐する．
 *  reqflg をチェックする前に割込みを禁止しないと，reqflg をチェック後
 *  に起動された割込みハンドラ内でディスパッチが要求された場合に，ディ
 *  スパッチされない．
 */

#define EXC_ENTRY(hdr) hdr
#define EXCHDR_ENTRY(inthdr) extern void inthdr(void)

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のディスパッチ
 *
 *   WindowsHALは専用フラグを持っているんで、割込み前の情報はそのまま
 *   格納されている。
 */
#define exc_sense_context sense_context

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
#define exc_sense_lock sense_lock

/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

/*
 *  atexitで登録された関数の読出し (Windowsは何もしない -> CRTに任せる)
 */
extern void	call_atexit(void);

/*
 *   Windows Dedicated Routines 
 */
extern LONG WINAPI HALExceptionHandler( EXCEPTION_POINTERS * exc );

/*
 *   致命的な失敗に対する対処ルーチン
 */
extern void FatalAssertion(int exp, LPCSTR format, ... );


/*
 *   ログの出力制御ルーチン
 */
extern void set_logcontrol(BOOL enable);

/*
 *   カーネルデバッグ用printf
 */

#ifdef KERNEL_DEBUG_MODE
#  define kprintf(x) _kernel_debugprintf x
#else
#  define kprintf(x)
#endif

extern void _kernel_debugprintf(const char * format, ... );

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
