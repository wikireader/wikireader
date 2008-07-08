;
;   TOPPERS/JSP Kernel
;       Toyohashi Open Platform for Embedded Real-Time Systems/
;       Just Standard Profile Kernel
;
;   Copyright (C) 2006 by Witz Corporation, JAPAN
;
;   上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
;   によって公表されている GNU General Public License の Version 2 に記
;   述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
;   を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
;   利用と呼ぶ）することを無償で許諾する．
;   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
;       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
;       スコード中に含まれていること．
;   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
;       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
;       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
;       の無保証規定を掲載すること．
;   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
;       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
;       と．
;     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
;         作権表示，この利用条件および下記の無保証規定を掲載すること．
;     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
;         報告すること．
;   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
;       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
;
;   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
;   よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
;   含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
;   接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
;
;   @(#) $Id: cpu_support.app,v 1.1 2006/04/10 08:19:25 honda Exp $
;

	$MAXIMUM
	module cpu_support_app

;
;	プロセッサ依存モジュール アセンブリ言語部（TLCS-900L1用）
;

#define	SUPPORT_CHG_IPM
#include "jsp_rename.h"
#include "cpu_rename.h"
#include "sys_rename.h"
#include "offset.inc"

; 外部参照シンボル定義
	extern large	__StackTop
	extern large	_reqflg
	extern large	_enadsp
	extern large	_intcnt
	extern large	_runtsk
	extern large	_schedtsk
	extern large	_call_texrtn
	extern large	_task_intmask


f_code section code large align=1,1

;
;	タスクディスパッチャ
;
;   dispatch は，タスクコンテキスト状態・割込み禁止状態で呼び出さなけ
;   ればならない．exit_and_dispatch も，タスクコンテキスト状態・割込
;   み禁止状態で呼び出すのが原則であるが，カーネル起動時に対応するため，
;   割込みコンテキスト状態で呼び出した場合にも対応している．
;	呼び出し条件: SRのIFF=7 (割込み禁止状態),
;				  intcnt = 0(タスクコンテキスト), タスクスタック
;
	public _dispatch

_dispatch:
	push	xiz			; 関数呼出で保護が必要なレジスタの保存
	ld		xwa, (_runtsk)	; 動作中タスクのTCBを取得
	ld		(xwa+TCB_sp), xsp	;
	lda		xhl, dispatch_r	;
	ld		(xwa+TCB_pc), xhl	;
	jr		dispatcher	;

;
;	呼び出し条件: SRのIFF=7 (割込み禁止状態),
;				  intcnt = 0(タスクコンテキスト), タスクスタック
;
;	xwa にはruntsk のアドレスが格納されている
;
dispatch_r:
	pop		xiz		;
	ld		bc, (xwa+TCB_enatex)	;
	bit		TCB_enatex_bit, bc	;
	jr		z, dispatch_r_1	; enatex が FALSE ならリターン
	cpw		(xwa+TCB_texptn), 0	; タスク例外要因がある場合
	jp		nz, _call_texrtn	;
	; call_texrtnから直接dispatch 呼び出し元へ戻る.
dispatch_r_1:				; タスク例外を実行しない場合
	ret				; dispatch 呼び出し元へ戻る.

;
;	タスク起動時処理
;
;	ここでは, CPUロック解除状態にし, タスクを起動する.
;
;	呼び出し条件: SRのIFF=7 (割込み禁止状態),
;				  intcnt = 0(タスクコンテキスト), タスクスタック
;
	public _activate_r

_activate_r:
	pop		xhl		; タスクの起動番地をwhl に設定
#ifdef SUPPORT_CHG_IPM			/* t_unlock_cpu 相当の処理 */
	push	sr			; 割込みマスクに task_intmask を設定
	ld		wa, (_task_intmask)	;
	andw	(xsp), ~0x7000	;
	or		(xsp), wa	;
	pop		sr			;
#else /* SUPPORT_CHG_IPM */
	ei		0		; 割込み許可
#endif /* SUPPORT_CHG_IPM */
	jp		xhl

;
;	dispatcher呼び出し条件:
;		・すべてのタスクのコンテキストは保存されている.
;		・SRのIFF=7 (割込み禁止状態)
;		・コンテキストはタスクコンテキスト(intcnt=0)
;	dispatcher 呼出時のスタック:
;		dispatch からきた場合: タスクスタック
;		exit_and_dispatch からきた場合: 
;			exit_task からきた場合はタスクスタック
;			カーネル起動時は割込みスタック
;		ret_int からきた場合: タスクスタック
;		dispatcher_2 での割込み待ちからきた場合: 割込みスタック
;
	public _exit_and_dispatch

_exit_and_dispatch:
	ldw		(_intcnt), 0	; ネストカウンタクリア(タスクコンテキスト)
dispatcher:
	ld		xwa, (_schedtsk)	; schedtsk を runtsk に設定
	ld		(_runtsk), xwa	;
	or		xwa, xwa		; 実行するタスクが無ければアイドルループへ
	jr		z, pre_idle_loop	;
	ld		xsp, (xwa+TCB_sp)	; タスクスタックポインタを復帰
	ld		xhl, (xwa+TCB_pc)	;
	jp		xhl			; 実行再開番地へジャンプ
;
;	実行すべきタスクが現れるまで待つ処理
;
pre_idle_loop:
	ld		xsp, __StackTop ; 割込み用のスタックへ切替え
	incw	1, (_intcnt)	; 非タスクコンテキスト
;
;	ここで非タスクコンテキスト,割込みスタックに切り換えたのは,
;	ここで発生する割込み処理にどのスタックを使うかという問題の解決と,
;	割込みハンドラ内でのディスパッチ防止という2つの意味がある．
;
idle_loop:
#ifdef SUPPORT_CHG_IPM			/* t_unlock_cpu 相当の処理 */
	push	sr			; 割込みマスクに task_intmask を設定
	ld		wa, (_task_intmask)	;
	andw	(xsp), ~0x7000	;
	or		(xsp), wa	;
	pop		sr			;
#else /* SUPPORT_CHG_IPM */
	ei		0		; 割込み許可
#endif /* SUPPORT_CHG_IPM */

	halt 		; IDLE2状態へ遷移
	nop			; 上記haltをコメント(nopのみ)にすると
	nop			; 低消費動作を行わないアイドルループとなる.
	nop			; 
	nop			; 
	ei		7			; 割込み禁止
	cpw		(_reqflg), 0	; reqflg が FALSE であれば
	jr		z, idle_loop	;         idle_loop へ
	ldw		(_reqflg), 0	; reqflg <--- FALSE
	decw		1, (_intcnt)	; タスクコンテキストに戻す
	jr		dispatcher		; dispatcher へ戻る

;
;  遅延ディスパッチ処理
;
;	呼び出し条件: SRのIFF=7 (割込み禁止状態),
;				  intcnt = 0(タスクコンテキスト), タスクスタック
;				  reqflg = TRUE
;				  INTNESTレジスタ = 1
;
ret_int:
	ldw		(_reqflg), 0	; reqflg <--- FALSE
	ld		de, 0	;
	ldc		intnest, de	; ディスパッチしたらintnest は0
	push	xiz			; 関数呼出で保護が必要なレジスタの保存
	ld		xwa, (_runtsk)	; runtsk を取得
	cpw		(_enadsp), 0	; enadsp が FALSE なら
	jr		z, ret_int_r	; ret_int_r へ
	cp		xwa, (_schedtsk)	; runtsk とschedtsk が同じなら
	jr		z, ret_int_r	; ret_int_r へ
	or		xwa, xwa	; runtsk = NULL なら
	jr		z, dispatcher	; 保存せずdispather へ
	ld		(xwa+TCB_sp), xsp	;
	lda		xhl, ret_int_r	;
	ld		(xwa+TCB_pc), xhl	;
	jr		dispatcher	;

;
;	呼び出し条件: SRのIFF=7 (割込み禁止状態),
;				  intcnt = 0(タスクコンテキスト), タスクスタック
;				  INTNESTレジスタ = 0
;
;	xwa にはruntsk のアドレスが格納されている
;
ret_int_r:
	pop		xiz
#ifdef SUPPORT_CHG_IPM		/* スタックにあるSR のIFF を操作 */
	ld		de, (_task_intmask)	;
	andw	(xsp+24), ~0x7000	;
	or		(xsp+24), de	;
#endif /* SUPPORT_CHG_IPM */
	ld		bc, (xwa+TCB_enatex)	;
	bit		TCB_enatex_bit, bc	;
	jr		z, nest_ctrl_r	; enatex が FALSE ならリターン
	cpw		(xwa+TCB_texptn), 0	; タスク例外要因がある場合
	call	nz, _call_texrtn	;
nest_ctrl_r:
	ld		de, 1	;
	ldc		intnest, de	; reti用に擬似的にintnest は1
	jr		interrupt_r

;
;	割込み/CPU例外の共通処理
;
;	呼出し条件:
;	・SRのIFF=7.
;	・スタックは多重割り込みなら割込みスタック, そうでなければ
;	  タスクスタック
;	・XHL には割込み/CPU例外ハンドラのアドレスが格納されている.
;	・AW には本割込みの優先度が設定された SR が格納されている.
;
;	レジスタがスタック上にどのように保存されているかを以下に示す.
;	この図では上が低位, 下が高位のアドレスで, スタックは下から
;	上方向に向かって積み上げられるものとする.
;
;	--------------------------------------
;	|        CPU例外ハンドラの引数       |
;	|          CPU例外情報(4byte)        |
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XSP(4byte)             | タスクスタックポインタ．
;	|             ※初段のみ             | 初段の割込みのみ保持される．
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XIY(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XIX(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XDE(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XBC(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	-------------------------------------- <-- 割込み入り口処理で処理される．
;	|             XHL(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	--------------------------------------
;	|             XAW(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	-------------------------------------- <-- 割込み発生時にハードウェア
;	|              SR(2byte)             |     にて処理される． p_excinfは
;	|                                    |     ここを指すように演算する．
;	--------------------------------------
;	|              PC(4byte)             |
;	|                                    |
;	|                                    |
;	|                                    |
;	--------------------------------------
;
;	ハンドラからリターンした後は, 多重割込みでなく, かつ reqflg が
;	TRUE になった時に，ret_int へ分岐する．
;
;	多重割込みかどうかは割込みネストカウンタの値で判定する.
;	intcnt != 0 ならば多重割込みであると判定する.
;	なお，ハードウェア仕様上，割込み発生から割込み禁止を行うまでの間に
;	多重割込みが発生すると，多重割込みであることをソフトウェアで検知で
;	きないため，最初に発生した割込み処理を実行しないままタスクディス
;	パッチする可能性がある．この多重割込みを検知するためにハードウェア
;	INTNEST レジスタを利用する．
;
;	reqflg はCPUロック状態でチェックする. そうでないと，
;	reqflg チェック後に起動された割込みハンドラ内で
;	ディスパッチが要求された場合に，ディスパッチされない.
;
	public _interrupt

_interrupt:
	push	xbc			; スクラッチレジスタの残りを保存
	push	xde			;
	push	xix			;
	push	xiy			;

	ld		xbc, xsp	; CPU例外情報引数に使用するためここで取得
	cpw		(_intcnt), 0	;
	jr		nz, from_int	;
	ld		xsp, __StackTop ; 初段の割込みの場合スタックを切り替え
	push	xbc			; タスクSPの保持
from_int:				;
	incw	1, (_intcnt)	; 割込みネストカウントをインクリメント

	add		xbc, 24	; CPU例外情報引数生成
	push	xbc			; 割込み処理では未使用
	push	wa			;
	pop		sr			; 本割込みより優先度の高い割込み許可
	call	xhl			; 割込みハンドラ/CPU例外処理を呼び出す
	ei		7			; 割込み禁止
	pop		xbc			; スタック数合わせ(CPU例外情報引数)

	decw	1, (_intcnt)	; 割込みネストカウントをデクリメント
	jr		nz, from_int_r	;
	pop		xbc			; 初段の割込みの場合タスクSPの復帰
	ld		xsp, xbc	;
	ldc		de, intnest	; 割込み発生から割込み禁止までの間に多重割込みが
	djnz	de, from_int_r	; 発生している場合はディスパッチしない
	cpw		(_reqflg), 0	; reqflg が TRUE であれば
	jp		nz, ret_int	;              ret_int へ
from_int_r:		;
interrupt_r:	;
	pop		xiy	;スクラッチレジスタを復帰
	pop		xix	;
	pop		xde	;
	pop		xbc	;
	pop		xhl	;
	pop		xwa	;
	reti	;

;
; 未使用割込みの処理
;
	public unused_interrupt

unused_interrupt:
	reti


	end


