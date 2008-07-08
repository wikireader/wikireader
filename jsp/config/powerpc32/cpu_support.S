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
 *  @(#) $Id: cpu_support.S,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *  プロセッサ依存モジュール アセンブリ言語部（PowerPC用）
 *　　　　　カーネル内部で使用する定義
 */

#define	_MACRO_ONLY
#include "jsp_kernel.h"
#include "offset.h"

/*
 *  TCB構造体のオフセットのチェック
 *　　　15ビットに収まっていないと符号拡張が起こり、
 *　　　期待する動作にならない。
 */
#define CHECK_OFFSET(offset)	(((offset)&~0x8fff) != 0)
#if CHECK_OFFSET(TCB_pc) || CHECK_OFFSET(TCB_sp) 		\
    || CHECK_OFFSET(TCB_enatex) || CHECK_OFFSET(TCB_texptn) 	\
    || CHECK_OFFSET(TCB_tinib) || CHECK_OFFSET(TINIB_task) 	\
    || CHECK_OFFSET(TINIB_exinf) 
	ここでコンパイルエラー
#endif


/*
 *　例外処理の前半で退避するレジスタ数
 *　　　汎用レジスタ：gpr0-1,3-12
 *　　　特殊レジスタ：LR,CTR,XER,SRR0,SRR1
 */
#define NUM_REG			17

/*  レジスタを待避するのに必要なバイト数  */
#define GPR0_12_SPRG_AREA_SIZE  (NUM_REG*4 + 4)  
		/*  +4は例外要因番号を保存するスペース  */
		/*  注意：4バイト境界になっている  */

#define GPR13_31_AREA_SIZE      (19*4)  /*  gpr13-31  */

    /*
     *　注意
     *　　　MPC860は浮動小数点レジスタがないため、この部分は
     *　　　テストされていない
     */
#ifdef SUPPORT_FLOATING_POINT_REG
/*  ハードウェアで浮動小数点レジスタが実装されている場合  */
#define FPR_AREA_SIZE       (32*8+8)    /*  fpr0-31,FPSCR  */
#define FPR0_13_AREA_SIZE   (14*8+8)    /*  fpr0-14,FPSCR  */
#define FPR14_31_AREA_SIZE  (18*8)      /*  fpr15-31  */

#else   /*  SUPPORT_FLOATING_POINT_REG  */
/*  ハードウェアで浮動小数点レジスタが実装されていない場合  */
#define FPR_AREA_SIZE       0           /*  fpr0-31,FPSCR  */
#define FPR0_13_AREA_SIZE   0           /*  fpr0-14,FPSCR  */
#define FPR14_31_AREA_SIZE  0           /*  fpr15-31  */
#endif  /* SUPPORT_FLOATING_POINT_REG */    

/*
 *  割込み、CPU例外受付時のレジスタの待避
 *　　注意
 *　　　浮動小数点レジスタの待避はディスパッチ呼び出しがあるとき
 *　　　（タスクスイッチの可能性があるとき）のみ行う。
 *　　　割込みハンドラやCPU例外ハンドラ内で浮動小数点レジスタを
 *　　　使用する場合は、各ハンドラ内で浮動小数点レジスタの待避／
 *　　　復元を行うこと。
 *　　　
 *　　　r4のSRR1はCPU例外発生時のCPUロック状態判別にも使用する。
 *　　　
 */
#define SAVE_GPR0_12_SPRG                   \
    subi    sp, sp, GPR0_12_SPRG_AREA_SIZE; \
    stw     r0, 0*4(sp);                    \
    stw     r3, 1*4(sp);                    \
    stw     r4, 2*4(sp);                    \
    stw     r5, 3*4(sp);                    \
    stw     r6, 4*4(sp);                    \
    stw     r7, 5*4(sp);                    \
    stw     r8, 6*4(sp);                    \
    stw     r9, 7*4(sp);                    \
    stw     r10, 8*4(sp);                   \
    stw     r11, 9*4(sp);                   \
    stw     r12,10*4(sp);                   \
    mfspr   r3, SRR0;                       \
    stw     r3, 11*4(sp);                   \
    mfspr   r4, SRR1;                       \
    stw     r4, 12*4(sp);                   \
    mfspr   r5, LR;                         \
    stw     r5, 13*4(sp);                   \
    mfspr   r6, CTR;                        \
    stw     r6, 14*4(sp);                   \
    mfcr    r7;             /*  CR  */      \
    stw     r7, 15*4(sp);                   \
    mfspr   r8, XER;                        \
    stw     r8, 16*4(sp)


/*  
 *  割込み、CPU例外受付時のレジスタの復元
 *  　　（gpr0-1,3-12, 特殊レジスタの復元）
 *  
 *  　　ディスパッチ呼び出しがない場合
 */
#define LOAD_R0_12_SPRG                         \
    /*  特殊レジスタの復元  */                  \
    lwz     r3, 11*4(sp);                       \
    mtspr   SRR0, r3;                           \
    lwz     r4, 12*4(sp);                       \
    mtspr   SRR1, r4;                           \
    lwz     r5, 13*4(sp);                       \
    mtspr   LR, r5;                             \
    lwz     r6, 14*4(sp);                       \
    mtspr   CTR, r6;                            \
    lwz     r7, 15*4(sp);                       \
    mtcr    r7;             /*  CR  */          \
    lwz     r8, 16*4(sp);                       \
    mtspr   XER, r8;                            \
    /*  汎用レジスタgpr0-1,3-12の復元  */       \
    lwz     r0, 0*4(sp);                        \
    lwz     r3, 1*4(sp);                        \
    lwz     r4, 2*4(sp);                        \
    lwz     r5, 3*4(sp);                        \
    lwz     r6, 4*4(sp);                        \
    lwz     r7, 5*4(sp);                        \
    lwz     r8, 6*4(sp);                        \
    lwz     r9, 7*4(sp);                        \
    lwz     r10, 8*4(sp);                       \
    lwz     r11, 9*4(sp);                       \
    lwz     r12, 10*4(sp)
    /*  マクロ定義ここまで  */


/*
 *  CPU例外の入口処理の内、各例外ベクタに配置する前半部分
 *  
 *  　マクロ引数
 *  　　exc_no：例外番号
 *  　
 *  　レジスタ割り当て
 *  　　SPRG2：exc_table（CPU例外擬似ベクタテーブルの先頭アドレス）
 *  　　CTR：C言語ルーチンの先頭アドレス
 *  　　r3：CPU例外ハンドラの引数
 *  　　r4：SRR1のコピー
 */
#define MAKE_EXCEPTION_ENTRY(exe_no)                            \
    SAVE_GPR0_12_SPRG;  /*  スクラッチレジスタの待避  */        \
                        /*  （r4にSRR1がコピーされる）  */      \
    mr  r3, sp;         /*  CPU例外ハンドラの引数設定  */       \
    /*  C言語ルーチンの先頭アドレス設定  */                     \
    mfspr   r5, SPRG2;  /*  r5←SPRG2：exc_table  */            \
                        /*  擬似ベクタテーブル読み出し  */      \
    lwz     r6, (exe_no<<2)(r5);                                \
    li      r7, exe_no; /*  例外要因番号の保存  */              \
    stw     r7, NUM_REG*4(sp)    /*  (cpu_experr()で使用)  */

/*
 *  CPU例外の入口処理の内、各例外ベクタに配置されない
 *  前半部分への分岐
 *  
 *  例外ベクタの配置によってはb命令による分岐（前後0x07ff,ffff）で
 *  届かない場合がある。この場合は絶対番地を指定して分岐する。
 */
#ifdef ABSOLUTE_JUMP_EXC_ENTRY
#define JUMP_EXC_ENTRY(label)			\
		LI32(r8, label);		\
		mtctr r8;			\
		bctr

#else	/*  ABSOLUTE_JUMP_EXE_ENTRY  */
#define JUMP_EXC_ENTRY(label)	b label
#endif	/*  ABSOLUTE_JUMP_EXE_ENTRY  */

/*  
 *   例外ベクタの定義
 */   

#ifndef IBM_PPC_EMB_ENV
/*  
 *   オリジナルのPowerPCアーキテクチャの場合
 *   　　モトローラMPCシリーズ、IPM PowerPC6xx/7xxシリーズは
 *   　　こちらに該当する。
 */   

/*
 *  例外ベクタテーブルの先頭アドレス
 *  　　マシンステータスレジスタMSRのIPビットが
 *  　　　　0のとき：0x0000,0000番地
 *  　　　　1のとき：0xfff0,0000番地
 * 　　　からのオフセットになる
 */
#define EXCEPTION_VECTOR_BASE   0x100

/*
 *  システムリセット例外
 */
    .section ".exception_vector","rxai"
    .align 2
    .org 0x100 - EXCEPTION_VECTOR_BASE

System_reset_exception:
    JUMP_EXC_ENTRY(start)               /*  スタートアップルーチン  */

/*
 *  マシン・チェック例外
 */
    .org 0x200 - EXCEPTION_VECTOR_BASE

Machine_check_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_MACHINE_CHECK)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  DSI例外
 *  　データ・メモリ・アクセス
 *  　データTLBエラー／ミス
 */
    .org 0x300 - EXCEPTION_VECTOR_BASE

DSI_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_DSI)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  ISI例外
 *  　命令フェッチ
 *  　命令TLBエラー／ミス
 */
    .org 0x400 - EXCEPTION_VECTOR_BASE

ISI_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_ISI)
    JUMP_EXC_ENTRY(Exception_Entry)


/*
 *  外部割込み
 *  　すべての外部割り込みで同じ番地に分岐する
 */
    .org 0x500 - EXCEPTION_VECTOR_BASE

External_interrupt:
    SAVE_GPR0_12_SPRG           /*  レジスタの待避  */
    /*
     *  本当はここで続きの処理を行いたいが、0x100バイト（64命令）後ろに
     *  アライメント例外の処理が入るため、続きはExternal_interrupt_1:で行
     *  う。
     */
    JUMP_EXC_ENTRY(External_interrupt_1)

/*
 *  アライメント例外
 */
    .org 0x600 - EXCEPTION_VECTOR_BASE

Alignment_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_ALIGNMENT)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  プログラム例外
 *  　　Floating point enabled
 *  　　Illegal instruction
 *  　　Privileged instruction
 *  　　Trap instruction
 */
    .org 0x700 - EXCEPTION_VECTOR_BASE

Program_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_PROGRAM)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  浮動小数点使用不可
 */
    .org 0x800 - EXCEPTION_VECTOR_BASE

Floating_point_unavailable:
    MAKE_EXCEPTION_ENTRY(EXC_NO_FLOATING_POINT_UNAVAILABLE)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  デクリメンタ例外
 */
    .org 0x900 - EXCEPTION_VECTOR_BASE

Decrementer_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_DECREMENTER)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  インプリメンテーション固有の例外0x00a00
 */
#ifdef IMPLEMENT_EXCEPTION_00A00
    .org 0xa00 - EXCEPTION_VECTOR_BASE

Implementation_exception_00a00:
    MAKE_EXCEPTION_ENTRY(EXC_NO_IMPLEMENT_EXCEPTION_00A00)
    JUMP_EXC_ENTRY(Exception_Entry)

#endif /* IMPLEMENT_EXCEPTION_00A00 */


/*
 *  システムコール
 */
    .org 0xc00 - EXCEPTION_VECTOR_BASE

System_call:
    MAKE_EXCEPTION_ENTRY(EXC_NO_SYSTEM_CALL)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  トレース（オプション）
 */
    .org 0xd00 - EXCEPTION_VECTOR_BASE

Trace_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_TRACE)
    JUMP_EXC_ENTRY(Exception_Entry)


/*
 *  浮動小数点補助
 */
    .org 0xe00 - EXCEPTION_VECTOR_BASE

Floating_point_assist:
    MAKE_EXCEPTION_ENTRY(EXC_NO_FLOATING_POINT_ASSIST)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  インプリメンテーション専用の処理 0x01000-0x2ffff
 *    インプリメンテーション固有の例外ベクタ（１つとは限らない）や
 *    他の用途に使用される
 */
#ifdef IMPLEMENT_EXCEPTION_01000_PROC
    .org 0x1000 - EXCEPTION_VECTOR_BASE

    IMPLEMENT_EXCEPTION_01000_PROC
    
#endif /* IMPLEMENT_EXCEPTION_01000_PROC */

#else	/*  IBM_PPC_EMB_ENV  */

/*  
 *   The IBM PowerPC Embedded Environmentの場合
 *   　IBM系PowerPC40xシリーズ対応
 */   

/*
 *  例外ベクタテーブルの先頭アドレス
 *   　 The IBM PowerPC Embedded Environmentの場合
 *  　　　・例外ベクタの先頭アドレスはEVPRレジスタで設定する
 *  　　　・マシンステータスレジスタMSRのIPビットがない
 */
#define EXCEPTION_VECTOR_BASE   0x100

/*
 *  Critical Interrupt
 *  　オフセット：0x100
 *  　Critical Interruptはインプリメンテーション依存なので
 *  　処理内容はマクロ定義しておく。
 *  　　マクロ名：CRITICAL_INTERRUPT_EXCEPTION_PROC
 *  　この処理内容は0x100バイトを超えてはいけない。
 *  　（超える場合は残りの部分を別の場所に配置して、
 *  　　そこに分岐すること）
 *
 *  　Critical Interruptはカーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 */
    .section ".exception_vector","rxai"
    .align 2
    .org 0x100 - EXCEPTION_VECTOR_BASE

Critical_Interrupt_exception:
    CRITICAL_INTERRUPT_EXCEPTION_PROC
    /*  注意：リターン命令はrfci(Return From Critical Interrupt)  */

/*
 *  マシン・チェック例外
 *  　この処理内容は0x100バイトを超えてはいけない。
 *  　（超える場合は残りの部分を別の場所に配置して、
 *  　　そこに分岐すること）
 *
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 */
    .org 0x200 - EXCEPTION_VECTOR_BASE

Machine_check_exception:
    MACHINE_CHECK_PROC

/*
 *  Data storage例外
 *  　データ・メモリ・アクセス
 */
    .org 0x300 - EXCEPTION_VECTOR_BASE

DSI_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_DATA_STORAGE)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  Instruction storage例外
 *  　命令フェッチ
 */
    .org 0x400 - EXCEPTION_VECTOR_BASE

ISI_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_INSTRUCTION_STORAGE)
    JUMP_EXC_ENTRY(Exception_Entry)


/*
 *  外部割込み
 *  　すべての外部割り込みで同じ番地に分岐する
 */
    .org 0x500 - EXCEPTION_VECTOR_BASE

External_interrupt:
    SAVE_GPR0_12_SPRG           /*  レジスタの待避  */
    /*
     *  本当はここで続きの処理を行いたいが、0x100バイト（64命令）後ろに
     *  アライメント例外の処理が入るため、続きはExternal_interrupt_1:で行
     *  う。
     */
    JUMP_EXC_ENTRY(External_interrupt_1)

/*
 *  アライメント例外
 */
    .org 0x600 - EXCEPTION_VECTOR_BASE

Alignment_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_ALIGNMENT)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  プログラム例外
 *  　　Illegal instruction
 *  　　Privileged instruction
 *  　　Trap instruction
 *  　　Floating point enabled
 *  　　Floating point unimplemented
 *  　　Auxiliary processor unavailable
 *  　　Auxiliary processor enabled
 */
    .org 0x700 - EXCEPTION_VECTOR_BASE

Program_exception:
    MAKE_EXCEPTION_ENTRY(EXC_NO_PROGRAM)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  浮動小数点使用不可
 */
#ifdef SUPPORT_IBM_PPC_EMB_APU	/*  APUを持つ機種は利用可能  */
    .org 0x800 - EXCEPTION_VECTOR_BASE

Floating_point_unavailable:
    MAKE_EXCEPTION_ENTRY(EXC_NO_FLOATING_POINT_UNAVAILABLE)
    JUMP_EXC_ENTRY(Exception_Entry)

#endif /* SUPPORT_IBM_PPC_EMB_APU */


/*
 *  システムコール
 */
    .org 0xc00 - EXCEPTION_VECTOR_BASE

System_call:
    MAKE_EXCEPTION_ENTRY(EXC_NO_SYSTEM_CALL)
    JUMP_EXC_ENTRY(Exception_Entry)

/*
 *  インプリメンテーション専用の処理（オフセット 0xd00-0xff0）
 *    インプリメンテーション固有の例外ベクタ
 *    
 * 　 オフセットの指定方法に注意すること。
 *  　具体的には
 *     .org オフセット - EXCEPTION_VECTOR_BASE
 * 　 のように指定する。詳しくは他の例外要因の該当個所を参照。
 *    例えば、PowerPC405ではオフセット0xf20に
 *    APU Unavailable例外の処理を配置する
 */
#ifdef IMPLEMENT_EXCEPTION_D00_PROC

    IMPLEMENT_EXCEPTION_D00_PROC
    
#endif /* IMPLEMENT_EXCEPTION_D00_PROC */

/*
 *  Programmable Interval timer
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
    .org 0x1000 - EXCEPTION_VECTOR_BASE

Programmable_Interval_timer:
	PROGRAMMABLE_INTERVAL_TIMER_PROC

/*
 *  Fixed Interval timer
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
    .org 0x1010 - EXCEPTION_VECTOR_BASE

Fixed_Interval_timer:
	FIXED_INTERVAL_TIMER_PROC

/*
 *  Watchdog timer
 *  　オフセット：0x1020
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 *
 *  　0x10バイトしか領域が割り当てられていないので注意
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 */
    .org 0x1020 - EXCEPTION_VECTOR_BASE

Watchdog_timer:
	WATCHDOG_TIMER_PROC

/*
 *  Data TLB miss
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
    .org 0x1100 - EXCEPTION_VECTOR_BASE

Data_TLB_miss:
	DATA_TLB_MISS_PROC

/*
 *  Instruction TLB miss
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
    .org 0x1200 - EXCEPTION_VECTOR_BASE

Instruction_TLB_miss:
	INSTRUCTION_TLB_MISS_PROC

/*
 *  Debug例外
 *  　オフセット：0x2000
 *
 *　　例外クラス：Critical
 *  　　リターン命令はrfci(Return From Critical Interrupt)
 *  　　を用いる。
 *  　
 *  　要因は以下の7種類
 *  　　Trap
 *  　　Instruction address compare
 *  　　Data address compare
 *  　　Instruction complete
 *  　　Branch taken
 *  　　Exception
 *  　　Unconditional debug event
 *  　
 *  　カーネル管理外の例外とする。
 *  　（この処理ルーチン内でサービスコールを使用しない。）
 *  　
 *  　0x10バイトしか領域が割り当てられていないので注意
 */
    .org 0x2000 - EXCEPTION_VECTOR_BASE

Debug_exception:
	DEBUG_PROC


/*
 *  インプリメンテーション専用の処理（オフセット 0x2010-0x2ff0）
 *    インプリメンテーション固有の例外ベクタ（１つとは限らない）や
 *    他の用途に使用される
 *    
 * 　 オフセットの指定方法に注意すること。
 *  　具体的には
 *     .org オフセット - EXCEPTION_VECTOR_BASE
 * 　 のように指定する。詳しくは他の例外要因の該当個所を参照。
 */
#ifdef IMPLEMENT_EXCEPTION_02010_PROC

    IMPLEMENT_EXCEPTION_02010_PROC
    
#endif /* IMPLEMENT_EXCEPTION_01000 */


/*
 *  System reset
 *    リセットベクタは0xffff,fffc番地に固定されている
 */
    .section ".system_reset_vector","rxai"
    .align 2
    
/*  この行を(0xffff,fffc - 4バイト×4命令)番地に配置する。  */
jump_to_start:    
    LI32(r8, start)
    mtctr	r8
    bctrl

/*  この行を0xffff,fffc番地に配置する。  */
reset_vector:
    b		jump_to_start
    /*  ここには1命令しか入らないので相対分岐命令を置き、  */
    /*  startルーチンへの分岐は上記のjump_to_startで行う。  */


#endif	/*  IBM_PPC_EMB_ENV  */




/*
 *  CPU例外入口処理ルーチンの続き
 *　　例外要因によらず、共通の処理
 *　　各例外ベクタから合流して、残りの処理をここで行う。
 *　　　・プロセッサコアの汎用レジスタと特殊レジスタの待避
 *　　　・C言語ルーチンの
 *      　　・先頭アドレスの設定
 *      　　・引数の設定
 *　　が済んだ状態でここに来る
 *
 *  　レジスタ割り当て（すべて設定済みの状態でここに来る）
 *  　　SPRG0：割込み／例外のネストカウンタ
 *  　　SPRG2：exc_table（CPU例外擬似ベクタテーブルの先頭アドレス）
 *  　　r3：CPU例外ハンドラの引数
 *  　　r4：SRR1のコピー
 *  　　r6：C言語ルーチンの先頭アドレス
 *
 *  regflg をチェックする前に割り込みを禁止しないと、reqflg をチェック
 *  後に起動された割り込みハンドラ内でディスパッチが要求された場合に、
 *  ディスパッチされない。
 *  
 *  例外ベクタからtextセクションにPC相対分岐で届かなくなったときのため、
 *  Exception_Entryをグローバル宣言している。
 *
 */

    .text
    .align 2
#ifdef ABSOLUTE_JUMP_EXC_ENTRY
    .global Exception_Entry
#endif	/*  ABSOLUTE_JUMP_EXC_ENTRY  */
    
Exception_Entry:
    mtctr   r6;         /*  CTR←C言語ルーチンの先頭アドレス */
    /*  割込みネストカウンタのインクリメント  */
    mfspr   r8, SPRG0   /*  SPRG0:割込み／例外ネストカウンタ  */
    addi    r8, r8, 1
    mtspr   SPRG0, r8

    /*  CPU例外発生時のコンテキストの判別  */
    cmpwi   crf0, r8, 1
    bne     exception_from_int

    /*  初段のCPU例外の場合  */
exception_from_task:
    /*
     *  コーリング・コンベンションに合わせて、スタックポインタを
     *　8バイトずらしておく。
     *　
     *         -8  ----------------------
     *            |      task sp         |
     *         -4  ----------------------　
     *            |                      |←C言語ルーチンの呼び出しにより
     * STACKTOP→  ----------------------   書き込まれる
     *  
     */
    LI32(r9, STACKTOP-STACK_MARGIN)
    stw     sp, 0(r9)     /*  タスクスタックポインタの保存  */
    mr      sp, r9        /*  スタック切り替え  */


/*
 *  マクロ定義：CPU例外ハンドラの呼出し
 *  　　　　　　初段の例外と多重例外の共通処理
 *  
 *  処理内容
 *  　C言語ルーチン呼び出し
 *  
 *  引数
 *  　  label：ラベル識別用文字列
 *  　  　　　　（複数箇所でこのマクロを使用できるようにするため）
 *  
 *  前提条件
 *  　 r3=C言語ルーチンの引数
 *  　 r4=SRR1
 *  　 CTR=C言語ルーチンの先頭アドレス
 *  　　の状態で使用される
 *  
 *  備考
 *  　CPU例外ハンドラの起動により、CPUロック状態を変更してはいけない
 *  　つまり、CPU例外発生時にCPUロック状態であれば、CPUロック状態の
 *  　まま、CPU例外ハンドラを起動する。
 *  　（CPU例外発生時にCPUロック解除状態であれば、CPUロック解除状態の
 *  　　まま、CPU例外ハンドラを起動する。）
 *  　　
 */
#define _CALL_EXCEPTION_ROUTINE(label)                                  \
    andi.   r6, r4, MSR_EE;   /*  r4：SSR1のコピー  */                  \
    /* 結果がゼロならば（CPUロック状態だったなら） */                   \
    /*  EQフラグがセットされる（CPUロック解除状態なら分岐）  */         \
    bne     exception_from_cpu_unlock_##label;                          \
                                                                        \
    /*  CPU例外発生時にCPUロック状態だった場合  */                      \
    /*  CPUロック状態のまま、CPU例外ハンドラを呼び出す  */              \
    /*  　C言語ルーチンからの戻り番地を設定  */                         \
    LI32(r7, exit_CALL_EXCEPTION_ROUTINE_##label);                      \
    mtspr   LR, r7;                                                     \
    bctr;               /*  C言語ルーチン呼び出し  */                   \
    /* exit_CALL_EXCEPTION_ROUTINE_##label番地に戻ってくる   */         \
                                                                        \
    /*  CPU例外発生時にCPUロック解除状態だった場合  */                  \
exception_from_cpu_unlock_##label:;                                     \
    mfmsr   r10;                                                        \
    ori     r11, r10, MSR_EE;   /*  EEビットをセット  */                \
    mtmsr   r11;                /*  割込み許可  */                      \
    bctrl;                      /*  C言語ルーチン呼び出し  */           \
    mfmsr   r10;                                                        \
    xori    r11, r10, MSR_EE;   /*  EEビットをクリア  */                \
    mtmsr   r11;                /*  割込み禁止 (a)  */                  \
exit_CALL_EXCEPTION_ROUTINE_##label:     /*  マクロの出口ラベル  */  
    

/* マクロ引数labelを確実にマクロ展開するため、マクロを2重にしている */
#define CALL_EXCEPTION_ROUTINE(label)   _CALL_EXCEPTION_ROUTINE(label)
    
    /*  C言語ルーチン呼び出し  */
    CALL_EXCEPTION_ROUTINE(exception_from_task)
    /*
     *  割込み禁止で戻ってくる
     *　　例外発生時にCPUロックが解除されていた場合も
     *　　上記(a)行で割込み禁止にしている
     */
    li      r0, 0
    mtspr   SPRG0, r0           /*  割込みネストカウンタをクリア  */
    lwz     sp, 0(sp)           /*  スタック切替  */
    lis     r3, reqflg@ha
    lwz     r4, reqflg@l(r3)    /*  r4=reqflg  */
    cmpwi   crf0, r4, 0         /*  reqflgのチェック  */
    beq     ret_to_task_exc /*  ディスパッチ要求がない場合は分岐  */        
    stw     r0, reqflg@l(r3)    /*  reqflgのクリア  */
    b       ret_exc             /*  出口処理へジャンプ  */
    
/*  多重例外の場合  */
exception_from_int: 
    /*
     *  コーリング・コンベンションに合わせて、スタックポインタを
     *　8バイトずらしておく
     *　
     *         -8  ----------------------
     *            |                      |
     *         -4  ----------------------　
     *            |                      |←C言語ルーチンの呼び出しにより
     *       sp→  ----------------------   書き込まれる
     *  
     */
    subi    sp, sp, STACK_MARGIN

    /*  C言語ルーチン呼び出し  */
    CALL_EXCEPTION_ROUTINE(exception_from_int)
    /*
     *  割込み禁止で戻ってくる
     *　　例外発生時にCPUロックが解除されていた場合も
     *　　上記(a)行で割込み禁止にしている
     *  　
     *  　分岐先ret_to_task_excではCPU例外からのリターン命令rfiにより
     *  　　　MSR←SRR1
     *  　となるので、元のコンテキストに戻るときにはCPU例外発生
     *  　直前のCPUロック／ロック解除状態が復元される
     */
    			/*  スタックポインタを元に戻す（上記参照） */
    addi    sp, sp, STACK_MARGIN
    mfspr   r3, SPRG0   /*  SPRG0:割込み／例外ネストカウンタ  */
    subi    r3, r3, 1   /*  割込みネストカウンタのディクリメント  */
    mtspr   SPRG0, r3
    b       ret_to_task_exc



/*
 *  割込み出入口処理ルーチンの続き
 *　　割込みベクタが0x100バイト（64命令分）しかないので、
 *　　残りの処理をここで行う。
 *　　割込みコントローラ依存部が単純であれば64命令に収まる
 *　　かも知れないが、可読性を考慮して、ここに記述した。
 *　　プロセッサコアの汎用レジスタと特殊レジスタの待避が
 *　　済んだ状態でここに来る
 *
 *  　すべての外部割り込みで同じ番地に分岐する
 *
 *  SPRG0レジスタを割込み／例外のネストカウンタとして使用する。
 *
 *  regflg をチェックする前に割り込みを禁止しないと、reqflg をチェック
 *  後に起動された割り込みハンドラ内でディスパッチが要求された場合に、
 *  ディスパッチされない。
 *  
 *  例外ベクタからtextセクションにPC相対分岐で届かなくなったときのため、
 *  External_interrupt_1をグローバル宣言している。
 *
 */

    .text
    .align 2
#ifdef ABSOLUTE_JUMP_EXC_ENTRY
    .global External_interrupt_1
#endif	/*  ABSOLUTE_JUMP_EXC_ENTRY  */

External_interrupt_1:
    /*  プロセッサコアのレジスタの待避は済んでいる  */
    
    PUSH_ICU_IPM    /*  割込みコントローラICUのIPMを待避  */
    			/*  IPMのデータサイズの如何によらず、 */
    			/*  スタックポインタは4バイト境界を  */
    			/*  守ること  */

    /*  割込みネストカウンタのインクリメント  */
    mfspr   r3, SPRG0   /*  SPRG0:割込み／例外ネストカウンタ  */
    addi    r3, r3, 1
    mtspr   SPRG0, r3

    /*  割込み元のコンテキストの判別  */
    cmpwi   crf0, r3, 1
    bne     interrupt_from_int

    /*  初段の割込みの場合  */
interrupt_from_task:
    /*
     *  コーリング・コンベンションに合わせて、スタックポインタを
     *　8バイトずらしておく。
     *　
     *         -8  ----------------------
     *            |      task sp         |
     *         -4  ----------------------　
     *            |                      |←C言語ルーチンの呼び出しにより
     * STACKTOP→  ----------------------   書き込まれる
     *  
     */
    LI32(r4, STACKTOP-STACK_MARGIN)  /*  r4=STACKTOP  */
    stw     sp, 0(r4)     /*  タスクスタックポインタの保存  */
    mr      sp, r4        /*  スタック切り替え  */

    /*
     *  ICU依存の割込み処理 
     * 　 処理内容
     *  　　・割込み要因の判別
     *  　　・割込みマスクの設定
     *  　　・割込み許可
     *  　　・C言語ルーチン呼び出し
     *  　　・割込み禁止
     *  　　・割り込み要求フラグのクリア（必要であれば）
     */
    PROC_ICU(FROM_TASK)
    /*  割込み禁止で戻ってくる  */
    
    li      r0, 0
    mtspr   SPRG0, r0           /*  割込みネストカウンタをクリア  */
    lwz     sp, 0(sp)           /*  スタック切替  */
    POP_ICU_IPM                 /*  割込みコントローラICUのIPMを復帰  */
    			        /*  IPMのデータサイズの如何によらず、 */
    			        /*  スタックポインタは4バイト境界を  */
    			        /*  守ること  */
    lis     r3, reqflg@ha   
    lwz     r4, reqflg@l(r3)    /*  r4=reqflg  */
    cmpwi   crf0, r4, 0         /*  reqflgのチェック  */
    beq     ret_to_task_int /*  ディスパッチ要求がない場合は分岐  */        
    stw     r0, reqflg@l(r3)    /*  reqflgのクリア  */
    b       ret_int             /*  出口処理へジャンプ  */

            
    /*  多重割込みの場合  */
interrupt_from_int:
    /*
     *  コーリング・コンベンションに合わせて、スタックポインタを
     *　8バイトずらしておく
     *　
     *         -8  ----------------------
     *            |                      |
     *         -4  ----------------------　
     *            |                      |←C言語ルーチンの呼び出しにより
     *       sp→  ----------------------   書き込まれる
     *  
     */
    subi    sp, sp, STACK_MARGIN

    PROC_ICU(FROM_INT)  /*  ICU依存の割込み処理  */
    /*  割込み禁止で戻ってくる  */
    
    			/*  スタックポインタを元に戻す（上記参照） */
    addi    sp, sp, STACK_MARGIN
    mfspr   r3, SPRG0   /*  SPRG0:割込み／例外ネストカウンタ  */
    subi    r3, r3, 1   /*  割込みネストカウンタのディクリメント  */
    mtspr   SPRG0, r3
    POP_ICU_IPM     /*  割込みコントローラICUのIPMを復帰  */
    			/*  IPMのデータサイズの如何によらず、 */
    			/*  スタックポインタは4バイト境界を  */
    			/*  守ること  */
    
    /*  
     *  ディスパッチャを呼ばずに単純に割込み元（例外発生元）に戻る場合
     *
     *  rfi命令は同期命令なので、上記のレジスタの復帰とspの加算が終了して
     *  から元のコンテキストに戻る。
     *  spの加算だけが先に済んで割込み許可になる心配はない。
     *
     *    CPU例外から復帰する場合
     *  　　CPU例外からのリターン命令rfiにより
     *  　　　MSR←SRR1
     *  　　となるので、元のコンテキストに戻るときにはCPU例外発生
     *  　　直前のCPUロック／ロック解除状態が復元される
     */
ret_to_task_int:
ret_to_task_exc:
    LOAD_R0_12_SPRG     /*  gpr0-1,3-12, 特殊レジスタの復元  */
    addi    sp, sp, GPR0_12_SPRG_AREA_SIZE
    rfi     		/*  割込み元へ復帰  */
    


/*  レジスタを待避するのに必要なバイト数  */
                                    /*  gpr13-31,LR  */
#define GPR13_31_LR_AREA_SIZE   (GPR13_31_AREA_SIZE +4)

/*  
 *  マクロ定義：汎用レジスタgpr13-31の待避
 */

#ifdef USE_MULTIPLE_WORD_LOAD_STORE
    /*  
     *  整数ロード／ストア・マルチプル命令
     *　　インプリメンテーションによってはかえって遅くなって
     *　　しまうので注意が必要
     */

#if SIL_ENDIAN == SIL_ENDIAN_LITTLE
　　！ここでコンパイル・エラー
　　　リトル・エンディアン・モードにあるときには機能しない
　　　（ハードウェアによる制限事項）
#endif


#define STORE_R13_31                                \
    stmw    r13, 0(sp)    /*  gpr13-31を待避  */

#else   /*  USE_MULTIPLE_WORD_LOAD_STORE  */
#define STORE_R13_31                                \
    stw     r13,  0*4(sp);                          \
    stw     r14,  1*4(sp);                          \
    stw     r15,  2*4(sp);                          \
    stw     r16,  3*4(sp);                          \
    stw     r17,  4*4(sp);                          \
    stw     r18,  5*4(sp);                          \
    stw     r19,  6*4(sp);                          \
    stw     r20,  7*4(sp);                          \
    stw     r21,  8*4(sp);                          \
    stw     r22,  9*4(sp);                          \
    stw     r23, 10*4(sp);                          \
    stw     r24, 11*4(sp);                          \
    stw     r25, 12*4(sp);                          \
    stw     r26, 13*4(sp);                          \
    stw     r27, 14*4(sp);                          \
    stw     r28, 15*4(sp);                          \
    stw     r29, 16*4(sp);                          \
    stw     r30, 17*4(sp);                          \
    stw     r31, 18*4(sp)                           
#endif  /*  USE_MULTIPLE_WORD_LOAD_STORE  */

    /*  マクロ定義ここまで  */


/*  
 *  マクロ定義：汎用レジスタgpr13-31の復元
 */

#ifdef USE_MULTIPLE_WORD_LOAD_STORE
    /*  
     *  整数ロード／ストア・マルチプル命令
     *　　インプリメンテーションによってはかえって遅くなって
     *　　しまうので注意が必要
     */
#define LOAD_R13_31                                 \
    lmw     r13, 0(sp)    /*  gpr13-31を復元  */
    
#else   /*  USE_MULTIPLE_WORD_LOAD_STORE  */
#define LOAD_R13_31                                 \
    lwz     r13,  0*4(sp);                          \
    lwz     r14,  1*4(sp);                          \
    lwz     r15,  2*4(sp);                          \
    lwz     r16,  3*4(sp);                          \
    lwz     r17,  4*4(sp);                          \
    lwz     r18,  5*4(sp);                          \
    lwz     r19,  6*4(sp);                          \
    lwz     r20,  7*4(sp);                          \
    lwz     r21,  8*4(sp);                          \
    lwz     r22,  9*4(sp);                          \
    lwz     r23, 10*4(sp);                          \
    lwz     r24, 11*4(sp);                          \
    lwz     r25, 12*4(sp);                          \
    lwz     r26, 13*4(sp);                          \
    lwz     r27, 14*4(sp);                          \
    lwz     r28, 15*4(sp);                          \
    lwz     r29, 16*4(sp);                          \
    lwz     r30, 17*4(sp);                          \
    lwz     r31, 18*4(sp)
#endif  /*  USE_MULTIPLE_WORD_LOAD_STORE  */

    /*  マクロ定義ここまで  */




/*
 *  タスクディスパッチャ
 *
 *  dispatch は、SPRG0 = 0,割込み禁止状態で呼び出さなければならない．
 *  exit_and_dispatch も，SPRG0 = 0・割込み禁止状態で呼び出すのが原則
 *  であるが，カーネル起動時に対応するため，SPRG0 = 1で呼び出した場合
 *  にも対応している．
 */

    .global dispatch
dispatch:
    /*  スクラッチレジスタの待避  */
    subi    sp, sp, GPR13_31_LR_AREA_SIZE + FPR14_31_AREA_SIZE
    STORE_R13_31                /*  gpr13-31の待避  */

    mfspr   r7, LR
    stw     r7, GPR13_31_LR_AREA_SIZE - 4(sp)   /*  LRの待避  */

    /*  浮動小数点レジスタの待避  */
#ifdef SUPPORT_FLOATING_POINT_REG
    /*  ハードウェアが浮動小数点レジスタを実装している場合  */
    /*  未完成：本来なら8バイトにアライメントする  */    
    stfd    f14,  0*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f15,  1*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f16,  2*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f17,  3*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f18,  4*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f19,  5*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f20,  6*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f21,  7*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f22,  8*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f23,  9*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f24, 10*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f25, 11*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f26, 12*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f27, 13*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f28, 14*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f29, 15*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f30, 16*8+GPR13_31_LR_AREA_SIZE(sp)
    stfd    f31, 17*8+GPR13_31_LR_AREA_SIZE(sp)
#endif  /* SUPPORT_FLOATING_POINT_REG */    

    lis     r3, runtsk@ha
    LI32(r4, dispatch_r)
    lwz     r5, runtsk@l(r3)    /*  r5 = runtsk  */
    stw     sp, TCB_sp(r5)      /*  runtsk->sp =  sp */
    stw     r4, TCB_pc(r5)      /*  runtsk->pc =  dispatch_r */
    b       dispatcher


    /*  ディスパッチャの出口  */
    /*      スタックポインタの復元は済んでいる  */
    /*  　　r10にruntskが入って呼ばれる  */
dispatch_r: 
    /*  スクラッチレジスタの復帰  */

    /*  浮動小数点レジスタ  */
#ifdef SUPPORT_FLOATING_POINT_REG    
    /*  ハードウェアが浮動小数点レジスタを実装している場合  */
    /*  未完成：本来なら8バイトにアライメントする  */    
    lfd     f14,  0*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f15,  1*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f16,  2*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f17,  3*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f18,  4*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f19,  5*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f20,  6*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f21,  7*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f22,  8*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f23,  9*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f24, 10*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f25, 11*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f26, 12*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f27, 13*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f28, 14*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f29, 15*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f30, 16*8+GPR13_31_LR_AREA_SIZE(sp)
    lfd     f31, 17*8+GPR13_31_LR_AREA_SIZE(sp)
#endif  /* SUPPORT_FLOATING_POINT_REG */    

    /*  汎用レジスタ  */
    LOAD_R13_31

    /*  LRの復元  */
    lwz     r7, GPR13_31_LR_AREA_SIZE - 4(sp)
    mtspr   LR, r7

    addi    sp, sp, GPR13_31_LR_AREA_SIZE + FPR14_31_AREA_SIZE

    /*
     *  タスク例外処理要求のチェック
     *  dispatch_rはdispatcherから呼び出されるため，
     *  runtskはr10に入っている．
     */                           
    lbz     r4, TCB_enatex(r10)         /*  r10=runtsk  */
    /*
     *  マスクデータのビット幅とエンディアンはmakeoffset.cの
     *  BIT_BBおよびBIT_LBで指定している
     */                                 
    andi.   r5, r4, TCB_enatex_mask     /*  r5 = runtsk->enatex  */
                    /* 結果がゼロならば、EQフラグがセットされる */

          /*  タスク例外処理禁止ならdispatch()の呼び出し元に戻る  */
    beqlr               /*  （LRに戻り番地が格納されている）  */
    lwz     r6, TCB_texptn(r10)          /*  r6 = runtsk->texptn  */
    cmpwi   crf0, r6, 0
                        /*  タスク例外要求がなければ */
                        /*  dispatch()の呼び出し元に戻る  */
    beqlr               /*  （LRに戻り番地が格納されている）  */

    b       call_texrtn /*  タスク例外処理ルーチン呼び出し  */
    /*
     *  call_texrtn()からここには戻ってこないで、dispatch()の
     *　呼び出し元（=LRレジスタが指し示す番地）に直接戻る。
     *　
     *　call_texrtn()の呼び出しにより(sp+3)〜(ps+7)番地の内容が
     *　破壊されるが、spの位置はdispatch()を呼び出したときと同じ
     *　なので問題ない
     *　
     *  sp→  ----------------------  
     *       |                      |
     *    +4  ----------------------　
     *       |                      |←call_texrtn()の呼び出しにより
     *    +8  ----------------------   書き込まれる
     *  
     *  
     *　注意
 　　*　　タスク例外処理ルーチン内で浮動小数点レジスタを使用する
 　　*　　場合は、タスク例外処理ルーチン側で浮動小数点レジスタの
 　　*　　待避／復元を行うこと。
     */


    .global exit_and_dispatch
exit_and_dispatch:
    li      r0, 0
    mtspr   SPRG0, r0           /*  ネストカウンタをクリア  */
    
dispatcher:
    /*
     * ここには割り込み禁止で来ること
     */
    LOAD_VAL32(r10, schedtsk)   /*  r10 = schedtsk  */
    lis     r5, runtsk@ha
	/*
	 * ここでruntskにschedtskを代入するのは２つの意味がある。
	 * 　(1) schedtsk != NULLの場合
	 * 　　　　通常のタスク切り替えを行う。
	 * 　(2) schedtsk == NULLの場合
	 * 　　　　runtskにNULLを代入しておく。
	 * 　　　　（dispatcher_1以降の割込み待ちで割込みが入り、その中で
	 * 　　　　　iget_tid()がコールされたときに正しくTSK_NONEを返すため
	 * 　　　　　には、実行状態のタスクがない時に、runtskをNULLにして
	 * 　　　　　おる必要がある。）
	 */
    stw     r10, runtsk@l(r5)       /*  runtsk=schedtsk(=r10)  */

    cmpwi   crf0, r10, 0
    beq     dispatcher_1            /*  schedtskが無ければ割込み待ちへ  */

    lwz     r6, TCB_pc(r10)         /*  実行再開番地を取り出して  */
    mtctr   r6                      /*  CTR<-実行再開番地  */
    lwz     sp, TCB_sp(r10)         /*  スタックポインタを復元  */
    bctr                            /*  ジャンプ（下記参照）  */

    /*  
     *  実行再開番地は以下の3通り
     *  　いずれの場合もr10=runtskで呼び出すこと
     *  　　・ディスパッチャの出口　dispatch_r
     *  　　・割込み／例外の出口　　ret_int_r
     *  　　・タスク起動直後　　　　activate_r
     */


    /*  
     *　実行すべきタスクが現れるまで待つ処理
     *
     *  ここで非タスクコンテキストに切り換えるのは，ここで発生する割込み
     *  処理にどのスタックを使うかという問題の解決と，割込みハンドラ内で
     *  のタスクディスパッチの防止という二つの意味がある．
     */ 
dispatcher_1:
    LI32(sp, STACKTOP)          /*  スタック切り替え  */
    li      r3, 1
    mtspr   SPRG0, r3           /*  ネストカウンタ＝1  */
    
    /*  ループ内で使う定数を用意しておく  */
    LI32(r4, reqflg)            /*  reqflgのアドレス  */
    li      r0, 0

dispatcher_2:
#ifdef SUPPORT_POWER_MANAGEMENT
                                /*  省電力モードがある場合  */    
    /*  
     *  割込み許可と省電力モードへの移行
     *  　・割込みから戻ってきた後に割込み禁止も行う
     *  　・r0〜r4の内容を破壊してはならない
     *
     *  注意
     *      「割込み許可」と「省電力モードへの移行」をアトミックに行え
     *      ないプロセッサでは、割込みのタイミングによっては「割込み許
     *      可」と「省電力モードへの移行」の間で割込みを受け付けてしま
     *      い、意図しない形で省電力モードに移行してしまうので注意。
     *      割込みによって実行可能なタスクが現れてもプロセッサがスリー
     *      プしたままで実行されなくなる。（実際にはタイマ割込みにより
     *      省電力モードから復帰するので、タスクが待たされる時間は最大
     *      でもタイマ割込み周期で押さえられる。）
     */ 
    SAVE_POWPER
#else   /* SUPPORT_POWER_MANAGEMENT */    
                                /*  省電力モードがない場合  */    
    mfmsr   r5
    ori     r6, r5, MSR_EE
    mtmsr   r6                  /*  割込み許可  */
    /*  ここで割込みが入って、戻ってくる  */
    mtmsr   r5                  /*  割込み禁止  */
#endif  /* SUPPORT_POWER_MANAGEMENT */    
    lwz     r6, 0(r4)           /*  r6 = reqflg  */
    cmpwi   crf0, r6, 0
               /*  ディスパッチ要求がなければループの先頭へ  */
    beq     dispatcher_2
    stw     r0, 0(r4)           /*  reqflgをクリア  */
    mtspr   SPRG0, r0           /*  ネストカウンタをクリア  */
    b       dispatcher          /*  実行再開番地を取り出す処理へジャンプ  */

	/*
	 *　割込み待ちの直前に行ったスタック切替の戻しを行っていないが、
	 *　ディスパッチャの出口でTCBから取り出した値をスタックポイン
	 *　タに設定するので問題ない。
	 */



/* 
 *  割り込みハンドラ/CPU例外ハンドラ出口処理
 *
 * 戻り先がタスクでreqflgがセットされている場合のみここにくる。
 * SPRG0 = 0,割り込み禁止状態,スクラッチレジスタを保存した
 * 状態で呼び出すこと。 
 *  
 *  また、r10にruntskの値を入れてからret_int_1にジャンプすること
 */

ret_int:
ret_exc:
    LOAD_VAL32(r5, enadsp)  	/*  r5 = enadsp  */
    LOAD_VAL32(r10, runtsk) 	/*  r10 = runtsk  */
    cmpwi   crf0, r5, 0
    beq     ret_int_1           /*  ディスパッチ禁止ならジャンプ  */
    
    LOAD_VAL32(r5, schedtsk)  	/*  r5 = schedtsk  */
    cmpw    crf0, r5, r10
    beq     ret_int_1           /*  runtsk=schedtskならジャンプ  */
    
    
    /*  残りのレジスタを保存  */

    subi    sp, sp, GPR13_31_AREA_SIZE + FPR_AREA_SIZE
    /*  汎用レジスタの待避  */
    STORE_R13_31                /*  gpr13-31の待避  */

    /*  浮動小数点レジスタの待避  */
#ifdef SUPPORT_FLOATING_POINT_REG    
    /*  ハードウェアが浮動小数点レジスタを実装している場合  */
    /*  未完成：本来なら8バイトにアライメントする  */    
    stfd    f0,   0*8+GPR13_31_AREA_SIZE(sp)
    stfd    f1,   1*8+GPR13_31_AREA_SIZE(sp)
    stfd    f2,   2*8+GPR13_31_AREA_SIZE(sp)
    stfd    f3,   3*8+GPR13_31_AREA_SIZE(sp)
    stfd    f4,   4*8+GPR13_31_AREA_SIZE(sp)
    stfd    f5,   5*8+GPR13_31_AREA_SIZE(sp)
    stfd    f6,   6*8+GPR13_31_AREA_SIZE(sp)
    stfd    f7,   7*8+GPR13_31_AREA_SIZE(sp)
    stfd    f8,   8*8+GPR13_31_AREA_SIZE(sp)
    stfd    f9,   9*8+GPR13_31_AREA_SIZE(sp)
    stfd    f10, 10*8+GPR13_31_AREA_SIZE(sp)
    stfd    f11, 11*8+GPR13_31_AREA_SIZE(sp)
    stfd    f12, 12*8+GPR13_31_AREA_SIZE(sp)
    stfd    f13, 13*8+GPR13_31_AREA_SIZE(sp)
    stfd    f14, 14*8+GPR13_31_AREA_SIZE(sp)
    stfd    f15, 15*8+GPR13_31_AREA_SIZE(sp)
    stfd    f16, 16*8+GPR13_31_AREA_SIZE(sp)
    stfd    f17, 17*8+GPR13_31_AREA_SIZE(sp)
    stfd    f18, 18*8+GPR13_31_AREA_SIZE(sp)
    stfd    f19, 19*8+GPR13_31_AREA_SIZE(sp)
    stfd    f20, 20*8+GPR13_31_AREA_SIZE(sp)
    stfd    f21, 21*8+GPR13_31_AREA_SIZE(sp)
    stfd    f22, 22*8+GPR13_31_AREA_SIZE(sp)
    stfd    f23, 23*8+GPR13_31_AREA_SIZE(sp)
    stfd    f24, 24*8+GPR13_31_AREA_SIZE(sp)
    stfd    f25, 25*8+GPR13_31_AREA_SIZE(sp)
    stfd    f26, 26*8+GPR13_31_AREA_SIZE(sp)
    stfd    f27, 27*8+GPR13_31_AREA_SIZE(sp)
    stfd    f28, 28*8+GPR13_31_AREA_SIZE(sp)
    stfd    f29, 29*8+GPR13_31_AREA_SIZE(sp)
    stfd    f30, 30*8+GPR13_31_AREA_SIZE(sp)
    stfd    f31, 31*8+GPR13_31_AREA_SIZE(sp)
    /*  
     * FPSCRの待避  
     *
     *　　スタックを8バイト使用するのは無駄であるが、
     *　　stfiwx命令はオプションなので選択しなかった。
     *　　（また、対応するロード命令がないのも問題）
     *
     */
    mffs    f0              /*  f0の下位2バイト←FPSCR  */
                            /*  @(sp+オフセット)←f0  */
    stfd    f0, GPR13_31_AREA_SIZE+FPR0_13_AREA_SIZE-8(sp)
#endif  /* SUPPORT_FLOATING_POINT_REG */    

    LI32(r5, ret_int_r)
                                /*  r10 = runtskでここに来る  */
    stw     sp, TCB_sp(r10)      /*  runtsk->sp = sp  */
                                /*  タスクスタックポインタの保存  */
    stw     r5, TCB_pc(r10)     /*  runtsk->pc = ret_int_r  */
                                /*  （実行再開番地の保存）  */

    b       dispatcher          /*  ディスパッチャ呼び出し  */


 /*  割込み／例外の出口処理の後半部分
  *     ret_intから
  *     　・直接、ret_int_1に合流する場合
  *     　・ディスパッチャを経由してやって来る場合
  *     　の2通りある
  *     　いずれの場合もr10にruntskが入っている
  */
ret_int_r:

#ifdef SUPPORT_FLOATING_POINT_REG    
    /*  ハードウェアが浮動小数点レジスタを実装している場合  */

    /*  FPSCRの復元  */
    lfd     f0, GPR13_31_AREA_SIZE+FPR0_13_AREA_SIZE-8(sp)
    mtfsf   0xf,f0          /*  FPSCR←f0の下位2バイト  */

    /*  浮動小数点レジスタの復元  */
    lfd     f0,   0*8+GPR13_31_AREA_SIZE(sp)
    lfd     f1,   1*8+GPR13_31_AREA_SIZE(sp)
    lfd     f2,   2*8+GPR13_31_AREA_SIZE(sp)
    lfd     f3,   3*8+GPR13_31_AREA_SIZE(sp)
    lfd     f4,   4*8+GPR13_31_AREA_SIZE(sp)
    lfd     f5,   5*8+GPR13_31_AREA_SIZE(sp)
    lfd     f6,   6*8+GPR13_31_AREA_SIZE(sp)
    lfd     f7,   7*8+GPR13_31_AREA_SIZE(sp)
    lfd     f8,   8*8+GPR13_31_AREA_SIZE(sp)
    lfd     f9,   9*8+GPR13_31_AREA_SIZE(sp)
    lfd     f10, 10*8+GPR13_31_AREA_SIZE(sp)
    lfd     f11, 11*8+GPR13_31_AREA_SIZE(sp)
    lfd     f12, 12*8+GPR13_31_AREA_SIZE(sp)
    lfd     f13, 13*8+GPR13_31_AREA_SIZE(sp)
    lfd     f14, 14*8+GPR13_31_AREA_SIZE(sp)
    lfd     f15, 15*8+GPR13_31_AREA_SIZE(sp)
    lfd     f16, 16*8+GPR13_31_AREA_SIZE(sp)
    lfd     f17, 17*8+GPR13_31_AREA_SIZE(sp)
    lfd     f18, 18*8+GPR13_31_AREA_SIZE(sp)
    lfd     f19, 19*8+GPR13_31_AREA_SIZE(sp)
    lfd     f20, 20*8+GPR13_31_AREA_SIZE(sp)
    lfd     f21, 21*8+GPR13_31_AREA_SIZE(sp)
    lfd     f22, 22*8+GPR13_31_AREA_SIZE(sp)
    lfd     f23, 23*8+GPR13_31_AREA_SIZE(sp)
    lfd     f24, 24*8+GPR13_31_AREA_SIZE(sp)
    lfd     f25, 25*8+GPR13_31_AREA_SIZE(sp)
    lfd     f26, 26*8+GPR13_31_AREA_SIZE(sp)
    lfd     f27, 27*8+GPR13_31_AREA_SIZE(sp)
    lfd     f28, 28*8+GPR13_31_AREA_SIZE(sp)
    lfd     f29, 29*8+GPR13_31_AREA_SIZE(sp)
    lfd     f30, 30*8+GPR13_31_AREA_SIZE(sp)
    lfd     f31, 31*8+GPR13_31_AREA_SIZE(sp)
#endif  /* SUPPORT_FLOATING_POINT_REG */    

    /*  汎用レジスタの復元  */
    LOAD_R13_31     /*  gpr13-31の復元  */
    addi    sp, sp, GPR13_31_AREA_SIZE + FPR_AREA_SIZE

    /*  ディスパッチャを経由しない場合はここから合流  */
    /*  （r10にruntskが入った状態でここに来る）  */
ret_int_1:
    /*  タスク例外処理要求のチェック  */
                                    /*  r10 = runtsk  */
    lbz     r4, TCB_enatex(r10)
    /*
     *  マスクデータのビット幅とエンディアンはmakeoffset.cの
     *  BIT_BBおよびBIT_LBで指定している
     */                                 
    andi.   r5, r4, TCB_enatex_mask /*  r5 = runtsk->enatex  */
                            /* 結果がゼロならば、EQフラグがセットされる */
    beq     ret_int_2               /*  タスク例外処理禁止ならジャンプ  */
    lwz     r6, TCB_texptn(r10)      /*  r6 = runtsk->texptn  */
        /*  タスク例外要求がなければならジャンプ  */
    cmpwi   crf0, r6, 0
    beq     ret_int_2
    
    /*
     *  コーリング・コンベンションに合わせて、スタックポインタを
     *　8バイトずらしておく。
     *　
     *    -8  ----------------------
     *       |                      |
     *    -4  ----------------------　
     *       |                      |←call_texrtn()の呼び出しにより
     *  sp→  ----------------------   書き込まれる
     *  
     */
    subi    sp, sp, STACK_MARGIN

    bl      call_texrtn             /*  タスク例外処理ルーチン呼び出し  */
		    	     /*  スタックポインタを元に戻す（上記参照） */
    addi    sp, sp, STACK_MARGIN
    /*  
     *　注意
 　　*　　タスク例外処理ルーチン内で浮動小数点レジスタを使用する
 　　*　　場合は、タスク例外処理ルーチン側で浮動小数点レジスタの
 　　*　　待避／復元を行うこと。
     */

ret_int_2:
    /*  
     *　call_texrtn()からここに戻って来る。(上記addi命令を経由）
     *　（call_texrtn()を呼ばない場合は直接ここに来る）
     *
 　　*　　結果的にret_to_task_intと同じ処理になったが、
 　　*　　無理にまとめると可読性が悪くなるので
 　　*　　別々のままにしている。
     */
    LOAD_R0_12_SPRG     /*  gpr0-1,3-12, 特殊レジスタの復元  */
    addi    sp, sp, GPR0_12_SPRG_AREA_SIZE
    /*  
     *  rfi命令は同期命令なので、上記のレジスタの復帰とspの加算が終了して
     *  から元のコンテキストに戻る。
     *  spの加算だけが先に済んで割込み許可になる心配はない。
     */
    rfi     /*  割込み元へ復帰  */




/*
 *  タスク起動処理
 *  　ディスパッチャから呼び出されるので、r10にはruntskが入っている
 *  　スタックポインタも設定済み
 */ 

    .globl activate_r
activate_r:
                                    /*  r10 = runtsk (ディスパッチャで設定) */
    lwz     r4, TCB_tinib(r10)      /*  r4 = runtsk->tinib  */

                                    /*  タスク起動番地の設定  */
    lwz     r5, TINIB_task(r4)      
    mtctr   r5                      /*  CTR = runtsk->tinib->task  */

                                    /*  タスクへの引数（拡張情報）  */
    lwz     r3, TINIB_exinf(r4)     /*  r3 = runtsk->tinib->TINIB_exinf  */

    /*
     *  LRレジスタにext_tskを設定することにより、ext_tskを
     *  呼ばないでタスクの関数を抜ける場合もext_tskに分岐する。
     */                                 
    LI32(r6, ext_tsk)
    mtspr   LR, r6                  /*  タスクからの戻り番地を設定  */

    mfmsr   r8
    ori     r9, r8, MSR_EE
    mtmsr   r9                      /*  割込み許可  */

    bctr                            /*  C言語ルーチン呼び出し  */


/*
 *  微少時間待ち 
 *　　引数r3：待ち時間をnsec単位で指定する
 */
    .globl sil_dly_nse
sil_dly_nse:
    LI32(r4, SIL_DLY_TIM1)
    sub.    r3, r3, r4          /* r3 からSIL_DLY_TIM1を引く */
    blelr                       /* 結果が0以下ならリターン */

    LI32(r5, SIL_DLY_TIM2)
sil_dly_nse_1:
    sub.    r3, r3, r5          /* r3 からSIL_DLY_TIM2を引く */
    bgt     sil_dly_nse_1       /* 結果が 0 より大きければループ */
    blr

/*  end of file  */
