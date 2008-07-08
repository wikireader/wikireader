/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: serial.c,v 1.16 2006/02/12 05:26:42 hiro Exp $
 */

/*
 *	シリアルインタフェースドライバ
 */

#include <t_services.h>
#include <serial.h>
#include <hw_serial.h>
#include "kernel_id.h"

/*
 *  バッファサイズとフロー制御に関連する定数
 */

#define	SERIAL_BUFSZ	256		/* ドライバのバッファサイズ */

#define	FC_STOP		'\023'		/* コントロール-S */
#define	FC_START	'\021'		/* コントロール-Q */

#define	BUFCNT_STOP	(SERIAL_BUFSZ - 64)	/* STOPを送る基準文字数 */
#define	BUFCNT_START	(SERIAL_BUFSZ - 128)	/* STARTを送る基準文字数 */

/*
 *  クローズの際に送信を待つ最大時間（msec単位）
 */
#define	MAX_FLUSH_WAIT	1000

/*
 *  シリアルポート初期化ブロック
 */
typedef struct serial_port_initialization_block {
	ID	rcv_semid;	/* 受信バッファ管理用セマフォのID */
	ID	snd_semid;	/* 送信バッファ管理用セマフォのID */
} SPINIB;

static const SPINIB spinib_table[TNUM_PORT] = {
	{ SERIAL_RCV_SEM1, SERIAL_SND_SEM1 }
#if TNUM_PORT >= 2
	,{ SERIAL_RCV_SEM2, SERIAL_SND_SEM2 }
#endif
#if TNUM_PORT >= 3
	,{ SERIAL_RCV_SEM3, SERIAL_SND_SEM3 }
#endif
};

/*
 *  シリアルポート管理ブロック
 */
typedef struct serial_port_control_block {
	const SPINIB *spinib;	/* シリアルポート初期化ブロック */
	SIOPCB	*siopcb;	/* シリアルI/Oポート管理ブロック */
	BOOL	openflag;	/* オープン済みフラグ */
	UINT	ioctl;		/* 動作制御の設定値 */

	UINT	rcv_read_ptr;	/* 受信バッファ読出しポインタ */
	UINT	rcv_write_ptr;	/* 受信バッファ書込みポインタ */
	UINT	rcv_count;	/* 受信バッファ中の文字数 */
	char	rcv_fc_chr;	/* 送るべき START/STOP */
	BOOL	rcv_stopped;	/* STOP を送った状態か？ */

	UINT	snd_read_ptr;	/* 送信バッファ読出しポインタ */
	UINT	snd_write_ptr;	/* 送信バッファ書込みポインタ */
	UINT	snd_count;	/* 送信バッファ中の文字数 */
	BOOL	snd_stopped;	/* STOP を受け取った状態か？ */

	char	rcv_buffer[SERIAL_BUFSZ];	/* 受信バッファ */
	char	snd_buffer[SERIAL_BUFSZ];	/* 送信バッファ */
} SPCB;

static SPCB	spcb_table[TNUM_PORT];

/*
 *  シリアルポートIDからシリアルポート管理ブロックを取り出すためのマクロ
 */
#define INDEX_PORT(portid)	((UINT)((portid) - 1))
#define get_spcb(portid)	(&(spcb_table[INDEX_PORT(portid)]))

/*
 *  ポインタのインクリメント
 */
#define INC_PTR(ptr)		{ if (++ptr == SERIAL_BUFSZ) ptr = 0; }

/*
 *  シリアルインタフェースドライバの初期化ルーチン
 */
void
serial_initialize(VP_INT exinf)
{
	SPCB	*spcb;
	UINT	i;

	sio_initialize();
	for (spcb = spcb_table, i = 0; i < TNUM_PORT; spcb++, i++) {
		spcb->spinib = &(spinib_table[i]);
		spcb->openflag = FALSE;
	}
}

/*
 *  シリアルポートのオープン
 */
ER
serial_opn_por(ID portid)
{
	SPCB	*spcb;
	ER	ercd;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}
	spcb = get_spcb(portid);

	_syscall(loc_cpu());
	if (spcb->openflag) {		/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  変数の初期化
		 */
		spcb->ioctl = (IOCTL_ECHO | IOCTL_CRLF
					| IOCTL_FCSND | IOCTL_FCRCV);


		spcb->rcv_read_ptr = spcb->rcv_write_ptr = 0;
		spcb->rcv_count = 0;
		spcb->rcv_fc_chr = '\0';
		spcb->rcv_stopped = FALSE;

		spcb->snd_read_ptr = spcb->snd_write_ptr = 0;
		spcb->snd_count = 0;
		spcb->snd_stopped = FALSE;

		/*
		 *  ハードウェア依存のオープン処理
		 */
		spcb->siopcb = sio_opn_por(portid, (VP_INT) spcb);

		/*
		 *  受信通知コールバックを許可する．
		 */
		sio_ena_cbr(spcb->siopcb, SIO_ERDY_RCV);
		spcb->openflag = TRUE;
		ercd = E_OK;
	}
	_syscall(unl_cpu());
	return(ercd);
}

/*
 *  シリアルポートのクローズ
 */
ER
serial_cls_por(ID portid)
{
	SPCB	*spcb;
	ER	ercd;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}
	spcb = get_spcb(portid);

	_syscall(loc_cpu());
	if (!(spcb->openflag)) {	/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  ハードウェア依存のクローズ処理
		 */
		sio_cls_por(spcb->siopcb);
		spcb->openflag = FALSE;
		ercd = E_OK;
	}
	_syscall(unl_cpu());
	return(ercd);
}

/*
 *  シリアルポートへの文字送信
 */
Inline BOOL
serial_snd_chr(SPCB *spcb, char c)
{
	if (sio_snd_chr(spcb->siopcb, c)) {
		return(TRUE);
	}
	else {
		sio_ena_cbr(spcb->siopcb, SIO_ERDY_SND);
		return(FALSE);
	}
}

/*
 *  シリアルポートへの送信
 */
static BOOL
serial_wri_chr(SPCB *spcb, char c)
{
	BOOL	buffer_full;

	/*
	 *  LF の前に CR を送信する．
	 */
	if (c == '\n' && (spcb->ioctl & IOCTL_CRLF) != 0) {
		if (serial_wri_chr(spcb, '\r')) {
			_syscall(wai_sem(spcb->spinib->snd_semid));
		}
	}

	_syscall(loc_cpu());
	if (spcb->snd_count == 0 && !(spcb->snd_stopped)
				&& serial_snd_chr(spcb, c)) {
		/*
		 *  シリアルI/Oデバイスの送信レジスタに文字を入れるこ
		 *  とに成功した場合．
		 */
		buffer_full = FALSE;
	}
	else {
		/*
		 *  送信バッファに文字を入れる．
		 */
		spcb->snd_buffer[spcb->snd_write_ptr] = c;
		INC_PTR(spcb->snd_write_ptr);
		spcb->snd_count++;
		buffer_full = (spcb->snd_count == SERIAL_BUFSZ);
	}
	_syscall(unl_cpu());
	return(buffer_full);
}

ER_UINT
serial_wri_dat(ID portid, char *buf, UINT len)
{
	SPCB	*spcb;
	BOOL	buffer_full;
	UINT	i;

	if (sns_dpn()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	spcb = get_spcb(portid);
	if (!(spcb->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	buffer_full = TRUE;		/* ループの1回めは wai_sem する */
	for (i = 0; i < len; i++) {
		if (buffer_full) {
			_syscall(wai_sem(spcb->spinib->snd_semid));
		}
		buffer_full = serial_wri_chr(spcb, *buf++);
	}
	if (!buffer_full) {
		_syscall(sig_sem(spcb->spinib->snd_semid));
	}
	return((ER_UINT) len);
}

/*
 *  シリアルポートからの受信
 */
static BOOL
serial_rea_chr(SPCB *spcb, char *c)
{
	BOOL	buffer_empty;

	_syscall(loc_cpu());

	/*
	 *  受信バッファから文字を取り出す．
	 */
	*c = spcb->rcv_buffer[spcb->rcv_read_ptr];
	INC_PTR(spcb->rcv_read_ptr);
	spcb->rcv_count--;
	buffer_empty = (spcb->rcv_count == 0);

	/*
	 *  START を送信する．
	 */
	if (spcb->rcv_stopped && spcb->rcv_count <= BUFCNT_START) {
		if (!serial_snd_chr(spcb, FC_START)) {
			spcb->rcv_fc_chr = FC_START;
		}
		spcb->rcv_stopped = FALSE;
	}
	_syscall(unl_cpu());

	/*
	 *  エコーバック処理．
	 */
	if ((spcb->ioctl & IOCTL_ECHO) != 0) {
		_syscall(wai_sem(spcb->spinib->snd_semid));
		if (!serial_wri_chr(spcb, *c)) {
			_syscall(sig_sem(spcb->spinib->snd_semid));
		}
	}
	return(buffer_empty);
}

ER_UINT
serial_rea_dat(ID portid, char *buf, UINT len)
{
	SPCB	*spcb;
	BOOL	buffer_empty;
	UINT	i;

	if (sns_dpn()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	spcb = get_spcb(portid);
	if (!(spcb->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	buffer_empty = TRUE;		/* ループの1回めは wai_sem する */
	for (i = 0; i < len; i++) {
		if (buffer_empty) {
			_syscall(wai_sem(spcb->spinib->rcv_semid));
		}
		buffer_empty = serial_rea_chr(spcb, buf++);
	}
	if (!buffer_empty) {
		_syscall(sig_sem(spcb->spinib->rcv_semid));
	}
	return((ER_UINT) len);
}

/*
 *  シリアルポートの制御
 */
ER
serial_ctl_por(ID portid, UINT ioctl)
{
	SPCB	*spcb;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	spcb = get_spcb(portid);
	if (!(spcb->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	spcb->ioctl = ioctl;
	return(E_OK);
}

/*
 *  シリアルポート状態の参照
 */
ER
serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor)
{
	SPCB	*spcb;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	spcb = get_spcb(portid);
	if (!(spcb->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	pk_rpor->reacnt = spcb->rcv_count;
	pk_rpor->wricnt = spcb->snd_count;
	return(E_OK);
}

/*
 *  シリアルポートからの送信可能コールバック
 */
void
sio_ierdy_snd(VP_INT exinf)
{
	SPCB	*spcb;

	spcb = (SPCB *) exinf;
	if (spcb->rcv_fc_chr != '\0') {
		/*
		 *  START/STOP を送信する．
		 */
		(void) sio_snd_chr(spcb->siopcb, spcb->rcv_fc_chr);
		spcb->rcv_fc_chr = '\0';
	}
	else if (!(spcb->snd_stopped) && spcb->snd_count > 0) {
		/*
		 *  送信バッファ中から文字を取り出して送信する．
		 */
		(void) sio_snd_chr(spcb->siopcb,
				spcb->snd_buffer[spcb->snd_read_ptr]);
		INC_PTR(spcb->snd_read_ptr);
		if (spcb->snd_count == SERIAL_BUFSZ) {
			_syscall(isig_sem(spcb->spinib->snd_semid));
		}
		spcb->snd_count--;
	}
	else {
		/*
		 *  送信すべき文字がない場合は，送信可能コールバックを
		 *  禁止する．
		 */
		sio_dis_cbr(spcb->siopcb, SIO_ERDY_SND);
	}
}

/*
 *  シリアルポートからの受信通知コールバック
 */
void
sio_ierdy_rcv(VP_INT exinf)
{
	SPCB	*spcb;
	char	c;

	spcb = (SPCB *) exinf;
	c = (char) sio_rcv_chr(spcb->siopcb);
	if ((spcb->ioctl & IOCTL_FCSND) != 0 && c == FC_STOP) {
		/*
		 *  送信を一時停止する．送信中の文字はそのまま送信する．
		 */
		spcb->snd_stopped = TRUE;
	}
	else if (spcb->snd_stopped && (c == FC_START
				|| (spcb->ioctl & IOCTL_FCANY) != 0)) {
		/*
		 *  送信を再開する．
		 */
		spcb->snd_stopped = FALSE;
		if (spcb->snd_count > 0) {
			c = spcb->snd_buffer[spcb->snd_read_ptr];
			if (serial_snd_chr(spcb, c)) {
				INC_PTR(spcb->snd_read_ptr);
				if (spcb->snd_count == SERIAL_BUFSZ) {
					_syscall(isig_sem(spcb->spinib
								->snd_semid));
				}
				spcb->snd_count--;
			}
		}
	}
	else if ((spcb->ioctl & IOCTL_FCSND) != 0 && c == FC_START) {
		/*
		 *  送信に対してフロー制御している場合，START は捨てる．
		 */
	}
	else if (spcb->rcv_count == SERIAL_BUFSZ) {
		/*
		 *  バッファフルの場合，受信した文字を捨てる．
		 */
	}
	else {
		/*
		 *  受信した文字を受信バッファに入れる．
		 */
		spcb->rcv_buffer[spcb->rcv_write_ptr] = c;
		INC_PTR(spcb->rcv_write_ptr);
		if (spcb->rcv_count == 0) {
			_syscall(isig_sem(spcb->spinib->rcv_semid));
		}
		spcb->rcv_count++;

		/*
		 *  STOP を送信する．
		 */
		if ((spcb->ioctl & IOCTL_FCRCV) != 0 && !(spcb->rcv_stopped)
					&& (spcb->rcv_count >= BUFCNT_STOP)) {
			if (!serial_snd_chr(spcb, FC_STOP)) {
				spcb->rcv_fc_chr = FC_STOP;
			}
			spcb->rcv_stopped = TRUE;
		}
	}
}
