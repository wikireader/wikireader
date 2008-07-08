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
 *  @(#) $Id: upd72001.c,v 1.4 2003/12/13 06:21:49 hiro Exp $
 */

/*
 *	μPD72001用 簡易SIOドライバ
 */

#include <s_services.h>
#include <upd72001.h>

/*
 *  デバイスレジスタのアクセス間隔時間（nsec単位）
 *
 *  200という値にあまり根拠はない．
 */
#define	UPD72001_DELAY	200

/*
 *  μPD72001のレジスタの番号
 */
#define	UPD72001_CR0	0x00u		/* コントロールレジスタ */
#define	UPD72001_CR1	0x01u
#define	UPD72001_CR2	0x02u
#define	UPD72001_CR3	0x03u
#define	UPD72001_CR4	0x04u
#define	UPD72001_CR5	0x05u
#define	UPD72001_CR10	0x0au
#define	UPD72001_CR12	0x0cu
#define	UPD72001_CR14	0x0eu
#define	UPD72001_CR15	0x0fu

#define	UPD72001_SR0	0x00u		/* ステータスレジスタ */

/*
 *  コントロールレジスタの設定値
 */
#define CR_RESET	0x18u		/* ポートリセットコマンド */

#define CR0_EOI		0x38u		/* EOI（End of Interrupt）*/

#define CR1_DOWN	0x00u		/* 全割込みを禁止 */
#define CR1_RECV	0x10u		/* 受信割込み許可ビット */
#define CR1_SEND	0x02u		/* 送信割込み許可ビット */

#define CR3_DEF		0xc1u		/* データ 8bit，受信イネーブル */
#define CR4_DEF		0x44u		/* ストップビット 1bit，パリティなし */
#define CR5_DEF		0xeau		/* データ 8bit，送信イネーブル */

#define CR10_DEF	0x00u		/* NRZ */
#define CR14_DEF	0x07u		/* ボーレートジェネレータイネーブル */
#define CR15_DEF	0x56u		/* ボーレートジェネレータ使用 */

#define SR0_RECV	0x01u		/* 受信通知ビット */
#define SR0_SEND	0x04u		/* 送信可能ビット */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	VP	data;		/* データレジスタの番地 */
	VP	ctrl;		/* コントロールレジスタの番地 */

	UB	cr3_def;	/* CR3の設定値（受信ビット数）*/
	UB	cr4_def;	/* CR4の設定値（ストップビット，パリティ）*/
	UB	cr5_def;	/* CR5の設定値（送信ビット数）*/
	UB	brg1_def;	/* ボーレート上位の設定値 */
	UB	brg2_def;	/* ボーレート下位の設定値 */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB	*siopinib;	/* シリアルI/Oポート初期化ブロック */
	VP_INT		exinf;		/* 拡張情報 */
	BOOL		openflag;	/* オープン済みフラグ */
	UB		cr1;		/* CR1の設定値（割込み許可）*/
	BOOL		getready;	/* 文字を受信した状態 */
	BOOL		putready;	/* 文字を送信できる状態 */
};

/*
 *  シリアルI/Oポート初期化ブロック
 *
 *  ID = 1 をポートB，ID = 2 をポートAに対応させている．
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ (VP) TADR_UPD72001_DATAB, (VP) TADR_UPD72001_CTRLB,
		CR3_DEF, CR4_DEF, CR5_DEF, BRG1_DEF, BRG2_DEF },
#if TNUM_SIOP >= 2
	{ (VP) TADR_UPD72001_DATAA, (VP) TADR_UPD72001_CTRLA,
		CR3_DEF, CR4_DEF, CR5_DEF, BRG1_DEF, BRG2_DEF },
#endif /* TNUM_SIOP >= 2 */
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  デバイスレジスタへのアクセス関数
 */
Inline UB
upd72001_read_reg(VP addr)
{
	UB	val;

	val = (UB) upd72001_reb_reg(addr);
	sil_dly_nse(UPD72001_DELAY);
	return(val);
}

Inline void
upd72001_write_reg(VP addr, UB val)
{
	upd72001_wrb_reg(addr, (VB) val);
	sil_dly_nse(UPD72001_DELAY);
}

Inline UB
upd72001_read_ctrl(VP addr, UB reg)
{
	upd72001_write_reg(addr, reg);
	return(upd72001_read_reg(addr));
}

Inline void
upd72001_write_ctrl(VP addr, UB reg, UB val)
{
	upd72001_write_reg(addr, reg);
	upd72001_write_reg(addr, val);
}

Inline void
upd72001_write_brg(VP addr, UB reg, UB val, UB brg2, UB brg1)
{
	upd72001_write_reg(addr, reg);
	upd72001_write_reg(addr, val);
	upd72001_write_reg(addr, brg2);
	upd72001_write_reg(addr, brg1);
	(void) upd72001_read_reg(addr);		/* ダミーリード */
}

/*
 *  状態の読出し（SR0の読出し）
 *
 *  μPD72001は，状態（SR0）を一度読むと受信通知ビットが落ちてしまうた
 *  め，状態を読み出す関数を設け，シリアルI/Oポート管理ブロック中の
 *  getready に受信通知状態，putready に送信可能状態を保存している（送
 *  信可能状態の保存は不要かもしれない）．
 *  状態レジスタを読んでも受信通知ビットが落ちないデバイス（こちらが普
 *  通と思われる）では，この関数は必要ない．
 */
static void
upd72001_get_stat(SIOPCB *siopcb)
{
	UB	sr0;

	sr0 = upd72001_read_ctrl(siopcb->siopinib->ctrl, UPD72001_SR0);
	if ((sr0 & SR0_RECV) != 0) {
		siopcb->getready = TRUE;
	}
	if ((sr0 & SR0_SEND) != 0) {
		siopcb->putready = TRUE;
	}
}

/*
 *  文字を受信できるか？
 */
Inline BOOL
upd72001_getready(SIOPCB *siopcb)
{
	upd72001_get_stat(siopcb);
	return(siopcb->getready);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
upd72001_putready(SIOPCB *siopcb)
{
	upd72001_get_stat(siopcb);
	return(siopcb->putready);
}

/*
 *  受信した文字の取出し
 */
Inline char
upd72001_getchar(SIOPCB *siopcb)
{
	siopcb->getready = FALSE;
	return((char) upd72001_read_reg(siopcb->siopinib->data));
}

/*
 *  送信する文字の書込み
 */
Inline void
upd72001_putchar(SIOPCB *siopcb, char c)
{
	siopcb->putready = FALSE;
	upd72001_write_reg(siopcb->siopinib->data, (UB) c);
}

/*
 *  EOI（End Of Interrupt）発行
 */
Inline void
upd72001_eoi()
{
	upd72001_write_ctrl((VP) TADR_UPD72001_CTRLA, UPD72001_CR0, CR0_EOI);
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
upd72001_initialize()
{
	SIOPCB	*siopcb;
	UINT	i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
		siopcb->siopinib = &(siopinib_table[i]);
		siopcb->openflag = FALSE;
	}
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
upd72001_openflag(void)
{
#if TNUM_SIOP < 2
	return(siopcb_table[0].openflag);
#else /* TNUM_SIOP < 2 */
	return(siopcb_table[0].openflag || siopcb_table[1].openflag);
#endif /* TNUM_SIOP < 2 */
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
upd72001_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB		*siopcb;
	const SIOPINIB	*siopinib;

	siopcb = get_siopcb(siopid);
	siopinib = siopcb->siopinib;

	upd72001_write_reg(siopinib->ctrl, CR_RESET);
	if (!upd72001_openflag()) {
		upd72001_write_ctrl((VP) TADR_UPD72001_CTRLA,
						UPD72001_CR2, 0x18);
		upd72001_write_ctrl((VP) TADR_UPD72001_CTRLB,
						UPD72001_CR2, 0x00);
	}
	siopcb->cr1 = CR1_DOWN;
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR1, siopcb->cr1);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR4, siopinib->cr4_def);
	upd72001_write_brg(siopinib->ctrl, UPD72001_CR12, 
				0x01, siopinib->brg2_def, siopinib->brg1_def);
	upd72001_write_brg(siopinib->ctrl, UPD72001_CR12,
				0x02, siopinib->brg2_def, siopinib->brg1_def);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR15, CR15_DEF);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR14, CR14_DEF);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR10, CR10_DEF);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR3, siopinib->cr3_def);
	upd72001_write_ctrl(siopinib->ctrl, UPD72001_CR5, siopinib->cr5_def);
	siopcb->exinf = exinf;
	siopcb->getready = siopcb->putready = FALSE;
	siopcb->openflag = TRUE;
	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
upd72001_cls_por(SIOPCB *siopcb)
{
	upd72001_write_ctrl(siopcb->siopinib->ctrl, UPD72001_CR1, CR1_DOWN);
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
upd72001_snd_chr(SIOPCB *siopcb, char c)
{
	if (upd72001_putready(siopcb)) {
		upd72001_putchar(siopcb, c);
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
upd72001_rcv_chr(SIOPCB *siopcb)
{
	if (upd72001_getready(siopcb)) {
		return((INT)(UB) upd72001_getchar(siopcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
upd72001_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	cr1_bit = 0;

	switch (cbrtn) {
	case SIO_ERDY_SND:
		cr1_bit = CR1_SEND;
		break;
	case SIO_ERDY_RCV:
		cr1_bit = CR1_RECV;
		break;
	}
	siopcb->cr1 |= cr1_bit;
	upd72001_write_ctrl(siopcb->siopinib->ctrl, UPD72001_CR1, siopcb->cr1);
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
upd72001_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	cr1_bit = 0;

	switch (cbrtn) {
	case SIO_ERDY_SND:
		cr1_bit = CR1_SEND;
		break;
	case SIO_ERDY_RCV:
		cr1_bit = CR1_RECV;
		break;
	}
	siopcb->cr1 &= ~cr1_bit;
	upd72001_write_ctrl(siopcb->siopinib->ctrl, UPD72001_CR1, siopcb->cr1);
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
static void
upd72001_isr_siop(SIOPCB *siopcb)
{
	if ((siopcb->cr1 & CR1_RECV) != 0 && upd72001_getready(siopcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		upd72001_ierdy_rcv(siopcb->exinf);
	}
	if ((siopcb->cr1 & CR1_SEND) != 0 && upd72001_putready(siopcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		upd72001_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
upd72001_isr()
{
	if (siopcb_table[0].openflag) {
		upd72001_isr_siop(&(siopcb_table[0]));
	}
#if TNUM_SIOP >= 2
	if (siopcb_table[1].openflag) {
		upd72001_isr_siop(&(siopcb_table[1]));
	}
#endif /* TNUM_SIOP >= 2 */
	upd72001_eoi();
}
