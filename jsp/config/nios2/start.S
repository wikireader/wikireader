/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2005 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN    
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
 *  @(#) $Id: start.S,v 1.5 2006/01/13 07:01:03 honda Exp $
 */

#define _MACRO_ONLY
#include <t_config.h>
#include <sys_defs.h>


     .section .entry, "xa"
     .align 2
     .global __reset
__reset:
#if NIOS2_ICACHE_SIZE > 0
    /* 
     * The assumption here is that the instruction cache size is always
     * a power of two.
     */

#if NIOS2_ICACHE_SIZE > 0x8000
    movhi r2, %hi(NIOS2_ICACHE_SIZE)
#else 
    movui r2, NIOS2_ICACHE_SIZE
#endif
0:
    initi r2
    addi r2, r2, -NIOS2_ICACHE_LINE_SIZE
    bgt r2, zero, 0b
1:

    .pushsection .debug_alt_sim_info
    .int 1, 1, 0b, 1b
    .popsection
#endif /* NIOS2_ICACHE_SIZE > 0 */

#ifndef BOOT_ROM
    movhi r3, %hiadj(_start)
    addi r3, r3, %lo(_start)
    jmp r3    
#else
    br _boot_rom
#endif  /* BOOT_ROM */


#ifdef BOOT_ROM
    .section .text
    .align 2
    .globl _
    .type _boot_rom, @function
_boot_rom:
#if NIOS2_DCACHE_SIZE > 0
#if NIOS2_DCACHE_SIZE > 0x8000
    movhi r2, %hi(NIOS2_DCACHE_SIZE)
#else
    movui r2, NIOS2_DCACHE_SIZE
#endif
0:
    initd 0(r2)
    addi r2, r2, -NIOS2_DCACHE_LINE_SIZE
    bgt r2, zero, 0b
1:
    .pushsection .debug_alt_sim_info
    .int 2, 1, 0b, 1b
    .popsection
#endif /* NIOS2_DCACHE_SIZE > 0 */

_boot_rom_1:
    movhi   r3, %hiadj(ROM_BASE)  /* r3 source */
    addi    r3, r3, %lo(ROM_BASE)
    movhi   r4, %hiadj(RAM_BASE)  /* r4 destination */
    addi    r4, r4, %lo(RAM_BASE)
    movhi   r2, %hiadj(_edata)
    addi    r2, r2, %lo(_edata)

_flash_copy:    
    ldw     r5, 0(r3)
    addi    r3, r3, 1
    stw     r5, 0(r4)
    addi    r4, r4, 1
    bgeu    r2, r4, _flash_copy

    movhi r2, %hiadj(_start)
    addi  r2, r2, %lo(_start)
    jmp   r2    
     nop
    
#endif /* BOOT_ROM */


    .section .text
    .align 2
    .globl _start
    .type _start, @function
_start:
#if NIOS2_DCACHE_SIZE > 0
#if NIOS2_DCACHE_SIZE > 0x8000
    movhi r2, %hi(NIOS2_DCACHE_SIZE)
#else
    movui r2, NIOS2_DCACHE_SIZE
#endif
0:
    initd 0(r2)
    addi r2, r2, -NIOS2_DCACHE_LINE_SIZE
    bgt r2, zero, 0b
1:
    .pushsection .debug_alt_sim_info
    .int 2, 1, 0b, 1b
    .popsection
#endif /* NIOS2_DCACHE_SIZE > 0 */

    /*
     *  スタックの設定
     */
    movhi sp, %hiadj(STACKTOP)
    addi  sp, sp, %lo(STACKTOP)
    
    /* 
     * set up the global pointer. 
     */
    movhi gp, %hiadj(_gp)
    addi  gp, gp, %lo(_gp)

start_0:
    movhi r2, %hiadj(_hardware_init_hook)
    addi  r2, r2, %lo(_hardware_init_hook)
    beq   zero, r2, start_1
    callr r2
    
    /*
     * Clear bss.
     */
start_1:
    movhi r3, %hiadj(__bss_start)
    addi  r3, r3, %lo(__bss_start)

    movhi r2, %hiadj(__bss_end)
    addi  r2, r2, %lo(__bss_end)

    beq   r3, r2, start_3
start_2:
    stw  zero, (r3)
    addi r3, r3, 4
    bltu r3, r2, start_2

start_3:
    movhi r2, %hiadj(_software_init_hook)
    addi  r2, r2, %lo(_software_init_hook)
    beq   zero, r2, start_4
    callr r2

start_4:
    
    call kernel_start

crt0_end:
    br crt0_end    
