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

#ifndef _PIC_ICU_H_
#define _PIC_ICU_H_

#ifndef _MACRO_ONLY
#include <sil.h>
#endif /* _MACRO_ONLY */

#include <rte_vr5500_cb.h>	/* ICU_BASE_ADDR */

/*
 *  割込みコントローラ(Programable Interrupt Controler)関係の定義
 */

/*  割込み番号の定義（0-7はmips3.hで使用。8以降を指定する。） */
#define INTNO_TIMER0	 8	/*  タイマ０      */
#define INTNO_SERIAL0	 9	/*  シリアル０    */
#define INTNO_GBUS	10	/*  GBUS-INT0-    */
#define	INTNO_BUS_ERR	11	/*  BUS_ERROR     */
#define	INTNO_TIMER1	12	/*  タイマ１      */
#define INTNO_SERIAL1	13	/*  シリアル１    */
#define INTNO_PARALEL	14	/*  パラレル      */
#define INTNO_DMAC	15	/*  DMAC_INTREQ-  */

/*  割込みコントローラが管理する割込みの本数  */
#define TMAX_ICU_INTNO	8u

/*  割込みコントローラのレジスタのアドレス定義  */
/* 以下のxxx_offsetは、アセンブラで使う。 */
#define INT0M_offset	0x00
#define INT1M_offset	0x10
#define INTR_offset	0x20
#define INTEN_offset	0x30

#define ICU_INT0M	INT0M_offset
#define ICU_INT1M	INT1M_offset
#define ICU_INTR	INTR_offset
#define ICU_INTEN	INTEN_offset

/*  割込み要因ビットパターン (下記、アセンブラ部分でも利用している。) */
#define TIMER0		BIT0
#define SERIAL0		BIT1
#define GBUS		BIT2
#define	BUS_ERR		BIT3
#define	TIMER1		BIT4
#define SERIAL1		BIT5
#define PARALEL		BIT6
#define DMAC		BIT7

/* ICU内のレジスタアクセス用の関数  */
#define icu_reb( addr )		sil_reb_mem( (VP)(ICU_BASE_ADDR + addr) )
#define icu_wrb( addr, val )	sil_wrb_mem( (VP)(ICU_BASE_ADDR + addr), val )

#define icu_orb( mem, val )	icu_wrb( mem, icu_reb( mem ) | val )
#define icu_andb( mem, val )	icu_wrb( mem, icu_reb( mem ) & val )

/*
 *  割込みコントローラの割込みマスク関係
 */

/*  構造体ICU_IPM内のオフセットを求めるためのマクロ（makeoffset.cで用いる）
    なお、このマクロで定義した値は、特に利用していない。*/
#define OFFSET_DEF_ICU_IPM	OFFSET_DEF(ICU_IPM, int1m)

/*  割込みコントローラに設定可能な割込みマスクビットパターン（最高値）*/
#define MAX_ICU_IPM  0xff

/*  割込みコントローラに設定する割込みマスクのチェック  */
#define CHECK_ICU_IPM(ipm)						       \
		CHECK_PAR( 0 < (ipm.int0m) && (ipm.int0m) <= MAX_ICU_IPM );    \
		CHECK_PAR( 0 < (ipm.int1m) && (ipm.int1m) <= MAX_ICU_IPM )

#ifndef _MACRO_ONLY

/*  割込みコントローラに対する割込みマスクの擬似テーブル  */
extern ICU_IPM icu_intmask_table[];

/*  割込みコントローラのintmaskテーブルの設定  */
Inline void icu_set_ilv(INTNO intno, ICU_IPM *ipm) {
	/* CHECK_ICU_IPM(ipm) は、上位ルーチンで実行済み */
	icu_intmask_table[intno].int0m = ipm->int0m;
	icu_intmask_table[intno].int1m = ipm->int1m;
}

/*  割り込みコントローラのマスク設定  */
Inline void icu_set_ipm(ICU_IPM *ipm) {
	/* CHECK_ICU_IPM(ipm) は、上位ルーチンで実行済み */
	icu_wrb( (VP) ICU_INT0M, ipm->int0m );
	icu_wrb( (VP) ICU_INT1M, ipm->int1m );
}

/*  割り込みコントローラのマスク取得  */
Inline void icu_get_ipm(ICU_IPM *ipm) {
	ipm->int0m = icu_reb( (VP) ICU_INT0M );
	ipm->int1m = icu_reb( (VP) ICU_INT1M );
}

#endif /* _MACRO_ONLY */

/*============================================================================*/
/*  アセンブラ処理関係  */

/*  割込み許可ビットの待避と復元  */
/*  割込みコントローラICUのIPMをスタックに保存  */
/*  ワード境界の関係で、本来は1バイトのマスクではあるけれども、
    ワード境界のために2バイト単位で扱う必要がある。 */
#define PUSH_ICU_IPM						\
	li	t1, ICU_BASE_ADDR;				\
	addi	sp, sp, -2*2;					\
	lb	t3, INT0M_offset(t1);	/* t3 = INT0M */	\
	lb	t4, INT1M_offset(t1);	/* t4 = INT1M */	\
	sh	t3, (sp);					\
	sh	t4, 2(sp)

/*  割込みコントローラICUのIPMをスタックから復元  */
#define POP_ICU_IPM						\
	li	t1, ICU_BASE_ADDR;				\
	lh	t3, (sp);					\
	lh	t4, 2(sp);					\
	sb	t3, INT0M_offset(t1);	/* INT0M = t3 */	\
	sb	t4, INT1M_offset(t1);	/* INT1M = t4 */	\
	addi	sp, sp, 2*2

/*  割込みコントローラICUのIPMを設定  */
/*      t0に割込み要因番号が入った状態で呼ばれる  */
/*      t0の内容を壊してはいけない  */
/*      t1に割込み要求クリアの定数が入っているので破壊してはならない。  */
#define SET_ICU_IPM								\
	la	t4, icu_intmask_table;	/*  データテーブルの先頭アドレス  */	\
	sll	t2, t0, 1;		/*  オフセット＝割込み要因番号×2倍	\
						(マスクは、2バイト)  */		\
	li	t3, ICU_BASE_ADDR;						\
	add	t4, t4, t2;		/*  先頭アドレス＋オフセット  */	\
	lh	t5, (t4);		/*  t5 = INT0M:INT1M  */		\
					/*  注意：リトルエンディアン依存  */	\
	sb	t5, INT0M_offset(t3);	/*  INT0M=t5の下位1バイト  */		\
	srl	t6, t5, 8;							\
	sb	t6, INT1M_offset(t3);	/*  INT1M=t5の上位1バイト  */

/*  デバイス名から個別処理を展開するマクロ  */
/*    割込み要因をt0に入れて proc_END に飛ぶ  */
#define MAKE_PROC(device)		\
proc_##device:				\
	li	t0, INTNO_##device;	\
 	j	proc_END;		\
	nop;

/*  割込み要因の判別  */
/*    割込みコントローラはMIPS3コアのInt0に接続されている  */
/*    マスクのチェック*/
#define PROC_INT0				\
	li      t2, ICU_BASE_ADDR;		\
	lb      t3, INTR_offset(t2);		\
	lb	t4, INT0M_offset(t2);		\
	and	t5, t3, t4;		/* INT0M とマスク */	\
	beq	t5, zero, proc_END;		\
	nop;					\
						\
proc_BIT0:					\
	andi	t4, t3, TIMER0;			\
	beq	t4, zero, proc_BIT1;		\
	nop;					\
MAKE_PROC(TIMER0)				\
						\
proc_BIT1:					\
	andi	t4, t3, SERIAL0;		\
	beq	t4, zero, proc_BIT2;		\
	nop;					\
MAKE_PROC(SERIAL0)				\
						\
proc_BIT2:					\
	andi	t4, t3, GBUS;			\
	beq	t4, zero, proc_BIT3;		\
	nop;					\
MAKE_PROC(GBUS)					\
						\
proc_BIT3:					\
	andi	t4, t3, BUS_ERR;		\
	beq	t4, zero, proc_BIT4;		\
	nop;					\
MAKE_PROC(BUS_ERR)				\
						\
proc_BIT4:					\
	andi	t4, t3, TIMER1;			\
	beq	t4, zero, proc_BIT5;		\
	nop;					\
MAKE_PROC(TIMER1)				\
						\
proc_BIT5:					\
	andi	t4, t3, SERIAL1;		\
	beq	t4, zero, proc_BIT6;		\
	nop;					\
MAKE_PROC(SERIAL1)				\
						\
proc_BIT6:					\
	andi	t4, t3, PARALEL;		\
	beq	t4, zero, proc_BIT7;		\
	nop;					\
MAKE_PROC(PARALEL)				\
						\
proc_BIT7:					\
	andi	t4, t3, DMAC;			\
	beq	t4, zero, proc_END;		\
	nop;					\
MAKE_PROC(DMAC)					\
						\
proc_END:

#endif /* _PIC_ICU_H_ */
