;;; Serial - interrupt driven serial I/O

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

BufferSize = 2048

;;; buffer is full if: (write + 1) mod size == read
;;; buffer is full if: write == read


RxBuffer:
        .space  BufferSize
RxRead:
        .long   0
RxWrite:
        .long   0

TxBuffer:
        .space  BufferSize
TxRead:
        .long   0
TxWrite:
        .long   0


        .section .text


;;; wait wor serial output ready
;;; input:
;;; output:
;;;   r4 = true if space in buffer, false if buffer is full
;;;   r5 = address of TxWrite
;;;   r6 = *NOT CHANGED*  (will be byte to put)
;;;   r7 = TxRead
;;;   r8 = TxWrite     (offset of byte to store)
;;;   r9 = TxWrite + 1 (next free space)
        .global Serial_PutReady
Serial_PutReady:
        xld.w   %r7, TxRead
        xld.w   %r5, TxWrite
        ld.w    %r7, [%r7]                            ; read
        ld.w    %r8, [%r5]                            ; write
        ld.w    %r9, %r8
        add     %r9, 1                                ; write + 1
        xand    %r9, (BufferSize - 1)                 ; mod buffer size

        cmp     %r7, %r9                              ; read == write ?
        jreq    Serial_PutReady_buffer_full
        ld.w    %r4, 1                                ; TRUE => buffer is not full
        ret

Serial_PutReady_buffer_full:
        ld.w    %r4, 0                                ; FALSE => buffer is full
        ret


;;; print a character
;;; input:
;;;   r6 = char
;;; output:
;;; temporary:
;;;   r4..r9
        .global Serial_PutChar
Serial_PutChar:
        call    Serial_PutReady                       ; wait for buffer space
        or      %r4, %r4
        jreq    Serial_PutChar

        xld.w   %r4, TxBuffer                         ; buffer
        add     %r4, %r8                              ; buffer + offset
        ld.b    [%r4], %r6                            ; store byte
        ld.w    [%r5], %r9                            ; update TxWrite

        xld.w   %r4, R8_INT_ESIF01
        DISABLE_INTERRUPTS
        ld.ub   %r8, [%r4]
        xoor    %r8, ESTX0                            ; enable Tx0 interrupt
        ld.b    [%r4], %r8
        ENABLE_INTERRUPTS
        ret


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
;;;   r5 = offset of byte to read
;;;   r6 = address of RxRead
;;; temporary:

        .global Serial_InputAvailable
Serial_InputAvailable:
        xld.w   %r6, RxRead
        xld.w   %r4, RxWrite
        ld.w    %r5, [%r6]                            ; read
        ld.w    %r4, [%r4]                            ; write
        cmp     %r5, %r4                              ; read == write ?
        jreq    Serial_InputAvailable_buffer_empty
        ld.w    %r4, 1                                ; TRUE => buffer is not empty
        ret

Serial_InputAvailable_buffer_empty:
        ld.w    %r4, 0                                ; FALSE => buffer is empty
        ret


;;; read a character
;;; input:
;;; output:
;;;   r4 = char
        .global Serial_GetChar
Serial_GetChar:
        call    Serial_InputAvailable
        or      %r4, %r4
        jreq    Serial_GetChar
        xld.w   %r4, RxBuffer
        add     %r4, %r5
        ld.ub	%r4, [%r4]
        add     %r5, 1
        xand    %r5, (BufferSize - 1)
        ld.w    [%r6], %r5                            ; update RxRead
        ret


;;; see if input is available
;;; input:
;;; output:
        .global Serial_initialise
Serial_initialise:
        xld.w   %r6, Vector_Serial_interface_Ch_0_Receive_buffer_full
        xld.w   %r7, Serial_RxInterrupt
        xcall   Vector_set

        xld.w   %r6, Vector_Serial_interface_Ch_0_Receive_error
        xld.w   %r7, Serial_RxInterrupt
        xcall   Vector_set

        xld.w   %r6, Vector_Serial_interface_Ch_0_Transmit_buffer_empty
        xld.w   %r7, Serial_TxInterrupt
        xcall   Vector_set

        ld.w    %r5, 0

        xld.w   %r4, RxRead
        ld.w    [%r4], %r5
        xld.w   %r4, RxWrite
        ld.w    [%r4], %r5

        xld.w   %r4, TxRead
        ld.w    [%r4], %r5
        xld.w   %r4, TxWrite
        ld.w    [%r4], %r5

	DISABLE_INTERRUPTS

        xld.w   %r4, R8_INT_FSIF01                    ; clear the interrupt
        xld.w   %r5, FSTX0 | FSERR0 | FSRX0
        ld.b    [%r4], %r5

	xld.w   %r4, R8_INT_ESIF01
        xld.w   %r5, ESRX0 | ESERR0
        ld.b    [%r4], %r5

	xld.w   %r4, R8_INT_PLCDC_PSIO0
        ld.ub   %r5, [%r4]
        xand    %r5, 0x0f
        xoor    %r5, 0x70
        ld.b    [%r4], %r5

        ENABLE_INTERRUPTS

        ret


;;; receive all bytes from receive FIFO
;;; input:
;;; output:
        .global Serial_RxInterrupt
Serial_RxInterrupt:
        pushn   %r14

        xld.w   %r0, R8_INT_FSIF01                    ; clear the interrupt
        xld.w   %r2, FSRX0 | FSERR0
        ld.b	[%r0], %r2

        xld.w   %r0, R8_EFSIF0_STATUS
        xld.w   %r1, R8_EFSIF0_RXD
        xld.w   %r2, RxRead
        xld.w   %r3, RxWrite

Serial_RxInterrupt_loop:
        ld.ub	%r4, [%r1]                            ; the received byte

        ld.w    %r5, [%r3]                            ; RxWrite

        xld.w   %r6, RxBuffer                         ; buffer start
        add     %r6, %r5                              ; + offset
        ld.b    [%r6], %r4                            ; store the byte

        ld.w    %r6, [%r2]                            ; RxRead

        add     %r5, 1                                ; next position
        xand    %r5, (BufferSize - 1)                 ; mod buffer size
        cmp     %r6, %r5                              ; read == write?
        jreq    Serial_RxInterrupt_no_store           ; ...yes => buffer overrun, byte is lost

        ld.w    [%r3], %r5                            ; update RxWrite

Serial_RxInterrupt_no_store:

        ld.ub   %r4, [%r0]
        ld.w    %r5, 0
        ld.b    [%r0], %r5                            ; clear error flags

        xand    %r4, RDBFx
        jrne    Serial_RxInterrupt_loop

Serial_RxInterrupt_done:
        popn    %r14
        reti

;;; discard erroneous bytes from receive FIFO
;;; input:
;;; output:
        .global Serial_RxErrorInterrupt
Serial_RxErrorInterrupt:
        pushn   %r14

        xld.w   %r0, R8_EFSIF0_STATUS
        xld.w   %r1, R8_EFSIF0_RXD
        xld.w   %r2, RxRead
        xld.w   %r3, RxWrite

Serial_RxErrorInterrupt_loop:
        ld.ub   %r4, [%r0]
        xcall   panic
        ld.w    %r5, 0
        ld.b    [%r0], %r5                            ; clear error flags

        xand    %r4, RDBFx
        jreq    Serial_RxErrorInterrupt_done

        ld.ub	%r4, [%r1]                            ; the received byte

        ld.w    %r5, [%r3]                            ; RxWrite

        xld.w   %r6, RxBuffer                         ; buffer start
        add     %r6, %r5                              ; + offset
        ld.b    [%r6], %r4                            ; store the byte

        ld.w    %r6, [%r2]                            ; RxRead

        add     %r5, 1                                ; next position
        xand    %r5, (BufferSize - 1)                 ; mod buffer size
        cmp     %r6, %r5                              ; read == write?
        jreq    Serial_RxErrorInterrupt_loop          ; ...yes => buffer overrun, byte is lost

        ld.w    [%r3], %r5                            ; update RxWrite

        jp      Serial_RxErrorInterrupt_loop

Serial_RxErrorInterrupt_done:
        xld.w   %r0, R8_INT_FSIF01                    ; clear the interrupt
        xld.w   %r2, FSERR0 | FSRX0
        ld.b	[%r0], %r2

        popn    %r14
        reti

;;; send one byte
;;; input:
;;; output:
        .global Serial_TxInterrupt
Serial_TxInterrupt:
        pushn   %r14

        xld.w   %r0, TxRead
        xld.w   %r1, TxWrite
        ld.w    %r2, [%r0]                            ; read
        ld.w    %r1, [%r1]                            ; write
        cmp     %r1, %r2                              ; read == write?
        jreq    Serial_TxInterrupt_buffer_empty       ; ...yes => all sent

        xld.w   %r1, TxBuffer                         ; base address
        add     %r1, %r2
        ld.ub   %r3, [%r1]                            ; get the byte to send

        add     %r2, 1                                ; increment offset
        xand    %r2, (BufferSize - 1)                 ; mod buffer size
        ld.w    [%r0], %r2                            ; update offset

        xld.w   %r0, R8_INT_FSIF01
        ld.b    %r2, [%r0]
        xoor    %r2, FSTX0
        ld.b    [%r0], %r2                            ; clear Tx0 interrupt

        xld.w   %r1, R8_EFSIF0_TXD                    ; transmit the byte
        ld.b	[%r1], %r3

        popn    %r14
        reti

Serial_TxInterrupt_buffer_empty:
        xld.w   %r0, R8_INT_ESIF01
        ld.b    %r2, [%r0]
        xand    %r2, ~ESTX0
        ld.b    [%r0], %r2                            ; disable Tx0 interrupt

        popn    %r14
        reti
