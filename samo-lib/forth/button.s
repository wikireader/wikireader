;;; button - serial input dripver for CTP
;;;
;;; Copyright (c) 2009 Openmoko Inc.
;;;
;;; Authors   Christopher Hall <hsw@openmoko.com>
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

        .section .bss

;;; buffer size = 2^n is conveient for modulo operation

BufferSize = 256

;;; buffer is full if: (write + 1) mod size == read
;;; buffer is full if: write == read


RxBuffer:
        .space  BufferSize
RxRead:
        .long   0
RxWrite:
        .long   0


ButtonMask = 0x07                                     ; only 3 bits used
PowerButton = 0x10                                    ; above ButtonMask


        .section .text


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
;;;   r5 = offset of byte to read
;;;   r6 = address of RxRead
;;; temporary:
        .global Button_available
Button_available:
        xld.w   %r6, RxRead
        xld.w   %r4, RxWrite
        ld.w    %r5, [%r6]                            ; read
        ld.w    %r4, [%r4]                            ; write
        cmp     %r5, %r4                              ; read == write ?
        jreq    Button_available_buffer_empty
        ld.w    %r4, 1                                ; TRUE => buffer is not empty
        ret

Button_available_buffer_empty:
        ld.w    %r4, 0                                ; FALSE => buffer is empty
        ret


;;; read a button
;;; input:
;;; output:
;;;   r4 = button
        .global Button_get
Button_get_wait:
        xcall   suspend                               ; suspend until more input
Button_get:
        call    Button_available
        or      %r4, %r4
        jreq    Button_get_wait
        ld.w    %r7, %r5

        xld.w   %r4, RxBuffer
        add     %r5, %r4
        xadd    %r7, 1
        xand    %r7, (BufferSize - 1)

        ld.ub   %r4, [%r5]+                           ; value
        ld.w    [%r6], %r7                            ; update RxRead
        ret


;;; poll the buttons
;;; input:
;;; output:
;;;   r4 = button
        .global Button_poll
Button_poll:
        xld.w   %r4, R8_P6_P6D                        ; address of buttond port
        ld.ub   %r4, [%r4]                            ; read buttond
        xand    %r4, ButtonMask                       ; mask unused bits
        ret


;;; flush the buffer
;;; input:
;;; output:
        .global Button_flush
Button_flush:
        xld.w   %r4, RxRead
        xld.w   %r5, RxWrite
        ld.w    %r6, 0

	DISABLE_INTERRUPTS
        ld.w    [%r4], %r6
        ld.w    [%r5], %r6
	ENABLE_INTERRUPTS

        ret


;;; initialisation
;;; input:
;;; output:
        .global Button_initialise
Button_initialise:
        xld.w   %r6, Vector_Key_input_interrupt_0
        xld.w   %r7, Button_RxInterrupt
        xcall   Vector_set

        xld.w   %r6, Vector_Key_input_interrupt_1
        xld.w   %r7, Button_RxInterrupt
        xcall   Vector_set

        xld.w   %r6, Vector_Port_input_interrupt_3
        xld.w   %r7, Button_P0Interrupt
        xcall   Vector_set

        ld.w    %r5, 0

        xld.w   %r4, RxRead
        ld.w    [%r4], %r5
        xld.w   %r4, RxWrite
        ld.w    [%r4], %r5

	DISABLE_INTERRUPTS

        xld.w   %r4, R8_INT_PK01L                     ; key priority
        xld.w   %r5, 0x66
        ld.b    [%r4], %r5

        xld.w   %r4, R8_INT_FK01_FP03                 ; clear key interrupts
        xld.w   %r5, FK1 | FK0 | FP3 | FP2 | FP1 | FP0
        ld.b    [%r4], %r5

        xld.w   %r4, R8_KINTCOMP_SCPK0                ; comparison = all buttons off
        xld.w   %r5, 0x00
        ld.b    [%r4], %r5

        xld.w   %r4, R8_KINTCOMP_SMPK0                ; enable 3 buttons
        xld.w   %r5, 0x07
        ld.b    [%r4], %r5

        xld.w   %r4, R8_KINTSEL_SPPK01                ; use P60..P62
        xld.w   %r5, 0x04
        ld.b    [%r4], %r5

        xld.w   %r4, R8_INT_PP01L                     ; P0 prority
        xld.w   %r5, 0x06
        ld.b    [%r4], %r5

        xld.w   %r4, R8_PINTSEL_SPT03                 ; select P03 interrupt
        xld.w   %r5, 0x00
        ld.b    [%r4], %r5

        xld.w   %r4, R8_PINTPOL_SPP07                 ; P03 active high
        xld.w   %r5, SPPT3
        ld.b    [%r4], %r5

        xld.w   %r4, R8_PINTEL_SEPT07                 ; P03 edge trigered
        xld.w   %r5, SEPT3
        ld.b    [%r4], %r5

        xld.w   %r4, R8_INT_EK01_EP03                 ; enable KINT0 and Port0
        xld.w   %r5, EK0 | EP3
        ld.b    [%r4], %r5

        ENABLE_INTERRUPTS

        ret


;;; receive new button value
;;; input:
;;; output:
        .global Button_RxInterrupt
Button_RxInterrupt:
        pushn   %r14

        xld.w   %r4, R8_INT_FK01_FP03
        xld.w   %r5, FK1 | FK0
        ld.b    [%r4], %r5

        xld.w   %r0, R8_P6_P6D
        xld.w   %r1, R8_KINTCOMP_SCPK0
        xld.w   %r2, RxRead
        xld.w   %r3, RxWrite

        ld.ub   %r10, [%r0]                           ; button
        xand    %r10, ButtonMask                      ; mask unused bits
        ld.b    [%r1], %r10                           ; update change of state

        ld.w    %r9, [%r3]                            ; RxWrite

        xld.w   %r11, RxBuffer                        ; buffer start
        add     %r11, %r9                             ; + offset

        ld.b    [%r11]+, %r10                         ; store button

        ld.w    %r11, [%r2]                           ; RxRead

        xadd    %r9, 1                                ; next position
        xand    %r9, (BufferSize - 1)                 ; mod buffer size
        cmp     %r11, %r9                             ; read == write?
        jreq    Button_RxInterrupt_buffer_full        ; ...yes => buffer overrun, value lost

        ld.w    [%r3], %r9                            ; update RxWrite

Button_RxInterrupt_buffer_full:

        popn    %r14
        reti


;;; receive power button short pulse
;;; input:
;;; output:
        .global Button_P0Interrupt
Button_P0Interrupt:
        pushn   %r14
        xld.w   %r4, R8_INT_FK01_FP03
        xld.w   %r5, FP3
        ld.b    [%r4], %r5


        xld.w   %r2, RxRead
        xld.w   %r3, RxWrite
        xld.w   %r10, PowerButton

        ld.w    %r9, [%r3]                            ; RxWrite

        xld.w   %r11, RxBuffer                        ; buffer start
        add     %r11, %r9                             ; + offset

        ld.b    [%r11]+, %r10                         ; store button

        ld.w    %r11, [%r2]                           ; RxRead

        xadd    %r9, 1                                ; next position
        xand    %r9, (BufferSize - 1)                 ; mod buffer size
        cmp     %r11, %r9                             ; read == write?
        jreq    Button_P0Interrupt_buffer_full        ; ...yes => buffer overrun, value lost

        ld.w    [%r3], %r9                            ; update RxWrite

Button_P0Interrupt_buffer_full:
        popn    %r14
        reti
