/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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
 */

/*
 *  カーネル用のスタートアップモジュール(S1C33用)
 */

#include "jsp_kernel.h"
#include "sys_config.h"
#include "cpu_config.h"

/*
 *  外部関数の参照
 */
extern void init_lib(void);
extern void init_sys(void);

extern void INT_ENTRY(timer_handler)(void);
extern void INT_ENTRY(sio_in_handler)(void);
extern void INT_ENTRY(sio_out_handler)(void);

/*
 *  プロトタイプ宣言
 */
void start(void);
static void undef_handler(void);
static void hardware_init_hook(void);
static void software_init_hook(void);

/*
 *  ベクターテーブル
 */
VP tps_VectorTable[] = {		/* offset   number */
	(VP) start,			/* 00  Reset                    */
	(VP) undef_handler,		/* 01                           */
	(VP) undef_handler,		/* 02                           */
	(VP) undef_handler,		/* 03                           */
	(VP) undef_handler,		/* 04  Zero division            */
	(VP) undef_handler,		/* 05                           */
	(VP) undef_handler,		/* 06  Address error exception  */
	(VP) undef_handler,		/* 07  debug                    */
	(VP) undef_handler,		/* 08  NMI                      */
	(VP) undef_handler,		/* 09                           */
	(VP) undef_handler,		/* 10                           */
	(VP) undef_handler,		/* 11                           */
	(VP) undef_handler,		/* 12  Software exception 0     */
	(VP) undef_handler,		/* 13  Software exception 1     */
	(VP) undef_handler,		/* 14  Software exception 2     */
	(VP) undef_handler,		/* 15  Software exception 3     */
	(VP) undef_handler,		/* 16 (00)                      */
	(VP) undef_handler,		/* 17 (01)                      */
	(VP) undef_handler,		/* 18 (02)                      */
	(VP) undef_handler,		/* 19 (03)                      */
	(VP) undef_handler,		/* 20 (04)                      */
	(VP) undef_handler,		/* 21 (05)                      */
	(VP) undef_handler,		/* 22 (06)                      */
	(VP) undef_handler,		/* 23 (07)                      */
	(VP) undef_handler,		/* 24 (08)                      */
	(VP) undef_handler,		/* 25 (09)                      */
	(VP) undef_handler,		/* 26 (10)                      */
	(VP) undef_handler,		/* 27 (11)                      */
	(VP) undef_handler,		/* 28 (12)                      */
	(VP) undef_handler,		/* 29 (13)                      */
	(VP) undef_handler,		/* 30 (14)                      */
	(VP) undef_handler,		/* 31 (15)                      */
	(VP) undef_handler,		/* 32 (16)                      */
	(VP) undef_handler,		/* 33 (17)                      */
	(VP) INT_ENTRY(timer_handler),	/* 34 (18) 16 bit programable timer 1-b */
	(VP) undef_handler,		/* 35 (19)                      */
	(VP) undef_handler,		/* 36 (20)                      */
	(VP) undef_handler,		/* 37 (21)                      */
	(VP) undef_handler,		/* 38 (22)                      */
	(VP) undef_handler,		/* 39 (23)                      */
	(VP) undef_handler,		/* 40 (24)                      */
	(VP) undef_handler,		/* 41 (25)                      */
	(VP) undef_handler,		/* 42 (26)                      */
	(VP) undef_handler,		/* 43 (27)                      */
	(VP) undef_handler,		/* 44 (28)                      */
	(VP) undef_handler,		/* 45 (29)                      */
	(VP) undef_handler,		/* 46 (30)                      */
	(VP) undef_handler,		/* 47 (31)                      */
	(VP) undef_handler,		/* 48 (32)                      */
	(VP) undef_handler,		/* 49 (33)                      */
	(VP) undef_handler,		/* 50 (34)                      */
	(VP) undef_handler,		/* 51 (35)                      */
	(VP) undef_handler,		/* 52 (36)                      */
	(VP) undef_handler,		/* 53 (37)                      */
	(VP) undef_handler,		/* 54 (38)                      */
	(VP) undef_handler,		/* 55 (39)                      */
	(VP) undef_handler,		/* 56 (40) Serial interface Ch.0 (error) */
	(VP) INT_ENTRY(sio_in_handler),	/* 57 (41) Serial interface Ch.0 (rx) */
	(VP) INT_ENTRY(sio_out_handler),/* 58 (42) Serial interface Ch.0 (tx) */
	(VP) undef_handler,		/* 59 (43)                      */
	(VP) undef_handler,		/* 60 (44)                      */
	(VP) undef_handler,		/* 61 (45)                      */
	(VP) undef_handler,		/* 62 (46)                      */
	(VP) undef_handler,		/* 63 (47)                      */
	(VP) undef_handler,		/* 64 (48)                      */
	(VP) undef_handler,		/* 65 (49)                      */
	(VP) undef_handler,		/* 66 (50)                      */
	(VP) undef_handler,		/* 67 (51)                      */
	(VP) undef_handler,		/* 68 (52)                      */
	(VP) undef_handler,		/* 69 (53)                      */
	(VP) undef_handler,		/* 70 (54)                      */
	(VP) undef_handler,		/* 71 (55)                      */
};

/*
 *  ブートルーチン
 *  Boot routine
 */
void start(void)
{
	/*
	 *  デフォルトデータエリアアドレスを設定する
	 */
	Asm("xld.w %r15, __dp");

	/*
	 *  非タスクコンテストのスタックをSTACKTOPに初期化する
	 */
	set_sp(STACKTOP);

	/*
	 * PSRを初期化し、割り込みを禁止する
	 */
	set_psr(0x00000000);


	/*
	 *  hardware_init_hook を呼び出す。(0でない場合)
	 *
	 *  ハードウェア依存に必要な初期化処理がある場合は，
	 *  hardware_init_hook という関数を用意すればよい．
	 *  具体的にはROM化の際、RAMを使用可能にするための
	 *  バスコントローラの初期化等を行う。
	 *  sys_config.c 内で hardware_init_hook を定義している．
	 */
	if (hardware_init_hook != (VP) 0) {
		hardware_init_hook();
	}

	/*
	 *  dataセクションを初期化する（ROM化対応）．
	 */
	memcpy(VECTOR_START, IVECTOR_START, (UW)IVECTOR_END - (UW)IVECTOR_START);
	memcpy(DATA_START, IDATA_START, (UW)IDATA_END - (UW)IDATA_START);

	/*
	 *  bssセクションを0クリアする。
	 */
	 memset(BSS_START, 0x00, BSS_END - BSS_START);

	/*
	 *  software_init_hook を呼び出す（0 でない場合）
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
	if (software_init_hook != (VP) 0) {
		software_init_hook();
	}

	/*
	 *  カーネルを起動する
	 */
	Asm("xjp kernel_start");

	do {
		Asm("halt");
	} while (TRUE);
}

/*
 *  ダミールーチン
 */
static void undef_handler(void)
{
	while(1){
		;
	}
}

/*
 *  システム依存の初期化処理
 */
static	void	hardware_init_hook(void)
{
	(*(s1c33Bcu_t *) S1C33_BCU_BASE).bTtbrProt = (IOREG)0x59;
	(*(s1c33Bcu_t *) S1C33_BCU_BASE).ulTtbr = (LIOREG)tps_VectorTable;

	return;
}


/*
 *  ソフトウェア依存の初期化処理
 */
static	void	software_init_hook(void)
{
	init_lib();
	init_sys();
}
