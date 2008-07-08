/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: serial_mini.c,v 1.4 2007/01/05 02:33:59 honda Exp $
 */

/*
 *	シリアルインタフェースドライバ
 */

#include <t_services.h>
#include <serial.h>
#include <hw_serial.h>
#include "kernel_id.h"

/*
 *  シリアルポート管理ブロックの定義
 */

#define	SERIAL_BUFSZ	9	/* シリアルインタフェース用バッファのサイズ */

typedef struct serial_queue_block {
	BOOL openflag;				/* オープン済みフラグ */
	UH   base;					/* hardware port base address */
	ID   rsemid;				/* receive semaphore id */
	ID   ssemid;				/* send semaphore id */
	char head;					/* queue head */
	char tail;					/* queue tail */
	char size;					/* バッファサイズ */
	char buffer[SERIAL_BUFSZ];	/* バッファエリア */
} SQUE;

static SQUE	in_queue[TNUM_SIOP] ={
	FALSE, TADR_SFR_UART0_BASE, SERIAL_RCV_SEM1, SERIAL_SND_SEM1, 0, 0, SERIAL_BUFSZ, "",
	FALSE, TADR_SFR_UART1_BASE, SERIAL_RCV_SEM2, SERIAL_SND_SEM2, 0, 0, SERIAL_BUFSZ, ""
};

/*
 *  シリアルポート管理ブロックの定義と初期化
 */

#define get_sque(portid)	(&(in_queue[portid-1]))

/*
 *  SFR UARTの初期化
 */
Inline void
SFR_uart_initialize(ID portid)
{
	SQUE  *q;
	VB *ip = (VB*)(TADR_SFR_INT_BASE+TADR_SFR_S0RIC_OFFSET);

	q = get_sque(portid);
	  			/* 送受信モードレジスタの初期化 */
	sil_wrb_mem((VP)(q->base+TADR_SFR_UMR_OFFSET), MR_DEF);
				/* 送信制御レジスタの初期化 */
	sil_wrb_mem((VP)(q->base+TADR_SFR_UC0_OFFSET), C0_DEF);
				/* 転送速度レジスタの初期化 */
	sil_wrb_mem((VP)(q->base+TADR_SFR_UBRG_OFFSET), BRG1_DEF);
				/* 割込みレベルの設定 */
	set_ic_ilvl((VP)(ip + ((INT)portid-1)*2), RB_LEVEL);
	sil_wrb_mem((VP)(q->base+TADR_SFR_UC1_OFFSET), C1R_DEF);
	sil_reb_mem((VP)(q->base+TADR_SFR_URB_OFFSET));		/* ダミーデータ受信 */
	sil_reb_mem((VP)(q->base+TADR_SFR_URB_OFFSET));		/* ダミーデータ受信 */
	q->openflag = TRUE;
}

/*
 *  SQUEの受信バッファからサイズを取り出す
 */
Inline int
queue_size(SQUE *q)
{
	int size = q->head - q->tail;
	if(size < 0)
		size += q->size;
	return size;
}

/*
 *  シリアルインタフェースドライバの起動
 */
void
serial_initialize(VP_INT exinf)
{
	SFR_uart_initialize(LOGTASK_PORTID);
}

/*
 *  シリアルポートのオープン
 */
ER
serial_opn_por(ID portid)
{
	SQUE  *q;
	VB    *ip=(VB*)(TADR_SFR_INT_BASE+TADR_SFR_S0RIC_OFFSET);
	ER	ercd;

	if (!(1 <= portid && portid <= TNUM_SIOP)) {
		return(E_ID);		/* ポート番号のチェック */
	}
	q = get_sque(portid);

	_syscall(loc_cpu());
	if (q->openflag) {		/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		SFR_uart_initialize(portid);
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
	SQUE  *q;
	VB    *ip=(VB*)(TADR_SFR_INT_BASE+TADR_SFR_S0RIC_OFFSET);
	ER	ercd;

	if (!(1 <= portid && portid <= TNUM_SIOP)) {
		return(E_ID);		/* ポート番号のチェック */
	}
	q = get_sque(portid);

	_syscall(loc_cpu());
	if (!(q->openflag)) {	/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		set_ic_ilvl((VP)(ip + ((INT)portid-1)*2), 0);
		sil_wrb_mem((VP)(q->base+TADR_SFR_UC1_OFFSET), C1S_DEF);
		q->openflag = FALSE;
		ercd = E_OK;
	}
	_syscall(unl_cpu());
	return(ercd);
}

/*
 *  シリアルポートへの送信
 */

ER_UINT
serial_wri_dat(ID portid, char *buf, UINT len)
{
	SQUE	*q;
	unsigned int	i;
	unsigned char   c;

	if (!(1 <= portid && portid <= TNUM_SIOP)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	q = get_sque(portid);
	if (!(q->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	wai_sem(q->ssemid);
	for (i = 0; i < len; i++) {
		while((sil_reb_mem((VP)(q->base+TADR_SFR_UC1_OFFSET)) & 0x2) == 0);
		sil_wrb_mem((VP)(q->base+TADR_SFR_UTB_OFFSET), *buf++);
	}
	sig_sem(q->ssemid);
	return(len);
}

/*
 *  シリアルポートからの受信
 */

ER_UINT
serial_rea_dat(ID portid, char *buf, UINT len)
{
	SQUE	*q;
	UINT	i;

	if (!(1 <= portid && portid <= TNUM_SIOP)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	q = get_sque(portid);
	if (!(q->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	for (i = 0; i < len; i++) {
		if (queue_size(q) == 0)
			wai_sem(q->rsemid);
		else
			pol_sem(q->rsemid);
		_syscall(loc_cpu());
		*buf++ = q->buffer[q->tail++];
		if (q->tail >= q->size) {
			q->tail = 0;
		}
		_syscall(unl_cpu());
	}
	return(len);
}

/*
 *  シリアルポートの制御
 */
ER
serial_ctl_por(ID portid, UINT ioctl)
{
	return(E_OK);
}

/*
 *  シリアルポート状態の参照
 */
ER
serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor)
{
	SQUE	*q;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_SIOP)) {
		return(E_ID);		/* ポート番号のチェック */
	}

	q = get_sque(portid);
	if (!(q->openflag)) {	/* オープン済みかのチェック */
		return(E_OBJ);
	}

	pk_rpor->reacnt = queue_size(q);
	pk_rpor->wricnt = 0;
	return(E_OK);
}

/*
 *  シリアルポート割込みサービスルーチン
 */

void
serial_handler_in(ID portid)
{
	SQUE	*q;
	int		rdata;				/* 受信バッファ */

	q = get_sque(portid);
	if (queue_size(q) < q->size) {
		rdata = sil_reb_mem((VP)(q->base+TADR_SFR_URB_OFFSET));	/* データ受信 */
		q->buffer[q->head++] = rdata;
		if (q->head >= q->size)
			q->head = 0;
		isig_sem(q->rsemid);
	}
}

/*
 *  割込みハンドラ
 */

void
serial_in_handler1(void)
{
	serial_handler_in(1);
}

void
serial_in_handler2(void)
{
	serial_handler_in(2);
}

