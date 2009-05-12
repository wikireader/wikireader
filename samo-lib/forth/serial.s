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

;;; macro for regs.inc
        .macro  REGDEF, address, bits, name
R\bits\()_\name = \address
        .endm

        .include "c33regs.inc"

;;; register usage
;;;  r0 .. r3  must be preserved
;;;  r4        result low
;;;  r5        result high
;;;  r6 .. r9  arguments 1..4
;;; r10 ..r14  reserved
;;; r15        __dp value

        .section .text


;;; Bits for: REG_EFSIFx_STATUS
RXDxNUM1 = (1 << 7)
RXDxNUM0 = (1 << 6)
TENDx    = (1 << 5)
FERx     = (1 << 4)
PERx     = (1 << 3)
OERx     = (1 << 2)
TDBEx    = (1 << 1)
RDBFx    = (1 << 0)


;;; print cr and lf
;;; input:
;;; output:
        .global Serial_PutCRLF
Serial_PutCRLF:
        xld.w   %r6, 0x0d
        xcall   Serial_PutChar
        xld.w   %r6, 0x0a
        jp      Serial_PutChar


;;; print a space
;;; input:
;;; output:
        .global Serial_PutSpace
Serial_PutSpace:
        xld.w   %r6, 0x20
        jp      Serial_PutChar


;;; print a character
;;; input:
;;;   r6 = char
;;; output:
        .global Serial_PutChar
Serial_PutChar:
        xld.w   %r4, R8_EFSIF0_STATUS
Serial_PutChar_wait:
        ld.ub   %r5, [%r4]
        xand    %r5, TDBEx
        jreq    Serial_PutChar_wait

        xld.w   %r5, R8_EFSIF0_TXD
        ld.b	[%r5], %r6
        ret

;;; print a string
;;; input:
;;;   r6 = address of '\0' terminated string
;;; output:
;;; temporary:
;;;   r9 = address during loop
        .global Serial_PutString
Serial_PutString:
        ld.w    %r9, %r6
Serial_PutString_loop:
        ld.ub   %r6, [%r9]+
        cmp     %r6, 0
        jreq    Serial_PutString_done
        cmp     %r6, 10
        jreq    Serial_PutString_crlf
        xcall   Serial_PutChar
        jp      Serial_PutString_loop
Serial_PutString_crlf:
        xcall   Serial_PutCRLF
        jp      Serial_PutString_loop
Serial_PutString_done:
        ret


;;; print a hex nibble
;;; input:
;;;   r6 = 4 bit number to print
;;; output:
        .global Serial_PutNibble
Serial_PutNibble:
        xand    %r6, 0x0f
        xadd    %r6, '0'
        xcmp    %r6, '9'
        jrle    Serial_PutNibble_l1
        xadd    %r6, 'a' - '9' - 1
Serial_PutNibble_l1:
        xcall   Serial_PutChar
        ret


;;; print a hex word
;;; input:
;;;   r6 = 32 bit number to print
;;; output:
;;; temporary:
;;;   r9 = thet word being output
        .global Serial_PutHex
Serial_PutHex:
        ld.w    %r9, %r6
        xsrl    %r6, 28
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 24
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 20
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 16
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 12
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 8
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xsrl    %r6, 4
        xcall   Serial_PutNibble

        ld.w    %r6, %r9
        xcall   Serial_PutNibble

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
        xld.w   %r4, R8_EFSIF0_RXD
        ld.ub	%r4, [%r4]
        ret


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
        .global Serial_InputAvailable
Serial_InputAvailable:
        xld.w   %r4, R8_EFSIF0_STATUS
        ld.ub   %r4, [%r4]
        xand    %r4, RDBFx
        jreq    Serial_InputAvailable_done
        ld.w    %r4, 1
Serial_InputAvailable_done:
        ret
