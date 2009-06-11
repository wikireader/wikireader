;;; debug - simple debug print

;;;      Copyright 2009 Christopher Hall <hsw@openmoko.com>
;;;
;;; Redistribution and use in source and binary forms, with or without
;;; modification, are permitted provided that the following conditions are
;;; met:
;;;
;;;  1. Redistributions of source code must retain the above copyright
;;;     notice, this list of conditions and the following disclaimer.
;;;
;;;  2. Redistributions in binary form must reproduce the above copyright
;;;     notice, this list of conditions and the following disclaimer in
;;;     the documentation and/or other materials provided with the
;;;     distribution.
;;;
;;; THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
;;; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;;; PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
;;; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
;;; BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;;; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
;;; OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
;;; IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

;;;     .include "c33regs.inc"

;;; register usage
;;;  r0 .. r3  must be preserved
;;;  r4        result low
;;;  r5        result high
;;;  r6 .. r9  arguments 1..4
;;; r10 ..r14  reserved
;;; r15        __dp value


        .section .text

;;; print cr and lf
;;; input:
;;; output:
        .global Debug_PutCRLF
Debug_PutCRLF:
        xld.w   %r6, 0x0d
        xcall   PolledSerial_PutChar
        xld.w   %r6, 0x0a
        xjp     PolledSerial_PutChar


;;; print a space
;;; input:
;;; output:
        .global Debug_PutSpace
Debug_PutSpace:
        xld.w   %r6, 0x20
        xjp     PolledSerial_PutChar


;;; print a string
;;; input:
;;;   r6 = address of '\0' terminated string
;;; output:
;;; temporary:
;;;   r9 = address during loop
        .global Debug_PutString
Debug_PutString:
        ld.w    %r9, %r6
Debug_PutString_loop:
        ld.ub   %r6, [%r9]+
        cmp     %r6, 0
        jreq    Debug_PutString_done
        cmp     %r6, 10
        jreq    Debug_PutString_crlf
        xcall   PolledSerial_PutChar
        jp      Debug_PutString_loop
Debug_PutString_crlf:
        xcall   Debug_PutCRLF
        jp      Debug_PutString_loop
Debug_PutString_done:
        ret


;;; print a hex nibble
;;; input:
;;;   r6 = 4 bit number to print
;;; output:
        .global Debug_PutNibble
Debug_PutNibble:
        xand    %r6, 0x0f
        xadd    %r6, '0'
        xcmp    %r6, '9'
        jrle    Debug_PutNibble_l1
        xadd    %r6, 'a' - '9' - 1
Debug_PutNibble_l1:
        xcall   PolledSerial_PutChar
        ret


;;; print a hex word
;;; input:
;;;   r6 = 32 bit number to print
;;; output:
;;; temporary:
;;;   r9 = thet word being output
        .global Debug_PutHex
Debug_PutHex:
        ld.w    %r9, %r6
        xsrl    %r6, 28
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 24
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 20
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 16
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 12
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 8
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 4
        xcall   Debug_PutNibble

        ld.w    %r6, %r9
        xcall   Debug_PutNibble

        ret



debug_8:
        pushn   %r2
        ld.w    %r1, %r7

        xcall   Debug_PutString
        xcall   Debug_PutSpace

        xld.w   %r2, 8

debug_8_loop:

        ld.w    %r6, [%r1]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace

        xsub    %r2, 1
        jrne    debug_8_loop

        xcall   Debug_PutCRLF
        popn    %r2

        ret

;;; print a hex word
;;; input:
;;; output:
;;; temporary:
        .global xdebug
xdebug:
        xcall   Debug_PutCRLF

        xld.w   %r6, debug_regs
        xcall   Debug_PutString
        ld.w    %r6, %r0
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, %r1
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, %r2
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, %r3
        xcall   Debug_PutHex
        xcall   Debug_PutSpace

        xcall   Debug_PutSpace
        ld.w    %r6, %sp
        xcall   Debug_PutHex

        xcall   Debug_PutCRLF

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
