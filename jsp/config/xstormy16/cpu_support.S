/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *      プロセッサ依存モジュール アセンブリ言語部（Xstormy16用）
 */

#define _MACRO_ONLY
#include <offset.h>
#include <cpu_sfrs.h>

/*
 *  タスク・割り込み・例外管理用RAM領域
 */
        .section .data_0,"aw"

                                /*
                                 *  割り込み・例外ネストカウンタ
                                 *  (カーネル起動時を非タスクコンテキストと
                                 *  して扱うため初期値を1とする)
                                 */
        .global _kernel_intnest
_kernel_intnest:
        .hword 1

                                /*
                                 *  割り込みハンドラテーブル
                                 *  (UART割り込みを含め17個)
                                 */
        .global _kernel_ih_table
_kernel_ih_table:
        .hword 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

                                /*
                                 *  例外ハンドラテーブル
                                 *  (未定義命令/奇数番地ワードアクセス/
                                 *  メモリ外/reserved)
                                 */
        .global _kernel_eh_table
_kernel_eh_table:
        .hword 0,0,0,0

/*
 *  割り込みベクトル
 *  (このsectionは8000h-8083hにリンクされる)
 */
        .section .int_vec,"ax"
        .global _start
        .align 1
_start:
	clr1 psw,#7
        br _kernel_int_irq_0
	clr1 psw,#7
        br _kernel_int_irq_1
	clr1 psw,#7
        br _kernel_int_irq_2
	clr1 psw,#7
        br _kernel_int_irq_3
	clr1 psw,#7
        br _kernel_int_irq_4
	clr1 psw,#7
        br _kernel_int_irq_5
	clr1 psw,#7
        br _kernel_int_irq_6
	clr1 psw,#7
        br _kernel_int_irq_7
	clr1 psw,#7
        br _kernel_int_irq_8
	clr1 psw,#7
        br _kernel_int_irq_9
	clr1 psw,#7
        br _kernel_int_irq_a
	clr1 psw,#7
        br _kernel_int_irq_b
	clr1 psw,#7
        br _kernel_int_irq_c
	clr1 psw,#7
        br _kernel_int_irq_d
	clr1 psw,#7
        br _kernel_int_irq_e
	clr1 psw,#7
        br _kernel_int_irq_f
        .org 0x80
        jmpf _kernel_int_irq_ex

/*
 *  割り込みハンドラ入り口
 */
_kernel_int_irq_0:
                                /*
                                 *  ワッチドッグタイマ割り込みかRESETかの判断
                                 */
        bn __WDTCR,#4,_after_reset
        bp __WDTCR,#5,_after_reset
        push r0
        mov r0,#_kernel_ih_table+0
        br _int_common
_after_reset:
	jmpf _int_reset
_kernel_int_irq_1:
        push r0
        mov r0,#_kernel_ih_table+2
        br _int_common
_kernel_int_irq_2:
        push r0
        mov r0,#_kernel_ih_table+4
        br _int_common
_kernel_int_irq_3:
        push r0
        mov r0,#_kernel_ih_table+6
        br _int_common
_kernel_int_irq_4:
        push r0
        mov r0,#_kernel_ih_table+8
        br _int_common
_kernel_int_irq_5:
        push r0
        mov r0,#_kernel_ih_table+10
        br _int_common
_kernel_int_irq_6:
        push r0
        mov r0,#_kernel_ih_table+12
        br _int_common
_kernel_int_irq_7:
        push r0
        mov r0,#_kernel_ih_table+14
        br _int_common
_kernel_int_irq_8:
        push r0
        mov r0,#_kernel_ih_table+16
        br _int_common
_kernel_int_irq_9:
        push r0
        mov r0,#_kernel_ih_table+18
        br _int_common
_kernel_int_irq_a:
        push r0
        mov r0,#_kernel_ih_table+20
        br _int_common
_kernel_int_irq_b:
        push r0
        mov r0,#_kernel_ih_table+22
        br _int_common
_kernel_int_irq_c:
        push r0
        mov r0,#_kernel_ih_table+24
        br _int_common
_kernel_int_irq_d:
        push r0
        mov r0,#_kernel_ih_table+26
        br _int_common
_kernel_int_irq_e:
        push r0
        mov r0,#_kernel_ih_table+28
        br _int_common
_kernel_int_irq_f:
        push r0
        mov r0,#_kernel_ih_table+30
        br _int_common

/*
 *  割り込みハンドラ入り口処理
 */
_int_common:
                                /*
                                 *  r1/r2の退避(r0は退避済み)
                                 */
        push r1
        push r2
                                /*
                                 *  ネストした割り込みかどうかチェック
                                 */
        mov r1,_kernel_intnest
        bz interrupt_from_task

/*
 *  ネストした割り込みの処理
 */
                                /*
                                 *  ネストカウンタをインクリメントし、
                                 *  上位の割り込みを許可
                                 */
        inc r1
        mov _kernel_intnest,r1
        set1 psw,#7
                                /*
                                 *  r3-r9(caller saved register)の退避
                                 */
        push r3
        push r4
        push r5
        push r6
        push r7
        push r8
        push r9
                                /*
                                 *  割り込みハンドラの呼び出し
                                 */
        mov r1,(r0)
        bz 1f
        mov r8,#0
        call r8,r1
1:
                                /*
                                 *  r1-r9の復元
                                 */
        pop r9
        pop r8
        pop r7
        pop r6
        pop r5
        pop r4
        pop r3
        pop r2
        pop r1
                                /*
                                 *  ネストカウンタをデクリメントし、
                                 *  割り込まれた元の割り込みハンドラへ戻る
                                 */
        clr1 psw,#7
        mov r0,_kernel_intnest
        dec r0
        mov _kernel_intnest,r0
        pop r0
        iret

/*
 *  ネストしていない割り込みの処理
 */
interrupt_from_task:
                                /*
                                 *  ネストカウンタをインクリメント
                                 */
        mov _kernel_intnest,#1
                                /*
                                 *  スタックを非タスク用に切り替え、上位
                                 *  割り込みを許可
                                 */
        mov r1,sp
        mov sp,#__stack
        set1 psw,#7
                                /*
                                 *  r3-r9(caller saved register)を
                                 *  タスク側スタックに退避
                                 */
        mov (r1++),r3
        mov (r1++),r4
        mov (r1++),r5
        mov (r1++),r6
        mov (r1++),r7
        mov r3,r8
        mov (r1++),r3
        mov r3,r9
        mov (r1++),r3
                                /*
                                 *  タスク側スタックポインタを割り込み用
                                 *  スタックに退避
                                 */
        push r1
                                /*
                                 *  割り込みハンドラの呼び出し
                                 */
        mov r1,(r0)
        bz 1f
        mov r8,#0
        call r8,r1
1:
                                /*
                                 *  割り込みを禁止
                                 */
        clr1 psw,#7

/*
 *  割り込み、例外共通の出口処理
 */
ret_int_and_exc:
                                /*
                                 *  タスク用スタックに戻す
                                 */
        pop r1
        mov sp,r1
                                /*
                                 *  ネストカウンタをデクリメント
                                 */
        mov _kernel_intnest,#0
                                /*
                                 *  reqflgをチェックし、FALSEなら、元の
                                 *  タスクに戻る処理にジャンプ
                                 */
        mov r0,#_kernel_reqflg
        mov r1,(r0)
        bz ret_to_task_int

/*
 *  割り込み中にタスクの切り替え要求が発生した場合の処理
 */
                                /*
                                 *  reqflgをクリア
                                 */
        mov r1,#0
        mov (r0),r1
                                /*
                                 *  ディスパッチ禁止か、runtsk=schedtsk
                                 *  なら、元のタスクに戻る処理へ
                                 */
        mov r0,#_kernel_enadsp
        mov r1,(r0)
        bz ret_int_1
        mov r0,#_kernel_runtsk
        mov r1,(r0)
        mov r2,#_kernel_schedtsk
        mov r3,(r2)
        bz r1,r3,ret_int_1
                                /*
                                 *  タスクの切り替えのため、r10-13/spを退避
                                 */
        push r10
        push r11
        push r12
        push r13
        mov r7,sp
        mov (r1,+TCB_sp),r7             /* r1 points runtsk */
                                /*
                                 *  mode=1(ret_int_rに戻る)を設定し
                                 *  ディスパッチャへ
                                 */
        mov r7,#1
        mov (r1,+TCB_mode),r7
        br dispatcher

/*
 *  割り込み中のディスパッチからの復帰処理
 */
ret_int_r:

                                /*
                                 *  r10-13を復元
                                 */
        pop r13
        pop r12
        pop r11
        pop r10

/*
 *  元のタスクに戻る処理
 */
ret_int_1:
        mov r0,#_kernel_runtsk
        mov r1,(r0)
                                /*
                                 *  タスク例外ハンドラ許可のチェック
                                 */
        mov.b r7,(r1,+TCB_enatex)
        bn r7,#TCB_enatex_bit,ret_to_task_int
                                /*
                                 *  タスク例外発生のチェック
                                 */
        mov r7,(r1,+TCB_texptn)
        bz ret_to_task_int
                                /*
                                 *  タスク例外ハンドラの呼び出し
                                 */
        callf _kernel_call_texrtn
                                /*
                                 *  r0-r9を復元し、元のタスクに戻る
                                 */
ret_to_task_int:
        pop r9
        pop r8
        pop r7
        pop r6
        pop r5
        pop r4
        pop r3
        pop r2
        pop r1
        pop r0
        iret

/*
 *  例外ハンドラ入り口
 */
        .global _kernel_int_irq_ex
_kernel_int_irq_ex:
        push r0
                                /*
                                 *  UART例外は割り込みとして処理
                                 */
        bn __U0CR,#0,1f                 /* UART0 RxRdy */
        bn __U0CR,#1,1f
        mov r0,#_kernel_ih_table+32
        br _int_common
1:
        bn __U0CR,#2,2f                 /* UART0 TxEmpty */
        bn __U0CR,#3,2f
        mov r0,#_kernel_ih_table+32
        br _int_common
2:
        bn __U1CR,#0,3f                 /* UART1 RxRdy */
        bn __U1CR,#1,3f
        mov r0,#_kernel_ih_table+32
        br _int_common
3:
        bn __U1CR,#2,4f                 /* UART1 TxEmpty */
        bn __U1CR,#3,4f
        mov r0,#_kernel_ih_table+32
        br _int_common
4:
                                /*
                                 *  r1,r2を退避し、r2に例外発生時のSPを設定
                                 *  (例外ハンドラへの引数)
                                 */
        push r1
        push r2
        mov r2,sp
        sub r2,#6
                                /*
                                 *  発生した例外を判定/クリアする
                                 */
        bn __EXCPL,#1,5f                /* undefined instruction */
        set1 psw,#11
        clr1 __EXCPL,#1
        clr1 psw,#11
        mov r0,#_kernel_eh_table+0
        br _exc_common
5:
        bn __EXCPL,#3,6f                /* word access to odd address */
        set1 psw,#11
        clr1 __EXCPL,#3
        clr1 psw,#11
        mov r0,#_kernel_eh_table+2
        br _exc_common
6:
        bn __EXCPL,#5,7f                /* acccess out of mamoey range */
        set1 psw,#11
        clr1 __EXCPL,#5
        clr1 psw,#11
        mov r0,#_kernel_eh_table+4
        br _exc_common
7:
        mov r0,#_kernel_eh_table+6      /* unknown exception */
        br _exc_common

/*
 *  例外ハンドラ入り口処理
 */
_exc_common:
                                /*
                                 *  ネストした例外かどうかチェック
                                 *  (例外は、他の割り込みにネストされる
                                 *  ことは無いので、割り込み禁止処理は
                                 *  不要)
                                 */
        mov r1,_kernel_intnest
        bz exception_from_task

/*
 *  ネストした例外の処理
 */
                                /*
                                 *  ネストカウンタをインクリメント
                                 */
        inc r1
        mov _kernel_intnest,r1
                                /*
                                 *  r3-r9(caller saved register)の退避
                                 */
        push r3
        push r4
        push r5
        push r6
        push r7
        push r8
        push r9
                                /*
                                 *  例外ハンドラの呼び出し
                                 */
        mov r1,(r0)
        bz 1f
        mov r8,#0
        call r8,r1
1:
                                /*
                                 *  r1-r9の復元
                                 */
        pop r9
        pop r8
        pop r7
        pop r6
        pop r5
        pop r4
        pop r3
        pop r2
        pop r1
                                /*
                                 *  ネストカウンタをデクリメントし、
                                 *  割り込まれた元の割り込みハンドラへ戻る
                                 */
        mov r0,_kernel_intnest
        dec r0
        mov _kernel_intnest,r0
        pop r0
        iret

/*
 *  ネストしていない例外の処理
 */
exception_from_task:
                                /*
                                 *  ネストカウンタをインクリメント
                                 */
        mov _kernel_intnest,#1
                                /*
                                 *  スタックを非タスク用に切り替える
                                 */
        mov r1,sp
        mov sp,#__stack
                                /*
                                 *  r3-r9(caller saved register)を
                                 *  タスク側スタックに退避
                                 */
        mov (r1++),r3
        mov (r1++),r4
        mov (r1++),r5
        mov (r1++),r6
        mov (r1++),r7
        mov r3,r8
        mov (r1++),r3
        mov r3,r9
        mov (r1++),r3
                                /*
                                 *  タスク側スタックポインタを割り込み用
                                 *  スタックに退避
                                 */
        push r1
                                /*
                                 *  例外ハンドラの呼び出し
                                 */
        mov r1,(r0)
        bz 1f
        mov r8,#0
        call r8,r1
1:
                                /*
                                 *  割り込み・例外共通出口処理へ
                                 */
        br ret_int_and_exc

/*
 *  exit_and_dispatch()
 *  元のコンテキストを保存せず新しいタスクへ切り替える
 *  (カーネル開始後最初のタスクの起動を含む)
 */
        .global _kernel_exit_and_dispatch
_kernel_exit_and_dispatch:
                                /*
                                 *  ネストカウンタをクリア
                                 *  (カーネル開始直後等、前に実行されていた
                                 *  タスクが無い場合、非タスクコンテキスト
                                 *  実行中を示すため、ネストカウンタは1に
                                 *  なっている。これをクリア)
                                 */
        mov _kernel_intnest,#0
                                /*
                                 *  ディスパッチャでジャンプ
                                 */
        br dispatcher

/*
 *  dispatch()
 *  元のコンテキストを保存し新しいタスクへ切り替える
 */
        .global _kernel_dispatch
_kernel_dispatch:
                                /*
                                 *  r10-r14(callee-saved egister)を保存
                                 */
        push r10
        push r11
        push r12
        push r13
        push psw
                                /*
                                 *  SPをruntsk->tskctxb.spに保存
                                 */
        mov r0,#_kernel_runtsk
        mov r1,(r0)
        mov r7,sp
        mov (r1,+TCB_sp),r7
                                /*
                                 *  mode=0(dispatch_rに戻る)を設定し
                                 *  ディスパッチャへ
                                 */
        mov r7,#0
        mov (r1,+TCB_mode),r7
        br dispatcher

/*
 *  dispatch()からの戻り処理
 *  (r3はTCBを示す)
 */
dispatch_r:
                                /*
                                 *  r10-r14(callee-saved egister)を復元
                                 */
        pop psw
        pop r13
        pop r12
        pop r11
        pop r10
                                /*
                                 *  タスク例外をチェックし、許可&
                                 *  例外ありなら呼び出す
                                 */
        mov.b r7,(r3,+TCB_enatex)
        bn r7,#TCB_enatex_bit,1f
        mov r7,(r3,+TCB_texptn)
        bz 1f
        jmpf _kernel_call_texrtn
1:
                                /*
                                 *  dispatch()を呼んだタスクに戻る
                                 */
        ret

/*
 *  create_context()/activate_context()
 *  新しいタスクの起動処理
 *  (r2はTCBを示す)
 */
        .global _kernel_create_context
_kernel_create_context:
        ret

        .global _kernel_activate_context
_kernel_activate_context:
                                /*
                                 *  スタックポインタを初期化
                                 *  tcb->tskctxb.sp=tcb->tinib->stk
                                 */
        mov r0,(r2,+TCB_tinib)
        mov r1,(r0,+TINIB_stk)
        mov (r2,+TCB_sp),r1
                                /*
                                 *  mode=2(activate_rに戻る)を設定
                                 */
        mov r1,#2
        mov (r2,+TCB_mode),r1
        ret

/*
 *  activate_context()されたタスクの起動処理
 *  (r3はTCBを示す)
 */
activate_r:
                                /*
                                 *  タスクがreturnで終了したときに、
                                 *  ext_tsk()が呼ばれるよう、これを
                                 *  スタックに積む
                                 */
        mov r1,#@lo(ext_tsk)
        push r1
        mov r2,#@hi(ext_tsk)
        push r2
                                /*
                                 *  タスク開始アドレスをスタックに積む
                                 */
        mov r0,(r3,+TCB_tinib)
        mov r1,(r0,+TINIB_task)
        push r1
        mov r2,#0
        push r2
                                /*
                                 *  PSW初期値をスタックに積む
                                 */
        mov r1,#0x0080
        push r1
                                /*
                                 *  r2=exinf
                                 */
        mov r2,(r0,+TINIB_exinf)
                                /*
                                 *  タスク起動
                                 */
        iret

/*
 *  ディスパッチャー
 */
dispatcher:
                                /*
                                 *  runtsk = schedtsk
                                 */
        mov r2,#_kernel_schedtsk
        mov r3,(r2)
        mov r0,#_kernel_runtsk
        mov (r0),r3
                                /*
                                 *  schedtsk = NULL(実行可能なタスク無し)
                                 *  の場合、アイドル処理へ
                                 */
        bz dispatcher_1
                                /*
                                 *  SPを復元
                                 */
        mov r7,(r3,+TCB_sp)
        mov sp,r7
                                /*
                                 *  modeにしたがって、
                                 *   =0 : dispatch_r
                                 *   =1 : ret_int_r
                                 *   =2 : activate_r
                                 */
        mov r7,(r3,+TCB_mode)
        bz dispatch_r
        bz r7,#1,ret_int_r
        br activate_r

/*
 *  アイドル処理(実行可能タスク無し)
 */
dispatcher_1:
                                /*
                                 *  スタックを非タスク用に切り替える
                                 */
        mov sp,#__stack
                                /*
                                 *  非タスクを示すために
                                 *  ネストカウンタに1をセット
                                 */
        mov _kernel_intnest,#1
                                /*
                                 *  タスク実行可能待ちのループ
                                 */
dispatcher_2:
                                /*
                                 *  割り込みレベルを0に(IL=0)
                                 */
        mov psw,#0x0000
                                /*
                                 *  アイドルルーチンを呼び出す
                                 */
        callf _kernel_idle
                                /*
                                 *  タスクが実行可能になったかのチェック
                                 *  (reqflg=1?)
                                 */
        mov r0,#_kernel_reqflg
        mov r1,(r0)
        bz dispatcher_2
                                /*
                                 *  reqflgとネストカウンタをクリアし、
                                 *  ディスパッチャに戻る
                                 */
        mov r1,#0
        mov (r0),r1
        mov _kernel_intnest,#0
        br dispatcher

/*
 *  デフォルトのアイドルルーチン
 */
        .weak _kernel_idle
_kernel_idle:
                                /*
                                 *  割り込みを許可
                                 */
        set1 psw,#7
                                /*
                                 *  割り込み待ち
                                 */
        halt
                                /*
                                 *  割り込みを禁止
                                 */
        clr1 psw,#7
        ret

        .end

