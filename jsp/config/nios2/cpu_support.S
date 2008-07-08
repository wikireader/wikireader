/* 
 *
 *  TOPPERS/JSP Kernel
 *     Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN  
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
 *  @(#) $Id: cpu_support.S,v 1.4 2005/07/06 01:48:21 honda Exp $
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
#include <nios2.h>

    .set noat
    .section .exceptions, "xa"
    .align 2
    .global _irq_entry
    .type   _irq_entry, @function
_irq_entry:
    /*
     *  例外要因の判定
     */
    rdctl   et, estatus
    andi    et, et, 1
    beq     et, zero, _check_trap
    rdctl   et, ipending
    beq     et, zero, _check_trap

    
   /*
    * 例外要因は割込み
    */
    addi ea, ea, -4      /* 戻り番地をデクリメント */

    addi  sp, sp, -76    /* レジスタの保存 */
    rdctl et, estatus
    stw   et,   0(sp)
    stw   at,   4(sp)
    stw   r2,   8(sp)
    stw   r3,  12(sp)
    stw   r4,  16(sp)
    stw   r5,  20(sp)
    stw   r6,  24(sp)
    stw   r7,  28(sp)
    stw   r8,  32(sp)
    stw   r9,  36(sp)
    stw   r10, 40(sp)
    stw   r11, 44(sp)
    stw   r12, 48(sp)
    stw   r13, 52(sp)
    stw   r14, 56(sp)
    stw   r15, 60(sp)
    stw   fp,  64(sp)
    stw   ra,  68(sp)
    stw   ea,  72(sp)

    /*
     * 多重割込みか判定
     */
    ldw   r3, %gprel(interrupt_count)(gp)  /* ネスト回数のチェック */
    bltu  zero, r3,  nest_int

    movhi r2, %hiadj(STACKTOP-4)
    addi  r2, r2, %lo(STACKTOP-4)
    stw   sp, 0(r2)     /* スタックポインタの保存 */        
    mov   sp, r2        /* スタックポインタの入れ替え */
nest_int:
    /*
     *  割込み要因の判定と呼び出し
     */
    call int_handler_call

ret_from_int:
    ldw   r3, %gprel(interrupt_count)(gp)   
    bltu  zero, r3, res_reg_and_ret /* ネスト回数が1以上なら戻る */

    ldw   r2, 0(sp)                 /* スタックポインタを戻す */
    mov   sp, r2                    /* スタックポインタを戻す */

    ldw   r3, %gprel(reqflg)(gp)    /* reqflgのチェック */
    beq   r3, zero, res_reg_and_ret
    br    ret_int

res_reg_and_ret:
    ldw   et,   0(sp)  /* レジスタの復帰 */
    wrctl estatus, et
    ldw   at,   4(sp)
    ldw   r2,   8(sp)
    ldw   r3,  12(sp)
    ldw   r4,  16(sp)
    ldw   r5,  20(sp)
    ldw   r6,  24(sp)
    ldw   r7,  28(sp)
    ldw   r8,  32(sp)
    ldw   r9,  36(sp)
    ldw   r10, 40(sp)
    ldw   r11, 44(sp)
    ldw   r12, 48(sp)
    ldw   r13, 52(sp)
    ldw   r14, 56(sp)
    ldw   r15, 60(sp)
    ldw   fp,  64(sp)
    ldw   ra,  68(sp)
    ldw   ea,  72(sp)
    addi  sp, sp, 76        
    eret      


_check_trap:        
    /*
     *  Trapか判定
     */
    ldw   et, -4(ea)               /* 例外を出した命令を取得 */
    xorhi et, et, 0x003b           /*  上位16bit             */
    xori  et, et, 0x683a           /*  下位16bit             */
    beq   et, zero, trap_handler

_check_exc:
    addi  sp, sp, -76
    rdctl et,   estatus
    stw   et,    0(sp)
    stw   at,    4(sp)
    stw   r2,    8(sp)
    stw   r3,   12(sp)
    stw   r4,   16(sp)
    stw   r5,   20(sp)
    stw   r6,   24(sp)
    stw   r7,   28(sp)
    stw   r8,   32(sp)
    stw   r9,   36(sp)
    stw   r10,  40(sp)
    stw   r11,  44(sp)
    stw   r12,  48(sp)
    stw   r13,  52(sp)
    stw   r14,  56(sp)
    stw   r15,  60(sp)
    stw   fp,   64(sp)
    stw   ra,   68(sp)
    stw   ea,   72(sp)
    mov   r4,   sp     /* ハンドラの引数 */

    /*
     * 多重割込みか判定
     */
    ldw   r3, %gprel(interrupt_count)(gp)  /* ネスト回数のチェック */
    bltu  zero, r3,  nest_int_exc
    
    movhi r2, %hiadj(STACKTOP-4)
    addi  r2, r2, %lo(STACKTOP-4)
    stw   sp, 0(r2)     /* スタックポインタの保存 */        
    mov   sp, r2        /* スタックポインタの入れ替え */
nest_int_exc:
    
    call exc_handler_call  /* CPU例外ハンドラ呼び出しルーチンの実行 */

    br ret_from_int        /* リターン処理 */


trap_handler:
   /*
    *  Trapハンドラー
    */
    eret  



    .set noat
    .align  2
    .global ret_int
    .global ret_exc
ret_int:
ret_exc:
    stw   zero, %gprel(reqflg)(gp)   /* reqflg を FALSEに */
    ldw   r5,   %gprel(runtsk)(gp)   /* r5 <- runtsk      */
    ldw   r6,   %gprel(enadsp)(gp)   /* r6 <- enadsp      */
    beq   r6,   zero, ret_int_1      /* enadsp が FALSE なら ret_int_1 へ */
    ldw   r4,   %gprel(schedtsk)(gp) /* r4 <- schedtsk    */
    beq   r4,   r5, ret_int_1        /* runtsk と schedtskが同じならret_int_1へ */
    addi  sp,   sp, -32              /* 残りのレジスタを保存 */    
    stw   r16,  0(sp)
    stw   r17,  4(sp)
    stw   r18,  8(sp)
    stw   r19, 12(sp)
    stw   r20, 16(sp)
    stw   r21, 20(sp)
    stw   r22, 24(sp)
    stw   r23, 28(sp)
    stw   sp,  TCB_sp(r5)         /* タスクスタックをTCBに保存 */
    movhi r2,  %hiadj(ret_int_r)  /* 実行開始番地を保存        */
    addi  r2,  r2, %lo(ret_int_r)
    stw   r2,  TCB_pc(r5)         /* 実行再開番地をTCBに保存   */
    br    dispatcher

ret_int_r:
    ldw   r16,  0(sp)       /* レジスタを復帰 */
    ldw   r17,  4(sp)
    ldw   r18,  8(sp)
    ldw   r19, 12(sp)
    ldw   r20, 16(sp)
    ldw   r21, 20(sp)
    ldw   r22, 24(sp)
    ldw   r23, 28(sp)
    addi  sp, sp, 32    
ret_int_1:
    /*
     *  タスク例外ルーチンの起動    
     *  ret_int_r は dispatcher から呼び出されるため，
     *  tcbのアドレスは r4 に入っている
     */
    ldw  r5, TCB_enatex(r4)       /* r5 <- enatex */
#if TCB_enatex_mask > 0xffff
    andhi r6, r5, %hi(TCB_enatex_mask)
#else
    andi r6, r5, %lo(TCB_enatex_mask)
#endif /* TCB_enatex_mask > 0xffff */
    andhi r6, r5, 4
    beq  r6, zero, ret_int_2      /* enatex が FALSE ならリターン */
    ldw  r7, TCB_texptn(r4)       /* r7 <- texptn, texptnが0でなければ */
    beq  zero, r7, ret_int_2      
    call call_texrtn              /* タスク例外ルーチンの呼び出し */

ret_int_2:
    ldw   et,   0(sp)    /* レジスタを復帰 */
    wrctl estatus, et
    ldw   at,   4(sp)
    ldw   r2,   8(sp)
    ldw   r3,  12(sp)
    ldw   r4,  16(sp)
    ldw   r5,  20(sp)
    ldw   r6,  24(sp)
    ldw   r7,  28(sp)
    ldw   r8,  32(sp)
    ldw   r9,  36(sp)
    ldw   r10, 40(sp)
    ldw   r11, 44(sp)
    ldw   r12, 48(sp)
    ldw   r13, 52(sp)
    ldw   r14, 56(sp)
    ldw   r15, 60(sp)
    ldw   fp,  64(sp)
    ldw   ra,  68(sp)
    ldw   ea,  72(sp)
    addi  sp, sp, 76        
    eret      


    

    .global dispatch
    .align  2
dispatch:
    addi  sp, sp, -40                /* レジスタを保存 */    
    stw   r16,  0(sp)
    stw   r17,  4(sp)
    stw   r18,  8(sp)
    stw   r19, 12(sp)
    stw   r20, 16(sp)
    stw   r21, 20(sp)
    stw   r22, 24(sp)
    stw   r23, 28(sp)
    stw   fp,  32(sp)
    stw   ra,  36(sp)
    ldw   r4,  %gprel(runtsk)(gp)  /* r4 <- runtsk              */
    stw   sp,  TCB_sp(r4)          /* タスクスタックをTCBに保存 */
    movhi r5,  %hiadj(dispatch_r)  /* 実行開始番地を保存        */
    addi  r5,  r5, %lo(dispatch_r)
    stw   r5,  TCB_pc(r4)          /* 実行再開番地をTCBに保存   */
    br    dispatcher

dispatch_r:
    ldw   r16,  0(sp)              /* レジスタを復帰 */
    ldw   r17,  4(sp)
    ldw   r18,  8(sp)
    ldw   r19, 12(sp)
    ldw   r20, 16(sp)
    ldw   r21, 20(sp)
    ldw   r22, 24(sp)
    ldw   r23, 28(sp)
    ldw   fp,  32(sp)     
    /*
     * タスク例外ルーチンの起動
     *  dispatch_r は dispatcher から呼び出されるため，
     *  tcb のアドレスは r4 に入っている
     */    
    ldw  r5, TCB_enatex(r4)       /* r5 <- enatex */
#if TCB_enatex_mask > 0xffff
    andhi r6, r5, %hi(TCB_enatex_mask)
#else
    andi r6, r5, %lo(TCB_enatex_mask)
#endif /* TCB_enatex_mask > 0xffff */
    beq  r6, zero, dispatch_r_1   /* enatex が FALSE ならリターン */
    ldw  r7, TCB_texptn(r4)       /* r7 <- texptn, texptnが0でなければ */
    beq  r7, zero, dispatch_r_1    
    call call_texrtn              /* タスク例外ルーチンの呼び出し */

dispatch_r_1:
    ldw   ra, 36(sp)              /* 残りのレジスタを復帰 */    
    addi  sp, sp, 40                
    ret



    .global exit_and_dispatch
exit_and_dispatch:
    stw      zero, %gprel(interrupt_count)(gp)  /* interrupt_count をクリア */
dispatcher:
    /*
     * ここは割込み禁止で来ること
     */    
    ldw   r4,   %gprel(schedtsk)(gp) /* r4 <- schedtsk              */
    stw   r4,   %gprel(runtsk)(gp)   /* schedtsk を runtskに        */
    beq   r4,   zero, dispatcher_1   /* schedtskがあるか            */
    ldw   sp,   TCB_sp(r4)           /* TCBからタスクスタックを復帰 */
    ldw   r5,   TCB_pc(r4)           /* TCBから実行再開番地を復帰   */
    jmp   r5
dispatcher_1:
    /*
     * ここで割込みモードに切り替えるのは，ここで発生する割込み処理
     * にどのスタックを使うかという問題の解決と，割込みハンドラ内で
     * のタスクディスパッチの防止という2つの意味がある．
     */
    movhi sp, %hiadj(STACKTOP)            /* 割込みスタックに変更  */
    addi  sp, sp, %lo(STACKTOP)              
    movi  r5, 1                           /* interrupt_count を1に */
    stw   r5, %gprel(interrupt_count)(gp)  
dispatcher_2:
    wrctl status, r5      /* 割込みの許可 */
    nop
    nop
    nop
    nop
    wrctl status, zero                      /* 割込みの禁止             */
    ldw   r6, %gprel(reqflg)(gp)            /* r6 <- reqflg             */
    beq   r6, zero, dispatcher_2            /* reqflg が FALSE なら     */
    stw   zero, %gprel(interrupt_count)(gp) /* interrupt_count をクリア */ 
    stw   zero, %gprel(reqflg)(gp)          /* reqflg を FALSE に       */ 
    br    dispatcher


    .text
    .global activate_r
    .align 2
activate_r:
    movi  r2, 1
    wrctl status, r2      /* 割込み許可       */
    ldw   r4, 4(sp)       /* 引数(exinf)      */
    ldw   r2, 0(sp)       /* タスクの実行番地 */
    addi  sp, sp, 8
    movhi ra, %hiadj(ext_tsk)
    addi  ra, ra, %lo(ext_tsk)
    jmp   r2              /* タスクの実行開始 */


