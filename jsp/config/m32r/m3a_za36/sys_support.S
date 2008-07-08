/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: sys_support.S,v 1.2 2007/05/28 02:03:55 honda Exp $
 */

/*
 *  ターゲットシステム依存モジュール アセンブリ言語部（m3a_za36用）
 */

#define	_MACRO_ONLY

#include <m32rasm.inc>

/*
 *  使用するSFR定義
 */
#define BSEL0CR		0x00ef5000	/* BSEL0制御レジスタ */
#define SDRF0		0x00ef6000	/* SDRAMリフレッシュ制御レジスタ0 */
#define SDRF1		0x00ef6004	/* SDRAMリフレッシュ制御レジスタ1 */
#define SDIR0		0x00ef6008	/* SDRAM初期化レジスタ0 */
#define SDIR1		0x00ef600c	/* SDRAM初期化レジスタ1 */
#define SD0ADR		0x00ef6020	/* SDRAM0アドレスレジスタ */
#define SD0ER		0x00ef6024	/* SDRAM0アクセスイネーブルレジスタ */
#define SD0TR		0x00ef6028	/* SDRAM0タイミングレジスタ */
#define SD0MOD		0x00ef602c	/* SDRAM0モードレジスタ */
#define SD1ADR		0x00ef6040	/* SDRAM1アドレスレジスタ */
#define SD1ER		0x00ef6044	/* SDRAM1アクセスイネーブルレジスタ */
#define SD1TR		0x00ef6048	/* SDRAM1タイミングレジスタ */
#define SD1MOD		0x00ef604c	/* SDRAM1モードレジスタ */
#define MCCR		0xfffffffc	/* キャッシュ制御レジスタ */
#define ICUIMASK	0x00eff01c	/* 割込みマスクレジスタ	*/

/*
 *	SDRAM設定マクロ定義
 */
#define SDRF1_VAL	0x00010017	/* SDRAMリフレッシュ制御レジスタ1設定値	*/
#define	SDIR0_VAL	0x00000001	/* SDRAM初期化レジスタ0設定値			*/
#define	SD0ADR_VAL	0x04000004	/* SDRAM0アドレスレジスタ設定値			*/
#define SD0TR_VAL	0x00020102	/* SDRAM0タイミングレジスタ設定値		*/
#define SD0MOD_VAL	0x00000020	/* SDRAM0モードレジスタ設定値			*/

/*
 *	ブロックセレクトコントローラの設定値
 */
#define BSEL0CR_VAL	0x01011100

/*
 *  ハードウェア初期化処理
 *
 *  data，bssセクションをSDRAM領域に配置する場合には，デバッガ側で
 *  SDRAMを初期化する．その場合は，hardware_init_hookでは何もしない．
 */
#if 0
Function hardware_init_hook

	/*
	 * ブロックセレクトコントローラの設定
	 */
	mLdadr	r2, BSEL0CR_VAL
	ld24	r3, #BSEL0CR
	st		r2, @r3

	/*
	 *  キャッシュモードの設定
	 */
	ldi	r0, #0x00   /* キャッシュOFF */
	ldi	r2, #MCCR	/* 符号拡張させてアドレスをロード */
	st	r0, @r2		/* 命令/データキャッシュモード選択 */

	/*
	 *  外部SDRAMの設定 
	 */
	/* --- SDRAM初期化 -- */
	ld24	r2, #SDIR0		/* SDRAM初期化レジスタ0 */
	mLdadr	r3, SDIR0_VAL	/* 初期化プリチャージ後ウエイト(DPC)       3BCLK	*/
							/* 初期化オートリフレッシュ回数(DARFC)     8回		*/
							/* 初期化オートリフレッシュ要求間隔(DARFI) 8BCLK	*/
	st		r3, @r2		/* 設定	*/

	ld24	r2, #SDIR1			/* H'00EF600c	*/
	ldi		r3, #0x00000001		/* 初期化開始	*/
	st		r3, @r2

	/* --- SDRAM初期化シーケンス完了待ち -- */
wait_sdram:
	ldh		r3, @r2
	bnez	r3, wait_sdram		/* 初期化終了？ */

	/* -- SDRAMモードレジスタセットコマンド発行 */
	ld24	r2, #SD0MOD			/* SDRAM0モードレジスタ					*/
	mLdadr	r3, SD0MOD_VAL			/* バーストモード						*/
								/* CL=2									*/
								/* バーストタイプ：シーケンシャル		*/
								/* バースト長							*/
	st		r3, @r2				/* 設定	*/

	/* ---- SDRAMタイミングレジスタ設定 -- */
	ld24	r2, #SD0TR			/* SDRAMタイミングレジスタ							*/
	mLdadr	r3, SD0TR_VAL		/* RASアクティブ期間(DRAS)             3BCLK		*/
								/* RAS-CASレーテンシ(DRCD)             1BCLK		*/
								/* RASプリチャージ(DPCG)               1BCLK		*/
								/* ライトリカバリ時間(DWR)             2BCLK		*/
								/* SDRAMコントローラCASレーテンシ(DCL) 2BCLK		*/
	st      r3, @r2			/* 設定	*/

	/* --- SDRAM領域設定(H'04000000から32Mバイト,32bitBUS)	*/
	ld24	r2, #SD0ADR		/* SDRAM0アドレスレジスタ							*/
	mLdadr	r3, SD0ADR_VAL		/* 先頭アドレス(DADR)         H'0400 xxxx			*/
								/* データバス幅選択(DBSZ)     32ビット				*/
								/* チャネルサイズ設定(DSZ)    64Mバイト				*/
	st		r3, @r2			/* 設定	*/

	/* ---- オートリフレッシュ有効、要求間隔:1344クロックに設定	*/
	ld24	r2, #SDRF1		/* SDRAMリフレッシュ制御レジスタ1					*/
	mLdadr	r3, SDRF1_VAL		/* オートリフレッシュ(DRFEN)	1(有効）			*/
								/* オートリフレッシュウェイト(DREFW) 8BCLK			*/
								/* オートリフレッシュ要求間隔(DRFC)  1344BCLK		*/
	st		r3, @r2

	/* SDRAMアクセス有効 */
	ld24	r2, #SD0ER			/* SDRAMアクセスイネーブルレジスタ	*/
	ldi		r3, #0x01			/* 設定値をレジスタに格納			*/
	st		r3, @r2
	rts
#endif
		
Function software_init_hook
	ld24 	r0, ICUIMASK + 1
	ldi 	r1, 7
	stb 	r1, @r0 			/* 割込みマスクを全許可に設定 */
	rts
