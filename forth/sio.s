;;; sio - simple serial I/O

        .include "regs.inc"

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
        .global sio_put_crlf
sio_put_crlf:
        xld.w   %r6, 0x0d
        xcall   sio_put_char
        xld.w   %r6, 0x0a
        jp      sio_put_char


;;; print a space
;;; input:
;;; output:
        .global sio_put_space
sio_put_space:
        xld.w   %r6, 0x20
        jp      sio_put_char


;;; print a character
;;; input:
;;;   r6 = char
;;; output:
        .global sio_put_char
sio_put_char:
        xld.w   %r4, REG_EFSIF0_STATUS
sio_put_char_wait:
        ld.ub   %r5, [%r4]
        xand    %r5, TDBEx
        jreq    sio_put_char_wait

        xld.w   %r5, REG_EFSIF0_TXD
        ld.b	[%r5], %r6
        ret

;;; print a string
;;; input:
;;;   r6 = address of '\0' terminated string
;;; output:
;;; temporary:
;;;   r9 = address during loop
        .global sio_put_string
sio_put_string:
        ld.w    %r9, %r6
sio_put_string_loop:
        ld.ub   %r6, [%r9]+
        cmp     %r6, 0
        jreq    sio_put_string_done
        cmp     %r6, 10
        jreq    sio_put_string_crlf
        xcall   sio_put_char
        jp      sio_put_string_loop
sio_put_string_crlf:
        xcall   sio_put_crlf
        jp      sio_put_string_loop
sio_put_string_done:
        ret


;;; print a hex nibble
;;; input:
;;;   r6 = 4 bit number to print
;;; output:
        .global sio_put_nibble
sio_put_nibble:
        xand    %r6, 0x0f
        xadd    %r6, '0'
        xcmp    %r6, '9'
        jrle    sio_put_nibble_l1
        xadd    %r6, 'a' - '9' - 1
sio_put_nibble_l1:
        xcall   sio_put_char
        ret


;;; print a hex word
;;; input:
;;;   r6 = 32 bit number to print
;;; output:
;;; temporary:
;;;   r9 = thet word being output
        .global sio_put_hex
sio_put_hex:
        ld.w    %r9, %r6
        xsrl    %r6, 28
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 24
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 20
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 16
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 12
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 8
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xsrl    %r6, 4
        xcall   sio_put_nibble

        ld.w    %r6, %r9
        xcall   sio_put_nibble

        ret


;;; read a character
;;; input:
;;; output:
;;;   r4 = char
        .global sio_get_char
sio_get_char:
        call    sio_input_available
        or      %r4, %r4
        jreq    sio_get_char
        xld.w   %r4, REG_EFSIF0_RXD
        ld.ub	%r4, [%r4]
        ret


;;; see if input is available
;;; input:
;;; output:
;;;   r4 = 0 => not ready
;;;        1 => ready
        .global sio_input_available
sio_input_available:
        xld.w   %r4, REG_EFSIF0_STATUS
        ld.ub   %r4, [%r4]
        xand    %r4, RDBFx
        jreq    sio_input_available_done
        ld.w    %r4, 1
sio_input_available_done:
        ret
