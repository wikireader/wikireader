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
 *  @(#) $Id: sample1.c,v 1.9 2003/12/24 08:23:58 takayuki Exp $
 */

/* 
 *  サンプルプログラム(1)の本体
 *
 *  JSPカーネルの基本的な動作を確認するためのサンプルプログラム．
 *
 *  プログラムの概要:
 *
 *  ユーザインタフェースを受け持つメインタスク（タスクID: MAIN_TASK，
 *  優先度: MAIN_PRIORITY）と，三つの並列実行されるタスク（タスクID:
 *  TASK1〜TASK3，初期優先度: MID_PRIORITY）で構成される．また，起動周
 *  期が2秒の周期ハンドラ（周期ハンドラID: CYCHDR1）を用いる．
 *
 *  並列実行されるタスクは，task_loop 回空ループを実行する度に，タスク
 *  が実行中であることをあらわすメッセージを表示する．
 *
 *  周期ハンドラは，三つの優先度（HIGH_PRIORITY，MID_PRIORITY，
 *  LOW_PRIORITY）のレディキューを回転させる．プログラムの起動直後は，
 *  周期ハンドラは停止状態になっている．
 *
 *  メインタスクは，シリアルI/Oポートからの文字入力を行い（文字入力を
 *  待っている間は，並列実行されるタスクが実行されている），入力された
 *  文字に対応した処理を実行する．入力された文字と処理の関係は次の通り．
 *  Control-C または 'Q' が入力されると，プログラムを終了する．
 *
 *  '1' : 以降のコマンドは TASK1 に対して行う．
 *  '2' : 以降のコマンドは TASK2 に対して行う．
 *  '3' : 以降のコマンドは TASK3 に対して行う．
 *  'a' : タスクを act_tsk により起動する．
 *  'A' : タスクに対する起動要求を can_act によりキャンセルする．
 *  'e' : タスクに ext_tsk を呼び出させ，終了させる．
 *  't' : タスクを ter_tsk により強制終了する．
 *  '>' : タスクの優先度を HIGH_PRIORITY にする．
 *  '=' : タスクの優先度を MID_PRIORITY にする．
 *  '<' : タスクの優先度を LOW_PRIORITY にする．
 *  'G' : タスクの優先度を get_pri で読み出す．
 *  's' : タスクに slp_tsk を呼び出させ，起床待ちにさせる．
 *  'S' : タスクに tslp_tsk(10秒) を呼び出させ，起床待ちにさせる．
 *  'w' : タスクを wup_tsk により起床する．
 *  'W' : タスクに対する起床要求を can_wup によりキャンセルする．
 *  'l' : タスクを rel_wai により強制的に待ち解除にする．
 *  'u' : タスクを sus_tsk により強制待ち状態にする．
 *  'm' : タスクの強制待ち状態を rsm_tsk により解除する．
 *  'M' : タスクの強制待ち状態を frsm_tsk により強制解除する．
 *  'd' : タスクに dly_tsk(10秒) を呼び出させ，時間経過待ちにさせる．
 *  'x' : タスクにパターン 0x0001 の例外処理を要求する．
 *  'X' : タスクにパターン 0x0002 の例外処理を要求する．
 *  'y' : タスクに dis_tex を呼び出させ，タスク例外を禁止する．
 *  'Y' : タスクに ena_tex を呼び出させ，タスク例外を許可する．
 *  'r' : 三つの優先度（HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY）の
 *        レディキューを回転させる．
 *  'c' : 周期ハンドラを動作させる．
 *  'C' : 周期ハンドラを停止させる．
 *  'z' : CPU例外を発生させる．
 *  'Z' : CPUロック状態でCPU例外を発生させる（プログラムを終了する）．
 *  'V' : vxget_tim で性能評価用システム時刻を2回読む．
 *  'v' : 発行したシステムコールを表示する（デフォルト）．
 *  'q' : 発行したシステムコールを表示しない．
 */

#include <t_services.h>
#include "kernel_id.h"
#include "sample1.h"

/*
 *  並行実行されるタスクへのメッセージ領域
 */
UB	message[3];

/*
 *  ループ回数
 */
UW	task_loop;		/* タスク内でのループ回数 */
UW	tex_loop;		/* 例外処理ルーチン内でのループ回数 */

/*
 *  並行実行されるタスク
 */
void task(VP_INT exinf)
{
	volatile UW	i;
	INT	n = 0;
	INT	tskno = (INT) exinf;
	UB	*graph[] = { "|", "  +", "    *" };
	UB	c;

	ena_tex();
	while (1) {
		syslog(LOG_NOTICE, "task%d is running (%03d).   %s",
					tskno, ++n, graph[tskno-1]);
		for (i = 0; i < task_loop; i++);
		c = message[tskno-1];
		message[tskno-1] = 0;
		switch (c) {
		case 'e':
			syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
			ext_tsk();
		case 's':
			syslog(LOG_INFO, "#%d#slp_tsk()", tskno);
			syscall(slp_tsk());
			break;
		case 'S':
			syslog(LOG_INFO, "#%d#tslp_tsk(10000)", tskno);
			syscall(tslp_tsk(10000));
			break;
		case 'd':
			syslog(LOG_INFO, "#%d#dly_tsk(10000)", tskno);
			syscall(dly_tsk(10000));
			break;
		case 'y':
			syslog(LOG_INFO, "#%d#dis_tex()", tskno);
			syscall(dis_tex());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%d#ena_tex()", tskno);
			syscall(ena_tex());
			break;
#ifdef CPUEXC1
		case 'z':
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			break;
		case 'Z':
			loc_cpu();
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			unl_cpu();
			break;
#endif /* CPUEXC1 */
		default:
			break;
		}
	}
}

/*
 *  並行して実行されるタスク用のタスク例外処理ルーチン
 */
void tex_routine(TEXPTN texptn, VP_INT exinf)
{
	volatile UW	i;
	INT	tskno = (INT) exinf;

	syslog(LOG_NOTICE, "task%d receives exception 0x%04x. ",
					tskno, texptn);
	for (i = 0; i < tex_loop; i++);

	if (texptn & 0x8000) {
		syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
		ext_tsk();
	}
}

/*
 *  CPU例外ハンドラ
 */
#ifdef CPUEXC1

void
cpuexc_handler(VP p_excinf)
{
	ID	tskid;

	syslog(LOG_NOTICE, "CPU exception handler (p_excinf = %08x).",
							 p_excinf);
	if (sns_ctx() != TRUE) {
		syslog(LOG_WARNING,
			"sns_ctx() is not TRUE in CPU exception handler.");
	}
	if (sns_dpn() != TRUE) {
		syslog(LOG_WARNING,
			"sns_dpn() is not TRUE in CPU exception handler.");
	}
	syslog(LOG_DEBUG,
		"sns_loc = %d sns_dsp = %d", sns_loc(), sns_dsp());
	syslog(LOG_DEBUG,
		"vxsns_loc = %d vxsns_ctx = %d vxsns_dsp = %d vxsns_dpn = %d",
		vxsns_loc(p_excinf), vxsns_ctx(p_excinf),
		vxsns_dsp(p_excinf), vxsns_dpn(p_excinf));

	if (!vxsns_loc(p_excinf) && !vxsns_ctx(p_excinf)) {
		syscall(iget_tid(&tskid));
		syscall(iras_tex(tskid, 0x8000));
	}
	else {
		syslog(LOG_NOTICE, "Sample program ends with exception.");
		kernel_exit();
	}
}

#endif /* CPUEXC1 */

/*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void cyclic_handler(VP_INT exinf)
{
	irot_rdq(HIGH_PRIORITY);
	irot_rdq(MID_PRIORITY);
	irot_rdq(LOW_PRIORITY);
}

/*
 *  メインタスク
 */
void main_task(VP_INT exinf)
{
	UB	c;
	ID	tskid = TASK1;
	volatile UW	i;
	INT count;
	INT	tskno = 1;
	ER_UINT	ercd;	
	PRI	tskpri;
	SYSTIM	stime1, stime2;
#ifndef OMIT_VGET_TIM
	SYSUTIM	utime1, utime2;
#endif /* OMIT_VGET_TIM */

	vmsk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", exinf);

	serial_ctl_por(TASK_PORTID, (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV));

	/*
 	 *  ループ回数の設定
	 */
	count = 0;
	task_loop = LOOP_REF;
	get_tim(&stime1);
	do {
		++ count;
		for (i = 0; i < task_loop; i++);
		get_tim(&stime2);
	}while(stime1 == stime2);
	task_loop = LOOP_REF * 400 / (stime2 - stime1);
	tex_loop = task_loop / 5;

	/*
 	 *  タスクの起動
	 */
	act_tsk(TASK1);
	act_tsk(TASK2);
	act_tsk(TASK3);

	/*
 	 *  メインループ
	 */
	do {
		serial_rea_dat(TASK_PORTID, &c, 1);
		switch (c) {
		case 'e':
		case 's':
		case 'S':
		case 'd':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
			message[tskno-1] = c;
			break;
		case '1':
			tskno = 1;
			tskid = TASK1;
			break;
		case '2':
			tskno = 2;
			tskid = TASK2;
			break;
		case '3':
			tskno = 3;
			tskid = TASK3;
			break;
		case 'a':
			syslog(LOG_INFO, "#act_tsk(%d)", tskno);
			syscall(act_tsk(tskid));
			break;
		case 'A':
			syslog(LOG_INFO, "#can_act(%d)", tskno);
			syscall(ercd = can_act(tskid));
			if (MERCD(ercd) >= 0) {
				syslog(LOG_NOTICE, "can_act(%d) returns %d",
						tskid, ercd);
			}
			break;
		case 't':
			syslog(LOG_INFO, "#ter_tsk(%d)", tskno);
			syscall(ter_tsk(tskid));
			break;
		case '>':
			syslog(LOG_INFO, "#chg_pri(%d, HIGH_PRIORITY)", tskno);
			chg_pri(tskid, HIGH_PRIORITY);
			break;
		case '=':
			syslog(LOG_INFO, "#chg_pri(%d, MID_PRIORITY)", tskno);
			chg_pri(tskid, MID_PRIORITY);
			break;
		case '<':
			syslog(LOG_INFO, "#chg_pri(%d, LOW_PRIORITY)", tskno);
			chg_pri(tskid, LOW_PRIORITY);
			break;
		case 'G':
			syslog(LOG_INFO, "#get_pri(%d, &tskpri)", tskno);
			syscall(ercd = get_pri(tskid, &tskpri));
			if (MERCD(ercd) >= 0) {
				syslog(LOG_NOTICE, "priority of task %d is %d",
						tskid, tskpri);
			}
			break;
		case 'w':
			syslog(LOG_INFO, "#wup_tsk(%d)", tskno);
			syscall(wup_tsk(tskid));
			break;
		case 'W':
			syslog(LOG_INFO, "#can_wup(%d)", tskno);
			syscall(ercd = can_wup(tskid));
			if (MERCD(ercd) >= 0) {
				syslog(LOG_NOTICE, "can_wup(%d) returns %d",
						tskid, ercd);
			}
			break;
		case 'l':
			syslog(LOG_INFO, "#rel_wai(%d)", tskno);
			syscall(rel_wai(tskid));
			break;
		case 'u':
			syslog(LOG_INFO, "#sus_tsk(%d)", tskno);
			syscall(sus_tsk(tskid));
			break;
		case 'm':
			syslog(LOG_INFO, "#rsm_tsk(%d)", tskno);
			syscall(rsm_tsk(tskid));
			break;
		case 'M':
			syslog(LOG_INFO, "#frsm_tsk(%d)", tskno);
			syscall(frsm_tsk(tskid));
			break;
		case 'x':
			syslog(LOG_INFO, "#ras_tsk(%d, 0x0001)", tskno);
			syscall(ras_tex(tskid, 0x0001));
			break;
		case 'X':
			syslog(LOG_INFO, "#ras_tsk(%d, 0x0002)", tskno);
			syscall(ras_tex(tskid, 0x0002));
			break;
		case 'r':
			syslog(LOG_INFO, "#rot_rdq(three priorities)");
			rot_rdq(HIGH_PRIORITY);
			rot_rdq(MID_PRIORITY);
			rot_rdq(LOW_PRIORITY);
			break;
		case 'c':
			sta_cyc(CYCHDR1);
			break;
		case 'C':
			stp_cyc(CYCHDR1);
			break;
#ifndef OMIT_VGET_TIM
		case 'V':
			syscall(vxget_tim(&utime1));
			syscall(vxget_tim(&utime2));
			syslog(LOG_NOTICE, "utime1 = %d, utime2 = %d",
						(UINT) utime1, (UINT) utime2);
			break;
#endif /* OMIT_VGET_TIM */
		case 'v':
			vmsk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG));
			break;
		case 'q':
			vmsk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG));
			break;
		default:
			break;
		}
	} while (c != '\003' && c != 'Q');

	syslog(LOG_NOTICE, "Sample program ends.");
	kernel_exit();
}
