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
 *  @(#) $Id: cpu_config.h,v 1.9 2003/12/24 07:24:40 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（Linux用）
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include <cpu_rename.h>

#ifndef _MACRO_ONLY

/*
 *  Linux用標準インクルードファイル
 */

#include <signal.h>
#include <setjmp.h>

/*
 *  chg_ims/get_ims をサポートするかどうかの定義
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

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	jmp_buf env;		/* CPUコンテキスト */
} CTXB;

/*
 *   CPU例外ハンドラ
 */

#define EXCHNO_ZDIV  SIGFPE

/*
 *  システム状態参照
 */


Inline int
current_stack()
{
    struct sigaltstack      ss;

    sigaltstack(((void *)0), &ss);
    return(ss.ss_flags & SS_ONSTACK);
}


Inline BOOL
sense_context()
{
	return(current_stack());
}

Inline BOOL
sense_lock()
{
    sigset_t currentSet;
    
    sigprocmask(SIG_BLOCK, ((void *)0), &currentSet);

#if defined(linux) & (defined(i386) | defined(_i386_) | defined(__i386__))
    if(currentSet.__val[0] == ~(0 | (1<<(SIGKILL-1)) | (1<<(SIGSTOP-1)))
       &&  currentSet.__val[1] == ~0 )
        return(TRUE);
    else
        return(FALSE);
#else
    
#error not supported.
    
#endif /* linux */    
}


#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は，chg_ipm をサポートするための変数．chg_ipm をサポー
 *  トしない場合には，task_intmask が常に 0 になっていると考えればよい．
 */

#ifdef SUPPORT_CHG_IMS
extern IMS	task_sigmask;	/* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IMS */

Inline void
t_lock_cpu()
{
    sigset_t  newmask;
    
    sigfillset(&newmask);
    sigprocmask(SIG_SETMASK,&newmask,0);
}

Inline void
t_unlock_cpu()
{
#ifdef SUPPORT_CHG_IMS
	/*
	 *  t_unlock_cpu が呼び出されるのは CPUロック状態のみであるた
	 *  め，処理の途中で task_intmask が書き換わることはない．
	 */
    sigprocmask(SIG_SETMASK,&task_sigmask,0);
#else /* SUPPORT_CHG_IMS */
    sigset_t  mask;
    
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK,&mask,0);
#endif /* SUPPORT_CHG_IMS */
}

/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

extern IMS	int_sigmask;	/* 非タスクコンテキストでの割込みマスク */

Inline void
i_lock_cpu()
{
    sigset_t  mask;
    
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK,&mask,&int_sigmask);
}

Inline void
i_unlock_cpu()
{
    sigprocmask(SIG_SETMASK,&int_sigmask,0);
}

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
{
    /*
     *  SIGUSR1をブロック、スタック切り替え有りで登録する。
     *  優先度を付けたい場合はマスクするビットを指定する
     *  必要があるのでこの方法は使えない。
     */
    
    struct sigaction action;

    action.sa_handler = inthdr;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask,SIGUSR1);
    action.sa_flags = SA_ONSTACK;
    
    sigaction(inhno, &action, 0);    
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
Inline void
define_exc(EXCNO excno, FP exchdr)
{
    /*
     * ブロックするマスク無し、スタック切り替え有りで登録する。
     */
    struct sigaction action;

    action.sa_handler = exchdr;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask,SIGUSR1);    
    action.sa_flags = SA_ONSTACK;    
    sigaction(excno, &action, 0);
}

/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 *  シグナルが発行されてるとシグナルスタックに自動的に切り替わる。
 *  全ての割り込みハンドラはディスパッチャを起動するSIGUSR1をマスクして
 *  動作する。そのためreqflgのチェック後シグナルが発行されて起動された
 *  ハンドラでディスパッチが要求されても正しく動く
 */


#define	INTHDR_ENTRY(inthdr)      \
void inthdr##_entry(void){        \
     inthdr();                           /* 割り込みハンドラを呼び出す */ \
     if(_kernel_reqflg)                  /* regflg がTRUEであれば      */ \
       raise(SIGUSR1);                   /* ディスパッチャを呼び出す   */ \
}                                      

#define INT_ENTRY(inthdr)   inthdr##_entry


/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 *  シグナルが発行されてるとシグナルスタックに自動的に切り替わる。 
 *  全てのCPU例外ハンドラはディスパッチャを起動するSIGUSR1をマスクして
 *  動作する。そのためreqflgのチェック後シグナルが発行されて起動された
 *  ハンドラでディスパッチが要求されても正しく動く 
 */
#define	EXCHDR_ENTRY(exchdr)	  \
void exchdr##_entry(VP sp){        \
     exchdr(sp);                         /* 割り込みハンドラを呼び出す */ \
     if(_kernel_reqflg)                  /* regflg がTRUEであれば      */ \
       raise(SIGUSR1);                   /* ディスパッチャを呼び出す   */ \
}

#define EXC_ENTRY(exchdr) exchdr##_entry

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のディスパッチ
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	return((*((UH *) p_excinf) & 0x1000) == 0);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	return((*((UH *) p_excinf) & 0x0700) == 0x0700);
}


/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */







