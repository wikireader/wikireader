/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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

/*
 *  H8S内蔵SCI用 簡易SIOドライバ
 *  ・下記は、２チャンネル用に記述している。H8Sシリーズの中には、３チャンネル
 *    以上サポートしているチップもあるが、そのときの対応は容易である。
 */

#include <h8s_sci.h>

/* SCIレジスタのアドレス */
/* ベースアドレス */
#define SCI0_BASE_ADDR	0xff78	/* チャネル０ */
#define SCI1_BASE_ADDR	0xff80	/* チャネル１ */
#define SCI2_BASE_ADDR	0xff88	/* チャネル２ */

/* レジスタオフセット */
#define SMR		0	/* シリアルモードレジスタ */
#define BRR		1	/* ビットレートレジスタ */
#define SCR		2	/* シリアルコントロールレジスタ */
#define TDR		3	/* トランスミットデータレジスタ */
#define SSR		4	/* シリアルステータスレジスタ */
#define RDR		5	/* レシーブデータレジスタ */
#define SCMR		6	/* スマートカードモードレジスタ */

/* 各レジスタのビットパターン */

/* RSR, RDR, TSR, TDR */
/* 定義すべきものは、特に無い */

/* SMR */
#define CA	BIT7
#define	CHR	BIT6	/* 8bit = 0 / 7bit = 1 */
#define	PE	BIT5	/* Parity OFF = 0 / Parity ON = 1 */
#define	OE	BIT4	/* EVEN Parity = 0 / ODD Parity = 1 */
#define	STOP	BIT3	/* 1 STOP BIT = 0 / 2 STOP BIT = 1 */
#define MP	BIT2
#define	CSK1	BIT1
#define	CSK0	BIT0

/*
 *　SMRの初期値
 *
 *　ビット7=0：調歩同期式モード
 *　ビット6=0：キャラクタレングス=8ビット
 *　ビット5=0：パリティの付加、チェックを禁止
 *　ビット4　：パリティモード（未使用）
 *　ビット3=0：ストップビットレングス＝1ビット
 *　ビット1,0：ボーレートジェネレータへのクロックソース選択
 *　　　　　　　ボード依存部でSCRn_CKSとして定義（nはch番号）
 */
#define SMR_INIT	0

/* SCR */
#define TIE	BIT7
#define	RIE	BIT6
#define	TE	BIT5
#define	RE	BIT4
#define	MPIE	BIT3
#define TEIE	BIT2
#define	CKE1	BIT1
#define	CKE0	BIT0

/* SSR */
#define TDRE	BIT7
#define	RDRF	BIT6
#define	ORER	BIT5
#define	FER	BIT4
#define	PER	BIT3
#define TEND	BIT2
#define	MPB	BIT1
#define	MPBT	BIT0


/* 
 * TNUM_PORT : シリアルドライバ（serial.c）、つまり GDICレベルでサポートする
 * 　　　　　　シリアルポートの数 
 * TNUM_SIOP : PDICレベル（プロセッサ内蔵SIO）でサポートするシリアルI/Oポート
 *　　　　　　 の数（現在の実装では最大3）
 *  
 *　いずれもuser_config.hで定義する。
 */

/* 以下で、レジスタのアドレスを、UW 型へキャストをしている。これは、ベース
   アドレスにオフセット値を加えて、最終的なレジスタのアドレスを求めている
   ためである。取得後に、最終的に VP 型にキャストされている。(h8s_sci.h) */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	UW	reg_base;	/* レジスタのベースアドレス */
	UH	boud_rate;	/* ボーレート[bps]　*/
	UB	boud_brr_def;	/* BRR 設定値（ボーレートの設定値） */
	UB	smr_def;	/* SMR 設定値
				   (送受信ビット数、ストップビット，パリティ) */
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
	{ (UW)SCI0_BASE_ADDR,
	  (UH)BAUD_RATE1,
	  (UB)BRR0_RATE,			/* N 値 */
	  (UB)(SMR_INIT | (SCR0_CKS & (CKE1 | CKE0)))
	},
#if TNUM_SIOP >= 2
	{ (UW)SCI1_BASE_ADDR,
	  (UH)BAUD_RATE1,
	  (UB)BRR1_RATE,			/* N 値 */
	  (UB)(SMR_INIT | (SCR1_CKS & (CKE1 | CKE0)))
	}
#endif /* TNUM_SIOP >= 2 */
#if TNUM_SIOP >= 3
	,{(UW)SCI2_BASE_ADDR,
	  (UH)BAUD_RATE2,
	  (UB)BRR2_RATE,			/* N 値 */
	  (UB)(SMR_INIT | (SCR2_CKS & (CKE1 | CKE0)))
	}
#endif /* TNUM_SIOP >= 3 */
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
 *  SCI用システムインタフェースレイヤ
 *
 *   base : ポートのベースアドレス
 *   offset : レジスタオフセット
 *   val : レジスタ値 
 */
#define h8s_sci_wrb( base, offset, val ) h8s_wrb_reg( (base) + (offset), (val) )
#define h8s_sci_reb( base, offset )	h8s_reb_reg( (base) + (offset) )

/*
 *  SCI用レジスタ操作関数
 */
Inline void
h8s_sci_or( const SIOPINIB *siopinib, INT offset, INT val )
{
	UB reg = h8s_sci_reb( siopinib->reg_base, offset );
	h8s_sci_wrb( siopinib->reg_base, offset, reg | (UB)val );
}

Inline void
h8s_sci_and( const SIOPINIB *siopinib, INT offset, INT val )
{
	UB reg = h8s_sci_reb( siopinib->reg_base, offset );
	h8s_sci_wrb( siopinib->reg_base, offset, reg & (UB)val );
}

/*
 *  文字を受信できるか？
 */
Inline BOOL
h8s_sci_getready(SIOPCB *siopcb)
{
	UB ssr;
	
	siopcb->getready = FALSE;
	ssr =  h8s_sci_reb( siopcb->siopinib->reg_base, SSR );
	if( ssr & RDRF ) {
		siopcb->getready = TRUE;
	}
	return( siopcb->getready );
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
h8s_sci_putready(SIOPCB *siopcb)
{
	siopcb->putready = FALSE;
	if( h8s_sci_reb( siopcb->siopinib->reg_base, SSR ) & TDRE ) {
		siopcb->putready = TRUE;
	}
	return( siopcb->putready );
}

/*
 *  受信した文字の取出し
 */
Inline char
h8s_sci_getchar(SIOPCB *siopcb)
{
	/* RDRFクリア */
	h8s_sci_and( siopcb->siopinib, SSR, ~RDRF );

	return( (char) h8s_sci_reb( siopcb->siopinib->reg_base, RDR ) );
}

/*
 *  送信する文字の書込み
 */
Inline void
h8s_sci_putchar(SIOPCB *siopcb, char c)
{
	h8s_sci_wrb( siopcb->siopinib->reg_base, TDR, c );

	/* TDREクリア */
	h8s_sci_and( siopcb->siopinib, SSR, ~TDRE );
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
h8s_sci_initialize(void)
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
	h8s_sci_and( siopinib, SCR, ~( TE | RE ) );

	/* ビット長など設定 */
	h8s_sci_wrb( siopinib->reg_base, SMR, siopinib->smr_def );

	/* ボーレート設定 */
	h8s_sci_wrb( siopinib->reg_base, BRR, siopinib->boud_brr_def );

	/*
	 *  割込み禁止とクロックソース選択
	 *　　クロックソースは内部クロックを選択
	 */
	h8s_sci_and( siopinib, SCR, ~( TIE | RIE | MPIE | TEIE | CKE1 | CKE0 ) );

	/* ボーレートの安定化(1bit分の待ち) */
	sil_dly_nse_long( 1000000000ul / (siopinib->boud_rate) );

	/* エラーフラグをクリア */
	h8s_sci_and( siopinib, SSR, ~( ORER | FER | PER ) );	/* 修正 */

	/* 送受信許可、受信割込み許可 */
	h8s_sci_or( siopinib, SCR, ( RIE | TE | RE ) );
}


/*
 *  SCI0の設定に矛盾がないかチェック
 */
#if defined(OMIT_SCI0) && (POL_PORTID == 1)
#error h8s_sci_putchar_pol serial port ID error.
#endif

/*
 *  カーネル起動時のバーナー出力用の初期化
 */
void
h8s_sci_init(void)
{
	h8s_sci_init_siopinib( get_siopinib(POL_PORTID) );
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
h8s_sci_openflag(void)
{
	BOOL ret;
#ifndef OMIT_SCI0
	ret = siopcb_table[0].openflag;
#else /* OMIT_SCI0 */
	ret = FALSE;
#endif /* OMIT_SCI0 */

#if TNUM_SIOP >= 2
	ret |= siopcb_table[1].openflag;
#endif /* TNUM_SIOP >= 2 */

#if TNUM_SIOP >= 3
	ret |= siopcb_table[2].openflag;
#endif /* TNUM_SIOP >= 3 */

	return(ret);
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
h8s_sci_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB 		*siopcb = get_siopcb(siopid);
	const SIOPINIB  *siopinib = siopcb->siopinib;

	/* SCIレジスタの初期化 */
	h8s_sci_init_siopinib( siopinib );

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
	while ( !(h8s_sci_reb( siopcb->siopinib->reg_base, SSR ) & TEND ) );

	h8s_sci_and( siopcb->siopinib, SCR, ~( TE | RE ) );	/* 修正 */
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
			/* UB のキャストは、符号拡張を防止するため */
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
		h8s_sci_or( siopcb->siopinib, SCR, TIE );
		return;
	case SIO_ERDY_RCV:
		h8s_sci_or( siopcb->siopinib, SCR, RIE );
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
		h8s_sci_and( siopcb->siopinib, SCR, ~TIE );
		return;
	case SIO_ERDY_RCV:
		h8s_sci_and( siopcb->siopinib, SCR, ~RIE );
		return;
	}
}

/*
 *  SIOの割込みサービスルーチン
 *　　全チャネル共通部分をインライン関数として定義しておく
 */
Inline void
h8s_scix_isr_in(ID portid)
{
	SIOPCB *siopcb = get_siopcb(portid);

	/* 受信通知コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_rcv( siopcb->exinf );
	}
}

Inline void
h8s_scix_isr_out(ID portid)
{
	SIOPCB *siopcb = get_siopcb(portid);

	/* 送信可能コールバックルーチンの呼び出し */
	if( siopcb->openflag ) {
		h8s_sci_ierdy_snd( siopcb->exinf );
	}
}

/*
 *  SIOの受信エラー割込みサービスルーチン
 *　　全チャネル共通部分をインライン関数として定義しておく
 *
 *  エラー処理は、下記エラーフラグのクリアのみ。
 *  ・オーバーランエラー、フレーミングエラー、パリティエラー
 */
Inline void
h8s_scix_isr_error(INT portid)
{
	SIOPCB *siopcb = get_siopcb(portid);

	if( siopcb->openflag ) {
		/*  エラーフラグクリア  */
		h8s_sci_and( siopcb->siopinib, SSR, ~( RDRF | ORER | FER | PER ) );
	}
}


#ifndef OMIT_SCI0
/*
 *  SIOの割込みサービスルーチン (SCI0専用)
 */
void
h8s_sci0_isr_in(void)
{
	h8s_scix_isr_in(1);
}

void
h8s_sci0_isr_out(void)
{
	h8s_scix_isr_out(1);
}

/*
 *  SIOの受信エラー割込みサービスルーチン (SCI0専用)
 */
void
h8s_sci0_isr_error(void)
{
	h8s_scix_isr_error(1);
}

#endif /*  OMIT_SCI0  */

#if TNUM_SIOP >=2
/*
 *  SIOの割込みサービスルーチン (SCI1専用)
 */
void
h8s_sci1_isr_in(void)
{
	h8s_scix_isr_in(2);
}

void
h8s_sci1_isr_out(void)
{
	h8s_scix_isr_out(2);
}

/*
 *  SIOの受信エラー割込みサービスルーチン (SCI1専用)
 */
void
h8s_sci1_isr_error(void)
{
	h8s_scix_isr_error(2);
}
#endif /* TNUM_SIOP >=2 */

#if TNUM_SIOP >=3
/*
 *  SIOの割込みサービスルーチン (SCI2専用)
 */
void
h8s_sci2_isr_in(void)
{
	h8s_scix_isr_in(3);
}

void
h8s_sci2_isr_out(void)
{
	h8s_scix_isr_out(3);
}

/*
 *  SIOの受信エラー割込みサービスルーチン (SCI2専用)
 */
void
h8s_sci2_isr_error(void)
{
	h8s_scix_isr_error(3);
}
#endif /* TNUM_SIOP >=3 */


/*
 *  H8S 内蔵 SCI 用ポーリング出力 (POL_PORTID専用、sys_putcで利用)
 */
void
h8s_sci_putchar_pol( char c )
{

	const SIOPINIB  *siopinib = get_siopinib( POL_PORTID );

	/* TDRE が 1 になるまで待つ */
	while ( !(h8s_sci_reb( siopinib->reg_base, SSR ) & TDRE ) );

	h8s_sci_wrb( siopinib->reg_base, TDR, c );

	/* TDREクリア */
	h8s_sci_and( siopinib, SSR, ~TDRE );

	/* TEND が 1 になるまで待つ */
	while ( !(h8s_sci_reb( siopinib->reg_base, SSR ) & TEND ) );
}
