/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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
 
#include <vr4131_dsiu.h>

/*
 *	VR4131内蔵DSIU用 簡易SIOドライバ
 */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	UB	lcr_def;	/* LCR設定値 b0-b6;
				   (送受信ビット数、ストップビット，パリティ) */
	UB	boud_hi_def;	/* DLM設定値（ボーレート上位の設定値） */
	UB	boud_lo_def;	/* DLL設定値（ボーレート下位の設定値） */
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
 *  （ポートは１つしかないが、他のソースコードとの親和性から配列で定義する）
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ (UB)	WORD_LENGTH_8 | STOP_BITS_1 | PARITY_NON,
	  (UB)	HI8(DIVISOR),
	  (UB)	LO8(DIVISOR) }
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
 *  状態の読出し（IIDの読出し）
 */
static void
vr4131_dsiu_get_stat(SIOPCB *siopcb)
{
	UB	iid;

	iid = vr4131_reb_mem( (VP) DSIUIID ) & INT_MASK;

	switch( iid ) {
		case INT_TRANS_EMPTY :
			siopcb->putready = TRUE;
			break;
		case INT_RECEIVE_DATA :
		case INT_CHAR_TIME_OUT :
			siopcb->getready = TRUE;
			break;
		default :
			break;
	}


}

/*
 *  文字を受信できるか？
 */
Inline BOOL
vr4131_dsiu_getready(SIOPCB *siopcb)
{
	return(siopcb->getready);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
vr4131_dsiu_putready(SIOPCB *siopcb)
{
	return(siopcb->putready);
}

/*
 *  受信した文字の取出し
 */
Inline char
vr4131_dsiu_getchar(SIOPCB *siopcb)
{
	siopcb->getready = FALSE;
	return((char) vr4131_reb_mem( (VP) DSIURB ));
}

/*
 *  送信する文字の書込み
 */
Inline void
vr4131_dsiu_putchar(SIOPCB *siopcb, char c)
{
	siopcb->putready = FALSE;

#ifndef GDB_STUB
	vr4131_wrb_mem( (VP) DSIUTH, c );
#else /* GDB_STUB */
	stub_putc( c );
#endif /* GDB_STUB */
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
vr4131_dsiu_initialize()
{
	SIOPCB	*siopcb = siopcb_table;
	UINT	i = 0;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	siopcb->siopinib = &(siopinib_table[i]);
	siopcb->openflag = FALSE;
}

/*
 *  SIOレジスタ初期化ルーチン
 */
void
vr4131_dsiu_init_siopinib( const SIOPINIB  *siopinib )
{
	/*
	 * DSIUレジスタの初期化
	 */
	/* 初期処理 */
	vr4131_wrb_mem( (VP) DSIUIE,  DIS_INT );

	/* ボーレートの設定 */
	vr4131_wrb_mem( (VP) DSIULC,  siopinib->lcr_def | DIVISOR_LATCH_ACC );

	vr4131_wrb_mem( (VP) DSIUDLL, siopinib->boud_lo_def );
	vr4131_wrb_mem( (VP) DSIUDLM, siopinib->boud_hi_def );

	/* モードの設定 */
	vr4131_wrb_mem( (VP) DSIULC,  siopinib->lcr_def );

	/* FIFOの設定 */
	vr4131_wrb_mem( (VP) DSIUFC,  FIFO_ENABLE );
	vr4131_wrb_mem( (VP) DSIUFC,  FIFO_ENABLE | RECEIVE_FIFO_RESET | TRANS_FIFO_RESET | RECEIVE_TRIG_1_BYTE );
	vr4131_wrb_mem( (VP) DSIUFC,  FIFO_ENABLE | RECEIVE_TRIG_1_BYTE );

	/* 終了処理 */
	vr4131_wrb_mem( (VP) DSIUMC,  RTS );

	vr4131_wrb_mem( (VP) DSIUIE,  RECEIVE_DATA_AVAILABLE);
}

/*
 *  カーネル起動時のバーナー出力用の初期化
 */
void
vr4131_dsiu_init(void)
{
	vr4131_dsiu_init_siopinib( get_siopinib(1) );
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
vr4131_dsiu_openflag(void)
{
	return( siopcb_table[0].openflag );
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
vr4131_dsiu_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB		*siopcb = get_siopcb(siopid);
	const SIOPINIB	*siopinib = siopcb->siopinib;

#ifndef GDB_STUB
	/* DSIUレジスタの初期化 */
	vr4131_dsiu_init_siopinib( siopinib );
#endif	/*  GDB_STUB  */

	/* 割込みレベル設定、割込み要求クリアは、sio_opn_por(hw_serial.h)で行う。 */

	siopcb->exinf = exinf;
	siopcb->getready = siopcb->putready = FALSE;
	siopcb->openflag = TRUE;

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
vr4131_dsiu_cls_por(SIOPCB *siopcb)
{
	vr4131_wrb_mem( (VP) DSIUIE, DIS_INT );

	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
vr4131_dsiu_snd_chr(SIOPCB *siopcb, char c)
{
	if (vr4131_dsiu_putready(siopcb)) {
		vr4131_dsiu_putchar(siopcb, c);
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
vr4131_dsiu_rcv_chr(SIOPCB *siopcb)
{
	if (vr4131_dsiu_getready(siopcb)) {
		return((INT)(UB) vr4131_dsiu_getchar(siopcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
vr4131_dsiu_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	ie_bit = 0;

	switch (cbrtn) {
	case SIO_ERDY_SND:
		ie_bit = TRANS_REG_EMPTY;
		break;
	case SIO_ERDY_RCV:
		ie_bit = RECEIVE_DATA_AVAILABLE | RECEIVE_LINE_STATUS;
		break;
	}

	vr4131_orb( (VP) DSIUIE, ie_bit );
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
vr4131_dsiu_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	ie_bit = 0;

	switch (cbrtn) {
	case SIO_ERDY_SND:
		ie_bit = TRANS_REG_EMPTY;
		break;
	case SIO_ERDY_RCV:
		ie_bit = RECEIVE_DATA_AVAILABLE | RECEIVE_LINE_STATUS;
		break;
	}

	vr4131_andb( (VP) DSIUIE, ~ie_bit );
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
static void
vr4131_dsiu_isr_siop(SIOPCB *siopcb)
{
	vr4131_dsiu_get_stat(siopcb);

	if ( vr4131_dsiu_getready(siopcb) ) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		vr4131_dsiu_ierdy_rcv(siopcb->exinf);
	}
	if ( vr4131_dsiu_putready(siopcb) ) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		vr4131_dsiu_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
vr4131_dsiu_isr()
{
	if (siopcb_table[0].openflag) {
		vr4131_dsiu_isr_siop(&(siopcb_table[0]));
	}
}

/*
 *  VR4131 内蔵 DSIU 用ポーリング出力 (sys_putcで利用)
 */
void
vr4131_dsiu_putchar_pol( char val ) {

	/* 送信部エンプティになるまで待つ。 */
	while( (vr4131_reb_mem( (VP)DSIULS ) & (TEMT) ) == 0 );

	vr4131_wrb_mem( (VP)DSIUTH, (VB) val );
}
