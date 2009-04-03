;;; sio - simple serial I/O

        .include "regs.inc"


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
        pushn   %r0
        xld.w   %r0, 0x0d
        xcall   sio_put_char
        xld.w   %r0, 0x0a
        xcall   sio_put_char
        popn    %r0
        ret


;;; print a space
;;; input:
;;; output:
        .global sio_put_space
sio_put_space:
        pushn   %r0
        xld.w   %r0, 0x20
        xcall   sio_put_char
        popn    %r0
        ret


;;; print a character
;;; input:
;;;   r0 = char
;;; output:
        .global sio_put_char
sio_put_char:
        pushn   %r2
        xld.w   %r1, REG_EFSIF0_STATUS
sio_put_char_wait:
        ld.ub   %r2, [%r1]
        xand    %r2, TDBEx
        jreq    sio_put_char_wait

        xld.w   %r1, REG_EFSIF0_TXD
        ld.b	[%r1], %r0
        popn    %r2
        ret

;;; print a string
;;; input:
;;;   r0 = address of '\0' terminated string
;;; output:
        .global sio_put_string
sio_put_string:
        pushn   %r1
        ld.w    %r1, %r0
sio_put_string_loop:
        ld.ub   %r0, [%r1]+
        cmp     %r0, 0
        jreq    sio_put_string_done
        xcall   sio_put_char
        jp      sio_put_string_loop
sio_put_string_done:
        popn    %r1
        ret


;;; print a hex nibble
;;; input:
;;;   r0 = 4 bit number to print
;;; output:
        .global sio_put_nibble
sio_put_nibble:
        pushn   %r0
        xand    %r0, 0x0f
        xadd    %r0, '0'
        xcmp    %r0, '9'
        jrle    sio_put_nibble_l1
        xadd    %r0, 'a' - '9' - 1
sio_put_nibble_l1:
        xcall   sio_put_char
        popn    %r0
        ret


;;; print a hex word
;;; input:
;;;   r0 = 32 bit number to print
;;; output:
        .global sio_put_hex
sio_put_hex:
        pushn    %r1

        ld.w    %r1, %r0
        xsrl    %r0, 28
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 24
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 20
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 16
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 12
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 8
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xsrl    %r0, 4
        xcall   sio_put_nibble

        ld.w    %r0, %r1
        xcall   sio_put_nibble

        popn    %r1
        ret


;;; read a character
;;; input:
;;; output:
;;;   r0 = char
        .global sio_get_char
sio_get_char:
        call    sio_input_available
        or      %r0, %r0
        jreq    sio_get_char
        xld.w   %r0, REG_EFSIF0_RXD
        ld.ub	%r0, [%r0]
        ret


;;; see if input is available
;;; input:
;;; output:
;;;   r0 = 0 => not ready
;;;        1 => ready
        .global sio_input_available
sio_input_available:
        xld.w   %r0, REG_EFSIF0_STATUS
        ld.ub   %r0, [%r0]
        xand    %r0, RDBFx
        jreq    sio_input_available_done
        ld.w    %r0, 1
sio_input_available_done:
        ret
