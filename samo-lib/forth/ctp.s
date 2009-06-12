;;; Serial - simple serial I/O

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
;;; each element is 8 bytes

ItemSize = 8
BufferSize = 256 * ItemSize

;;; buffer is full if: (write + 1) mod size == read
;;; buffer is full if: write == read


RxBuffer:
        .space  BufferSize
RxRead:
        .long   0
RxWrite:
        .long   0

x:      .long   0
y:      .long   0
state:  .long   0

        .section .text


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
;;;   r5 = offset of byte to read
;;;   r6 = address of RxRead
;;; temporary:
        .global CTP_PositionAvailable
CTP_PositionAvailable:
        xld.w   %r6, RxRead
        xld.w   %r4, RxWrite
        ld.w    %r5, [%r6]                            ; read
        ld.w    %r4, [%r4]                            ; write
        cmp     %r5, %r4                              ; read == write ?
        jreq    CTP_PositionAvailable_buffer_empty
        ld.w    %r4, 1                                ; TRUE => buffer is not empty
        ret

CTP_PositionAvailable_buffer_empty:
        ld.w    %r4, 0                                ; FALSE => buffer is empty
        ret


;;; read a position
;;; input:
;;; output:
;;;   r4 = x  (-1 => end of touch)
;;;   r5 = y  (-1 => end of touch)
        .global CTP_GetPosition
CTP_GetPosition:
        call    CTP_PositionAvailable
        or      %r4, %r4
        jreq    CTP_GetPosition
        ld.w    %r7, %r5

        xld.w   %r4, RxBuffer
        add     %r5, %r4
        xadd    %r7, ItemSize
        xand    %r7, (BufferSize - 1)

        ld.w    %r4, [%r5]+                           ; x
        ld.w    %r5, [%r5]                            ; y
        ld.w    [%r6], %r7                            ; update RxRead
        ret


;;; initialisation
;;; input:
;;; output:
        .global CTP_initialise
CTP_initialise:
	xld.w   %r4, R8_P0_P0D                        ; CTP reset = 1
        ld.ub   %r5, [%r4]
        xoor    %r5, 0x80
        ld.b    [%r4], %r5

        xld.w   %r6, 200                              ; delay for reset pulse
        xcall   delay_us

	xld.w   %r4, R8_P0_P0D                        ; CTP reset = 0
        ld.ub   %r5, [%r4]
        xand    %r5, ~0x80
        ld.b    [%r4], %r5

        xld.w   %r6, Vector_Serial_interface_Ch_1_Receive_buffer_full
        xld.w   %r7, CTP_RxInterrupt
        xcall   Vector_set

        xld.w   %r6, Vector_Serial_interface_Ch_1_Receive_error
        xld.w   %r7, CTP_RxInterrupt
        xcall   Vector_set

        ;xld.w   %r6, Vector_Serial_interface_Ch_1_Transmit_buffer_empty
        ;xld.w   %r7, CTP_TxInterrupt
        ;xcall   Vector_set

        ld.w    %r5, 0

        xld.w   %r4, RxRead
        ld.w    [%r4], %r5
        xld.w   %r4, RxWrite
        ld.w    [%r4], %r5

        xld.w   %r4, state
        ld.w    [%r4], %r5

	DISABLE_INTERRUPTS

        xld.w   %r4, R8_INT_FSIF01                    ; clear the interrupt
        xld.w   %r5, FSTX1 | FSERR1 | FSRX1
        ld.b    [%r4], %r5

	xld.w   %r4, R8_INT_ESIF01                    ; enable interrupt
        ld.b    %r5, [%r4]
        xoor    %r5, ESRX1 | ESERR1
        ld.b    [%r4], %r5

	xld.w   %r4, R8_INT_PSIO1_PAD
        ld.ub   %r5, [%r4]
        xand    %r5, 0xf0
        xoor    %r5, 0x07
        ld.b    [%r4], %r5

        xld.w   %r4, R8_EFSIF1_STATUS
        ld.b    %r5, [%r4]
        xld.w   %r4, R8_EFSIF1_RXD
        ld.b    %r5, [%r4]                            ; FIFO 1
        ld.b    %r5, [%r4]                            ; FIFO 2
        ld.b    %r5, [%r4]                            ; FIFO 3
        ld.b    %r5, [%r4]                            ; FIFO 4
        ld.b    %r5, [%r4]                            ; UART

        ENABLE_INTERRUPTS

        ret


;;; receive all bytes from receive FIFO
;;; input:
;;; output:
        .global CTP_RxInterrupt
CTP_RxInterrupt:
        pushn   %r14

        xld.w   %r0, R8_INT_FSIF01                    ; clear the interrupt
        xld.w   %r2, FSRX1 | FSERR1
        ld.b	[%r0], %r2

        xld.w   %r0, R8_EFSIF1_STATUS
        xld.w   %r1, R8_EFSIF1_RXD
        xld.w   %r2, RxRead
        xld.w   %r3, RxWrite
        xld.w   %r4, state
        xld.w   %r5, x
        xld.w   %r6, y

        ld.w    %r10, [%r4]                           ; state

CTP_RxInterrupt_loop:
        ld.ub	%r9, [%r1]                            ; the received byte

        xcmp    %r9, 0xaa                             ; header byte
        jreq    CTP_RxInterrupt_reset

        ld.w    %r7, %r5                              ; x
        xcmp    %r10, 1
        jrult   CTP_RxInterrupt_xy_high               ; 0
        jreq    CTP_RxInterrupt_xy_low                ; 1
        ld.w    %r7, %r6                              ; y
        xcmp    %r10, 3
        jrult   CTP_RxInterrupt_xy_high               ; 2
        jreq    CTP_RxInterrupt_xy_low                ; 3

        ld.w    %r12, -1                              ; x-null
        ld.w    %r13, -1                              ; y-null

        cmp     %r9, 1                                ; touch?
        jrult   CTP_RxInterrupt_no_touch              ; ...no
        jrne    CTP_RxInterrupt_reset                 ; invalid

        ld.w    %r12, [%r5]                           ; x value
        ld.w    %r13, [%r6]                           ; y value
        sra     %r12, 1
        sra     %r13, 1

CTP_RxInterrupt_no_touch:

        ld.w    %r9, [%r3]                            ; RxWrite

        xld.w   %r11, RxBuffer                        ; buffer start
        add     %r11, %r9                             ; + offset

        ld.w    [%r11]+, %r12                         ; store x
        ld.w    [%r11], %r13                          ; store y

        ld.w    %r11, [%r2]                           ; RxRead

        xadd    %r9, ItemSize                         ; next position
        xand    %r9, (BufferSize - 1)                 ; mod buffer size
        cmp     %r11, %r9                             ; read == write?
        jreq    CTP_RxInterrupt_reset                 ; ...yes => buffer overrun, value lost

        ld.w    [%r3], %r9                            ; update RxWrite

CTP_RxInterrupt_reset:
        ld.w    %r10, 0                               ; reset the state
        jp      CTP_RxInterrupt_next

CTP_RxInterrupt_xy_high:
        xsla    %r9, 7
        ld.w    [%r7], %r9
        jp      CTP_RxInterrupt_inc_state

CTP_RxInterrupt_xy_low:
        ld.w    %r8, [%r7]
        or      %r8, %r9
        ld.w    [%r7], %r8

CTP_RxInterrupt_inc_state:
        add     %r10, 1

CTP_RxInterrupt_next:
        ld.ub   %r9, [%r0]                            ; read status
        ld.w    %r7, 0
        ld.b    [%r0], %r7                            ; clear error flags

        xand    %r9, RDBFx
        jrne    CTP_RxInterrupt_loop

CTP_RxInterrupt_done:
        ld.w    [%r4], %r10                           ; update state

        popn    %r14
        reti
