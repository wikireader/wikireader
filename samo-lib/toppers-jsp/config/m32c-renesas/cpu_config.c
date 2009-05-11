/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: cpu_config.c,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

/*
 *  プロセッサ依存モジュール（M32C用）
 */

#include "jsp_kernel.h"
#include "sil.h"
#include "check.h"
#include "task.h"
#include "oaks32.h"

/* 
 *  割込み/CPU例外ネストカウンタ
 */
char	intnest;

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
	/* M32C(100ピン版)の場合, 以下の6つのレジスタ初期化が必要 */
	sil_wrb_mem((VP)TADR_SFR_PD11 , 0xff);
	sil_wrb_mem((VP)TADR_SFR_PD12 , 0xff);
	sil_wrb_mem((VP)TADR_SFR_PD13 , 0xff);
	sil_wrb_mem((VP)TADR_SFR_PD14 , 0xff);
	sil_wrb_mem((VP)TADR_SFR_PD15 , 0xff);
	sil_wrb_mem((VP)TADR_SFR_PUR4 , 0);

	/* 割込みネストカウント初期化(非タスクコンテキスト) */
	intnest = 1;
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
}

/*
 * software_init_hookの仮定義
 */
#ifndef EXTERNAL_SOFT_HOOK
const VP_INT software_init_hook = 0;
#endif  /* EXTERNAL_SOFT_HOOK */
