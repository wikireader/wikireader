; *
; *  TOPPERS/JSP Kernel
; *      Toyohashi Open Platform for Embedded Real-Time Systems/
; *      Just Standard Profile Kernel
; * 
; *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
; *                              Toyohashi Univ. of Technology, JAPAN
; * 
; *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
; *  によって公表されている GNU General Public License の Version 2 に記
; *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
; *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
; *  利用と呼ぶ）することを無償で許諾する．
; *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
; *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
; *      スコード中に含まれていること．
; *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
; *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
; *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
; *      の無保証規定を掲載すること．
; *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
; *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
; *      と．
; *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
; *        作権表示，この利用条件および下記の無保証規定を掲載すること．
; *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
; *        報告すること．
; *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
; *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
; * 
; *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
; *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
; *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
; *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
; * 
; *  @(#) $Id: cpu_support.S,v 1.5 2003/12/04 08:25:44 honda Exp $
; *

        .c_mode
        .align  1
        .global __kernel_dispatch
        .include  "c5402dsk.h"
        .mmregs

__kernel_dispatch:
        PSHM   AR1                       ;AR1,6,7をスタックに保存
        PSHM   AR6                       ;PCは呼び出し時にスタックに保存される
        PSHM   AR7                       ;他の補助レジスタARxは保存する必要はない
        LDM    SP,A
        MVDM   *(__kernel_runtsk),AR1    ;AR1 <- runtsk
        NOP
        STL    A,*AR1(TCB_sp)            ;タスクスタックをTCBに保存
        ST     #dispatch_r,*AR1(TCB_pc)  ;実行再開番地をTCBに保存
        B      dispatcher_1

dispatch_r:
        POPM   AR7                       ;レジスタを復帰
        POPM   AR6
        POPM   AR1 
        B      __kernel_calltex          ;タスク例外ルーチンの呼び出し

        .global __kernel_exit_and_dispatch
__kernel_exit_and_dispatch:
        SSBX   INTM                      ;割り込み禁止
        ST     #0,*(__kernel_nest)               ;カーネルの初期化処理時のため
dispatcher_1:
        LD     *(__kernel_schedtsk),A    ;schedtskがあるか？
        BC     dispatcher_2,AEQ          ;無ければジャンプ
        MVDK   *(__kernel_schedtsk),*(__kernel_runtsk)   ;schedtskをruntskに
        MVDM   *(__kernel_runtsk),AR1
        NOP
        MVDK   *AR1(TCB_sp),SP           ;TCBからスタックを復帰
        LD     *AR1(TCB_pc),A            ;TCBから実行再開番地を復帰
        BACC   A    
        
dispatcher_2:
        STM    #STACKTOP,SP
        ST     #0x01,*(__kernel_nest)
        RSBX   INTM                      ;割り込み許可
        IDLE   1                         ;リセットかマスクされていない割込み待ち
        SSBX   INTM                      ;割り込み禁止
        ST     #0x0,*(__kernel_nest)
        B      dispatcher_1


;
;  タスクの起動処理
;

        .global __kernel_activate_r
;         .global _activate_r
__kernel_activate_r:
;_activate_r:
        RSBX   INTM                      ;割り込み許可
        POPM   AR2
        POPM   AR3
        LDM    AR2,B
        LDM    AR3,A
        BACC   B



        .global _interrupt
_interrupt:
        PSHM   ST0
        PSHM   AL
        PSHM   AH
        PSHM   AG
        PSHM   BL
        PSHM   BH
        PSHM   BG
        PSHM   ST1
        PSHM   AR0
        PSHM   AR2
        PSHM   AR3
        PSHM   AR4
        PSHM   AR5
        PSHM   T
        PSHM   TRN
        PSHM   BK
        PSHM   BRC
        PSHM   RSA
        PSHM   REA
        PSHM   PMST
        LD     *(__kernel_nest),A
        ADDM   #1,*(__kernel_nest)               ;割り込みのネスト回数をインクリメント
        BC     _interrupt_from_int,ANEQ
        LDM    SP,A                      ;スタック入れ替えと
        STM    #STACKTOP,SP              ;元のスタックポインタの
        PSHM   AL                        ;保存を行う
        PSHM   IMR
        MVDM   *(_INT_VECNO),AR2         ;オフセット値をAR2にセット
        STM    #IMR,AR3                  ;同じ割り込みが何度もかからないように
        LD     *AR2(__kernel_int_mask_table),A   ;            ｜
        AND    *AR3,A                    ;            ↓
        STL    A,*AR3                    ;現在の割り込みをマスクする 
        LD     *AR2(__kernel_int_table),A        ;割り込みハンドラのアドレスを取得
        RSBX   INTM                      ;これ以降割り込みを受け受け付ける
        CALA   A                         ;割り込みハンドラへ
        SSBX   INTM                      ;割り込みを禁止
        ADDM   #-1,*(__kernel_nest)              ;デクリメント
        POPM   IMR
        POPM   SP                        ;戻り先がタスクなのでスタックを戻す
        LD     *(__kernel_reqflg),A
        BC     _ret_to_task_int,AEQ      ;reqflgがFALSEならジャンプ
        ST     #0,*(__kernel_reqflg)
        B      ret_int

_interrupt_from_int:
        LDM    SP,A
        ANDM   #0xFFFE,*(SP)
        NOP
        NOP
        PSHM   AL
        PSHM   IMR
        MVDM   *(_INT_VECNO),AR2         ;オフセット値をAR2にセット
        STM    #IMR,AR3                  ;同じ割り込みが何度もかからないように
        LD     *AR2(__kernel_int_mask_table),A   ;            ｜
        AND    *AR3,A                    ;            ↓
        STL    A,*AR3                    ;現在の割り込みをマスクする 
        LD     *AR2(__kernel_int_table),A        ;割り込みハンドラのアドレスを取得
        RSBX   INTM                      ;これ以降割り込みを受け付ける
        CALA   A                         ;割り込みハンドラヘ
        SSBX   INTM                      ;割り込み禁止
        POPM   IMR
        POPM   SP
        ADDM   #-1,*(__kernel_nest)              ;割り込みのネスト回数をデクリメント
_ret_to_task_int:                        ;レジスタを復帰し割り込み元へ戻る。
        POPM   PMST
        POPM   REA
        POPM   RSA
        POPM   BRC
        POPM   BK
        POPM   TRN
        POPM   T
        POPM   AR5
        POPM   AR4
        POPM   AR3
        POPM   AR2
        POPM   AR0
        POPM   ST1
        POPM   BG
        POPM   BH
        POPM   BL
        POPM   AG
        POPM   AH
        POPM   AL
        POPM   ST0
        RETE
        
        .global ret_int
ret_int:
        LD     *(__kernel_enadsp),A
        BC     ret_int_1,AEQ
        MVDM    *(__kernel_runtsk),AR0
        MVDM    *(__kernel_schedtsk),AR2
        NOP
        CMPR   EQ,AR2                    ;runtskとschedtskを比較 
        BC     ret_int_1,TC
        PSHM   AR1
        PSHM   AR6
        PSHM   AR7
        LDM    SP,A
        STL    A,*AR0(TCB_sp)
        ST     #ret_int_r,*AR0(TCB_pc)
        B      dispatcher_1
ret_int_r:
        POPM   AR7
        POPM   AR6
        POPM   AR1
ret_int_1:
        LDM    SP,A
        ANDM   #0xFFFE,*(SP)
        NOP
        NOP
        PSHM   AL
        FRAME  #-1
        LD     #__kernel_calltex,A
        CALA   A
        FRAME  #1
        POPM   SP
        NOP
        POPM   PMST
        POPM   REA
        POPM   RSA
        POPM   BRC
        POPM   BK
        POPM   TRN
        POPM   T
        POPM   AR5
        POPM   AR4
        POPM   AR3
        POPM   AR2
        POPM   AR0
        POPM   ST1
        POPM   BG
        POPM   BH
        POPM   BL
        POPM   AG
        POPM   AH
        POPM   AL
        POPM   ST0
        RETE

;
;   微少時間待ち
;
        .global	_sil_dly_nse
_sil_dly_nse:
        SSBX      SXM
        NOP
        LD	*(AL), A
        SUB	#SIL_DLY_TIM1, A, A
        BC	_sil_dly_nse1, AGT
        RET
_sil_dly_nse1:
        SUB	#SIL_DLY_TIM2, A, A
        BC	_sil_dly_nse1, AGT
        RET

