/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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
 */

/*
 *  プロセッサ依存モジュール(S1C33用)
 */


#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include "cpu_rename.h"


/*
 *  設定可能な最高割り込み優先度
 */
#define	TPS_INTLEV_MAX	(0x04)
#define	TPS_CPULOCK_LEV	(TPS_INTLEV_MAX + 1)

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅	 */


#ifndef _MACRO_ONLY
/*
 *  TCB 関連の定義
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */
/*
 *  タスクコンテキストブロックの定義
 *  Task context block.
 */
typedef struct task_context_block {
	VP sp;				/* スタックポインタ	*/
	FP pc;				/* プログラムカウンタ	*/
} CTXB;


/*
 *  最高優先順位タスクへのディスパッチ(cpu_config.c)
 *
 *  dispatch は，タスクコンテキストから呼び出されたサービスコール処理
 *  内で，CPUロック状態で呼び出さなければならない．
 */
extern void dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ(cpu_config.c)
 *
 *  exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 */
extern void exit_and_dispatch(void);

/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

/*
 *  CPUロック解除時に復元する割り込みレベル
 */
extern UINT	tps_OrgIntLevel;

/*
 *  割込みネストカウンタ
 */
extern UINT	tps_IntNestCnt;

/*
 *  ビットサーチマクロ使用の設定
 */
#ifndef __c33pe
#define CPU_BITMAP_SEARCH
#endif  /* __c33pe */

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#include "cpu_insn.h"

/*
 * システム状態参照関数の別名定義
 */
#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock
#define	t_lock_cpu	lock_cpu
#define	i_lock_cpu	lock_cpu
#define	t_unlock_cpu	unlock_cpu
#define	i_unlock_cpu	unlock_cpu

/*
 *  現在のコンテキスト状態を参照
 */
Inline BOOL
sense_context()
{
	return ((tps_IntNestCnt) ? TRUE : FALSE);
}

/*
 *  現在のCPUロック状態を参照
 */
Inline BOOL
sense_lock()
{
	UW	ulIntLevel;

	ulIntLevel = get_psr();

	ulIntLevel = (ulIntLevel & S1C33_PSR_MASK_IL);
	return ((ulIntLevel < (TPS_CPULOCK_LEV << 8)) ? FALSE : TRUE);
}

/*
 * CPUロック状態設定処理
 */
Inline void
lock_cpu()
{
	UW	ulPsr, ulIntLevel;

	ulPsr = get_psr();
	ulIntLevel = (ulPsr & S1C33_PSR_MASK_IL);

	if(ulIntLevel < (TPS_CPULOCK_LEV << 8)){
		ulPsr &= ~S1C33_PSR_MASK_IL;
		set_psr(ulPsr | (TPS_CPULOCK_LEV << 8));
		tps_OrgIntLevel = ulIntLevel;
	}
}

Inline void
unlock_cpu()
{
	UW	ulPsr;

	ulPsr = get_psr();
	ulPsr &= ~S1C33_PSR_MASK_IL;
	ulPsr |= tps_OrgIntLevel;
	tps_OrgIntLevel = (TPS_INIT_INTLEVEL << 8);

	set_psr(ulPsr);
}

/*
 *  CPU例外の発生した時のシステム状態の参照
 */
/*
 *  CPU例外の発生した時のコンテキストの参照
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	return ((tps_IntNestCnt > 1) ? TRUE : FALSE);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	UW	ulPsr;

	ulPsr = ((*((UW *)p_excinf) & S1C33_PSR_MASK_IL) >> 8);
	return (ulPsr == TPS_CPULOCK_LEV) ? TRUE : FALSE;
}

/*
 *  例外ベクタテーブルの構造の定義
 */
typedef struct exc_vector_entry {
	FP exchdr;				/* 例外ハンドラの起動番地 */
} EXCVE;

/*
 *  CPU例外ハンドラの設定
 */
#define	define_exc	define_inh

/*
 *  割込みハンドラの設定
 */
Inline void
define_inh(INHNO inhno, FP inthdr)
{
	EXCVE	*excvt;

	excvt = (EXCVE *) get_ttbr();
	excvt[inhno].exchdr = inthdr;
}

/*
 *  割込みハンドラの出入口処理の生成マクロ
 */
#define INTHDR_ENTRY(inthdr)	void inthdr##_entry(void) { tpsIntPreWrap(); asm("xcall " #inthdr); tpsIntPostWrap(); }
#define	INT_ENTRY(inthdr)	inthdr##_entry

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 */
#define EXCHDR_ENTRY(exchdr)	INTHDR_ENTRY(exchdr)
#define	EXC_ENTRY(exchdr)	exchdr##_entry

/*
 *  割込み/CPU例外ハンドラの出入口処理
 *
 *  idle中は割り込まれたスタック==stacktopなので、
 *  idleルーチンにもretiで復帰できる位置に割り込みスタックを設定している
 */
Inline void
tpsIntPreWrap(void)
{
#if TPS_DAREA_CNT == 4			/* データエリア数に応じて退避	*/
	Asm("pushn %r11");		/* レジスタ数を変更する。	*/
#elif TPS_DAREA_CNT == 3
	Asm("pushn %r12");
#elif TPS_DAREA_CNT == 2
	Asm("pushn %r13");
#elif TPS_DAREA_CNT == 1
	Asm("pushn %r14");
#else
	Asm("pushn %r15");
#endif /* TPS_DAREA */

#ifdef	__c33adv
	Asm("pushs 	%sor				");
#else	/* __c33adv */
	Asm("ld.w	%r0, %ahr			");
	Asm("ld.w	%r1, %alr			");
#endif	/* __c33adv */

	Asm("ld.w	%r2, %sp			");	/* スタックの切り替え、		*/
	Asm("xld.w	%r3, _kernel_tps_IntNestCnt	");	/* 割り込みカウンタの更新、	*/
	Asm("ld.w	%r5, [%r3]			");	/* 多重割り込みの許可を行う	*/
	Asm("xld.w	%%r4, %0 - 8  " : : "g"(STACKTOP));
	Asm("cmp	%r5, 0x00			");
	Asm("jrne	0f				");
	Asm("ld.w	%sp, %r4			");
	Asm("0:						");
	Asm("add	%r5, 1				");
	Asm("ld.w	[%r3], %r5			");
	Asm("ld.w	%r4, %psr			");
	Asm("or		%r4, 0x10			");
	Asm("ld.w	%psr, %r4			");
	Asm("ld.w	%r6, %r2			");

#ifdef	__c33adv						/* pushs命令によるスタック使用	*/
	Asm("add	%r6, 24");				/* サイズを加算する		*/
#endif	/* __c33adv */
	
#if TPS_DAREA_CNT == 4
	Asm("add	%r6, 48");
#elif TPS_DAREA_CNT == 3
	Asm("add	%r6, 52");
#elif TPS_DAREA_CNT == 2
	Asm("add	%r6, 56");
#elif TPS_DAREA_CNT == 1
	Asm("add	%r6, 60");
#else
	Asm("ext	0x0001");
	Asm("add	%r6, 0x00");
#endif /* TPS_DAREA */
}

Inline void
tpsIntPostWrap(void)
{
	Asm("ld.w	%r4, %psr		");	/* 割り込みをディセーブルする	*/
	Asm("xand	%r4, 0xfffff0ff		");
	Asm("xld.w	%%r5, %0"
		: : "g"(TPS_CPULOCK_LEV << 8));
	Asm("or		%r4, %r5		");
	Asm("ld.w	%psr, %r4		");

	Asm("ld.w	%r4, [%r3]		");	/* tps_IntNestCntを更新する	*/
	Asm("sub	%r4, 1			");
	Asm("ld.w	[%r3], %r4		");

	Asm("ld.w	%sp, %r2		");	/* スタック領域を復元する	*/

	Asm("cmp	%r4, 0x00		");
	Asm("xjrne	0f			");
	Asm("xld.w	%r3, _kernel_reqflg	");
	Asm("ld.w	%r3, [%r3]		");
	Asm("cmp	%r3, 1			");	/* tps_IntNestCnt== 0でディス	  */
	Asm("xjreq	_kernel_ret_int		");	/* パッチが発生していればジャンプ */
	Asm("0:					");	/* する				  */

#ifdef	__c33adv					/* 退避した特殊レジスタを復元する */
	Asm("pops	%sor			");
#else	/* __c33adv */
	Asm("ld.w	%alr, %r1		");
	Asm("ld.w	%ahr, %r0		");
#endif	/* __c33adv */

#if TPS_DAREA_CNT == 4
	Asm("popn %r11");
#elif TPS_DAREA_CNT == 3
	Asm("popn %r12");
#elif TPS_DAREA_CNT == 2
	Asm("popn %r13");
#elif TPS_DAREA_CNT == 1
	Asm("popn %r14");
#else
	Asm("popn %r15");
#endif /* TPS_DAREA */

	Asm("reti");
}

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
