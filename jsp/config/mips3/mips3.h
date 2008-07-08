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

#ifndef _MIPS3_H_
#define _MIPS3_H_

#include <util.h>

/*
 *  MIPS3 用定義
 */

/* CPUコアの内部レジスタの名称 */
#define zero    $0      /* 常時ゼロ */
#define at      $1      /* アセンブラのテンポラリ */
#define v0      $2      /* 関数の戻り値 */
#define v1      $3
#define a0      $4      /* 関数の引数 */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8      /* テンポラリレジスタ tx (x=0-9)：関数呼び出しで破壊 */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16     /* sx (x=0-7) : 関数呼び出しで不変なレジスタ */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24     /* テンポラリレジスタ tx (x=0-9)：関数呼び出しで破壊 */
#define t9      $25
#define k0      $26     /* OS用に予約済み */
#define kt0     $26
#define k1      $27
#define kt1     $27
#define gp      $28     /* 大域変数領域のベースアドレス */
#define sp      $29     /* スタックポインタ */
#define fp      $30     /* フレームポインタ */
/* もしくは */
#define s8      $30     /* s8 : 関数呼び出しで不変なレジスタ */
#define ra      $31     /* 関数からの戻りアドレス */

/* CP0の内部レジスタの名称 */
#define Index           $0
#define Random          $1
#define EntryLo0        $2
#define EntryLo1        $3
#define Context         $4
#define PageMask        $5
#define Wired           $6
#define Error           $7
#define BadVAddr        $8
#define Count           $9
#define EntryHi         $10
#define Compare         $11
#define Status          $12
#define Cause           $13
#define EPC             $14
#define PRId            $15
#define Config          $16
#define LLAddr          $17
#define WatchLo         $18
#define WatchHi         $19
#define XContext        $20
/* $21-$24 - 予約 */
#define Performance     $25
#define ParityErr       $26
#define CacheErr        $27
#define TagLo           $28
#define TagHi           $29
#define ErrorEPC        $30
/* $31 - 予約 */

/*  インラインアセンブラ内で使う場合の定義  */
#define str_k0		"$26"
#define str_Status	"$12"

/*  ステータスレジスタ関係  */
#define SR_IE		BIT0			/*  IEビット  */
#define SR_EXL		BIT1			/*  EXLビット  */
#define SR_ERL		BIT2			/*  ERLビット  */

#define SR_EXL_IE	(SR_EXL | SR_IE)	/*  EXL,IEビット  */
#define SR_ERL_EXL	(SR_ERL | SR_EXL)	/*  ERL,EXLビット  */
#define SR_ERL_EXL_IE	(SR_ERL_EXL | SR_IE)	/*  ERL,EXL,IEビット  */
#define SR_IM		0xff00			/*  IMビットを取り出すマスク  */

#define SR_UX		BIT5			/*  UXビット  */
#define SR_SX		BIT6			/*  SXビット  */
#define SR_KX		BIT7			/*  SXビット  */
#define SR_DE		BIT16			/*  DEビット  */
#define SR_SR		BIT20			/*  SRビット  */
#define SR_BEV		BIT22			/*  BEVビット  */
#define SR_RE		BIT25			/*  REビット  */
#define SR_CU0		BIT28			/*  CU0ビット  */
#define SR_CU1		BIT29			/*  CU1ビット  */
#define SR_CU2		BIT30			/*  CU2ビット  */
#define SR_XX		BIT31			/*  XXビット  */

/*  原因レジスタ関係  */
#define Cause_IP0       BIT8    /*  IP0ビット  */
#define Cause_IP1       BIT9    /*  IP1ビット  */
#define Cause_IP2       BIT10   /*  IP2ビット  */
#define Cause_IP3       BIT11   /*  IP3ビット  */
#define Cause_IP4       BIT12   /*  IP4ビット  */
#define Cause_IP5       BIT13   /*  IP5ビット  */
#define Cause_IP6       BIT14   /*  IP6ビット  */
#define Cause_IP7       BIT15   /*  IP7ビット  */

/*  外部割り込みに対するIPビットの別名  */
#define Cause_Int0	Cause_IP2  /*  Int0要求ビット  */
#define Cause_Int1	Cause_IP3  /*  Int1要求ビット  */
#define Cause_Int2	Cause_IP4  /*  Int2要求ビット  */
#define Cause_Int3	Cause_IP5  /*  Int3要求ビット  */
#define Cause_Int4	Cause_IP6  /*  Int4要求ビット  */

/*  原因レジスタから原因コードを取り出すマスク  */
#define ExcCode_mask    0x7c

/* 例外コード */
#define Int     0       /* 割り込み例外 */
#define Mod     1       /* TLB変更例外 */
#define TLBL    2       /* TLB不一致例外 (ロードまたは命令フェッチ) */
#define TLBS    3       /* TLB不一致例外 (ストア) */
#define AdEL    4       /* アドレスエラー例外 (ロードまたは命令フェッチ) */
#define AdES    5       /* アドレスエラー例外 (ストア) */
#define IBE     6       /* バスエラー例外 (命令フェッチ) */
#define DBE     7       /* バスエラー例外 (データのロードまたはストア) */
#define Sys     8       /* システムコール例外 */
#define Bp      9       /* ブレイクポイント例外 */
#define RI      10      /* 予約命令例外 */
#define CpU     11      /* コプロセッサ使用不可例外 */
#define Ov      12      /* 演算オーバーフロー例外 */
#define Tr      13      /* トラップ例外 */
/* 14 - 予約 */
#define FPE     15      /* 浮動小数点例外 */
/* 16-22 - 予約 */
#define WATCH   23      /* ウォッチ例外 */
/* 24-31 - 予約 */

/*  割込み要因番号  */
#define INTNO_IP0               0
#define INTNO_IP1               1
#define INTNO_IP2               2
#define INTNO_IP3               3
#define INTNO_IP4               4
#define INTNO_IP5               5
#define INTNO_IP6               6
#define INTNO_IP7               7

/*  コアの割込みの本数（ソフトウェア割込みを含む） */
#define TMAX_CORE_INTNO		8

/*
 *  MIPS3コアのステータスレジスタに設定可能な最高割込み許可ビットパターン
 */
#define MAX_IPM  0xff

/*
 *  MIPS3コアに設定する割込みマスクのチェック
 */
#define CHECK_CORE_IPM(ipm)  CHECK_PAR(0 <= (ipm) && (ipm) <= MAX_IPM)

/*
 *  CPU例外の種類数
 */
#define TMAX_CORE_EXCNO	32u

/*
 *  ログ出力時のスタック構造の定義
 *    cpu_config.hにおけるcpu_experr関数、cpu_support.Sを参照のこと。
 */
#ifndef _MACRO_ONLY

typedef struct exc_stack {

	/*  レジスタ群の定義  */
	UW	sp;
	UW	at;
	UW	v0;
	UW	v1;
	UW	a0;
	UW	a1;
	UW	a2;
	UW	a3;
	UW	t0;
	UW	t1;
	UW	t2;
	UW	t3;
	UW	t4;
	UW	t5;
	UW	t6;
	UW	t7;
	UW	t8;
	UW	t9;
	UW	gp;
	UW	fp;
	UW	ra;

	UW	CP0_Status;
	UW	CP0_EPC;

	UW	hi;
	UW	lo;

} EXCSTACK;

#endif /* _MACRO_ONLY */

#endif /* _MIPS3_H_ */
