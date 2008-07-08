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

#ifndef _VR4131_ICU_H_
#define _VR4131_ICU_H_

/*
 *  割込みコントローラ(ICU)関係の定義
 */

/*  割込み番号の定義（0-7番は mips3.h で使用。8番以降を指定する。） */
/*  SYSINT1REG  */
#define INTNO_BAT        8	/*  バッテリ割込み  */
#define INTNO_POWER      9	/*  パワースイッチ割込み  */
#define INTNO_RTCL1     10	/*  RTCLong1割込み  */
#define INTNO_ETIME     11	/*  ElipsedTimeタイマ割込み */
#define INTNO_GIU       12	/*  GIU割込み  */
#define INTNO_SIU       13	/*  SIU割込み  */
#define INTNO_SOFTINT   14	/*  ソフトウェア割込み  */
#define INTNO_CLKRUN    15	/*  CLKRUN割込み  */

/*  SYSINT2REG  */
#define INTNO_RTCL2     16	/*  RTCLong2割込み  */
#define INTNO_LED       17	/*  LED割込み  */
#define INTNO_TCLK      18	/*  TClockカウンタ割込み  */
#define INTNO_FIR       19	/*  FIR割込み  */
#define INTNO_DSIU      20	/*  DSIU割込み  */
#define INTNO_PCI       21	/*  PCI割込み  */
#define INTNO_SCU       22	/*  SCU割込み  */
#define INTNO_CSI       23	/*  CSI割込み  */
#define INTNO_BCU       24	/*  BCU割込み  */

/*  割込みコントローラが管理する割込みの本数  */
#define TMAX_ICU_INTNO	17u

/*  割込みコントローラのレジスタのアドレス定義  */
/* 以下の xxx_asm、xxx_offset は、アセンブラでの利用向け。 */
#define ICU_BASE_ADDR		0x0f000000
#define ICU_BASE_ADDR_asm	ASM_SIL( ICU_BASE_ADDR )

#define SYSINT1_offset		0x80
#define MSYSINT1_offset		0x8c
#define MDSIUINT_offset		0x96
#define SYSINT2_offset		0xa0
#define MSYSINT2_offset		0xa6

#define MSYSINT1REG	(ICU_BASE_ADDR + MSYSINT1_offset)
				/* システム割込みマスクレジスタ1(レベル1) */
#define MDSIUINTREG	(ICU_BASE_ADDR + MDSIUINT_offset)
				/* DSIU割込みマスクレジスタ(レベル2)  */
#define MSYSINT2REG	(ICU_BASE_ADDR + MSYSINT2_offset)
				/* システム割込みマスクレジスタ2(レベル1) */

/*  割込み要因ビットパターン (下記、アセンブラ部分でも利用している。) */
/* MSYSINT1REG / SYSINT1REG 関係 */
#define CLKRUNINTR	BIT12	/* CLKRUN 割込み */
#define SOFTINTR	BIT11	/* ソフトウェア割込み */
#define SIUINTR		BIT9	/* SIU 割込み */
#define	GIUINTR		BIT8	/* GIU(下位) 割込み */
#define	ETIMERINTR	BIT3	/* ElapsedTimeタイマ 割込み */
#define RTCL1INTR	BIT2	/* RTCLong1タイマ 割込み */
#define POWERINTR	BIT1	/* パワースイッチ割込み */
#define BATINTR		BIT0	/* バッテリ割込み */

/* MSYSINT2REG / SYSINT2REG 関係 */
#define BCUINTR		BIT9	/* BCU 割込み */
#define CSIINTR		BIT8	/* CSI 割込み */
#define SCUINTR		BIT7	/* SCU 割込み */
#define PCIINTR		BIT6	/* PCI 割込み */
#define DSIUINTR	BIT5	/* DSIU 割込み */
#define FIRINTR		BIT4	/* FIR 割込み */
#define TCLKINTR	BIT3	/* VTClockカウンタ 割込み */
#define LEDINTR		BIT1	/* LED 割込み */
#define RTCL2INTR	BIT0	/* RTCLong2 割込み */

/* MDSIUINTREG 関係 */
#define INTDSIU  	BIT11	/* DSIUの変化割込み許可 */

/*
 *  割込みコントローラの割込みマスク関係
 */

/*  構造体ICU_IPM内のオフセットを求めるためのマクロ（makeoffset.cで用いる）
    なお、このマクロで定義した値は、特に利用していない。*/
#define OFFSET_DEF_ICU_IPM	OFFSET_DEF(ICU_IPM, msysint2)

/*  MSYSINT1,2に設定してはいけないビット  */
#define NG_BIT_MSYSINT1	(BIT4 | BIT5 | BIT6 | BIT7 | BIT10 | BIT13 |BIT14 | BIT15)
#define NG_BIT_MSYSINT2	(BIT2 | BIT10 | BIT11 | BIT12 | BIT13 | BIT14 | BIT15)

/*  割込みコントローラに設定する割込みマスクのチェック  */
#define CHECK_ICU_IPM(ipm)						\
		CHECK_PAR(!(ipm.msysint1 & NG_BIT_MSYSINT1));		\
		CHECK_PAR(!(ipm.msysint2 & NG_BIT_MSYSINT2))

#ifndef _MACRO_ONLY

/*  割込みコントローラに対する割込みマスクの擬似テーブル  */
extern ICU_IPM icu_intmask_table[];

/*  割込みコントローラのintmaskテーブルの設定  */
Inline void icu_set_ilv(INTNO intno, ICU_IPM *ipm) {
	/* CHECK_ICU_IPM(ipm) は、上位ルーチンで実行済み */
	icu_intmask_table[intno].msysint1 = ipm->msysint1;
	icu_intmask_table[intno].msysint2 = ipm->msysint2;
}

/*  割り込みコントローラのマスク設定  */
Inline void icu_set_ipm(ICU_IPM *ipm) {
	/* CHECK_ICU_IPM(ipm) は、上位ルーチンで実行済み */
	vr4131_wrh_mem( (VP) MSYSINT1REG, ipm->msysint1 );
	vr4131_wrh_mem( (VP) MSYSINT2REG, ipm->msysint2 );
}

/*  割り込みコントローラのマスク取得  */
Inline void icu_get_ipm(ICU_IPM *ipm) {
	ipm->msysint1 = vr4131_reh_mem( (VP) MSYSINT1REG );
	ipm->msysint2 = vr4131_reh_mem( (VP) MSYSINT2REG );
}

#endif /* _MACRO_ONLY */

/*============================================================================*/
/*  アセンブラ処理関係  */

/*  割込み許可ビットの待避と復元  */
/*  割込みコントローラICUのIPMをスタックに保存  */
#define PUSH_ICU_IPM						\
    li      t1, ICU_BASE_ADDR_asm;				\
    addi    sp, sp, -2*2;					\
    lh      t3, MSYSINT1_offset(t1);	/* t3 = MSYSINT1REG */	\
    lh      t4, MSYSINT2_offset(t1);	/* t4 = MSYSINT2REG */	\
    sh      t3, (sp);						\
    sh      t4, 2(sp)

/*  割込みコントローラICUのIPMをスタックから復元  */
#define POP_ICU_IPM							      \
    li      t1, ICU_BASE_ADDR_asm;					      \
    lw      t3, (sp);			/* t3 = MSYSINT2REG:MSYSINT1REG */    \
					/* 注意：リトルエンディアン依存 */    \
    sh      t3, MSYSINT1_offset(t1);	/* MSYSINT1REG = t3の下位2バイト*/    \
    srl     t4, t3, 16;							      \
    sh      t4, MSYSINT2_offset(t1);	/* MSYSINT2REG = t3の上位2バイト*/    \
    addi    sp, sp, 2*2

/*  割込みコントローラICUのIPMを設定  */
/*      t0に割込み要因番号が入った状態で呼ばれる  */
/*      t0の内容を壊してはいけない  */
/*      t1に割込み要求クリアの定数が入っているので破壊してはならない。  */
#define SET_ICU_IPM							      \
    la      t4, icu_intmask_table;	/* データテーブルの先頭アドレス */    \
    sll     t2, t0, 2;			/* オフセット＝割込み要因番号×4倍 */ \
    li      t3, ICU_BASE_ADDR_asm;					      \
    add     t4, t4, t2;			/* 先頭アドレス＋オフセット */        \
    lw      t5, (t4);			/* t5 = MSYSINT2REG:MSYSINT1REG */    \
					/* 注意：リトルエンディアン依存 */    \
    sh      t5, MSYSINT1_offset(t3);	/* MSYSINT1REG = t5の下位2バイト */   \
    srl     t6, t5, 16;							      \
    sh      t6, MSYSINT2_offset(t3)	/* MSYSINT2REG = t5の上位2バイト */

/*  デバイス名から個別処理を展開するマクロ
      割込み要因を t0 に入れて proc_END に飛ぶ  */
#define MAKE_PROC(device)	\
proc_##device:			\
    li      t0, INTNO_##device;	\
    j       proc_END;		\
    nop;

/*  割込み要因の判別  */
/*    割込みコントローラはMIPS3コアのInt0に接続されているマスクのチェック */
#define PROC_INT0						\
/*  タイマの応答性を上げるため、SYSINT2REGから調べる  */	\
    li      t1, ICU_BASE_ADDR_asm;				\
    lh      t3, SYSINT2_offset(t1);    /* t3 = SYSINT2REG */	\
    lh      t4, MSYSINT2_offset(t1);   /* t4 = MSYSINT2REG */	\
    and     t5, t3, t4;     /*  割込み要求ビットにマスク  */	\
    beq     t5, zero, proc_SYSINT1;				\
    andi    t6, t5, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 );	\
    beq     t6, zero, proc_SYSINT2_HIGH_5BIT;			\
								\
/*  SYSINT2REG (b0:4) の処理  */				\
    andi    t7, t5, ( RTCL2INTR | LEDINTR );			\
    beq     t7, zero, proc_SYSINT2_BIT3_4;			\
    andi    t8, t5, LEDINTR;					\
    beq     t8, zero, proc_LED;					\
MAKE_PROC(RTCL2);						\
MAKE_PROC(LED);							\
								\
proc_SYSINT2_BIT3_4:						\
    andi    t1, t5, TCLKINTR;					\
    beq     t1, zero, proc_FIR;					\
MAKE_PROC(TCLK);						\
MAKE_PROC(FIR);							\
								\
/*  SYSINT2REG (b5:9) の処理  */				\
proc_SYSINT2_HIGH_5BIT:						\
    andi    t7, t5, (DSIUINTR | PCIINTR);			\
    beq     t7, zero, proc_SYSINT2_BIT7_7;			\
    andi    t2, t5, DSIUINTR;					\
    beq     t2, zero, proc_PCI;					\
MAKE_PROC(DSIU);						\
MAKE_PROC(PCI);							\
								\
proc_SYSINT2_BIT7_7:						\
    andi    t3, t5, SCUINTR;					\
    beq     t3, zero, proc_SYSINT2_BIT8_9;			\
MAKE_PROC(SCU);							\
								\
proc_SYSINT2_BIT8_9:						\
    andi    t4, t5, CSIINTR;					\
    beq     t4, zero, proc_BCU;					\
MAKE_PROC(CSI);							\
MAKE_PROC(BCU);							\
								\
/*  SYSINT1REGのチェック  */					\
/*  t1にICU_BASE_ADDRが入った状態でここに来る  */		\
proc_SYSINT1:							\
    lh      t3, SYSINT1_offset(t1);	/* t3 = SYSINT1REG */	\
    lh      t4, MSYSINT1_offset(t2);	/* t4 = MSYSINT1REG */	\
    and     t5, t3, t4;     /*  割込み要求ビットにマスク  */	\
    andi    t6, t5, 0xff;					\
    beq     t6, zero, proc_SYSINT1_HIGH_BYTE;			\
								\
/*  SYSINT1REG (下位バイト) の処理  */				\
    andi    t7, t5, (BATINTR | POWERINTR);			\
    beq     t7, zero, proc_SYSINT1_BIT2_3;			\
    andi    t8, t5, BATINTR;					\
    beq     t8, zero, proc_POWER;				\
MAKE_PROC(BAT);							\
MAKE_PROC(POWER);						\
								\
proc_SYSINT1_BIT2_3:						\
    andi    t1, t5, ETIMERINTR;					\
    beq     t1, zero, proc_ETIME;				\
MAKE_PROC(RTCL1);						\
MAKE_PROC(ETIME);						\
								\
/*  SYSINT1REG (上位バイト) の処理  */				\
proc_SYSINT1_HIGH_BYTE:						\
    andi    t8, t5, (GIUINTR | SIUINTR);			\
    beq     t8, zero, proc_SYSINT1_BIT11_12;			\
    andi    t9, t5, GIUINTR;					\
    beq     t9, zero, proc_SIU;					\
MAKE_PROC(GIU);							\
MAKE_PROC(SIU);							\
								\
proc_SYSINT1_BIT11_12:						\
    andi    t1, t5, SOFTINTR;					\
    beq     t1, zero, proc_CLKRUN;				\
MAKE_PROC(SOFTINT);						\
MAKE_PROC(CLKRUN);						\
								\
proc_END:

/*============================================================================*/

/* 割込み処理に関する、割込み要因の判断分岐処理のシステム依存部 */

	/*  呼び出されたとき、       */
	/*    a1にステータスレジスタ */
	/*    a2に原因レジスタ       */
	/*  の値が入っている         */

#define PROC_INTERRUPT_SYS							\
	and     t2, a2, a1;          /*  割込み要求ビットにマスクをかける  */	\
	andi    t3, t2, Cause_IP0;   /*  IP0ビット取り出し  */			\
	bne     t3, zero, proc_IP0;						\
	andi    t4, t2, Cause_IP1;   /*  IP1ビット取り出し  */			\
	bne     t4, zero, proc_IP1;						\
	andi    t5, t2, Cause_IP2;   /*  IP2ビット取り出し  */			\
	bne     t5, zero, proc_IP2;						\
	andi    t6, t2, Cause_IP3;   /*  IP3ビット取り出し  */			\
	bne     t6, zero, proc_IP3;						\
	andi    t7, t2, Cause_IP4;   /*  IP4ビット取り出し  */			\
	bne     t7, zero, proc_IP4;						\
	nop;									\
	/* VR4131の場合、Cause_IP5とCause_IP6は未接続なので省略 */		\
	/*（ハードウェア編p196参照）*/						\
										\
	/* なんらかの原因で分岐できない場合 */					\
	j       join_interrupt_and_exception;					\
	nop;									\
										\
										\
/*  MIPS3コアレベルで分岐したレベルでの処理		*/			\
/*    割込み要因番号を t0 に入れて			*/			\
/*    割込み要求クリアのための定数を t1 に入れて	*/			\
/*    set_ICU_IPM へ飛ぶ				*/			\
proc_IP7:   /*  割込み要因IP7（タイマ）の場合  */				\
	xori    t1, zero, Cause_IP7;						\
	j       set_ICU_IPM;							\
	ori     t0, zero, INTNO_IP7;						\
										\
proc_IP0:   /*  割込み要因IP0（ソフトウェア割込み0）の場合  */			\
	xori    t1, zero, Cause_IP0;						\
	j       set_ICU_IPM;							\
	ori     t0, zero, INTNO_IP0;						\
										\
proc_IP1:   /*  割込み要因IP1（ソフトウェア割込み1）の場合  */			\
	xori    t1, zero, Cause_IP1;						\
	j       set_ICU_IPM;							\
	ori     t0, zero, INTNO_IP1;						\
										\
proc_IP2:   /*  割込み要因IP2（Int0）の場合  */					\
            /*  「すべての割込み」が通知される。*/				\
	PROC_INT0;	/* 分岐処理は vr4131_icu.h でマクロ定義されている */	\
	xori    t1, zero, Cause_IP2;						\
	j       set_ICU_IPM;							\
	nop;									\
										\
proc_IP3:   /*  割込み要因IP3（Int1）の場合  */ 				\
            /*  「rtc_long1_intr」（インターバルタイマ）が通知される。*/	\
	xori    t1, zero, Cause_IP3;						\
	j       set_ICU_IPM;							\
	ori     t0, zero, INTNO_IP3;						\
										\
proc_IP4:   /*  割込み要因IP4（Int2）の場合  */ 				\
            /*  「rtc_long2_intr」（インターバルタイマ）が通知される。*/	\
	xori    t1, zero, Cause_IP4;						\
	j       set_ICU_IPM;							\
	ori     t0, zero, INTNO_IP4;						\
										\
/*  割込みコントローラ依存のマスク設定  */					\
set_ICU_IPM:									\
										\
	SET_ICU_IPM;	/* 割込みマスクを設定するマクロ			    */	\
			/*   実装を行うときには、下記にてt0、t1は利用するの */	\
			/*   で破壊しないように、注意しなければならない。   */	\
										\
/*  原因レジスタIPビットに保持されている各種割込みの割込み要求をクリアする。	\
    t1には、割込み要求ビットを反転したものが入っている。 */			\
										\
	mfc0    t8, Cause;							\
	and     t8, t8, t1;							\
	mtc0    t8, Cause;							\
										\
/*  ステータスレジスタのマスク設定とC言語ルーチン呼び出し  */			\
/*    t0に割込み要因番号が設定された状態でここに来る  */			\
	la      t3, int_table;   /* 擬似ベクタアドレス */			\
	sll     t4, t0, 3;       /* 割込み要因番号を8倍				\
				      INT_TABLE型は、				\
					ハンドラのアドレス(4バイト)		\
					＋MIPS3コアの割込みマスク(4バイト)	\
				      の、合計8バイト。 */			\
	add     t5, t3, t4;      /* ベクタアドレスを算出 */			\
	lw      t6, INT_TABLE_intmask(t5);					\
           		    	/*  IPM(割込み許可ビット)読み出し。		\
				    割込みマスク以外の値は、			\
					IEビットはセット			\
					EXLビットはリセット			\
				    状態になっている。*/			\
	lw      t7, (t5);        /* C言語ルーチン先頭アドレス読み出し  */	\
										\
	jalr    ra, t7;          /* C言語ルーチン呼び出し  */			\
	mtc0    t6, Status;      /* 割込み許可；ステータスレジスタのマスク設定*/\
										\
	mfc0    t0, Status;							\
	ori     t0, t0, SR_EXL;  /* 割込み禁止；IEビットの値は保持しなければなら\
						ないのでEXLビットを用いる。*/	\
	mtc0    t0, Status;							\
										\
	/*  CP0ハザードのための時間稼ぎ  */					\
	NOP_FOR_CP0_HAZARD;

#endif /* _VR4131_ICU_H_ */
