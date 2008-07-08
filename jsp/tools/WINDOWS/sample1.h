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
 *  @(#) $Id: sample1.h,v 1.6 2003/12/20 09:13:29 takayuki Exp $
 */

/*
 *  サンプルプログラム(1)のヘッダファイル
 */

#include "cpu_defs.h"
#include "sys_defs.h"

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

#elif defined(ARM7TDMI)

#define CPUEXC1		4		/* ロードエラー例外 */
#define RAISE_CPU_EXCEPTION	(*((volatile int *) 0xFFFFFEC1))

#elif defined(V850)

#elif defined(H8)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */

#elif defined(H8S)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define LOOP_REF	4000L		/* 速度計測用のループ回数 */

#elif defined(MICROBLAZE)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define STACK_SIZE	2048		/* タスクのスタックサイズ */

#elif defined(I386)

#define CPUEXC1		0		/* ゼロ除算例外 */
#define RAISE_CPU_EXCEPTION   syslog(LOG_NOTICE, "zerodiv = %d", 10 / 0)
#define OMIT_VGET_TIM

#elif defined(TMS320C54X)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define STACK_SIZE	320		/* タスクのスタックサイズ */
#define LOOP_REF	500000L		/* 速度計測用のループ回数 */

#elif defined(LINUX)

#undef CPUEXC1				/* CPU例外ハンドラをサポートしない */
#define OMIT_VGET_TIM
#define LOOP_REF	4000000		/* 速度計測用のループ回数 */

#elif defined(WINDOWS)

#define LOOP_REF	5000000		/* 速度計測用のループ回数 */

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

#ifndef LOOP_REF
#define LOOP_REF	1000000		/* 速度計測用のループ回数 */
#endif /* LOOP_REF */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef _MACRO_ONLY

extern void	task(VP_INT tskno);
extern void	main_task(VP_INT exinf);
extern void	tex_routine(TEXPTN texptn, VP_INT tskno);
extern void	cyclic_handler(VP_INT exinf);
extern void	cpuexc_handler(VP p_excinf);

#endif /* _MACRO_ONLY */
