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

#define _MACRO_ONLY
#include <jsp_kernel.h>

/*
 *  ターゲットハードウェア依存する初期化処理
 */
	.h8300s
	.text
	.align 2

	.global	_hardware_init_hook

_hardware_init_hook:

#ifndef GDB_STUB	/* ROM の初期化などは、GDB スタブで行うので不要 */
	/*
	 *  SYSCR の設定
	 *  ・割込み制御モードを割込み制御モード2に設定
	 *  ・内蔵RAM無効
	 */
	mov.l	#(H8S_BASE_ADDR+SYSCR), er0
	mov.b	@er0, r1l		/* r1l = SYSCR */

	or.b	#INTM1, r1l		/* INTM1 = 1(割込み制御モード2に設定) */
	bset	#RAME_BIT, r1l		/* RAME  = 1(内蔵RAM有効) */
	mov.b	r1l, @er0		/* SYSCR = r1l */

	/*
	 *  MCU 動作モードの設定
	 *  ・ボード上のディップスイッチでモード5に設定
	 *    (モード5：アドバンスト・内蔵ROM無効拡張モード；アドレス空間は16MB)
	 *  ・MCU 動作モードの設定により、I/Oポートは以下のように設定される。
         *      ポートA, B, C：アドレス出力
         *      ポートD      ：データ入出力
         *      ポートF      ：PF6〜PF3 -> バス制御出力
	 */

	/*
	 * I/O ポートの初期化 (DRAMインタフェース関係)
	 */
	mov.l	#(H8S_BASE_ADDR+PGDDR), er0	/* ポートＧ の設定 */
	mov.b	#(PG4DDR|PG3DDR), r1l		/* モード５での利用なので、
						   CS0,CS1 イネーブル */
	mov.b	r1l, @er0

        /*
         *  バスコントローラの設定
         *  ・外部ROM (0x000000-0x01ffff;128kB)：エリア0
         *  ・外部RAM (0x200000-0x21ffff:128kB)：エリア1
         *  ・内蔵RAM (0xfff400-0xfffbff:  2kB)：
	 *  ・内部I/O (0xfffe3f-0xffffff:     )：エリア7
         *
	 *  設定値
	 *  ・エリア0 ：・16ビットアクセス空間		( ABW0=1 ; ABWCR )
	 *		・3ステートアクセス		( AST0=1 ; ASTCR )
	 *		・プログラムウエイトを3ステート挿入
	 *						( W01=1, W00=1 ; WCRL )
	 *  ・エリア1 ：・16ビットアクセス空間		( ABW1=1 ; ABWCR )
	 *		・3ステートアクセス		( AST1=1 ; ASTCR )
	 *		・プログラムウエイトを3ステート挿入
	 *						( W11=1, W10=1 ; WCRL )
	 *  ・アイドルサイクルの挿入
	 *	      ：・異なるエリアの外部リードサイクルが連続したときに,
	 *		  アイドルサイクルを挿入	( ICIS1=1 ; BCRH )
	 *	      ：・外部リードサイクルと外部ライトサイクルが連続したときに,
	 *		  アイドルサイクルを挿入	( ICIS0=1 ; BCRH )
	 *	      ：・バーストサイクルは２ステート	( BRSTS1=1; BCRH )
         *  ( 注意：内蔵RAM及び内蔵I/Oレジスタに対するアクセスステート数は
         *          ASTCRの設定値に関わらず固定。)
	 */

	/* ABWCR, ASTCR の設定は初期値のままなので、省略 */

	/* ウエイトコントロールレジスタL (WCRL) の設定 */
	mov.l	#(H8S_BASE_ADDR+WCRL), er0
	mov.b	#(W11|W10|W01|W00), r1l
	mov.b	r1l, @er0

	/* バスコントロールレジスタH (BCRH) の設定 */
	mov.l	#(H8S_BASE_ADDR+BCRH), er0
 	mov.b	#(ICIS1|ICIS0|BRSTS1), r1l	/* レジスタ初期値と同値 */
	mov.b 	r1l, @er0

#endif /* GDB_STUB */

	/*
	 * I/O ポートの初期化 (その他)
	 */

	/* ポート１の設定 */
	/* このポートには、ターゲットボードではディップスイッチが接続されている。
	   P1DDR は初期値で「入力」なので、設定不要 */

	/* ポート２の設定 */
	/* このポートには、ターゲットボードでは LED が接続されている。
	   初期化マクロは、ターゲットボードに関するファイルに定義されている。 */
	PORT2_INIT

	/* ポート３の設定 */
	/* このポートには、内蔵SCI が接続されている。
	   SCI の SCR、SMRの方が主となる設定を行うので、設定不要 */

	mov.l	@bss_clear_k, er0
	jmp	@er0

bss_clear_k:
	.long  bss_clear
