/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: sh1sci.c,v 1.12 2005/11/14 08:00:44 honda Exp $
 */

/*
 *   SH1内蔵シリアルコミュニケーションインタフェースSCI用 簡易ドライバ
 *　　　　
 *　　　　SCI0のみのサポートだが、2chへの拡張性は残している。
 *
 *　　　　sh1sci.{c,h}とhw_serial.hの分割の基準
 *　　　　　　・sh1sci.{c,h}：シリアルデバイスに依存する部分のみ記述
 *　　　　　　・hw_serial.h：割込みコントローラ依存
 *　　　　　　
 *　　　　　　・クロック周波数依存について
 *　　　　　　　　・デバイス依存のパラメータはsh1sci.c内で用意する。
 *　　　　　　　　・大元のクロック周波数はコンパイルオプションとして与える
 */

#include <s_services.h>
#include <sh1sci.h>

/*
 *  コントロールレジスタのアドレス （すべて1バイトサイズ）
 */
	/*  チャネル0  */
#define SCI_SMR0 (VB *)0x5fffec0	/*  シリアルモードレジスタ */
#define SCI_BRR0 (VB *)0x5fffec1	/*  ビットレートレジスタ  */
#define SCI_SCR0 (VB *)0x5fffec2	/*  シリアルコントロールレジスタ */
#define SCI_TDR0 (VB *)0x5fffec3	/*  トランスミットデータレジスタ */
#define SCI_SSR0 (VB *)0x5fffec4	/*  シリアルステータスレジスタ */
#define SCI_RDR0 (VB *)0x5fffec5	/*  レシーブデータレジスタ */

	/*  チャネル1  */
#define SCI_SMR1 (VB *)0x5fffec8	/*  シリアルモードレジスタ */
#define SCI_BRR1 (VB *)0x5fffec9	/*  ビットレートレジスタ  */
#define SCI_SCR1 (VB *)0x5fffeca	/*  シリアルコントロールレジスタ */
#define SCI_TDR1 (VB *)0x5fffecb	/*  トランスミットデータレジスタ */
#define SCI_SSR1 (VB *)0x5fffecc	/*  シリアルステータスレジスタ */
#define SCI_RDR1 (VB *)0x5fffecd	/*  レシーブデータレジスタ */


/*
 *  コントロールレジスタの設定値
 */
/*  シリアルステータスレジスタSSRの各ビット  */
#define SSR_TDRE	0x80u	/* トランスミットデータレジスタエンプティ */
#define SSR_RDRF	0x40u	/* レシーブデータレジスタフル 		*/
#define SSR_ORER	0x20u	/* オーバーランエラー 			*/
#define SSR_FER		0x10u	/* フレーミングエラー 			*/
#define SSR_PER		0x08u	/* パリティエラー 			*/
#define SSR_TEND	0x04u	/* トランスミットエンド 		*/
#define SSR_MPB		0x02u	/* マルチプロセッサビット 		*/
#define SSR_MPBT	0x01u	/* マルチプロセッサビットトランスファ 	*/

/*  シリアルコントロールレジスタSCRの各ビット  */
#define SCR_TIE		0x80u	/* トランスミットインタラプトイネーブル */
#define SCR_RIE		0x40u	/* レシーブインタラプトイネーブル 	*/
#define SCR_TE		0x20u	/* トランスミットイネーブル 		*/
#define SCR_RE		0x10u	/* レシーブイネーブル 			*/
#define SCR_MPIE	0x08u	/* マルチプロセッサインタラプトイネーブル */
				/* トランスミットエンドインタラプト	*/
#define SCR_TEIE	0x04u	/* イネーブル 				*/
#define SCR_CKE_MASK	0x03u	/* クロックソース選択用マスク		*/
#define SCR_CKE		0x00u	/* クロックイネーブル			*/
				/*   調歩同期式のとき  			*/
				/*   	内部クロック使用  		*/
				/*   	出力なし  			*/

/*  シリアルモードジスタSMRの各ビット  */
				/* コミュニケーションモード 	*/
#define SMR_CA_CLOCK	0x80u	/* 	クロック同期式 		*/
#define SMR_CA_ASYNC	0x00u	/* 	調歩同期式 		*/
	/*  調歩同期式通信：Asynchronous Communication method	*/
				/* キャラクタレングス 		*/
#define SMR_CHR8	0x00u	/* 	8ビット 		*/
#define SMR_CHR7	0x40u	/* 	7ビット 		*/
				/* パリティイネーブル 		*/
#define SMR_PE		0x20u	/* 	パリティあり 		*/
#define SMR_PE_NON	0x00u	/* 	パリティなし 		*/
#define SMR_OE_ODD	0x10u	/* パリティモード（奇数パリティ）*/
#define SMR_STOP2	0x08u	/* ストップビットレングス：2 	*/
#define SMR_STOP1	0x00u	/* ストップビットレングス：1 	*/
#define SMR_MP		0x04u	/* マルチプロセッサモード 	*/

/*
 *  クロック周波数依存
 *  	京都マイクロコンピュータKZ-SH1-01のクロック周波数は20MHz
 *  	CQ出版 RISC評価キット SH-1のクロック周波数は19.6608MHz
 *  	常盤商行SH1/CPUBのクロック周波数は16MHz
 */

/*
 *  分周比とビットレートの設定
 */
#if CONFIG_BAUD == 9600
/*
 *
 *  クロック	　20MHzの場合
 *  ビットレート　9600bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=64で誤差0.16%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=64
 */
#ifdef CONFIG_20MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	64u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　19.6608MHz
 *  ビットレート　9600bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=63で誤差0%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=63
 */
#ifdef CONFIG_19MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	63u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　16MHz
 *  ビットレート　9600bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=51で誤差0.16%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=51
 */
#ifdef CONFIG_16MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	51u	/*  ビットレート  	*/
#endif

/*
 *  ボーレート設定後、クロックが安定するまでの待ち時間
 *  　（最初の1ビット分）　
 *  BPS=9600bpsの場合
 *  　t = 1 / BPS = 104,167 = 105,000[nsec]
 */
#define SH1SCI_DELAY 	105000


#elif CONFIG_BAUD == 19200


/*
 *
 *  クロック	　20MHzの場合
 *  ビットレート　19200bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=32で誤差 -1.36%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=32
 */
#ifdef CONFIG_20MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	32u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　19.6608MHz
 *  ビットレート　19200bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=31で誤差0%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=31
 */
#ifdef CONFIG_19MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	31u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　16MHz
 *  ビットレート　19200bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=25で誤差0.16%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=25
 */
#ifdef CONFIG_16MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	25u	/*  ビットレート  	*/
#endif

/*
 *  ボーレート設定後、クロックが安定するまでの待ち時間
 *  　（最初の1ビット分）　
 *  BPS=19200bpsの場合
 *  　t = 1 / BPS = 52,083 = 53,000[nsec]
 */
#define SH1SCI_DELAY 	53000


#elif CONFIG_BAUD == 38400


/*
 *
 *  クロック	　20MHzの場合
 *  ビットレート　38400bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=15で誤差1.73%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=15
 */
#ifdef CONFIG_20MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	15u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　19.6608MHz
 *  ビットレート　38400bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=15で誤差0%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=15
 */
#ifdef CONFIG_19MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	15u	/*  ビットレート  	*/
#endif

/*
 *  クロック	　16MHz
 *  ビットレート　38400bps
 *
 *  　n=0(分周しないのでそのまま)
 *  　N=12で誤差0.16%
 *  よって、
 *  　シリアルモードレジスタSMRのクロックセレクトビットCKS=00
 *  　ビットレートレジスタBRR=16
 */
#ifdef CONFIG_16MHZ
#define SMR_CKS 	0x0u	/*  分周比  		*/
#define SCI_BRR 	16u	/*  ビットレート  	*/
#endif

/*
 *  ボーレート設定後、クロックが安定するまでの待ち時間
 *  　（最初の1ビット分）　
 *  BPS=38400bpsの場合
 *  　t = 1 / BPS = 26,042 = 27,000[nsec]
 */
#define SH1SCI_DELAY 	27000

#else	/*  CONFIG_BAUD  */

#error unsupported baud rate.

#endif 	/*  CONFIG_BAUD  */


/*
 *  シリアルI/Oポート管理ブロックの定義
 *  　2chサポートに拡張する場合は初期値用のデータも含める
 */
struct sio_port_control_block {
	VP_INT		exinf;		/* 拡張情報 */
	BOOL		openflag;	/* オープン済みフラグ */
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 *  　　ID = 1 をSCI0に対応させている．
 */
static SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
				/*  ポートIDからデバイス番号を求めるマクロ  */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  文字を受信できるか？
 */
Inline BOOL
sh1sci_getready(SIOPCB *siopcb)
{
	VB ssr0 = sil_reb_mem(SCI_SSR0);
	return(ssr0 & SSR_RDRF);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
sh1sci_putready(SIOPCB *siopcb)
{
	VB ssr0 = sil_reb_mem(SCI_SSR0);
	return(ssr0 & SSR_TDRE);
}

/*
 *  受信した文字の取出し
 */
Inline char
sh1sci_getchar(SIOPCB *siopcb)
{
	char data;

	data = sil_reb_mem(SCI_RDR0);
	
	/*  レシーブデータレジスタフル・フラグのクリア  */
	sh1_anb_reg(SCI_SSR0, (VB)~SSR_RDRF);
	return data;
}

/*
 *  送信する文字の書込み
 */
Inline void
sh1sci_putchar(SIOPCB *siopcb, char c)
{
#ifdef GDB_STUB
	gdb_stub_putc( c );
#else
	sil_wrb_mem(SCI_TDR0 ,c);
	
	/*  トランスミットデータレジスタエンプティ・フラグのクリア*/
	sh1_anb_reg(SCI_SSR0, (VB)~SSR_TDRE);
#endif
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
sh1sci_initialize(void)
{
	SIOPCB	*siopcb;
	UINT	i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
		siopcb->openflag = FALSE;
	}
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
sh1sci_openflag(void)
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
sh1sci_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB *siopcb = get_siopcb(siopid);

#ifndef GDB_STUB
	VB scr0, smr0;
	VH pbcr1;

	sh1_anb_reg(SCI_SCR0, (VB)~(SCR_TE | SCR_RE));	/*  送受信停止  */

	/*  ピンアサイン
	 *     シリアルデバイス自体の事項ではないので
	 *     本来はこのファイルに記述するのはふさわしくないが、
	 *     hw_serial.cを用意するのも煩雑なので暫定的にここに入れている。
	 */
	pbcr1 = sil_reh_mem((VP)PBCR1);
						/* PB8:RxD0 PB9:TxD0  */
	pbcr1 = (pbcr1 & PBCR1_TD0_RD0_MASK) |
			PBCR1_TD0 | PBCR1_RD0;
	sil_wrh_mem((VP)PBCR1, pbcr1);
						/*  送受信フォーマット  */
	smr0 = 	SMR_CA_ASYNC |	/*  調歩同期式  		*/
		SMR_CHR8 |	/*  キャラクタレングス：8ビット */
		SMR_PE_NON |	/*  パリティなし  		*/
		SMR_STOP1 |	/*  ストップビットレングス：1 	*/
		SMR_CKS;	/*  クロックセレクト（分周比）  */
	sil_wrb_mem(SCI_SMR0, smr0);
	
	sil_wrb_mem(SCI_BRR0, SCI_BRR);	/*  ボーレート設定  	*/

					/*  割込み禁止とクロックソースの選択 */
	scr0 = (VB)(~(SCR_TIE | SCR_RIE | SCR_TE | SCR_RE |
			  SCR_MPIE | SCR_TEIE | SCR_CKE_MASK) | SCR_CKE);
	sil_wrb_mem(SCI_SCR0, scr0);
	
	/*
	 * ボーレートの設定後、1カウント分待たなければならない。
	 */
	sil_dly_nse(SH1SCI_DELAY);

					/* エラーフラグをクリア	*/
	sh1_anb_reg(SCI_SSR0, (VB)~(SSR_ORER | SSR_FER | SSR_PER));
					/* 受信割り込み許可   	*/
					/* 送受信許可  		*/
	sh1_orb_reg(SCI_SCR0, (SCR_RIE | SCR_TE | SCR_RE));
			/*  送信割込みの許可は送信制御関数で行う  */

#endif	/*  GDB_STUB  */

	siopcb->exinf = exinf;
	siopcb->openflag = TRUE;
	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sh1sci_cls_por(SIOPCB *siopcb)
{
				/*  送受信停止、割込み禁止 	*/
	sh1_anb_reg(SCI_SCR0, (VB)~(SCR_TIE | SCR_RIE | SCR_TE | SCR_RE));
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
sh1sci_snd_chr(SIOPCB *siopcb, char c)
{
	if (sh1sci_putready(siopcb)) {
		sh1sci_putchar(siopcb, c);
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
sh1sci_rcv_chr(SIOPCB *siopcb)
{
	if (sh1sci_getready(siopcb)) {
		return((INT)(UB)sh1sci_getchar(siopcb));
		/*  (UB)でキャストするのはゼロ拡張にするため  */
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
sh1sci_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:	/* 送信割り込み要求を許可 */
		sh1_orb_reg(SCI_SCR0, SCR_TIE);
		break;
	case SIO_ERDY_RCV:	/* 受信割り込み要求を許可 */
		sh1_orb_reg(SCI_SCR0, SCR_RIE);
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
sh1sci_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:	/* 送信割り込み要求を禁止 */
		sh1_anb_reg(SCI_SCR0, (VB)~SCR_TIE);
		break;
	case SIO_ERDY_RCV:	/* 受信割り込み要求を禁止 */
		sh1_anb_reg(SCI_SCR0, (VB)~SCR_RIE);
		break;
	}
}

/*
 *  シリアルI/Oポートに対する送信割込み処理
 */
Inline void
sh1sci_isr_siop_out(SIOPCB *siopcb)
{
	VB scr0 = sil_reb_mem(SCI_SCR0);
	
	if ((scr0 & SCR_TIE) != 0 && sh1sci_putready(siopcb)) {
		/*
		 *  送信通知コールバックルーチンを呼び出す．
		 */
		sh1sci_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
Inline void
sh1sci_isr_siop_in(SIOPCB *siopcb)
{
	VB scr0 = sil_reb_mem(SCI_SCR0);

	if ((scr0 & SCR_RIE) != 0 && sh1sci_getready(siopcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		sh1sci_ierdy_rcv(siopcb->exinf);
	}
}

/*
 *  SIO送信割込みサービスルーチン
 *  
 *  　SH1内蔵のSCIでは割込み番号が送受信別、チャネル別に分かれているので、
 *  　SCI0の送信割込み以外でこのルーチンが呼ばれることはない
 *  
 */
void
sh1sci_isr_out(void)
{
	if (siopcb_table[0].openflag) {
		sh1sci_isr_siop_out(get_siopcb(1));
	}
}

/*
 *  SIO受信割込みサービスルーチン
 *  
 *  　SH1内蔵のSCIでは割込み番号が送受信別、チャネル別に分かれているので、
 *  　SCI0の受信割込み以外でこのルーチンが呼ばれることはない
 *  
 */
void
sh1sci_isr_in(void)
{
	if (siopcb_table[0].openflag) {
		sh1sci_isr_siop_in(get_siopcb(1));
	}
}

/*
 *  SIO受信エラー割込みサービスルーチン
 *  
 *  　SH1内蔵のSCIでは割込み番号がチャネル別に分かれているので、
 *  　SCI0の受信エラー割込み以外でこのルーチンが呼ばれることはない
 *  　
 *  　エラー処理自体はエラーフラグのクリアのみにとどめている。
 *  　　　・オーバーランエラー
 *  　　　・フレーミングエラー
 *  　　　・パリティエラー
 */
void
sh1sci_isr_error(void)
{
	VB ssr0;
	
	if (siopcb_table[0].openflag) {
		ssr0 = sil_reb_mem(SCI_SSR0);	/*  1度読み出して  */
		ssr0 &= ~(SSR_RDRF | SSR_ORER | SSR_FER | SSR_PER);
		sil_wrb_mem(SCI_SSR0, ssr0);	/*  エラーフラグクリア  */
	}
}

/*
 * ポーリングによる文字の送信
 */
void
sh1sci_putc_pol(char c)
{
	while(!sh1sci_putready(&siopcb_table[0]));
	sh1sci_putchar(&siopcb_table[0], c);
}
