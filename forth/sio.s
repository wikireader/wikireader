;;; simple serial I/O

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
        .global putcrlf
putcrlf:
        pushn   %r0
        xld.w   %r0, 0x0d
        xcall   putchar
        xld.w   %r0, 0x0a
        xcall   putchar
        popn    %r0
        ret


;;; print a space
;;; input:
;;; output:
        .global putspace
putspace:
        pushn   %r0
        xld.w   %r0, 0x20
        xcall   putchar
        popn    %r0
        ret


;;; print a character
;;; input:
;;;   r0 = char
;;; output:
        .global putchar
putchar:
        pushn   %r2
        xld.w   %r1, REG_EFSIF0_STATUS
putchar_wait:
        ld.ub   %r2, [%r1]
        xand    %r2, TDBEx
        jreq    putchar_wait

        xld.w   %r1, REG_EFSIF0_TXD
        ld.b	[%r1], %r0
        popn    %r2
        ret

;;; print a string
;;; input:
;;;   r0 = address of '\0' terminated string
;;; output:
        .global puts
puts:
        pushn   %r1
        ld.w    %r1, %r0
puts_loop:
        ld.ub   %r0, [%r1]+
        cmp     %r0, 0
        jreq    puts_done
        xcall   putchar
        jp      puts_loop
puts_done:
        popn    %r1
        ret


;;; print a hex nibble
;;; input:
;;;   r0 = 4 bit number to print
;;; output:
        .global putnibble
putnibble:
        pushn   %r0
        xand    %r0, 0x0f
        xadd    %r0, '0'
        xcmp    %r0, '9'
        jrle    putnibble_l1
        xadd    %r0, 'a' - '9' - 1
putnibble_l1:
        xcall   putchar
        popn    %r0
        ret


;;; print a hex word
;;; input:
;;;   r0 = 32 bit number to print
;;; output:
        .global puthex
puthex:
        pushn    %r1

        ld.w    %r1, %r0
        xsrl    %r0, 28
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 24
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 20
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 16
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 12
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 8
        xcall   putnibble

        ld.w    %r0, %r1
        xsrl    %r0, 4
        xcall   putnibble

        ld.w    %r0, %r1
        xcall   putnibble

        popn    %r1
        ret


;;; read a character
;;; input:
;;; output:
;;;   r0 = char
        .global getchar
getchar:
        pushn   %r1
        xld.w   %r1, REG_EFSIF0_STATUS
getchar_wait:
        ld.ub   %r0, [%r1]
        xand    %r0, RDBFx
        jreq    getchar_wait
        popn    %r1

        xld.w   %r0, REG_EFSIF0_RXD
        ld.ub	%r0, [%r0]
        ret
