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


;;; read a character
;;; input:
;;; output:
;;;   r4 = char
        .global CTP_GetChar
CTP_GetChar:
        call    CTP_InputAvailable
        or      %r4, %r4
        jreq    CTP_GetChar
        xld.w   %r4, R8_EFSIF1_RXD
        ld.ub	%r4, [%r4]
        ret


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
        .global CTP_InputAvailable
CTP_InputAvailable:
        xld.w   %r4, R8_EFSIF1_STATUS
        ld.ub   %r4, [%r4]
        xand    %r4, RDBFx
        jreq    CTP_InputAvailable_done
        ld.w    %r4, 1
CTP_InputAvailable_done:
        ret
