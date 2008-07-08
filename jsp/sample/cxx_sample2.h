/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 Takagi Nobuhisa
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
 *  @(#) $Id: cxx_sample2.h,v 1.4 2005/07/06 01:16:44 honda Exp $
 */

#include <t_services.h>

/*
 *  各タスクの優先度の定義
 */

#define MAIN_PRIORITY	5		/* メインタスクの優先度 */
					/* HIGH_PRIORITY より高くすること */

#define HIGH_PRIORITY	9		/* 並列に実行されるタスクの優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  ターゲット依存の定義（CPU例外ハンドラの起動方法など）
 */

#ifdef M68K

#define CPUEXC1		5		/* ゼロ除算例外 */
#define RAISE_CPU_EXCEPTION	syslog(LOG_NOTICE, "zerodiv = %d", 10 / 0)

#elif defined(SH3)

#define CPUEXC1		224		/* ロードエラー例外 */
#define RAISE_CPU_EXCEPTION	(*((volatile int *) 0xFFFFFEC1))

#elif defined(SH1)

#define CPUEXC1		9		/* CPUアドレスエラー例外 */
#define RAISE_CPU_EXCEPTION	(*((volatile int *) 0xFFFFFEC1))
#ifdef TOKIWA_SH1
#define	STACK_SIZE	512		/* タスクのスタックサイズ */
#endif /* TOKIWA_SH1 */

#elif defined(ARMV4)

#define CPUEXC1		4		/* ロードエラー例外 */
#define RAISE_CPU_EXCEPTION	(*((volatile int *) 0xFFFFFEC1))

#elif defined(V850)

#elif defined(H8)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define	TASK_PORTID	2		/* 文字入力するシリアルポートID */

#elif defined(H8S)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define LOOP_REF	4000L		/* 速度計測用のループ回数 */

#elif defined(MICROBLAZE)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define STACK_SIZE	2048		/* タスクのスタックサイズ */

#elif defined(IA32)

#define CPUEXC1		0		/* ゼロ除算例外 */
#define RAISE_CPU_EXCEPTION   syslog(LOG_NOTICE, "zerodiv = %d", 10 / 0)
#define OMIT_VGET_TIM

#elif defined(TMS320C54X)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define STACK_SIZE	320		/* タスクのスタックサイズ */
#define LOOP_REF	500000L		/* 速度計測用のループ回数 */

#elif defined(XSTORMY16)

#define CPUEXC1		0		/* 無効命令例外 */
#define RAISE_CPU_EXCEPTION	__asm__( ".hword 0x0006" )
#define OMIT_VGET_TIM			/* vxget_tim()は非サポート */
#define LOOP_REF 	4000L		/* 速度計測用のループ回数 */
#define STACK_SIZE	256		/* タスクのスタックサイズ */
#define TASK_PORTID	2		/* SIOはポート2を用いる */

#elif defined(MIPS3) || defined(MIPS64)

#define CPUEXC1     Bp      /* ブレークポイント例外（ゼロ除算時に発生） */
#define RAISE_CPU_EXCEPTION   syslog(LOG_NOTICE, "zerodiv = %d", 10 / 0)

#elif defined(LINUX)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define OMIT_VGET_TIM
#define LOOP_REF	4000000		/* 速度計測用のループ回数 */

#elif defined(NIOS2)

#define CPUEXC1		0		  /* 未実装命令例外 */
#define RAISE_CPU_EXCEPTION	  Asm("div zero, zero, zero");

#endif

/*
 *  ターゲットに依存する可能性のある定数の定義
 */

#ifndef TASK_PORTID
#define	TASK_PORTID	1		/* 文字入力するシリアルポートID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE	8192		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef _MACRO_ONLY

#ifdef __cplusplus
extern "C" {
#endif

extern void main_task(VP_INT exinf);
extern void task(VP_INT exinf);
extern void _toppers_cxxrt_reset_specific(void) throw();

#ifdef __cplusplus
}
#endif

#endif /* _MACRO_ONLY */
