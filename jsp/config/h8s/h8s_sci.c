/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#include <h8s_sci.h>

/*
 *  H8S内蔵SCI用 簡易SIOドライバ
 *  ・下記は、２チャンネル用に記述している。H8Sシリーズの中には、２チャンネル
 *    以上サポートしているチップもあるが、そのときの対応は容易である。
 */

/* TNUM_SIOP : サポートするシリアルポートの数 (デフォルト値 2)
   TNUM_PORT : サポートするシリアルI/Oポートの数 (デフォルト値 2)
   (sys_config.h に記載) */

/* 以下で、レジスタのアドレスを、UW 型へキャストをしている。これは、ベース
   アドレスにオフセット値を加えて、最終的なレジスタのアドレスを求めている
   ためである。取得後に、最終的に VP 型にキャストされている。(h8s_sci.h) */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	UW	reg_base;	/* レジスタのベースアドレス */
	UB	smr_def;	/* SMR 設定値 b6-b3;
				   (送受信ビット数、ストップビット，パリティ) */
	UB	boud_brr_def;	/* BRR 設定値（ボーレートの設定値） */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB	*siopinib;	/* シリアルI/Oポート初期化ブロック */
	VP_INT		exinf;		/* 拡張情報 */
	BOOL		openflag;	/* オープン済みフラグ */
	BOOL		getready;	/* 文字を受信した状態 */
	BOOL		putready;	/* 文字を送信できる状態 */
};

/*
 *  シリアルI/Oポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ (UW)	SCI0_BASE_ADDR,
	  (UB)	0,
	  (UB)	BRR_RATE		/* N 値 */
	},
#if TNUM_SIOP >= 2
	{ (UW)	SCI1_BASE_ADDR,
	  (UB)	0,
	  (UB)	BRR_RATE		/* N 値 */
	}
#endif /* TNUM_SIOP >= 2 */
};

/*
 *  シリアルI/Oポート初期化ブロックの取出し
 */
#define INDEX_SIOPINIB(siopid)	((UINT)((siopid) - 1))
#define get_siopinib(siopid)	(&(siopinib_table[INDEX_SIOPINIB(siopid)]))

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
 *  SCI用レジスタ操作関数
 */
Inline void
h8s_sci_or( const SIOPINIB *siopinib, UW reg, UB val )
{
	h8s_sci_wrb( siopinib->reg_base, reg, h8s_sci_reb( siopinib->reg_base, reg ) | val );
}

Inline void
h8s_sci_and( const SIOPINIB *siopinib, UW reg, UB val )
{
	h8s_sci_wrb( siopinib->reg_base, reg, h8s_sci_reb( siopinib->reg_base, reg ) & val );
}

/*
 *  文字を受信できるか？
 */
Inline BOOL
h8s_sci_getready(SIOPCB *siopcb)
{
	siopcb->getready = FALSE;
	if( h8s_sci_reb( siopcb->siopinib->reg_base, (UW) SSR ) & RDRF )
		siopcb->getready = TRUE;

	return( siopcb->getready );
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
h8s_sci_putready(SIOPCB *siopcb)
{
	siopcb->putready = FALSE;
	if( h8s_sci_reb( siopcb->siopinib->reg_base, (UW) SSR ) & TDRE )
		siopcb->putready = TRUE;

	return( siopcb->putready );
}

/*
 *  受信した文字の取出し
 */
Inline char
h8s_sci_getchar(SIOPCB *siopcb)
{
	/* RDRFクリア */
	h8s_sci_and( siopcb->siopinib, (UW) SSR, (UB) ~RDRF );

	return( (char) h8s_sci_reb( siopcb->siopinib->reg_base, (UW) RDR ) );
}

/*
 *  送信する文字の書込み
 */
Inline void
h8s_sci_putchar(SIOPCB *siopcb, char c)
{
#ifndef GDB_STUB
	h8s_sci_wrb( siopcb->siopinib->reg_base, (UW) TDR, c );

	/* TDREクリア */
	h8s_sci_and( siopcb->siopinib, (UW) SSR, (UB) ~TDRE );
#else
	stub_putc( c );
#endif
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
h8s_sci_initialize()
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
 *  SIOレジスタ初期化ルーチン
 */
void
h8s_sci_init_siopinib( const SIOPINIB  *siopinib )
{
	/*
	 * SCIレジスタの初期化
	 */

	/* 送受信停止 */
	h8s_sci_and( siopinib, (UW) SCR, (UB) ~( TE | RE ) );

	/* ビット長など設定 */
	h8s_sci_wrb( siopinib->reg_base, (UW) SMR, siopinib->smr_def );

	/* ボーレート設定 */
	h8s_sci_wrb( siopinib->reg_base, (UW) BRR, siopinib->boud_brr_def );

	/* 割込み禁止とクロックソース選択 */
	h8s_sci_and( siopinib, (UW) SCR, (UB) ~( TIE | RIE | MPIE | TEIE | CKE1 | CKE0 ) );

	/* ボーレートの安定化(1bit分の待ち) */
	sil_dly_nse( 1/BAUD_RATE );

	/* エラーフラグをクリア */
	h8s_sci_and( siopinib, (UW) SSR, ~( ORER | FER | PER ) );

	/* 送受信許可、受信割込み許可 */
	h8s_sci_or( siopinib, (UW) SCR, ( RIE | TE | RE ) );
}

/*
 *  カーネル起動時のバーナー出力用の初期化
 */
void
h8s_sci_init(void)
{
	h8s_sci_init_siopinib( get_siopinib(1) );
#if TNUM_SIOP >= 2
	h8s_sci_init_siopinib( get_siopinib(2) );
#endif /* TNUM_SIOP >= 2 */
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
h8s_sci_openflag(void)
{
#if TNUM_SIOP < 2
	return( siopcb_table[0].openflag );
#else /* TNUM_SIOP < 2 */
	return( siopcb_table[0].openflag || siopcb_table[1].openflag );
#endif /* TNUM_SIOP < 2 */
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
h8s_sci_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB 		*siopcb = get_siopcb(siopid);
	const SIOPINIB  *siopinib = siopcb->siopinib;

#ifndef GDB_STUB
	/* SCIレジスタの初期化 */
	h8s_sci_init_siopinib( siopinib );
#endif	/* GDB_STUB */

	/* 割込みレベル設定、割込み要求クリアは、sio_opn_por で行う。 */

	siopcb->exinf = exinf;
	siopcb->getready = siopcb->putready = FALSE;
	siopcb->openflag = TRUE;

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
h8s_sci_cls_por(SIOPCB *siopcb)
{
	/* TEND が 1 になるまで待つ */
	while ( !(h8s_sci_reb( siopcb->siopinib->reg_base, (UW) SSR ) & TEND ) );

	h8s_sci_and( siopcb->siopinib, (UW) SCR, ~( TE | RE ) );
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
h8s_sci_snd_chr(SIOPCB *siopcb, char c)
{
	if( h8s_sci_putready(siopcb) ) {
		h8s_sci_putchar(siopcb, c);
		siopcb->putready = FALSE;
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
h8s_sci_rcv_chr(SIOPCB *siopcb)
{
	if( h8s_sci_getready(siopcb) ) {
		siopcb->getready = FALSE;
		return( (INT)(UB) h8s_sci_getchar(siopcb) );
			/* UB のキャストは、符号拡張防止するため */
	}

	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
h8s_sci_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:
		h8s_sci_or( siopcb->siopinib, (UW) SCR, TIE );
		return;
	case SIO_ERDY_RCV:
		h8s_sci_or( siopcb->siopinib, (UW) SCR, RIE );
		return;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
h8s_sci_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch( cbrtn ) {
	case SIO_ERDY_SND:
		h8s_sci_and( siopcb->siopinib, (UW) SCR, (UB) ~TIE );
		return;
	case SIO_ERDY_RCV:
		h8s_sci_and( siopcb->siopinib, (UW) SCR, (UB) ~RIE );
		return;
	}
}

/*
 *  SIOの割込みサービスルーチン (SCI0専用)
 */
void
h8s_sci0_isr_in()
{
	SIOPCB *siopcb = get_siopcb(1);

	/* 受信通知コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_rcv( siopcb->exinf );
	}
}

void
h8s_sci0_isr_out()
{
	SIOPCB *siopcb = get_siopcb(1);

	/* 送信可能コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_snd( siopcb->exinf );
	}
}

/*
 *  SIOの受信エラー割込みサービスルーチン (SCI0専用)
 *
 *  エラー処理は、下記エラーフラグのクリアのみ。
 *  ・オーバーランエラー、フレーミングエラー、パリティエラー
 */
void
h8s_sci0_isr_error(void)
{
	SIOPCB *siopcb = get_siopcb(1);

	if( siopcb->openflag ) {
		/*  エラーフラグクリア  */
		h8s_sci_and( siopcb->siopinib, (UW) SSR, ~( RDRF | ORER | FER | PER ) );
	}
}

/*
 *  SIOの割込みサービスルーチン (SCI1専用)
 */
void
h8s_sci1_isr_in()
{
	SIOPCB *siopcb = get_siopcb(2);

	/* 受信通知コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_rcv( siopcb->exinf );
	}
}

void
h8s_sci1_isr_out()
{
	SIOPCB *siopcb = get_siopcb(2);

	/* 送信可能コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_snd( siopcb->exinf );
	}
}

/*
 *  SIOの受信エラー割込みサービスルーチン (SCI1専用)
 *
 *  エラー処理は、下記エラーフラグのクリアのみ。
 *  ・オーバーランエラー、フレーミングエラー、パリティエラー
 */
void
h8s_sci1_isr_error(void)
{
	SIOPCB *siopcb = get_siopcb(2);

	if( siopcb->openflag ) {
		/*  エラーフラグクリア  */
		h8s_sci_and( siopcb->siopinib, (UW) SSR, ~( RDRF | ORER | FER | PER ) );
	}
}

/*
 *  H8S 内蔵 SCI 用ポーリング出力 (LOGTASK_PORTID専用、sys_putcで利用)
 */
void
h8s_sci0_putchar_pol( char c )
{

	const SIOPINIB  *siopinib = get_siopinib( LOGTASK_PORTID );

	/* TDRE が 1 になるまで待つ */
	while ( !(h8s_sci_reb( siopinib->reg_base, (UW) SSR ) & TDRE ) );

	h8s_sci_wrb( siopinib->reg_base, (UW) TDR, (UB) c );

	/* TDREクリア */
	h8s_sci_and( siopinib, (UW) SSR, (UB) ~TDRE );

	/* TEND が 1 になるまで待つ */
	while ( !(h8s_sci_reb( siopinib->reg_base, (UW) SSR ) & TEND ) );
}
