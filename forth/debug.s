;;; debug - simple debug print

        .include "regs.inc"

;;; register usage
;;;  r0 .. r3  must be preserved
;;;  r4        result low
;;;  r5        result high
;;;  r6 .. r9  arguments 1..4
;;; r10 ..r14  reserved
;;; r15        __dp value

        .section .text

debug_8:
        pushn   %r2
        ld.w    %r1, %r7

        xcall   Serial_PutString
        xcall   Serial_PutSpace

        xld.w   %r2, 8

debug_8_loop:

        ld.w    %r6, [%r1]+
        xcall   Serial_PutHex
        xcall   Serial_PutSpace

        xsub    %r2, 1
        jrne    debug_8_loop

        xcall   Serial_PutCRLF
        popn    %r2

        ret

        .global xdebug
xdebug:
        xcall   Serial_PutCRLF

        xld.w   %r6, debug_regs
        xcall   Serial_PutString
        ld.w    %r6, %r0
        xcall   Serial_PutHex
        xcall   Serial_PutSpace
        ld.w    %r6, %r1
        xcall   Serial_PutHex
        xcall   Serial_PutSpace
        ld.w    %r6, %r2
        xcall   Serial_PutHex
        xcall   Serial_PutSpace
        ld.w    %r6, %r3
        xcall   Serial_PutHex
        xcall   Serial_PutSpace

        xcall   Serial_PutSpace
        ld.w    %r6, %sp
        xcall   Serial_PutHex

        xcall   Serial_PutCRLF

        xld.w   %r6, debug_data
        ld.w    %r7, %r1
        xcall   debug_8


        xld.w   %r6, debug_instr
        ld.w    %r7, %r0
        xcall   debug_8

        xld.w   %r6, debug_return
        ld.w    %r7, %sp
        xcall   debug_8

        ret


debug_regs:
        .asciz  "r0..r3 sp: "

debug_instr:
        .asciz  "ip: "

debug_data:
        .asciz  "sp: "

debug_return:
        .asciz  "rp: "

        .balign 4
