/* 
 *
 *  TOPPERS/JSP Kernel
 *     Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 * 上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 * によって公表されている GNU General Public License の Version 2 に記
 * 述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 * を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 * 利用と呼ぶ）することを無償で許諾する．
 * (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *     権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *     スコード中に含まれていること．
 * (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *     用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *     者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *     の無保証規定を掲載すること．
 * (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *     用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *     と．
 *   (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *       作権表示，この利用条件および下記の無保証規定を掲載すること．
 *   (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *       報告すること．
 * (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *     害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *
 * 本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 * よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 * 含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 * 接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 *
 *  @(#) $Id: cpu_support.S,v 1.15 2007/04/19 06:38:27 honda Exp $
 */

#define _MACRO_ONLY
        
/*
 *  アプリケーションと共通のインクルードファイル
 */
#include <kernel.h>
                
/*
 *  ターゲット依存情報の定義
 */
#include <t_config.h>

#include "jsp_kernel.h"
#include "offset.h"
#include <microblaze_asm.inc>
#include <microblaze.h>


/*
 * INTCの設計について，
 *
 *  保存するレジスタ
 *  R3-R4   Return Valus        (Volatile)
 *  R5-R10  Passing parameters  (Volatile)
 *  R11-R12 Temporaries         (Volatile)
 *  R14     Return address for interrupt
 *  R15     Return address for Sub-routine
 *  R16     Return address for Trap(Debugger)
 *  R17     Return address for Exeptions
 * ?R18     Reserved for Assember    
 */

#define STACK_TOP (STACKTOP - 0x4) 

#define INTC_ISR (INTC_BASE + INTC_INT_STATUS_REG)
#define INTC_IPR (INTC_BASE + INTC_INT_PENDING_REG)
#define INTC_IER (INTC_BASE + INTC_INT_ENABLE_REG)
#define INTC_IAR (INTC_BASE + INTC_INT_ACK_REG)
#define INTC_SIE (INTC_BASE + INTC_SET_INT_ENABLE)
#define INTC_CIE (INTC_BASE + INTC_CLEAR_INT_ENABLE)
#define INTC_IVR (INTC_BASE + INTC_INT_VECTOR_REG)
#define INTC_MER (INTC_BASE + INTC_MASTER_ENABLE_REG)


/*
 *  例外エントリ
 *  Not Support!
 */
	.text
	.globl exception_entry
	.align 2
exception_entry:
	nop
	



	.text
	.globl interrupt_entry
	.align 2
interrupt_entry:
/*
 * スタックポインタの切り替えが必要
 */
	addik r1,r1,-64
	swi r3,  r1, 60
	swi r4,  r1, 56
	swi r5,  r1, 52
	swi r6,  r1, 48
	swi r7,  r1, 44
	swi r8,  r1, 40
	swi r9,  r1, 36
	swi r10, r1, 32
	swi r11, r1, 28
	swi r12, r1, 24
	swi r14, r1, 20
	swi r15, r1, 16
	swi r16, r1, 12
	swi r17, r1, 8
	swi r18, r1, 4
	mfs r3,  rmsr     /* msrの保存 */
	swi r3,  r1, 0

	lwi   r5,  r13, interrupt_count
	bgti  r5,  nest_int      /* ネスト回数が1以上ならスタックを切り替えない */
	
	/*
	 * スタックポインタ切り替え
	 */
	la    r4,  r0, STACK_TOP   /* タスク独立部のスタックの読み込み */
	sw    r1,  r0, r4          /* タスクスタックの保存             */
	Mov   r1,  r4              /* スタックポインタ切り替え         */
nest_int:
	/* 
     * 割込みネスト回数のインクリメント
	 */
	addi  r5,  r5,  1 
	swi   r5,  r13, interrupt_count

	lwi   r3,  r0, INTC_IVR    /* ベクタの読み込み                 */
	add   r3,  r3, r3          /* ベクタを4倍                      */
	add   r3,  r3, r3	        
	lwi   r5,  r3, int_handler_table /* ハンドラアドレスの読み込み */
	lwi   r6,  r3, int_bit_table     /* マスクビットの読み込み     */
	swi   r6,  r0, INTC_CIE          /* 発生した割込みをマスク     */
	ori   r4,  r0, 0x02 | MSR_CACHE_SETTING /* r4 = 0x02           */
	Push  r6                         /* マスクビットをスタックに   */

	mts   rmsr,r4                    /* 割込み許可(MSR(IE)をセット)*/

	brald r15, r5             /* ハンドラ呼び出し */
	 nop

    la    r4, r0, MSR_CACHE_SETTING  
	mts   rmsr,r4                    /* 割込み禁止                 */

//    ori   r4,  r0, 0x03       /* r4 = 0x03                         */
//    swi   r4,  r0, INTC_MER   /* INTC_MER = 0x0 INTCスタート       */

	Pop   r6                         /* マスクビットの取り出し     */
//	swi   r6,  r0, INTC_IAR          /* ACK                        */        
	swi   r6,  r0, INTC_SIE          /* 発生割込みの許可           */

	/*
	 *  割込みネスト回数のデクリメント
     */
	lwi   r5,  r13, interrupt_count
	addi  r5,  r5,  -1 
	swi   r5,  r13, interrupt_count
	
	bgti  r5,  ret_to_task_int      /* ネスト回数が1以上なら戻る */

	lw    r1,  r0, r1	            /* スタックポインタを戻す    */  
	lwi   r4,  r13, reqflg          /* reqflg のチェック         */
	beqi  r4,  ret_to_task_int      /* FALSE なら タスクに戻る   */
	bri   ret_int                   /* TRUE なら ret_int に      */
ret_to_task_int:
	lwi  r3,  r1, 0
	mts  rmsr,r3
	lwi  r18, r1, 4
	lwi  r17, r1, 8
	lwi  r16, r1, 12
	lwi  r15, r1, 16
	lwi  r14, r1, 20
	lwi  r12, r1, 24
	lwi  r11, r1, 28
	lwi  r10, r1, 32
	lwi  r9,  r1, 36
	lwi  r8,  r1, 40
	lwi  r7,  r1, 44
	lwi  r6,  r1, 48
	lwi  r5,  r1, 52
	lwi  r4,  r1, 56
	lwi  r3,  r1, 60
	rtid r14,0 
	addik r1,r1,64



	/*
	 * 未登録の割込みが発生すると呼び出される
	 * Not Yet!
	 */
	.globl no_reg_interrupt
	.align 2
no_reg_interrupt:
	nop


	/*
	 * タスクディスパッチャ
	 * 
	 * dispatch は，interrupt_count = 0 
     * MSRの IE=0 の割込み禁止状態で呼び出さなければならない．
	 * exit_and_dispatch も同様に，interrupt_count = 0 割込み禁止状態で
     * 呼び出すのが原則であるが，カーネル起動時に対応するため，
     * interrupt_count = 1 で呼び出した場合にも対応している．
	 *    
	 */
	
	.globl dispatch
	.align  2
dispatch:
	addi  r1,  r1,  -64  /* スクラッチレジスタ以外を保存する */
	swi   r15, r1,  60
	mfs   r3,  rmsr      /* msrの保存(キャッシュの設定の保存) */
	swi   r3,  r1,  56                
	swi   r18, r1,  52   /* 必要か? */
	swi   r19, r1,  48
	swi   r20, r1,  44
	swi   r21, r1,  40
	swi   r22, r1,  36
	swi   r23, r1,  32
	swi   r24, r1,  28
	swi   r25, r1,  24
	swi   r26, r1,  20
	swi   r27, r1,  16
	swi   r28, r1,  12
	swi   r29, r1,  8
	swi   r30, r1,  4
	swi   r31, r1,  0
	lwi   r4 , r13, runtsk     /* r4 <- runtsk */
	swi   r1 , r4,  TCB_sp 	   /* タスクスタックをTCBに保存 */	
	la    r5 , r0,  dispatch_r /* 実行再開番地を保存 */
	swi   r5 , r4,  TCB_pc     /* 実行再開番地をTCBに保存   */
	bri   dispatcher

dispatch_r:
	lwi  r31,  r1, 0      /* レジスタを復帰 */
	lwi  r30,  r1, 4
	lwi  r29,  r1, 8
	lwi  r28,  r1, 12
	lwi  r27,  r1, 16
	lwi  r26,  r1, 20
	lwi  r25,  r1, 24
	lwi  r24,  r1, 28
	lwi  r23,  r1, 32
	lwi  r22,  r1, 36
	lwi  r21,  r1, 40
	lwi  r20,  r1, 44
	lwi  r19,  r1, 48
	lwi  r18,  r1, 52
    lwi  r3,   r1, 56   /* MSRを戻す(キャッシュの設定を戻す) */
    mts  rmsr,r3            
	addi r1,   r1, 60   /* スタックポインタを戻す */
	/*
	 *  タスク例外処理ルーチンの起動
	 *  dispatch_r は dispatcher から呼び出されるため，
	 *  tcb のアドレスは r4 に入っている
     *  Not Yet!
	 */
	lwi  r5,  r4,  TCB_enatex  /* r5 <- enatex                 */
	andi r6,  r5,  TCB_enatex_mask  
	beqi r6,  dispatch_r_1     /* enatex が FALSE ならリターン */
	lwi  r7,  r4,  TCB_texptn  /* r5 <- texptn                 */
	beqi r7,  dispatch_r_1     /* texptn が 0 でなければ       */
	brlid r15  call_texrtn     /* タスク例外ルーチンの呼び出し */	
	 nop
dispatch_r_1:
	Pop     r15
	rtsd	r15,8 
	 nop



	.globl exit_and_dispatch
exit_and_dispatch:
	/* interrupt_count をクリア */
	swi   r0,  r13, interrupt_count 
dispatcher:
	/*
	 * ここは割込み禁止で来ること
	 */
	lwi  r4,  r13, schedtsk  /* r4 <- schedtsk              */
	swi  r4,  r13, runtsk    /* schedtsk を runtsk に       */        
	beqi r4,  dispatcher_1   /* schedtsk があるか           */
	lwi  r1,  r4,  TCB_sp    /* TCBからタスクスタックを復帰 */
	lwi  r5,  r4,  TCB_pc    /* TCBから実行再開番地を復帰   */
	bra  r5	  
dispatcher_1:
	/*
	 * ここで割込みモードに切り替えるのは，ここで発生する割込み処理
     * にどのスタックを使うかという問題の解決と，割込みハンドラ内で
	 * のタスクディスパッチの防止という2つの意味がある．
	 */
	la   r1,  r0,  STACKTOP         /* 割込みスタックに変更             */
	la   r6,  r0,  1                /* interrupt_count を1に            */
	swi  r6,  r13, interrupt_count
	la   r5,  r0, 0x02 | MSR_CACHE_SETTING /* IE = '1'               */
	la   r4,  r0, MSR_CACHE_SETTING        /* IE = '0'               */        
dispatcher_2:
	mts   rmsr, r5   /* 割込み許可(MSR(IE)をセット)      */
	nop
	nop
	mts   rmsr, r4                  /* 割込み禁止(MSR(IE)をクリア)      */
	lwi   r6,  r13,  reqflg         /* r6 <- reqflg                     */
	beqi  r6,  dispatcher_2         /* reqflg が FALSE なら             */
	swi   r0,  r13, interrupt_count /* interrupt_count をクリア         */
	swi   r0,  r13,  reqflg         /* reqflg を FALSE に               */	
	bri   dispatcher

	/*
	 *  割込みハンドラ/CPU例外ハンドラ出口処理
	 * 
	 * 戻り先がタスクで reqflg がセットされている場合のみここにくる．
	 * interrupt_count = 0，割込み禁止状態，スクラッチレジスタを保存した
	 * 状態で呼び出すこと．
	 */
	.align 2
	.globl ret_int
	.globl ret_exc
ret_exc:
ret_int:
	swi  r0,  r13,  reqflg   /* reqflg を FALSE に */
	lwi  r4,  r13,  runtsk   /* r4 <- runtsk        */
	lwi  r6,  r13,  enadsp   /* r6 <- enadsp        */
	beqi r6,  ret_int_1      /* enadsp が FALSE なら ret_int_1 へ */
	lwi  r5,  r13,  schedtsk /* r5 <- schedtsk                    */
	sub  r6,  r5,   r4       /* runtsk と schedtsk が同じなら     */
	beqi r6,  ret_int_1      /* ret_int_1 へ                      */
	addi  r1,  r1,  -52      /* 残りのレジスタを保存する          */
	swi   r19, r1,  48
	swi   r20, r1,  44
	swi   r21, r1,  40
	swi   r22, r1,  36
	swi   r23, r1,  32
	swi   r24, r1,  28
	swi   r25, r1,  24
	swi   r26, r1,  20
	swi   r27, r1,  16
	swi   r28, r1,  12
	swi   r29, r1,  8
	swi   r30, r1,  4
	swi   r31, r1,  0	
	swi   r1 , r4,  TCB_sp 	   /* タスクスタックをTCBに保存 */	
	la    r6 , r0,  ret_int_r  /* 実行再開番地を保存 */
	swi   r6 , r4,  TCB_pc     /* 実行再開番地をTCBに保存   */
	bri   dispatcher

ret_int_r:	
	lwi  r31,  r1,  0      /* レジスタを復帰 */
	lwi  r30,  r1,  4
	lwi  r29,  r1,  8
	lwi  r28,  r1,  12
	lwi  r27,  r1,  16
	lwi  r26,  r1,  20
	lwi  r25,  r1,  24
	lwi  r24,  r1,  28
	lwi  r23,  r1,  32
	lwi  r22,  r1,  36
	lwi  r21,  r1,  40
	lwi  r20,  r1,  44
	lwi  r19,  r1,  48
	addi r1,   r1,  52
	lwi  r3,   r1,   0    /* MSR を戻す（キャッシュの設定を戻す）*/
	mts  rmsr, r3                
ret_int_1:
	/*
	 * タスク例外ルーチンの起動
	 * ret_int_r は dispatcher から呼び出されるため，
	 * tcb のアドレスは r4 に入っている
	 */
	lwi  r5,  r4,  TCB_enatex  /* r5 <- enatex                 */
	andi r6,  r5,  TCB_enatex_mask  
	beqi r6,  ret_int_2        /* enatex が FALSE ならリターン */
	lwi  r7,  r4,  TCB_texptn  /* r5 <- texptn                 */
	beqi r7,  ret_int_2        /* texptn が 0 でなければ       */
	brlid r15  call_texrtn     /* タスク例外ルーチンの呼び出し */	
	 nop
ret_int_2:
	lwi  r3,  r1, 0    /* MSR を戻す（割込みIE=0で割込み禁止になる）*/
	mts  rmsr,r3
	lwi  r18, r1, 4
	lwi  r17, r1, 8
	lwi  r16, r1, 12
	lwi  r15, r1, 16
	lwi  r14, r1, 20
	lwi  r12, r1, 24
	lwi  r11, r1, 28
	lwi  r10, r1, 32
	lwi  r9,  r1, 36
	lwi  r8,  r1, 40
	lwi  r7,  r1, 44
	lwi  r6,  r1, 48
	lwi  r5,  r1, 52
	lwi  r4,  r1, 56
	lwi  r3,  r1, 60
	rtid r14,0 
	 addik r1,r1,64	



	/*
	 * タスク起動処理
     *   
     *  スタックの取り方
     *  Microblaze Processor Reference Guide 52〜53より
     *  関数呼出しのさい引数は，r5〜r10 に格納する．
     *  それと同時に呼出し元はスタックフレームに引数の格納場所を確保する必要
     *  がある．さらにその上にはリンクレジスタ(R15)の分の領域が必要となる．
     *  呼出し先は更に他の関数を呼び出す場合等に r5〜r10 をこの領域に保存す
     * る．
     *  Low Address
     *             --------------------
     *  new_sp -> | Link Register(R15) |
     *             --------------------
     *            |   Arg1用の領域     |
     *             --------------------
     *            |   Arg2用の領域     |
     *             --------------------        
     *            |      ....          |
     *             --------------------
     *  High Address
	 */

	.text
	.globl activate_r
	.align 2
activate_r:
	ori   r4,  r0, 0x02 | MSR_CACHE_SETTING  /* msrの初期値        */
	mts   rmsr,r4              /* 割込み許可(MSR(IE)をセット)      */
    lw    r11, r1, r0          /* タスクの実行番地                 */
    lwi   r5,  r1,  4          /* 引数（exinf）                    */
	la    r15, r0,  ext_tsk -8 /* タスクからの戻り先               */
	bra   r11                  /* タスクの実行開始                 */


        
    /*
     * 微少時間待ち
     */
     .globl _sil_dly_nse
_sil_dly_nse:
     addi   r5, r5, -SIL_DLY_TIM1
     bgti   r5, _sil_dly_nse_1
     rtsd   r15, 8
      nop  
_sil_dly_nse_1:
     addi   r5, r5, -SIL_DLY_TIM2
     bgti   r5, _sil_dly_nse_1
     rtsd   r15, 8
      nop          
