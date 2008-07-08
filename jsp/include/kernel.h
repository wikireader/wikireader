/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: kernel.h,v 1.22 2007/05/08 07:33:51 honda Exp $
 */

/*
 *	μITRON4.0仕様標準インクルードファイル
 *
 *  このファイルでは，スタンダードプロファイルで必要なものと，JSPカー
 *  ネル独自の拡張機能で必要なものだけを定義している．データ型の定義は，
 *  スタンダードプロファイルを満たすちょうどの長さにはしていない．
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 *
 *  このインクルードファイルは，標準インクルードファイル（t_services.h 
 *  と jsp_kernel.h）でインクルードされる．また，他の ITRON仕様OS から
 *  ソフトウェアをポーティングする場合などには，このファイルを直接イン
 *  クルードしてもよい．この例外を除いて，他のファイルから直接インクルー
 *  ドされることはない．
 *
 *  この中でインクルードしているファイルを除いて，他のインクルードファ
 *  イルに依存していない．
 */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  カーネル・アプリケーション 共通インクルードファイル
 */
#include <t_stddef.h>

/*
 *  ITRON仕様共通規定のデータ型・定数・マクロ
 */
#include <itron.h>

/*
 *  システムやプロセッサに依存する定義
 */
#include <sys_defs.h>
#include <cpu_defs.h>

/*
 *  システムログサービスのための定義
 */
#include <t_syslog.h>

/*
 *  補助マクロ
 */
#define	TROUND_VP(sz)	(((sz) + sizeof(VP) - 1) & ~(sizeof(VP) - 1))
#define	TCOUNT_VP(sz)	(((sz) + sizeof(VP) - 1) / sizeof(VP))

/*
 *  データ型の定義
 */
#ifndef _MACRO_ONLY

typedef	UINT		TEXPTN;		/* タスク例外要因のビットパターン */
typedef	UINT		FLGPTN;		/* イベントフラグのビットパターン */

typedef	struct t_msg {			/* メールボックスのメッセージヘッダ */
	struct t_msg	*next;
} T_MSG;

typedef	struct t_msg_pri {		/* 優先度付きメッセージヘッダ */
	T_MSG		msgque;		/* メッセージヘッダ */
	PRI		msgpri;		/* メッセージ優先度 */
} T_MSG_PRI;

#endif /* _MACRO_ONLY */

/*
 *  サービスコールの宣言
 */
#ifndef _MACRO_ONLY

/*
 *  タスク管理機能
 */
extern ER	act_tsk(ID tskid) throw();
extern ER	iact_tsk(ID tskid) throw();
extern ER_UINT	can_act(ID tskid) throw();
extern void	ext_tsk(void) throw();
extern ER	ter_tsk(ID tskid) throw();
extern ER	chg_pri(ID tskid, PRI tskpri) throw();
extern ER	get_pri(ID tskid, PRI *p_tskpri) throw();

/*
 *  タスク付属同期機能
 */
extern ER	slp_tsk(void) throw();
extern ER	tslp_tsk(TMO tmout) throw();
extern ER	wup_tsk(ID tskid) throw();
extern ER	iwup_tsk(ID tskid) throw();
extern ER_UINT	can_wup(ID tskid) throw();
extern ER	rel_wai(ID tskid) throw();
extern ER	irel_wai(ID tskid) throw();
extern ER	sus_tsk(ID tskid) throw();
extern ER	rsm_tsk(ID tskid) throw();
extern ER	frsm_tsk(ID tskid) throw();
extern ER	dly_tsk(RELTIM dlytim) throw();

/*
 *  タスク例外処理機能
 */
extern ER	ras_tex(ID tskid, TEXPTN rasptn) throw();
extern ER	iras_tex(ID tskid, TEXPTN rasptn) throw();
extern ER	dis_tex(void) throw();
extern ER	ena_tex(void) throw();
extern BOOL	sns_tex(void) throw();

/*
 *  同期・通信機能
 */
extern ER	sig_sem(ID semid) throw();
extern ER	isig_sem(ID semid) throw();
extern ER	wai_sem(ID semid) throw();
extern ER	pol_sem(ID semid) throw();
extern ER	twai_sem(ID semid, TMO tmout) throw();

extern ER	set_flg(ID flgid, FLGPTN setptn) throw();
extern ER	iset_flg(ID flgid, FLGPTN setptn) throw();
extern ER	clr_flg(ID flgid, FLGPTN clrptn) throw();
extern ER	wai_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn) throw();
extern ER	pol_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn) throw();
extern ER	twai_flg(ID flgid, FLGPTN waiptn,
			MODE wfmode, FLGPTN *p_flgptn, TMO tmout) throw();

extern ER	snd_dtq(ID dtqid, VP_INT data) throw();
extern ER	psnd_dtq(ID dtqid, VP_INT data) throw();
extern ER	ipsnd_dtq(ID dtqid, VP_INT data) throw();
extern ER	tsnd_dtq(ID dtqid, VP_INT data, TMO tmout) throw();
extern ER	fsnd_dtq(ID dtqid, VP_INT data) throw();
extern ER	ifsnd_dtq(ID dtqid, VP_INT data) throw();
extern ER	rcv_dtq(ID dtqid, VP_INT *p_data) throw();
extern ER	prcv_dtq(ID dtqid, VP_INT *p_data) throw();
extern ER	trcv_dtq(ID dtqid, VP_INT *p_data, TMO tmout) throw();

extern ER	snd_mbx(ID mbxid, T_MSG *pk_msg) throw();
extern ER	rcv_mbx(ID mbxid, T_MSG **ppk_msg) throw();
extern ER	prcv_mbx(ID mbxid, T_MSG **ppk_msg) throw();
extern ER	trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout) throw();

/*
 *  メモリプール管理機能
 */
extern ER	get_mpf(ID mpfid, VP *p_blk) throw();
extern ER	pget_mpf(ID mpfid, VP *p_blk) throw();
extern ER	tget_mpf(ID mpfid, VP *p_blk, TMO tmout) throw();
extern ER	rel_mpf(ID mpfid, VP blk) throw();

/*
 *  時間管理機能
 */
extern ER	set_tim(const SYSTIM *p_systim) throw();
extern ER	get_tim(SYSTIM *p_systim) throw();
extern ER	isig_tim(void) throw();

extern ER	sta_cyc(ID cycid) throw();
extern ER	stp_cyc(ID cycid) throw();

/*
 *  システム状態管理機能
 */
extern ER	rot_rdq(PRI tskpri) throw();
extern ER	irot_rdq(PRI tskpri) throw();
extern ER	get_tid(ID *p_tskid) throw();
extern ER	iget_tid(ID *p_tskid) throw();
extern ER	loc_cpu(void) throw();
extern ER	iloc_cpu(void) throw();
extern ER	unl_cpu(void) throw();
extern ER	iunl_cpu(void) throw();
extern ER	dis_dsp(void) throw();
extern ER	ena_dsp(void) throw();
extern BOOL	sns_ctx(void) throw();
extern BOOL	sns_loc(void) throw();
extern BOOL	sns_dsp(void) throw();
extern BOOL	sns_dpn(void) throw();

/*
 *  実装独自サービスコール
 */
extern BOOL	vxsns_ctx(VP p_excinf) throw();
extern BOOL	vxsns_loc(VP p_excinf) throw();
extern BOOL	vxsns_dsp(VP p_excinf) throw();
extern BOOL	vxsns_dpn(VP p_excinf) throw();
extern BOOL	vxsns_tex(VP p_excinf) throw();
extern BOOL	vsns_ini(void) throw();

#endif /* _MACRO_ONLY */

/*
 *  オブジェクト属性の定義
 */
#define TA_HLNG		0x00u		/* 高級言語用インタフェース */
#define TA_ASM		0x01u		/* アセンブリ言語用インタフェース */

#define TA_TFIFO	0x00u		/* タスクの待ち行列をFIFO順に */
#define TA_TPRI		0x01u		/* タスクの待ち行列を優先度順に */

#define TA_MFIFO	0x00u		/* メッセージキューをFIFO順に */
#define TA_MPRI		0x02u		/* メッセージキューを優先度順に */

#define TA_ACT		0x02u		/* タスクを起動された状態で生成 */

#define TA_WSGL		0x00u		/* イベントフラグの待ちタスクを1つに */
#define TA_CLR		0x04u		/* イベントフラグのクリア指定 */

#define	TA_STA		0x02u		/* 周期ハンドラを動作状態で生成 */

/*
 *  サービスコールの動作モードの定義
 */
#define	TWF_ANDW	0x00u		/* イベントフラグのAND待ち */
#define	TWF_ORW		0x01u		/* イベントフラグのOR待ち */

/*
 *  その他の定数の定義
 */
#define	TSK_SELF	0		/* 自タスク指定 */
#define	TSK_NONE	0		/* 該当するタスクがない */
#define	TPRI_SELF	0		/* 自タスクのベース優先度の指定 */
#define	TPRI_INI	0		/* タスクの起動時優先度の指定 */

/*
 *  構成定数とマクロ
 */

/*
 *  優先度の範囲
 */
#define	TMIN_TPRI	1		/* タスク優先度の最小値 */
#define	TMAX_TPRI	16		/* タスク優先度の最大値 */
#define	TMIN_MPRI	1		/* メッセージ優先度の最小値 */
#define	TMAX_MPRI	16		/* メッセージ優先度の最大値 */

/*
 *  バージョン情報
 */
#define	TKERNEL_MAKER	0x0118u		/* カーネルのメーカーコード */
#define	TKERNEL_PRID	0x0001u		/* カーネルの識別番号 */
#define	TKERNEL_SPVER	0x5402u		/* ITRON仕様のバージョン番号 */
#define	TKERNEL_PRVER	0x1043u		/* カーネルのバージョン番号 */

/*
 *  キューイング／ネスト回数の最大値
 */
#define	TMAX_ACTCNT	1		/* 起動要求キューイング数の最大値 */
#define	TMAX_WUPCNT	1		/* 起床要求キューイング数の最大値 */
#define	TMAX_SUSCNT	1		/* 強制待ち要求ネスト数の最大値 */

/*
 *  ビットパターンのビット数
 */
#define	TBIT_TEXPTN	(sizeof(TEXPTN) * CHAR_BIT)
					/* タスク例外要因のビット数 */
#define	TBIT_FLGPTN	(sizeof(FLGPTN) * CHAR_BIT)
					/* イベントフラグのビット数 */

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_H_ */
