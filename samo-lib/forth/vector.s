;;; vector.s
;;;
;;; Copyright 2009 Christopher Hall <hsw@openmoko.com>
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

        .include "regs.inc"

;;; register usage
;;;  r0 .. r3  must be preserved
;;;  r4        result low
;;;  r5        result high
;;;  r6 .. r9  arguments 1..4
;;; r10 ..r14  reserved
;;; r15        __dp value


        .section .text

;;; undefined interrupt
;;; input:
;;; output:
        .global undefined_interrupt
undefined_interrupt:
        xcall   panic
undefined_interrupt_size = . -undefined_interrupt
        .rept   vector_count
        xcall   panic
        .endr


;;; initialise vector table
;;; input:
;;; output:
        .global Vector_initialise
Vector_initialise:

        DISABLE_INTERRUPTS

        xld.w   %r4, R32_CMU_PROTECT
        xld.w   %r5, CMU_PROTECT_OFF
        ld.w    [%r4], %r5

        xld.w   %r6, R32_CMU_OPT
        ld.w    %r7, [%r6]
        or      %r7, 1
        ld.w    [%r6], %r7

        ld.w    %r5, CMU_PROTECT_ON
        ld.w    [%r4], %r5

        xld.w   %r8, __START_VectorTable
	ld.w    %ttbr, %r8

        xld.w   %r7, undefined_interrupt
        xld.w   %r9, vector_count                     ; fill table
        xld.w   %r10, undefined_interrupt_size
initialise_loop:
        ld.w    [%r8]+, %r7
        add     %r7, %r10
        sub     %r9, 1
        jrne    initialise_loop

        ENABLE_INTERRUPTS
        ret                                           ; exit


;;; get vector
;;; input:
;;;   r6 = vector number
;;; output:
;;;   r4 = vector address
        .global Vector_get
Vector_get:
        sla     %r6, 2
        xld.w   %r8, __START_VectorTable
        add     %r6, %r8
        ld.w    %r4, [%r6]
        ret


;;; set vector
;;; input:
;;;   r6 = vector number
;;;   r7 = vector address
;;; output:
;;;   r4 = previous vector address
        .global Vector_set
Vector_set:
        sla     %r6, 2
        xld.w   %r8, __START_VectorTable
        add     %r6, %r8
        ld.w    %r4, [%r6]
        ld.w    [%r6], %r7
        ret


;;; panic - dump the registers
;;; input:
;;; output:
        .global panic
panic:
        pushn   %r15
        ld.w    %r0, %ahr
        ld.w    %r1, %alr
        pushn   %r1

        xcall   Debug_PutCRLF
        xld.w   %r6, message_panic
        xcall   Debug_PutString
        xcall   Debug_PutCRLF

        xld.w   %r6, message_sp
        xcall   Debug_PutString
        ld.w    %r6, %sp
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        ld.w    %r0, %sp

        xld.w   %r6, message_ahr
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_alr
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_r0
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_r4
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_r8
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_r12
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

;;;
        ld.w    %r6, [%r0]+
        xld.w   %r7, undefined_interrupt
        sub     %r6, %r7
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        xld.w   %r6, undefined_interrupt_size
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF
;;;

        xld.w   %r6, message_ps
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_ip
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

        xld.w   %r6, message_stack
        xcall   Debug_PutString
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutSpace
        ld.w    %r6, [%r0]+
        xcall   Debug_PutHex
        xcall   Debug_PutCRLF

stop:   jp      stop

        popn    %r1
        ld.w    %ahr, %r0
        ld.w    %alr, %r1
        pushn   %r1
        popn    %r15
        ret

message_panic:
        .asciz  "Panic: Register Dump"
message_sp:
        .asciz  "sp:   "
message_ahr:
        .asciz  "ahr:  "
message_alr:
        .asciz  "alr:  "
message_r0:
        .asciz  "r0:   "
message_r4:
        .asciz  "r4:   "
message_r8:
        .asciz  "r8:   "
message_r12:
        .asciz  "r12:  "

message_ps:
        .asciz  "ps:   "

message_ip:
        .asciz  "ip:   "

message_stack:
        .asciz  "[sp]: "

        .balign 4
