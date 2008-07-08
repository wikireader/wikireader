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
; *  @(#) $Id: start.S,v 1.6 2003/12/04 08:25:44 honda Exp $
; *

        .c_mode
        .mmregs
        .include  "c5402dsk.h"
        .global   _main
        .align    1
;スタック領域にはデータ領域の最後の余った領域を割り当てる。
;__stack:.usect ".stack",0
        .text
_start:
        ;******************************************
        ;      ステータスレジスタの初期化
        ;******************************************
        SSBX   INTM                    ;すべてのマスカブル割り込みを禁止
        STM    #0x0FFFF,IFR            ;IFRのクリア

        SSBX   SXM                     ;符号ビットセット
        SSBX   CPL                     ;コンパイラ互換モード
        RSBX   OVM                     ;非飽和モード
        LD     #0,ARP                  ;補助レジスタポインタ
        RSBX   CMPT
        RSBX   C16
        RSBX   FRCT
        
        ;******************************************
        ;     スタックポインタの初期化
        ;******************************************
        STM    #STACKTOP,SP
        ANDM   #0x0fffe,*(SP)          ;SPを偶数アドレスにアライン
        
        ;******************************************
        ;     _hardware_init_hook の呼び出し
        ;******************************************
start_0:
        LD     #_hardware_init_hook,A
        CC     _hardware_init_hook,ANEQ
        
        ;******************************************
        ;     bssセクションの初期化
        ;******************************************
start_1:
        STM    #___bss__,AR2
        RPTZ  A,#___end__-___bss__-1
        STL    A,*AR2+

        LD     #cinit,A                ;cinit=-1ならcinitは無い
        ADD    #1,A,B
        BC     start_4,BEQ

        
        RSBX   SXM                     ;符号ビットクリア
        NOP
        LD     #cinit,A
        B      start_2
start_3:
        READA  *(AR2)                  ;AR2 = 変数のアドレス
        ADD    #1,A                    ;初期化データへのポインタを求める
        RPT    *(AR1)                  ;length+1 繰り返す 
        READA  *AR2+                   ;変数を初期化
        ADD    *(AR1),A                ;
        ADD    #1,A                    ;データ長分だけポインタを進める
start_2:
        READA  *(AR1)                  ;AR1 = length
        ADD    #1,A                    ;A += 1
        BANZ   start_3,*AR1-           ;if (length != 0) length--、ジャンプ
        
        ;*****************************************
        ;    software_init_hookを呼び出す
        ;*****************************************
start_4:
        LD     #_software_init_hook,A
        CC     _software_init_hook,ANEQ

;        ST     #1,*(_nest)
        CALL   _kernel_start
        
        
        
          