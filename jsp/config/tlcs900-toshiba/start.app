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
;   @(#) $Id: start.app,v 1.1 2006/04/10 08:19:25 honda Exp $
;

	$MAXIMUM
	module start_app

;
;	カーネル用のスタートアップモジュール（TLCS-900用）
;
#include "jsp_rename.h"
#include "cpu_rename.h"
#include "sys_rename.h"
#include "tlcs900vec.inc"

; 外部参照シンボル定義
	extern large	__StackTop
	extern large	__AreaRAM
	extern large	__AreaRAM_size
	extern large	__DataRAM
	extern large	__DataRAM_ini
	extern large	__DataRAM_size
	extern large	hardware_init_hook
	extern large	software_init_hook
	extern large	_kernel_start

f_code section code large align=1,1

	public _start
_start:
; 割込み禁止
	di		; リセット時はIFFに7が設定されるためDI状態となる
			; ハードウェアリセット時はdi命令不要
			; ソフトにて jp _start する可能性を考慮しコードを残す

; スタック初期化
	ld		xsp, __StackTop

; INTNESTレジスタ初期化
	ld		hl, 0
	ldc		intnest, hl

; hardware_init_hook を呼出し（0 でない場合）
; ターゲットハードウェアに依存して必要な初期化処理がある場合
; は，hardware_init_hook という関数を用意する．
	ld		xhl, hardware_init_hook
	cp		xhl, 0
	call	nz, hardware_init_hook

; 初期値無しRAM領域のクリア
	ld		xde, __AreaRAM			; 先頭番地とサイズ取得
	ld		xbc, __AreaRAM_size
	ld		ix, bc					; 最下位bit保持(後で使用)
	srl		1, xbc					; 2byte単位で処理するためサイズ/2
	jr		z, area_clr_byte
	ld		xhl, xde				; 先頭番地保持(後で使用)
	ldw		(xde+), 0				; 先頭番地クリア
	sub		xbc, 1					; サイズデクリメント
	jr		z, area_clr_byte
	ldirw	(xde+), (xhl+)			; 先頭番地(前で0設定)の値を次番地へ...
									; の要領でBCレジスタ分ブロックロード
	cp		qbc, 0
	jr		eq, area_clr_byte			; 上位16bitが0なら終了
	ld		wa, qbc
area_clr_loop:						; 上位16bit(0x10000単位)での処理
	ldirw	(xde+), (xhl+)			; 0x10000分ブロックロード
	djnz	wa, area_clr_loop
area_clr_byte:
	bit		0, ix					; 領域サイズが奇数なら1byteクリア
	jr		z, area_clr_end
	ldb		(xde), 0
area_clr_end:

; 初期値有りRAM領域へのデータロード
	ld		xde, __DataRAM			; ロード元・ロード先番地とサイズ取得
	ld		xhl, __DataRAM_ini
	ld		xbc, __DataRAM_size
	or		xbc, xbc				; サイズ0なら終了
	jr		z, data_ld_end
	ldirb	(xde+), (xhl+)			; BCレジスタ分ブロックロード
	cp		qbc, 0
	jr		eq, data_ld_end			; 上位16bitが0なら終了
	ld		wa, qbc
data_ld_loop:						; 上位16bit(0x10000単位)での処理
	ldirb	(xde+), (xhl+)			; 0x10000分ブロックロード
	djnz	wa, data_ld_loop
data_ld_end:

; software_init_hook を呼出し（0 でない場合）
; ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
; 理がある場合は，software_init_hook という関数を用意する．
	ld		xhl, software_init_hook
	cp		xhl, 0
	call	nz, software_init_hook

; カーネル起動
	jp		_kernel_start

; EXITループ
exit:
	jr		exit

	end
