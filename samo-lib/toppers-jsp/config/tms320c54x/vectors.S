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
; *  @(#) $Id: vectors.S,v 1.6 2003/12/04 08:25:44 honda Exp $
; *

       .include "c5402dsk.h"
       .sect   "vectors"
       .align  0x80
       .mmregs

RESET: B _start
       NOP
       NOP

NMI:   RETE
       NOP
       NOP
       NOP

; ソフトウェア割り込み(TRAP,INTM)
sint17 .space 4*16
sint18 .space 4*16
sint19 .space 4*16
sint20 .space 4*16
sint21 .space 4*16
sint22 .space 4*16
sint23 .space 4*16
sint24 .space 4*16
sint25 .space 4*16
sint26 .space 4*16
sint27 .space 4*16
sint28 .space 4*16
sint29 .space 4*16
sint30 .space 4*16

int0:  BD   _interrupt
       STM  #16,_INT_VECNO

int1:  BD   _interrupt
       STM  #17,_INT_VECNO

int2:  BD   _interrupt
       STM  #18,_INT_VECNO

tint0: BD  _interrupt
       STM  #19,_INT_VECNO

rint0: BD  _interrupt
       STM  #20,_INT_VECNO

xint0: BD  _interrupt
       STM  #21,_INT_VECNO

dma0:  BD  _interrupt
       STM  #22,_INT_VECNO

tint1: BD  _interrupt
       STM  #23,_INT_VECNO

int3:  BD  _interrupt
       STM  #24,_INT_VECNO

hpi:   BD  _interrupt
       STM  #25,_INT_VECNO

rint1: BD  _interrupt
       STM  #26,_INT_VECNO

xint1: BD  _interrupt
       STM  #27,_INT_VECNO

dma4:  BD  _interrupt
       STM  #28,_INT_VECNO

dma5:  BD  _interrupt
       STM  #29,_INT_VECNO
