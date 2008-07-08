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
 *  @(#) $Id: syslog.c,v 1.9 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	システムログ機能
 */

#undef OMIT_SYSLOG
#include "jsp_kernel.h"
#include "time_event.h"
#include "syslog.h"

/*
 *  コンテキストに依らないCPUロック／ロック解除
 */
#define	lock_cpu()	(sense_context() ? i_lock_cpu() : t_lock_cpu())
#define	unlock_cpu()	(sense_context() ? i_unlock_cpu() : t_unlock_cpu())

#ifdef __logini

/*
 *  ログバッファとそれにアクセスするためのポインタ
 */
SYSLOG	syslog_buffer[TCNT_SYSLOG_BUFFER];	/* ログバッファ */
UINT	syslog_count;			/* ログバッファ中のログの数 */
UINT	syslog_head;			/* 先頭のログの格納位置 */
UINT	syslog_tail;			/* 次のログの格納位置 */
UINT	syslog_lost;			/* 失われたログの数 */

/*
 *  出力すべきログ情報の重要度（ビットマップ）
 */
UINT	syslog_logmask;			/* ログバッファに記録すべき重要度 */
UINT	syslog_lowmask;			/* 低レベル出力すべき重要度 */

/*
 *  システムログ機能の初期化
 */
void
syslog_initialize()
{
	syslog_count = 0;
	syslog_head = syslog_tail = 0;
	syslog_lost = 0;

	syslog_logmask = 0;
	syslog_lowmask = LOG_UPTO(LOG_NOTICE);
}     

#endif /* __logini */

/* 
 *  ログ情報の出力
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 */
#ifdef __vwri_log

SYSCALL ER
vwri_log(UINT prio, SYSLOG *p_log)
{
	BOOL	locked;

	locked = sense_lock();
	if (!locked) {
		lock_cpu();
	}

	/*
	 *  ログ時刻の設定
	 */
	p_log->logtim = systim_offset + current_time;

	/*
	 *  ログバッファに記録
	 */
	if ((syslog_logmask & LOG_MASK(prio)) != 0) {
		syslog_buffer[syslog_tail] = *p_log;
		syslog_tail++;
		if (syslog_tail >= TCNT_SYSLOG_BUFFER) {
			syslog_tail = 0;
		}
		if (syslog_count < TCNT_SYSLOG_BUFFER) {
			syslog_count++;
		}
		else {
			syslog_head = syslog_tail;
			syslog_lost++;
		}
	}

	/*
	 *  低レベル出力
	 */
	if ((syslog_lowmask & LOG_MASK(prio)) != 0) {
		syslog_print(p_log, sys_putc);
	}

	if (!locked) {
		unlock_cpu();
	}
	return(E_OK);
}

#endif /* __vwri_log */

/*
 *  ログバッファからの読出し
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 */
#ifdef __vrea_log

SYSCALL ER_UINT
vrea_log(SYSLOG *p_log)
{
	BOOL	locked;
	ER_UINT	ercd;

	locked = sense_lock();
	if (!locked) {
		lock_cpu();
	}
	if (syslog_count > 0) {
		*p_log = syslog_buffer[syslog_head];
		syslog_count--;
		syslog_head++;
		if (syslog_head >= TCNT_SYSLOG_BUFFER) {
			syslog_head = 0;
		}
		ercd = (ER_UINT) syslog_lost;
		syslog_lost = 0;
	}
	else {
		ercd = E_OBJ;
	}
	if (!locked) {
		unlock_cpu();
	}
	return(ercd);
}

#endif /* __vrea_log */

/* 
 *  出力すべきログ情報の重要度の設定
 */
#ifdef __vmsk_log

SYSCALL ER
vmsk_log(UINT logmask, UINT lowmask)
{
	syslog_logmask = logmask;
	syslog_lowmask = lowmask;
	return(E_OK);
}

#endif /* __vmsk_log */

/* 
 *  システムログ機能の終了処理
 *
 *  ログバッファに記録されたログ情報を，低レベル出力機能を用いて出力す
 *  る．
 */
#ifdef __logter

void
syslog_terminate()
{
	syslog_printf("-- buffered messages --", NULL, sys_putc);
	syslog_output(sys_putc);
}

#endif /* __logter */
