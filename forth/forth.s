;;; forth.s
;;; based on the public domain eforh implementations
;;; found in the files eforth.4th and eforth.S

;;; symbols used in the ( -- ) comments
;;;
;;; a   aligned address (to 4 byte boundary)
;;; b   byte address
;;; c   character
;;; ca  code address
;;; cy  carry
;;; d   signed double integer (2 cells)
;;; F   logical false
;;; f   flag 0 or non-zero
;;; h   half-word address (to 2 byte boundary)
;;; la  link address
;;; n   signed integer
;;; na  name address
;;; pa  param address
;;; T   logical true
;;; t   flag T or F
;;; u   unsigned integer
;;; ud  unsigned double integer (2 cells)
;;; va  vocabulary address
;;; w   unspecified word value

;;; some characters
backspace = 0x08
line_feed = 0x0a
carriage_return = 0x0d
delete = 0x7f

;;; some special constants
CELL_SIZE = 4                                   ; bytes per cell
BITS_PER_CELL = 32                              ; bits per cell

FALSE = 0
TRUE = -1

;;; header flags
IMMEDIATE = 0x80
COMPILE_ONLY = 0x40
NORMAL = 0


;;; registers
;;; r0..r7 general
;;; r8..r9 maybe used by extended instructions
;;; r10    stack pointer
;;; r11    ip
;;; r12    pp
;;; r13    work regiter
;;; r14    not_used
;;; r15    __DP for C

        .macro  NEXT                            ; inner interpreter
        ld.w    %r12, [%r11]+                   ; incr IP
        ld.w    %r13, [%r12]+                   ; %r12 -> param address
        jp      %r13                            ; execute the code
        .endm


;;; inline forth counted strings

        .macro  FSTRING, text
        .byte   str_\@_finish - str_\@_start
str_\@_start:
        .ascii  "\text"
str_\@_finish:
        .balign 4
        .endm


;;; the header

__last_name = 0                                 ; to link the list

        .macro  HEADER, label, name, flags, code
        .section .data
        .balign 4
        .global \label
\label\():
        .long   \code                           ; code
        .long   param_\label                    ; param
        .long   \flags                          ; flags

prev_\label = __last_name
        .long   prev_\label                     ; link

        .global name_\label
name_\label\():
__last_name = .
        FSTRING "\name"

        .section .text
        .balign 4

        .global param_\label
param_\label\():
        .endm


;;; code definitions

        .macro  CODE, label, name, flags
        HEADER  \label, "\name", \flags, param_\label
        .endm

        .macro  END_CODE
        .endm


;;; colon definitions

        .macro  COLON, label, name, flags
        HEADER  \label, "\name", \flags, param_docolon
        .endm


;;; variable definitions

        .macro  VARIABLE, label, name, flags
        HEADER  \label, "\name", \flags, param_dovar
        .endm


;;; user variables sections

        .section .user_defaults

user_defaults:

        .section .user_variables
user_variables:


;;; define user variables

        .macro  USER, label, name, flags, defaults

        .section .user_defaults
        .global \label\()_default
\label\()_default:
        .long   \defaults

        .section .user_variables
        .global \label\()_variable
\label\()_variable:
        .long   \defaults

        HEADER  \label, "\name", \flags, param_douser
	.long   \label\()_variable

        .endm


;;; miscellaneous variables

        .section .bss

terminal_buffer:
        .space   65536
        .global initial_stack_pointer
initial_stack_pointer:     ; NOTE: stack underflows over return space!
        .space   65536
        .global initial_return_pointer
initial_return_pointer:


;;; Program Code
        .section .text
main:
        xld.w   %r15, __dp
        xld.w   %r10, initial_stack_pointer
        xld.w   %r0, initial_return_pointer
        ld.w    %sp, %r0
        xld.w   %r0, xmessage
        xcall   sio_put_string
        xld.w   %r11, cold_start                ; initial ip value
        NEXT

cold_start:
        .long   cold, branch, cold_start        ; just run cold in a loop

xmessage:
        .asciz  "start\r\n"
        .balign 4

debug_8:
        xcall   sio_put_string

        ld.w    %r0, %r1
        xcall   sio_put_hex

        xcall   sio_put_space

        xld.w   %r2, 8

debug_8_loop:
        xcall   sio_put_space

        ld.w    %r0, [%r1]+
        xcall   sio_put_hex

        xsub    %r2, 1
        jrne    debug_8_loop

        xcall   sio_put_crlf
        ret

xdebug:
        pushn   %r14
        xcall   sio_put_crlf

        xld.w   %r0, debug_message
        ld.w    %r1, %r11
        xcall   debug_8

        xld.w   %r0, debug_data
        ld.w    %r1, %r10
        xcall   debug_8

        xld.w   %r0, debug_return
        ld.w    %r1, %sp
        xcall   debug_8

        popn    %r14
        ret


debug_message:
        .asciz  "debug:\r\nr11: "

debug_data:
        .asciz  "r10: "

debug_return:
        .asciz  "sp:  "

message:
        .asciz  "starting the test program\r\n"

sp_message:
        .asciz  "old %sp = "

        .balign 4


;;; .( Special interpreters )

        CODE    dolit, "(dolit)", COMPILE_ONLY ; ( -- w ) COMPILE-ONLY
        ld.w    %r12, [%r11]+
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r12
        NEXT
        END_CODE

        CODE    docolon, "(docolon)", COMPILE_ONLY ; ( -- )
        ld.w    %r0, %r11                          ; save previous ip
        pushn   %r0                                ; ...
        ld.w    %r11, [%r12]                       ; ip = param address
        NEXT
        END_CODE

        CODE    execute, "execute", NORMAL      ; ( a -- )
        ld.w    %r12, [%r10]+                   ; point to code ptr
        ld.w    %r13, [%r12]+                   ; code / param address
        jp      %r13
        END_CODE

        CODE    exit, "exit", NORMAL            ; ( -- )
        popn    %r0                             ; restore ip
        ld.w    %r11, %r0                       ; ..
        NEXT
        END_CODE


;;; .( Loop & Branch - absolute address )

;;; : (next) ( -- ) \ hiLevel model  16bit absolute branch
;;;   r> r> dup if 1- >r @ >r exit then drop cell+ >r ;
        CODE    donext, "(next)",  COMPILE_ONLY
        ld.w    %r0, [%sp]
        or      %r0, %r0
        jreq    donext_l1
        xsub    %r0, 1
        ld.w    [%sp], %r0
        ld.w    %r11, [%r11]
        NEXT
donext_l1:
        popn    %r0
no_branch:
        xadd    %r11, CELL_SIZE
        NEXT
        END_CODE

        CODE    qbranch, "?branch", COMPILE_ONLY ; ( f -- ) COMPILE-ONLY
        ld.w    %r0, [%r10]+
        or      %r0, %r0
        jrne    no_branch
        ld.w    %r11, [%r11]
        NEXT
        END_CODE

        CODE    branch, "branch", COMPILE_ONLY  ; ( -- ) COMPILE-ONLY
        ld.w    %r11, [%r11]
        NEXT
        END_CODE

;;; .( Memory fetch & store )

        CODE    store, "!", NORMAL              ; ( w a -- )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]+
        ld.w    [%r0], %r1
        NEXT
        END_CODE

        CODE    fetch, "@", NORMAL              ; ( a -- w )
        ld.w    %r0, [%r10]
        ld.w    %r0, [%r0]
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    hstore, "h!", NORMAL            ; ( c h -- )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]+
        ld.h    [%r0], %r1
        NEXT
        END_CODE

        CODE    hfetch, "h@", NORMAL            ; ( h -- c )
        ld.w    %r0, [%r10]
        ld.uh   %r0, [%r0]
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    cstore, "c!", NORMAL            ; ( c b -- )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]+
        ld.b    [%r0], %r1
        NEXT
        END_CODE

        CODE    cfetch, "c@", NORMAL            ; ( b -- c )
        ld.w    %r0, [%r10]
        ld.ub   %r0, [%r0]
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; .( Return Stack )

        CODE    rp_fetch, "rp@", NORMAL         ; ( -- a )
        ld.w    %r0, %sp
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    rp_store, "rp!", COMPILE_ONLY  ; ( a -- ) COMPILE-ONLY
        ld.w    %r0, [%r10]+
        ld.w    %sp, %r0
        NEXT
        END_CODE

        CODE    r_from, "r>", COMPILE_ONLY     ; ( -- w ) COMPILE-ONLY
        popn    %r0
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    r_fetch, "r@", NORMAL           ; ( -- w )
        ld.w    %r0, [%sp]
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    to_r, ">r", COMPILE_ONLY       ; ( w -- ) COMPILE-ONLY
        ld.w    %r0, [%r10]+
        pushn   %r0
        NEXT
        END_CODE

;;; .( Data Stack )

        CODE    sp_fetch, "sp@", NORMAL         ; ( -- a )
        ld.w    %r0, %r10
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    sp_store, "sp!", NORMAL         ; ( a -- )
        ld.w    %r10, [%r10]
        NEXT
        END_CODE

        CODE    drop, "drop", NORMAL            ; ( w -- )
        ld.w    %r0, [%r10]+
        NEXT
        END_CODE

        CODE    dup, "dup", NORMAL              ; ( w -- w w )
        ld.w    %r0, [%r10]
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    swap, "swap", NORMAL            ; ( w1 w2 -- w2 w1 )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]+
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r1
        NEXT
        END_CODE

        CODE    over, "over", NORMAL           ; ( w1 w2 -- w1 w2 w1 )
        xld.w   %r0, [%r10 + 4]
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE


;;; : ?DUP ( w -- w w, 0 ) DUP IF DUP THEN ;
	COLON   qdup, "?dup", NORMAL
	.long	dup, qbranch, qdup_l1
	.long	dup
qdup_l1:
	.long	exit

;;; : NIP ( w1 w2 -- w2 ) SWAP DROP ;
        COLON   nip, "nip", NORMAL
        .long   swap, drop, exit

;;; : ROT ( w1 w2 w3 -- w2 w3 w1 ) >R SWAP R> SWAP ;
        COLON   rot, "rot", NORMAL
        .long   to_r, swap, r_from, swap, exit

;;; : -ROT ( w1 w2 w3 -- w3 w1 w2 ) ROT ROT ;
        COLON   minus_rot, "-rot", NORMAL
        .long   rot, rot, exit

;;; : 2DROP ( w w  -- ) DROP DROP ;
        COLON   twodrop, "2drop", NORMAL
        .long   drop, drop, exit

;;; : 2DUP ( w1 w2 -- w1 w2 w1 w2 ) OVER OVER ;
        COLON   twodup, "2dup", NORMAL
        .long   over, over, exit


;;; .( Logic )

        CODE    zero_less, "0<", NORMAL         ; ( n -- t )
        ld.w    %r0, [%r10]
        or      %r0, %r0
        jrlt    zero_less_l1
        ld.w    %r0, FALSE
        ld.w    [%r10], %r0
        NEXT
zero_less_l1:
        ld.w    %r0, TRUE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    _and, "and", NORMAL             ; ( w w -- w )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        and     %r0, %r1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    _or, "or", NORMAL               ; ( w w -- w )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        or      %r0, %r1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

        CODE    _xor, "xor", NORMAL             ; ( w w -- w )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        xor     %r0, %r1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : INVERT ( w -- w ) -1 XOR ;
        COLON   invert, "invert", NORMAL
        .long   dolit, -1, _xor, exit

;;; .( Arithmetic )

        CODE    umplus, "um+", NORMAL ; ( u u -- u cy ) \ or ( u u -- ud )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        add     %r0, %r1
        ld.w    [%r10], %r0
        ld.w    %r0, 0
        adc     %r0, %r0
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : + ( u u -- u ) UM+ DROP ;
        CODE    plus, "+", NORMAL               ; ( w w -- w )
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        add     %r0, %r1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : 1+ ( w -- w+1 ) 1 + ;
        CODE    increment, "1+", NORMAL
        ld.w    %r0, [%r10]
        xadd    %r0, 1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : 1- ( w -- w-1 ) 1 - ;
        CODE    decrement, "1-", NORMAL
        ld.w    %r0, [%r10]
        xsub    %r0, 1
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; :  NEGATE ( n -- -n ) INVERT 1+ ;
	COLON   negate "negate",0
	.long	invert, increment, exit

;;; : DNEGATE ( d -- -d ) INVERT >R INVERT 1 UM+ R> + ;
	COLON   dnegate, "dnegate", NORMAL
	.long	invert, to_r, invert
	.long	dolit, 1, umplus
	.long	r_from, plus, exit

;;; : - ( w w -- w ) NEGATE + ;
        CODE    minus, "-", NORMAL              ; ( w w -- w )
        ld.w    %r1, [%r10]+
        ld.w    %r0, [%r10]
        sub     %r0, %r1
        ld.w    [%r10], %r0
        NEXT
        END_CODE


;;; : ABS ( n -- +n ) DUP 0< IF NEGATE THEN ;

        COLON   abs, "abs", NORMAL
        .long   dup, zero_less, qbranch, abs_l1
        .long   negate
abs_l1:
        .long   exit


;;; .( User variables )
;;;
;;; : (douser) ( -- a ) R> @ UP @ + ; COMPILE-ONLY <- wrong!
        CODE    douser, "(douser)", COMPILE_ONLY
        ld.w    %r0, [%r12]
        ld.w    %r0, [%r0]                      ; user is another pointer!
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : (dovar) ( -- a ) R> ; COMPILE-ONLY <- wrong
        CODE   dovar, "(dovar)", COMPILE_ONLY
        ld.w    %r0, [%r12]                     ; %r0 = parameter address
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; 8 \ start offset
        USER    user_reserved, "(ureserved)", NORMAL, "0,0,0,0"

;;; DUP USER SP0      1 CELL+ \ initial data stack pointer
        USER    sp0, "sp0", NORMAL, initial_stack_pointer

;;; DUP USER RP0      1 CELL+ \ initial return stack pointer
        USER    rp0, "rp0", NORMAL, initial_return_pointer

;;; DUP USER 'KEY?    1 CELL+ \ character input ready vector
        USER    tkey_query, "\047key?", NORMAL, rx_query
;;; DUP USER 'EMIT    1 CELL+ \ character output vector
        USER    temit, "\047emit", NORMAL, tx_store

;;; DUP USER 'EXPECT  1 CELL+ \ line input vector
        USER    texpect, "\047expect", NORMAL, accept

;;; DUP USER 'TAP     1 CELL+ \ input case vector
        USER    ttap, "\047tap", NORMAL, ktap

;;; DUP USER 'ECHO    1 CELL+ \ input echo vector
        USER    techo, "\047echo", NORMAL, tx_store

;;; DUP USER 'PROMPT  1 CELL+ \ operator prompt vector
        USER    tprompt, "\047prompt", NORMAL, dot_ok

;;; DUP USER BASE     1 CELL+ \ number base
        USER    base, "base", NORMAL, 10

;;; DUP USER temp     1 CELL+ \ scratch
        USER    temp, "temp", COMPILE_ONLY, 0

;;; DUP USER SPAN     1 CELL+ \ #chars input by EXPECT
        USER    span, "span", NORMAL, 0

;;; DUP USER >IN      1 CELL+ \ input buffer offset
        USER to_in, ">in", NORMAL, 0

;;; DUP USER #TIB     1 CELL+ \ #chars in the input buffer
;;;       1 CELLS ALLOT \   address  of input buffer
        USER    hash_tib, "#tib", NORMAL, "0,terminal_buffer"

;;; DUP USER UP       1 CELL+ \ user base pointer
        ;; not needed

;;; DUP USER CSP      1 CELL+ \ save stack pointers
        ;; not needed

;;; DUP USER 'EVAL    1 CELL+ \ interpret/compile vector
        USER    teval, "\047eval", NORMAL, dollar_interpret

;;; DUP USER 'NUMBER  1 CELL+ \ numeric input vector
        USER    tnumber, "\047number", NORMAL, numberq

;;; DUP USER HLD      1 CELL+ \ formated numeric string
        USER    hld, "hld", NORMAL, 0

;;; DUP USER HANDLER  1 CELL+ \ error frame pointer
        USER    handler, "handler", NORMAL, 0

;;; DUP USER CONTEXT  1 CELL+ \ first search vocabulary
;;;   =VOCS CELL+ \ vocabulary stack
        USER    context, "context", NORMAL, "0, 0,0,0,0, 0,0,0,0"

;;; DUP USER CURRENT  1 CELL+ \ definitions vocabulary
;;;       1 CELL+ \ newest vocabulary
        USER    current, "current", NORMAL, "0,0"

;;; DUP USER CP       1 CELL+ \ dictionary code pointer
;;;       1 CELL+ \ dictionary name pointer
;;;       1 CELL+ \ last name compiled
        USER    cp, "cp", NORMAL, "end_of_code, end_of_dictionary, last_name"


;;; .( Comparison )

;;; : 0= ( w -- t ) IF 0 EXIT THEN -1 ;
        COLON   zero_equal, "0=", NORMAL
        .long   qbranch, zero_equal_l1
        .long   dolit, FALSE, exit
zero_equal_l1:
        .long   dolit, TRUE, exit

;;; : = ( w w -- t ) XOR 0= ;
        COLON   equal, "=", NORMAL
        .long   _xor, zero_equal, exit

;;; : U< ( u u -- t ) 2DUP XOR 0< IF  NIP 0< EXIT THEN - 0< ;
        COLON   uless, "u<", NORMAL
        .long   twodup, _xor, zero_less, qbranch, uless_l1
        .long   nip, zero_less, exit
uless_l1:
        .long   minus, zero_less, exit

;;; :  < ( n n -- t ) 2DUP XOR 0< IF DROP 0< EXIT THEN - 0< ;
        COLON   less, "<", NORMAL
        .long   twodup, _xor, zero_less, qbranch, less_l1
        .long   drop, zero_less, exit
less_l1:
        .long   minus, zero_less, exit

;;; : MAX ( n n -- n ) 2DUP      < IF SWAP THEN DROP ;
        COLON   max, "max", NORMAL
        .long   twodup, less, qbranch, max_l1
        .long   swap
max_l1:
        .long   drop, exit

;;; : MIN ( n n -- n ) 2DUP SWAP < IF SWAP THEN DROP ;
        COLON   min, "min", NORMAL
        .long   twodup, swap, less, qbranch, min_l1
        .long   swap
min_l1:
        .long   drop, exit

;;; : WITHIN ( u ul uh -- t ) OVER - >R - R> U< ;
        COLON   within, "within", NORMAL
        .long   over, minus, to_r, minus, r_from, uless, exit


;;; .( Divide )

;;; : UM/MOD ( udl udh un -- ur uq )
;;;   2DUP U<
;;;   IF NEGATE  15
;;;     FOR >R DUP  UM+  >R >R DUP  UM+  R> + DUP
;;;         R> R@ SWAP >R  UM+  R> OR
;;;       IF >R DROP 1+ R> ELSE DROP THEN R>
;;;     NEXT DROP SWAP EXIT
;;;   THEN DROP 2DROP  -1 DUP ;
        COLON   um_slash_mod, "um/mod", NORMAL
	.long	twodup, uless
	.long	qbranch, um_slash_mod_l4
	.long	negate, dolit, BITS_PER_CELL - 1, to_r
um_slash_mod_l1:
	.long	to_r, dup, umplus
	.long	to_r, to_r, dup, umplus
	.long	r_from, plus, dup
	.long	r_from, r_fetch, swap, to_r
	.long	umplus, r_from, _or
	.long	qbranch, um_slash_mod_l2
	.long	to_r, drop, dolit, 1, plus, r_from
	.long	branch, um_slash_mod_l3
um_slash_mod_l2:
	.long	drop
um_slash_mod_l3:
	.long	r_from
	.long	donext, um_slash_mod_l1
	.long	drop, swap, exit
um_slash_mod_l4:
	.long	drop, twodrop
	.long	dolit, -1, dup, exit

;;; : M/MOD ( d n -- r q ) \ floored
;;;   DUP 0<  DUP >R
;;;   IF NEGATE >R DNEGATE R>
;;;   THEN >R DUP 0< IF R@ + THEN R> UM/MOD R>
;;;   IF SWAP NEGATE SWAP THEN ;
        COLON   m_slash_mod,  "m/mod",  0
	.long	dup, zero_less, dup, to_r
	.long	qbranch, m_slash_mod_l1
	.long	negate, to_r, dnegate, r_from
m_slash_mod_l1:
	.long	to_r, dup, zero_less
	.long	qbranch, m_slash_mod_l2
	.long	r_fetch, plus
m_slash_mod_l2:
	.long	r_from, um_slash_mod, r_from
	.long	qbranch, m_slash_mod_l3
	.long	swap, negate, swap
m_slash_mod_l3:
	.long	exit

;;; : /MOD ( n n -- r q ) OVER 0< SWAP M/MOD ;
        COLON   slash_mod, "/mod", NORMAL
        .long   over, zero_less, swap, m_slash_mod, exit

;;; : MOD ( n n -- r ) /MOD DROP ;
        COLON   mod, "/", NORMAL
        .long   slash_mod, drop, exit

;;; : / ( n n -- q ) /MOD NIP ;
        COLON   divide, "/", NORMAL
        .long   slash_mod, nip, exit


;;; .( Multiply )
;;;
;;; : UM* ( u1 u2 -- ud )
;;;   0 SWAP ( u1 0 u2 ) 15
;;;   FOR DUP  UM+  >R >R DUP  UM+  R> + R>
;;;     IF >R OVER  UM+  R> + THEN
;;;   NEXT ROT DROP ;
        CODE    umult, "um*", NORMAL
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        mltu.w  %r0, %r1
        ld.w    %r0, %alr
        ld.w    [%r10], %r0
        xsub    %r10, CELL_SIZE
        ld.w    %r0, %ahr
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : * ( n n -- n ) UM* DROP ;
        CODE    times, "*", NORMAL
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        mlt.w   %r0, %r1
        ld.w    %r0, %alr
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : M* ( n n -- d )
;;;   2DUP XOR 0< >R  ABS SWAP ABS UM*  R> IF DNEGATE THEN ;
        CODE    multd, "m*", NORMAL
        ld.w    %r0, [%r10]+
        ld.w    %r1, [%r10]
        mlt.w   %r0, %r1
        ld.w    %r0, %alr
        ld.w    [%r10], %r0
        ld.w    %r0, %ahr
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; : */MOD ( n n n -- r q ) >R M* R> M/MOD ;
;;; : */ ( n n n -- q ) */MOD NIP ;


;;; .( Bits & Bytes )
;;;
;;; : BYTE+ ( b -- b ) [ =BYTE ] LITERAL + ;
        COLON   byte_plus, "byte+", NORMAL
        .long   increment, exit

;;; : BYTE- ( b -- b ) [ =BYTE ] LITERAL - ;
        COLON   byte_minus, "byte-", NORMAL
        .long   decrement, exit

;;; : CELL+ ( a -- a ) [ =CELL ] LITERAL + ;
        COLON   cell_plus, "cell+", NORMAL
        .long   dolit, CELL_SIZE, plus, exit

;;; : CELL- ( a -- a ) [ =CELL ] LITERAL - ;
        COLON   cell_minus, "cell-", NORMAL
        .long   dolit, CELL_SIZE, minus, exit

;;; : CELLS ( n -- n ) [ =CELL ] LITERAL * ;
        COLON   cells, "cells", NORMAL
        .long   dolit, CELL_SIZE, times, exit

;;; : BL ( -- 32 ) 32 ;
        COLON   blank, "bl", NORMAL
        .long   dolit, 32, exit

;;; subsitute unprintable character with '.'
;;; : >CHAR ( c -- c )
;;;   127 AND DUP 127 BL WITHIN IF DROP [ CHAR . ] LITERAL THEN ;
	COLON   to_char, ">char", NORMAL
	.long	dolit, 0x7f, _and, dup
	.long	dolit, 0x7f, blank, within
	.long	qbranch, to_char_l1
	.long	drop, dolit, '.'
to_char_l1:
	.long	exit

;;; : DEPTH ( -- n ) SP@ SP0 @ SWAP - 1 CELLS / ;
        COLON   depth, "depth", NORMAL
        .long   sp_fetch, sp0, fetch, swap, minus
        .long   dolit, 1, cells, divide, exit

;;; : PICK ( +n -- w ) 1+ CELLS SP@ + @ ;
        COLON   pick, "pick", NORMAL
        .long   increment, cells, sp_fetch, plus, fetch, exit

;;; align to a CELL boundary
;;; : ALIGNED ( b -- a ) ; IMMEDIATE
        COLON   aligned, "aligned", IMMEDIATE
        .long   dolit, CELL_SIZE - 1, plus
        .long   dolit, -CELL_SIZE, _and, exit

;;; .( Memory access )

;;; : +! ( n a -- ) SWAP OVER @ + SWAP ! ;
        COLON   plus_store, "+!", NORMAL
        .long   swap, over, fetch, plus, swap, store, exit

;;; : 2! ( d a -- ) SWAP OVER ! CELL+ ! ;
        COLON   dstore, "2!", NORMAL
        .long   swap, over, store, cell_plus, store, exit

;;; : 2@ ( a -- d ) DUP CELL+ @ SWAP @ ;
        COLON   dfetch, "2@", NORMAL
        .long   dup, cell_plus, fetch, swap, fetch, exit

;;; : COUNT ( b -- b +n ) DUP BYTE+ SWAP C@ ;
        COLON   count, "count", NORMAL
        .long   dup, byte_plus, swap, cfetch, exit

;;; : HERE ( -- a ) CP @ ;
        COLON   here, "here", NORMAL
        .long   cp, fetch, exit

;;; : PAD ( -- a ) HERE 80 + ;
        COLON   pad, "pad", NORMAL
        .long   here, dolit, 80, plus, exit

;;; : TIB ( -- a ) #TIB CELL+ @ ;
        COLON   tib, "tib", NORMAL
        .long   hash_tib, cell_plus, fetch, exit

;;; : NP ( -- a ) CP CELL+ ;
        COLON   np, "np", NORMAL
        .long   cp, cell_plus, exit

;;; : LAST ( -- a ) NP CELL+ ;
        COLON   last, "last", NORMAL
        .long   np, cell_plus, exit


;;; : (dovoc)	( -- )  R> CONTEXT ! ; COMPILE-ONLY
	COLON   dovoc, "(dovoc)", COMPILE_ONLY
	.long	r_from, context, store, exit

;;; \ Make FORTH the context vocabulary
;;; : FORTH	( -- ) COMPILE (dovoc) [ =HEAD ] , [ =LINK ] , ;
	COLON   forth, "forth", NORMAL
	.long	dovoc
	.long	last_name                       ; vocabulary head pointer
	.long	last_name                       ; vocabulary link pointer

;;; : @EXECUTE ( a -- ) @ ?DUP IF EXECUTE THEN ;
        COLON   atexecute, "@execute", NORMAL
        .long   fetch, qdup, qbranch, atexecute_l1
        .long   execute
atexecute_l1:
        .long   exit

;;; : CMOVE ( b b u -- )
;;;   FOR AFT >R COUNT R@ C! R> 1+ THEN NEXT 2DROP ;
        CODE    cmove, "cmove", NORMAL
        ld.w    %r0, [%r10]+                    ; count
        ld.w    %r1, [%r10]+                    ; dst
        ld.w    %r2, [%r10]+                    ; src
        or      %r0, %r0
        jreq    cmove_done
cmove_loop:
        ld.ub   %r3, [%r2]+
        ld.b    [%r1]+, %r3
        xsub    %r0, 1
        jrne    cmove_loop
cmove_done:
        NEXT
        END_CODE


;;; : -TRAILING ( b u -- b u )
;;;   FOR AFT DUP R@ + C@  BL XOR
;;;     IF R> BYTE+ EXIT THEN THEN
;;;   NEXT 0 ;
        COLON   minus_trailing, "-trailing", NORMAL
        .long   to_r
minus_trailing_l1:
        .long   dup, r_fetch, plus, cfetch,  blank, _xor
        .long   qbranch, minus_trailing_l2
        .long   r_from, byte_plus, exit
minus_trailing_l2:
        .long   donext, minus_trailing_l1
        .long   dolit, 0, exit

;;; : FILL ( b u c -- )
;;;   SWAP FOR SWAP AFT 2DUP C! BYTE+ THEN NEXT 2DROP ;
        COLON   fill, "fill", NORMAL
        .long   swap, to_r
fill_l1:
        .long   twodup, cstore, byte_plus
        .long   donext, fill_l1
        .long   twodrop, exit

;;; : ERASE ( b u -- ) 0 FILL ;
        COLON   erase, "erase", NORMAL
        .long   dolit, 0, fill, exit

;;; : PACK$ ( b u a -- a ) \ null terminated
;;;   DUP >R  2DUP C! BYTE+ SWAP CMOVE  R> ;
        COLON   pack_dollar, "pack$", NORMAL
	.long	aligned, dup, to_r
	.long	twodup, cstore, byte_plus
	.long	swap, cmove, r_from, exit


;;; .( Numeric Output ) \ single precision

;;; : DIGIT ( u -- c ) 9 OVER < 7 AND + [ CHAR 0 ] LITERAL + ;
        COLON   digit, "digit", NORMAL
        .long   dolit, 9, over, less, dolit, 7, _and, plus, dolit, '0', plus, exit

;;; : EXTRACT ( n base -- n c ) 0 SWAP UM/MOD SWAP DIGIT ;
        COLON   extract, "extract", NORMAL
        .long   dolit, 0, swap, um_slash_mod, swap, digit, exit

;;; : <# ( -- ) PAD HLD ! ;
        COLON   less_hash, "<#", NORMAL
        .long   pad, hld, store, exit
;;;
;;; : HOLD ( c -- ) HLD @ BYTE- DUP HLD ! C! ;
        COLON   hold, "hold", NORMAL
        .long   hld, fetch, byte_minus, dup, hld, store, cstore, exit

;;; : # ( u -- u ) BASE @ EXTRACT HOLD ;
        COLON   hash, "#", NORMAL
        .long   base, fetch, extract, hold, exit

;;; : #S ( u -- 0 ) BEGIN # DUP WHILE REPEAT ;
        COLON   hash_s, "#s", NORMAL
hash_s_l1:
        .long   hash, dup
        .long   qbranch, hash_s_l2
        .long   branch, hash_s_l1
hash_s_l2:
        .long   exit

;;; : SIGN ( n -- ) 0< IF [ CHAR - ] LITERAL HOLD THEN ;
        COLON   sign, "sign", NORMAL
        .long   zero_less, qbranch, sign_l1
        .long   dolit, '-', hold
sign_l1:
        .long   exit

;;; : #> ( w -- b u ) DROP HLD @ PAD OVER - ;
        COLON   hash_greater, "#>", NORMAL
        .long   drop, hld, fetch, pad, over, minus, exit

;;; : (str) ( w -- b u ) DUP >R ABS <# #S R> SIGN #> ;
        COLON   paren_str, "(str)"
        .long   dup, to_r, abs, less_hash, hash_s, r_from, sign, hash_greater, exit

;;; : HEX ( -- ) 16 BASE ! ;
        COLON   hex, "hex", NORMAL
        .long   dolit, 16, base, store, exit

;;; : DECIMAL ( -- ) 10 BASE ! ;
        COLON   decimal, "decimal", NORMAL
        .long   dolit, 10, base, store, exit


;;; .( Numeric Input ) \ single precision

;;; : DIGIT? ( c base -- u t )
;;;   >R [ CHAR 0 ] LITERAL - 9 OVER <
;;;   IF 7 - DUP 10 < OR THEN DUP R> U< ;
        COLON   digitq, "digit?", NORMAL
        .long	to_r, dolit, '0', minus
	.long	dolit, 9, over, less
	.long	qbranch, digitq_l1
;	.long	dolit, 7, minus           ;*uppercase*
	.long	dolit, 'a' - '0' - 10, minus    ;*lower case*
	.long	dup, dolit, 10, less, _or
digitq_l1:
	.long	dup, r_from, uless, exit

;;; : NUMBER? ( a -- n T, a F )
;;;   BASE @ >R  0 OVER COUNT ( a 0 b n)
;;;   OVER C@ [ CHAR $ ] LITERAL =
;;;   IF HEX SWAP BYTE+ SWAP 1- THEN ( a 0 b' n')
;;;   OVER C@ [ CHAR - ] LITERAL = >R ( a 0 b n)
;;;   SWAP R@ - SWAP R@ + ( a 0 b" n") ?DUP
;;;   IF 1- ( a 0 b n)
;;;     FOR DUP >R C@ BASE @ DIGIT?
;;;       WHILE SWAP BASE @ * +  R> BYTE+
;;;     NEXT R@ ( ?sign) NIP ( b) IF NEGATE THEN SWAP
;;;       ELSE R> R> ( b index) 2DROP ( digit number) 2DROP 0
;;;       THEN DUP
;;;   THEN R> ( n ?sign) 2DROP R> BASE ! ;
        COLON   numberq, "number?", NORMAL
	.long	base, fetch, to_r, dolit, 0, over, count
	.long	over, cfetch, dolit, '$', equal
	.long	qbranch, numberq_l1
	.long	hex, swap, byte_plus
	.long	swap, decrement
numberq_l1:
	.long	over, cfetch, dolit, '-', equal, to_r
	.long	swap, r_fetch, minus, swap, r_fetch, plus, qdup
	.long	qbranch, numberq_l6
	.long	decrement, to_r
numberq_l2:
	.long	dup, to_r, cfetch, base, fetch, digitq
	.long	qbranch, numberq_l4
	.long	swap, base, fetch, times, plus, r_from
	.long	byte_plus
	.long	donext, numberq_l2
	.long	r_fetch, swap, drop
	.long	qbranch, numberq_l3
	.long	negate
numberq_l3:
	.long	swap
	.long	branch, numberq_l5
numberq_l4:
	.long	r_from, r_from, twodrop, twodrop, dolit, 0
numberq_l5:
	.long	dup
numberq_l6:
	.long	r_from, twodrop
	.long	r_from, base, store, exit

;;; .( Basic I/O )
;;;
;;; : KEY? ( -- c T | F ) 'KEY? @EXECUTE ;
        COLON   key_query, "key?", NORMAL
        .long   tkey_query, atexecute, exit

;;; : KEY ( -- c ) BEGIN KEY? UNTIL ;
        COLON   key, "key", NORMAL
key_l1:
        .long   key_query, qbranch, key_l1, exit

;;; : EMIT ( c -- ) 'EMIT @EXECUTE ;
        COLON   emit, "emit", NORMAL
        .long   temit, atexecute, exit

;;; if key is pressed, wait for second key press
;;; return true if the second key is enter
;;; : ENOUGH? ( -- f ) KEY? DUP IF 2DROP KEY 13 = THEN ;
        COLON   enoughq, "enough?", NORMAL
        .long   key_query, dup
        .long   qbranch, enoughq_l1
        .long   twodrop, key, dolit, 13, equal
enoughq_l1:
        .long   exit

;;; :  PACE ( -- ) 11 EMIT ;
;;; : SPACE ( -- ) BL EMIT ;
        COLON   space, "space", NORMAL
        .long   blank, emit, exit

;;; : CHARS ( +n c -- ) SWAP 0 MAX FOR AFT DUP EMIT THEN NEXT DROP ;
        COLON   chars, "chars", NORMAL
        .long   swap, dolit, 0, max
        .long   to_r, branch, chars_l2
chars_l1:
        .long   dup, emit
chars_l2:
        .long   donext, chars_l1
        .long   drop, exit

;;; : SPACES ( +n -- ) BL CHARS ;
        COLON   spaces, "spaces", NORMAL
        .long   blank, chars, exit

;;; : do$ ( -- a )
;;;   R> R@ R> COUNT + ALIGNED >R SWAP >R ; COMPILE-ONLY
	COLON   do_dollar, "do$", COMPILE_ONLY
	.long	r_from, r_fetch, r_from, count, plus
	.long	aligned, to_r, swap, to_r, exit

;;; : ($") ( -- a ) do$ ; COMPILE-ONLY
	COLON   do_dollar_quote, "($\042)", COMPILE_ONLY
	.long	do_dollar, exit

;;; : TYPE ( b u -- ) FOR AFT COUNT EMIT THEN NEXT DROP ;
	COLON   type, "type", NORMAL
	.long	to_r
	.long	branch, type_l2
type_l1:
	.long	dup, cfetch, emit, increment
type_l2:
	.long	donext, type_l1
	.long	drop, exit

;;; : .$ ( a -- ) COUNT TYPE ;
        COLON   dot_dollar, ".$", NORMAL
        .long   count, type, exit

;;; : (.") ( -- ) do$ .$ ; COMPILE-ONLY
        COLON   do_dot_quote, "(.\042)", COMPILE_ONLY
        .long   do_dollar, dot_dollar, exit

;;; : CR ( -- ) 13 EMIT 10 EMIT ;
        COLON   cr, "cr", NORMAL
        .long   dolit, carriage_return, emit
        .long   dolit, line_feed, emit, exit

;;; :  .R ( n +n -- ) >R (str)      R> OVER - SPACES TYPE ;
        COLON   dot_r, ".r", NORMAL
        .long   to_r, paren_str, r_from, over, minus, spaces, type, exit

;;; : U.R ( u +n -- ) >R <# #S #> R> OVER - SPACES TYPE ;
        COLON   u_dot_r, "u.", NORMAL
        .long   to_r, less_hash, hash_s, hash_greater, r_from, over, minus, spaces, type, exit

;;; : U. ( u -- ) <# #S #> SPACE TYPE ;
        COLON   u_dot, "u.", NORMAL
        .long   less_hash, hash_s, hash_greater, space, type, exit

;;; :  . ( w -- ) BASE @ 10 XOR IF U. EXIT THEN (str) SPACE TYPE ;
        COLON   dot, ".", NORMAL
        .long   base, fetch, dolit, 10, _xor
        .long   qbranch, dot_l1
        .long   u_dot, exit
dot_l1:
        .long   paren_str, space, type, exit

;;; : ? ( a -- ) @ . ;
        COLON   question, "?", NORMAL
        .long   fetch, dot, exit

;;; : H? ( h -- ) H@ . ;
        COLON   hquestion, "h?", NORMAL
        .long   hfetch, dot, exit

;;; : C? ( b -- ) C@ . ;
        COLON   cquestion, "c?", NORMAL
        .long   cfetch, dot, exit


;;; .( Parsing )
;;;
;;; : (parse) ( b u c -- b u delta \ <string> )
;;;   temp !  OVER >R  DUP \ b u u
;;;   IF 1-  temp @ BL =
;;;     IF \ b u' \ 'skip'
;;;       FOR COUNT temp @  SWAP - 0< INVERT  WHILE
;;;       NEXT ( b) R> DROP 0 DUP EXIT \ all delim
;;;         THEN  1-  R>
;;;     THEN OVER SWAP \ b' b' u' \ 'scan'
;;;     FOR COUNT temp @ SWAP -  temp @ BL =
;;;       IF 0< THEN WHILE
;;;     NEXT DUP >R  ELSE R> DROP DUP >R 1-
;;;                  THEN OVER -  R>  R> - EXIT
;;;   THEN ( b u) OVER R> - ;
	COLON   paren_parse, "(parse)", NORMAL
	.long	temp, store, over, to_r, dup
	.long	qbranch, paren_parse_l8
	.long	decrement, temp, fetch, blank, equal
	.long	qbranch, paren_parse_l3
	.long	to_r
paren_parse_l1:
	.long	blank, over, cfetch
	.long	minus, zero_less, invert
	.long	qbranch, paren_parse_l2
	.long	increment
	.long	donext, paren_parse_l1
	.long	r_from, drop, dolit, 0, dup, exit
paren_parse_l2:
	.long	r_from
paren_parse_l3:
	.long	over, swap
	.long	to_r
paren_parse_l4:
	.long	temp, fetch, over, cfetch, minus
	.long	temp, fetch, blank, equal
	.long	qbranch, paren_parse_l5
	.long	zero_less
paren_parse_l5:
	.long	qbranch, paren_parse_l6
	.long	increment
	.long	donext, paren_parse_l4
	.long	dup, to_r
	.long	branch, paren_parse_l7
paren_parse_l6:
	.long	r_from, drop, dup
	.long	increment, to_r
paren_parse_l7:
	.long	over, minus
	.long	r_from, r_from, minus, exit
paren_parse_l8:
	.long	over, r_from, minus, exit

;;; : PARSE ( c -- b u \ <string> )
;;;   >R  TIB >IN @ +  #TIB @ >IN @ -  R> (parse) >IN +! ;
	COLON   parse, "parse", NORMAL
	.long	to_r, tib, to_in, fetch, plus
	.long	hash_tib, fetch, to_in, fetch, minus
	.long	r_from, paren_parse
	.long	to_in, plus_store, exit

;;; : .( ( -- ) [ CHAR ) ] LITERAL PARSE TYPE ; IMMEDIATE
        COLON   dot_paren, ".(", IMMEDIATE
        .long   dolit, ')', parse, type, exit

;;; : ( ( -- ) [ CHAR ) ] LITERAL PARSE 2DROP ; IMMEDIATE
        COLON   paren, "(", IMMEDIATE
        .long   dolit, ')', parse, twodrop, exit

;;; : \ ( -- ) #TIB @ >IN ! ; IMMEDIATE
        COLON   backslash, "\\", IMMEDIATE
        .long   hash_tib, fetch, to_in, store, exit
;;;
;;; : CHAR ( -- c ) BL PARSE DROP C@ ;
        COLON   char, "char", NORMAL
        .long   blank, parse, drop, cfetch, exit

;;; : CTRL ( -- c ) CHAR $001F AND ;
        COLON   ctrl, "ctrl", NORMAL
        .long   char, 0x1f, _and, exit

;;; this puts the name in the right place for being the next defined item
;;; : TOKEN ( -- a \ <string> ) \ and reserve space for dictionary header
;;;   BL PARSE 31 MIN NP @ [ =DICTIONARY-HEADER-CELLS ] CELLS + PACK$ ;
        COLON   token, "token", NORMAL
	.long	blank, parse
	.long	dolit, 31, min
	.long	np, fetch, dolit, DICTIONARY_HEADER_CELLS, cells, plus, pack_dollar
	.long	exit

;;; : WORD ( c -- a \ <string> ) PARSE HERE PACK$ ;
	COLON   word, "word", NORMAL
	.long	parse, here, pack_dollar, exit

;;; .( Dictionary Search )

;;;  0: code address
;;;  4: param address
;;;  8: flags
;;; 12: link address
;;; 16: count (byte)  (name adress points here)
;;; 17: name string (count bytes)
;;; 17+count: (zeros as required to .balign 4)
DICTIONARY_HEADER_CELLS = 3                  ; code ptr, flags, link ptr

;;; : NAME>CODE ( na -- ca ) 4 CELLS - ;
        COLON   name_to_code, "name>code", NORMAL
        .long   dolit, 4, cells, minus, exit

;;; : NAME>PARAM ( na -- pa ) 3 CELLS - ;
        COLON   name_to_param, "name>param", NORMAL
        .long   dolit, 3, cells, minus, exit

;;; : NAME>FLAGS ( na -- fa ) 2 CELLS - ;
        COLON   name_to_flags, "name>flags", NORMAL
        .long   dolit, 2, cells, minus, exit

;;; : NAME>LINK ( na -- la ) CELL- ;
        COLON   name_to_link, "name>link", NORMAL
        .long   cell_minus, exit

;;; : CODE>NAME ( ca -- na ) 4 CELLS + ;
        COLON   code_to_name, "code>name", NORMAL
        .long   dolit, 4, cells, plus, exit

;;; return TRUE if counted strings are equal
;;; : SAME? ( a a -- a a f )
;;;   2DUP COUNT NIP SWAP COUNT NIP 2DUP =
;;;   IF DROP  \ a a u
;;;     FOR AFT OVER R@ + BYTE+ C@
;;;             OVER R@ + BYTE+ C@ XOR
;;;       IF R> DROP FALSE EXIT THEN THEN
;;;     NEXT TRUE
;;;   ELSE 2DROP FALSE
;;;   THEN ;
        COLON   sameq, "same?", NORMAL
        .long   twodup, count, nip, swap, count, nip, twodup, equal
        .long   qbranch, same_l3
	.long	drop, to_r
	.long	branch, same_l2
same_l1:
	.long	over, r_fetch, plus, byte_plus, cfetch
	.long	over, r_fetch, plus, byte_plus, cfetch
	.long	_xor
	.long	qbranch, same_l2
	.long	r_from, drop, dolit, FALSE,  exit
same_l2:
	.long	donext, same_l1
	.long	dolit, TRUE, exit
same_l3:
	.long	twodrop, dolit, FALSE, exit

;;; : find ( a va -- ca na, a F )
;;;   BEGIN @ DUP                \ a na na
;;;     IF                       \ a na
;;;       SAME? 0=               \ a na f
;;;     ELSE                     \ a na
;;;       DROP FALSE EXIT        \ a F
;;;     THEN
;;;   WHILE NAME>LINK            \ a la
;;;   REPEAT
;;;   \ a na
;;;   NIP DUP NAME>CODE SWAP ;
        COLON   find, "find", NORMAL
find_l1:
        .long   fetch, dup, qbranch, find_l2
        .long   sameq, zero_equal
        .long   branch, find_l3
find_l2:
        .long   drop, dolit, FALSE
        .long   exit
find_l3:
        .long   qbranch, find_l4
        .long   name_to_link, branch, find_l1
find_l4:
        .long   nip, dup, name_to_code, swap
        .long   exit


;;; : NAME? ( a -- ca na, a F )
;;;   CONTEXT  DUP 2@ XOR IF CELL- THEN >R \ context<>also
;;;   BEGIN R>  CELL+  DUP >R  @  ?DUP
;;;   WHILE find  ?DUP
;;;   UNTIL R> DROP EXIT THEN R> DROP  0 ;
	COLON   nameq, "name?", NORMAL
	.long	context, dup, dfetch, _xor
	.long	qbranch, nameq_l1
	.long	cell_minus
nameq_l1:
        .long	to_r
nameq_l2:
        .long	r_from, cell_plus, dup, to_r
	.long	fetch, qdup
	.long	qbranch, nameq_l3
	.long	find, qdup
	.long	qbranch, nameq_l2
	.long	r_from, drop, exit
nameq_l3:
	.long	r_from, drop
	.long	dolit, 0, exit


;;; .( Terminal )
;;;
;;; : ^H ( b b b -- b b b ) \ backspace
;;;   >R OVER R@ < DUP
;;;   IF [ CTRL H ] LITERAL 'ECHO @EXECUTE THEN R> + ;
	COLON   bksp, "^h", NORMAL
	.long	to_r, over, r_from, swap, over, _xor
	.long	qbranch, bksp_l1
	.long	dolit, backspace, techo, atexecute, dolit, 1, minus
	.long	blank, techo, atexecute
	.long	dolit, backspace, techo, atexecute
bksp_l1:
	.long	exit


;;; : TAP ( bot eot cur key -- bot eot cur )
;;;   DUP 'ECHO @EXECUTE OVER C! 1+ ;
        COLON   tap, "tap", NORMAL
        .long   dup, techo, atexecute, over, cstore, increment, exit

;;; : kTAP ( bot eot cur key -- bot eot cur )
;;;   DUP 13 XOR
;;;   IF [ CTRL H ] LITERAL XOR IF BL TAP ELSE ^H THEN EXIT
;;;   THEN DROP NIP DUP ;
        COLON   ktap, "ktap", NORMAL
	.long	dup, dolit, carriage_return, _xor
	.long	qbranch, ktap_l2
	.long	dolit, delete, _xor
	.long	qbranch, ktap_l1
	.long	blank, tap, exit
ktap_l1:
	.long	bksp, exit
ktap_l2:
	.long	drop, swap, drop, dup, exit

;;; : accept ( b u -- b u )
;;;   OVER + OVER
;;;   BEGIN 2DUP XOR
;;;   WHILE  KEY  DUP BL -  95 U<
;;;     IF TAP ELSE 'TAP @EXECUTE THEN
;;;   REPEAT DROP  OVER - ;

        COLON   accept, "accept", NORMAL
        .long   over, plus, over
accept_l1:
        .long   twodup, _xor, qbranch, accept_l4
        .long   key, dup, blank, minus, dolit, 95, uless
        .long   qbranch, accept_l2
        .long   tap, branch, accept_l3
accept_l2:
        .long   ttap, atexecute
accept_l3:
        .long   branch, accept_l1
accept_l4:
        .long   drop, over, minus, exit

;;; : EXPECT ( b u -- ) 'EXPECT @EXECUTE SPAN ! DROP ;
        colon   expect, "expect", NORMAL
	.long	texpect, atexecute, span, store, drop, exit

;;; : QUERY ( -- )
;;;   TIB 80 'EXPECT @EXECUTE #TIB !  0 NIP >IN ! ;
	COLON   query, "query", NORMAL
	.long	tib, dolit, 80, texpect, atexecute, hash_tib, store
	.long	drop, dolit, 0, to_in, store, exit

;;; .( Error handling )
;;;
;;; : CATCH ( ca -- err#/0 )
;;;   SP@ >R  HANDLER @ >R  RP@ HANDLER !
;;;   EXECUTE
;;;   R> HANDLER !  R> DROP  0 ;
	COLON   catch, "catch", NORMAL
	.long	sp_fetch
        .long   to_r, handler, fetch, to_r
	.long	rp_fetch, handler, store, execute
	.long	r_from, handler, store
	.long	r_from, drop, dolit, 0, exit

;;; : THROW ( err# -- err# )
;;;   HANDLER @ RP!  R> HANDLER !  R> SWAP >R SP! DROP R> ;
	COLON   throw, "throw", NORMAL
	.long	handler, fetch, rp_store
	.long	r_from, handler, store
	.long	r_from, swap, to_r, sp_store
	.long	drop, r_from, exit

;;; VARIABLE NULL$
        VARIABLE null_dollar, "null$", NORMAL
        .long   0

;;; : ABORT ( -- ) NULL$ THROW ;
        COLON   abort, "abort", NORMAL
        .long   null_dollar, throw, exit
;;;
;;; : (abort") ( f -- ) IF do$ THROW THEN do$ DROP ; COMPILE-ONLY
	COLON   do_abort_quote, "(abort\042)", COMPILE_ONLY
	.long	qbranch, do_abort_quote_l1
	.long	do_dollar, throw
do_abort_quote_l1:
	.long	do_dollar, drop, exit

;;; .( Interpret )
;;;
;;; : $INTERPRET ( a -- )
;;;   NAME?  ?DUP
;;;   IF @ [ =COMP ] LITERAL AND
;;;     ABORT" compile ONLY" EXECUTE EXIT
;;;   THEN
;;;   'NUMBER @EXECUTE
;;;   IF EXIT THEN THROW ;
	COLON   dollar_interpret, "$interpret", NORMAL
	.long	nameq, qdup
	.long	qbranch, dollar_interpret_l1
	.long	name_to_flags, fetch, dolit, COMPILE_ONLY, _and
	.long	do_abort_quote
	FSTRING " compile only "
	.long	execute
        .long   exit
dollar_interpret_l1:
	.long	tnumber, atexecute
	.long	qbranch, dollar_interpret_l2
	.long	exit
dollar_interpret_l2:
	.long	throw

;;; : [ ( -- ) [ ' $INTERPRET ] LITERAL 'EVAL ! ; IMMEDIATE
	COLON   left_bracket, "[", IMMEDIATE
	.long	dolit, dollar_interpret, teval, store, exit

;;; : .OK ( -- ) [ ' $INTERPRET ] LITERAL 'EVAL @ = IF ."  ok" THEN CR ;
	COLON   dot_ok, ".ok", NORMAL
	.long	dolit, dollar_interpret, teval, fetch, equal
	.long	qbranch, dot_ok_l1
	.long	do_dot_quote
	FSTRING	" Ok"
dot_ok_l1:
	.long	cr, exit


;;; : ?STACK ( -- ) DEPTH 0< IF $" underflow" THROW THEN ;
	COLON   qstack, "?stack", NORMAL
	.long	depth, zero_less
	.long	do_abort_quote
	FSTRING	" underflow"
	.long	exit

;;; : EVAL ( -- )
;;;   BEGIN TOKEN DUP C@
;;;   WHILE 'EVAL @EXECUTE ?STACK
;;;   REPEAT DROP 'PROMPT @EXECUTE ;
        COLON   eval, "eval", NORMAL
eval_l1:
	.long	token, dup, cfetch
	.long	qbranch, eval_l2
	.long	teval, atexecute, qstack
	.long	branch, eval_l1
eval_l2:
	.long	drop, tprompt, atexecute, exit

;;; .( Device I/O )
;;;
;;; CODE IO? ( -- f ) \ FFFF is an impossible character
;;;   XOR BX, BX
;;;   MOV DL, # $0FF  \ input
;;;   MOV AH, # 6     \ MS-DOS Direct Console I/O
;;;   INT $021
;;;   0<> IF          \ ?key ready
;;;     OR AL, AL
;;;     0= IF         \ ?extended ascii code
;;;       INT $021
;;;       MOV BH, AL  \ extended code in msb
;;;     ELSE MOV BL, AL
;;;     THEN
;;;     PUSH BX
;;;     MOVE BX, # -1
;;;   THEN
;;;   PUSH BX
;;;   NEXT
;;; END-CODE


;;; input character
        CODE    rx_query, "rx?", NORMAL         ; ( -- c T | F )
        xcall   sio_input_available
        or      %r0, %r0
        jreq    rx_query_no_character
        xcall   sio_get_char
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        ld.w    %r0, TRUE
rx_query_no_character:
        xsub    %r10, CELL_SIZE
        ld.w    [%r10], %r0
        NEXT
        END_CODE

;;; output a character
        CODE    tx_store, "tx!", NORMAL         ; ( c -- )
        ld.w    %r0, [%r10]+
        xcall   sio_put_char
        NEXT
        END_CODE

;;; : !IO ( -- ) ; IMMEDIATE \ initialize I/O device


;;; .( Shell )
;;;
;;; : PRESET ( -- ) SP0 @ SP!  [ =TIB ] LITERAL #TIB CELL+ ! ;
        COLON   preset, "preset", NORMAL
	.long	sp0, fetch, sp_store
	.long	dolit, terminal_buffer, hash_tib, cell_plus, store
	.long	exit

;;; : XIO ( a a a -- ) \ reset 'EXPECT 'TAP 'ECHO 'PROMPT
;;;   [ ' accept ] LITERAL 'EXPECT !
;;;   'TAP !  'ECHO !  'PROMPT ! ;
        COLON   xio, "xio", NORMAL
        .long   dolit, accept, texpect, store
        .long   ttap, store
        .long   techo, store
        .long   tprompt, store
        .long   exit

;;; : FILE ( -- )
;;;   [ ' PACE ] LITERAL [ ' DROP ] LITERAL [ ' kTAP ] LITERAL XIO ;
;;;
;;; : HAND ( -- )
;;;   [ ' .OK  ] LITERAL 'EMIT @ [ ' kTAP ] LITERAL XIO ;
        COLON   hand, "hand", NORMAL
        .long   dolit, dot_ok, temit, fetch, dolit, ktap, xio, exit

;;; CREATE I/O  ' ?RX , ' TX! , \ defaults
;;;
;;; : CONSOLE ( -- ) I/O 2@ 'KEY? 2! HAND ;
        COLON   console, "console", NORMAL
        .long   dolit, rx_query, tkey_query, store
        .long   dolit, tx_store, temit, store
        .long   hand, exit

;;; : que ( -- ) QUERY EVAL ;
;        COLON   que, "que", NORMAL
;        .long   query, eval, exit

;;; : QUIT ( -- ) \ clear return stack ONLY
;;;   RP0 @ RP!
;;;   BEGIN [COMPILE] [
;;;     BEGIN [ ' que ] LITERAL CATCH ?DUP
;;;     UNTIL ( a)
;;;     CONSOLE  NULL$ OVER XOR
;;;     IF CR TIB #TIB @ TYPE
;;;        CR >IN @ [ CHAR ^ ] LITERAL CHARS
;;;        CR .$ ."  ? "
;;;     THEN PRESET
;;;   AGAIN ;
        COLON   quit, "quit", NORMAL
	.long	rp0, fetch, rp_store
quit_l1:
	.long	left_bracket
quit_l2:
	.long	query
	.long	dolit, eval, catch
        .long   qdup
	.long	qbranch, quit_l2
	.long	tprompt, fetch, to_r
	.long	console, null_dollar, over, _xor
	.long	qbranch, quit_l3
	.long	space, count, type
	.long	do_dot_quote
	FSTRING	" ? "
quit_l3:
        .long	r_from, dolit, dot_ok, _xor
	.long	qbranch, quit_l4
	.long	dolit, '?', emit
quit_l4:
	.long	preset
	.long	branch, quit_l1


;;; .( Compiler Primitives )

;;; : ' ( -- ca ) TOKEN NAME? IF EXIT THEN THROW ;
        COLON   tick, "'", NORMAL
        .long   token, nameq
        .long   qbranch, tick_l1
        .long   exit
tick_l1:
        .long   throw, exit

;;; : ALLOT ( n -- ) CP +! ;
        COLON   allot, "allot", NORMAL
        .long   cp, plus_store, exit

;;; : , ( w -- ) HERE ALIGNED DUP CELL+ CP ! ! ;
        COLON   comma, ",", NORMAL
        .long   here, aligned, dup, cell_plus, cp
        .long   store, store, exit

;;; : [COMPILE] ( -- \ <string> ) ' , ; IMMEDIATE
        COLON   bracket_compile, "[compile]", IMMEDIATE
        .long   tick, comma, exit

;;; : COMPILE ( -- ) R> DUP @ , CELL+ >R ; COMPILE-ONLY
        COLON   compile, "compile", COMPILE_ONLY
        .long   r_from, dup, fetch, comma, cell_plus, to_r, exit

;;; : LITERAL ( w -- ) COMPILE (dolit) , ; IMMEDIATE
        COLON   literal, "literal", IMMEDIATE
        .long   compile, dolit, comma, exit

;;; : $," ( -- ) [ CHAR " ] LITERAL PARSE HERE PACK$ C@ 1+ ALLOT ;
        COLON   dollar_comma_quote, "$,\042", NORMAL
        .long   dolit, '\"', parse, here, pack_dollar, cfetch, increment, allot, exit

;;; : RECURSE ( -- ) LAST @ CURRENT @ ! ; IMMEDIATE
        COLON   recurse, "recurse", IMMEDIATE
        .long   last, fetch, current, fetch, store, exit


;;; .( Structures )

;;; : FOR ( -- a ) COMPILE >R HERE ; IMMEDIATE
        COLON   for, "for", IMMEDIATE
        .long   compile, to_r, here, exit

;;; : BEGIN ( -- a ) HERE ; IMMEDIATE
        COLON   begin, "begin", IMMEDIATE
        .long   here, exit

;;; : NEXT ( a -- ) COMPILE (next) , ; IMMEDIATE
        COLON   next, "next", IMMEDIATE
	.long	compile, donext, comma, exit

;;; : UNTIL ( a -- ) COMPILE ?branch , ; IMMEDIATE
        COLON   until, "until", IMMEDIATE
        .long   compile, qbranch, comma, exit

;;; : AGAIN ( a -- ) COMPILE  branch , ; IMMEDIATE
        COLON   again, "again", IMMEDIATE
        .long   compile, branch, comma, exit

;;; : IF ( -- A )   COMPILE ?branch HERE 0 , ; IMMEDIATE
        COLON   if, "if", IMMEDIATE
        .long   compile, qbranch, here, dolit, 0, comma, exit

;;; : AHEAD ( -- A ) COMPILE branch HERE 0 , ; IMMEDIATE
        COLON   ahead, "ahead", IMMEDIATE
        .long   compile, branch, here, dolit, 0, comma, exit

;;; : REPEAT ( A a -- ) [COMPILE] AGAIN HERE SWAP ! ; IMMEDIATE
        COLON   repeat, "repeat", IMMEDIATE
        .long   compile, again, here, swap, store, exit

;;; : THEN ( A -- ) HERE SWAP ! ; IMMEDIATE
        COLON   then, "then", IMMEDIATE
        .long  here, swap, store, exit

;;; : AFT ( a -- a A ) DROP [COMPILE] AHEAD [COMPILE] BEGIN SWAP ; IMMEDIATE
        COLON   aft, "aft", IMMEDIATE
        .long   drop, compile, ahead, compile, begin, swap, exit

;;; : ELSE ( A -- A )  [COMPILE] AHEAD SWAP [COMPILE] THEN ; IMMEDIATE
        COLON   else, "else", IMMEDIATE
        .long   compile, ahead, swap, compile, then, exit

;;; : WHILE ( a -- A a )    [COMPILE] IF SWAP ; IMMEDIATE
        COLON   while, "while", IMMEDIATE,
        .long   compile, if, swap, exit

;;; : ABORT" ( -- \ <string> ) COMPILE (abort") $," ; IMMEDIATE
	COLON   abortquote, "abort\042", IMMEDIATE
	.long	compile, do_abort_quote, dollar_quote, exit

;;; : $" ( -- \ <string> ) COMPILE ($") $," ; IMMEDIATE
        COLON   dollar_quote, "$\042", IMMEDIATE
	.long	compile, do_dollar_quote, dollar_comma_quote, exit

;;; : ." ( -- \ <string> ) COMPILE ."| $," ; IMMEDIATE
        COLON   dot_quote, ".\042", IMMEDIATE
	.long	compile, do_dot_quote, dollar_comma_quote, exit


;;; .( Name Compiler )

;;; : ?UNIQUE ( a -- a )
;;;   DUP NAME? IF ."  redefined " OVER .$ THEN DROP ;
        COLON   qunique, "?unique", NORMAL
        .long   dup, nameq
        .long   qbranch, qunique_l1
        .long   do_dot_quote
        FSTRING " redefined "
        .long   over, dot_dollar
qunique_l1:
        .long   drop, exit

;;; \ assumes the name is in the right place and the header
;;; \ has already be reserved (TOKEN does this)
;;; : $,n ( na -- )
;;;   DUP C@
;;;   IF ?UNIQUE
;;;     ( na ) DUP LAST ! \ for OVERT
;;;     ( na ) HERE ALIGNED SWAP
;;;     ( cp na ) DUP NAME>LINK
;;;     ( cp na la) CURRENT @ @  \ previous name
;;;     ( cp na la na') SWAP !
;;;     ( cp na ) DUP COUNT + ALIGNED NP !
;;;     ( cp na ) NAME>PARAM ! EXIT
;;;   THEN $" name" THROW ;
        COLON   dollar_comma_n, "$,n", NORMAL
        .long   dolit, 0x12345678, DEBUG, drop
        .long   dup, cfetch                     ;cfetch for string count (strlen ??)
        .long   qbranch, dollar_comma_n_l1
        .long   qunique
        .long   dup, last, store
        .long   here, aligned, swap
        .long   dolit, 0x12340001, DEBUG, drop
        .long   dup, name_to_link
        .long   current, fetch, fetch
        .long   swap, store
        .long   dolit, 0x12340002, DEBUG, drop
        .long   dup
        .long   dolit, 0x12340003, DEBUG, drop
        .long   count, plus, aligned            ;skip over the name
        .long   np, store
        .long   dolit, 0x12340004, DEBUG, drop
        .long   name_to_param                   ;*****how about code docolon,dovar or same as param
        .long   dolit, 0x12340005, DEBUG, drop
        .long   store
        .long   exit
dollar_comma_n_l1:
        .long   do_dollar_quote
        FSTRING "name"
        .long   throw


;;; .( FORTH Compiler )

;;; : $COMPILE ( a -- )
;;;   NAME? ?DUP
;;;   IF NAME>FLAGS @ [ =IMED ] LITERAL AND
;;;     IF EXECUTE ELSE , THEN EXIT
;;;   THEN
;;;   'NUMBER @EXECUTE
;;;   IF [COMPILE] LITERAL EXIT
;;;   THEN THROW ;
	colon   dollar_compile, "$compile", NORMAL
	.long	nameq, qdup
	.long	qbranch, dollar_compile_l2
	.long	name_to_flags, fetch, dolit, IMMEDIATE, _and
	.long	qbranch, dollar_compile_l1
	.long	execute, exit
dollar_compile_l1:
	.long	comma, exit
dollar_compile_l2:
	.long	tnumber, atexecute
	.long	qbranch, dollar_compile_l3
	.long	literal, exit
dollar_compile_l3:
	.long	throw

;;; : OVERT ( -- ) LAST @ CURRENT @  ! ;
        COLON   overt, "overt", NORMAL
        .long   last, fetch, current, fetch, store, exit

;;; : ; ( -- )
;;;   COMPILE EXIT [COMPILE] [ OVERT ; COMPILE-ONLY IMMEDIATE
        COLON   semicolon, "\073", COMPILE_ONLY + IMMEDIATE
        .long   compile, exit
        .long   left_bracket, overt, exit

;;; : ] ( -- ) [ ' $COMPILE ] LITERAL 'EVAL ! ;
	COLON   right_bracket, "]", NORMAL
	.long	dolit, dollar_compile, teval, store, exit

;;; : CALL, ( ca -- ) \  DTC 8086 relative call
;;;   [ =CALL ] LITERAL , HERE CELL+ - , ;
;;;
;;; : : ( -- \ <string> ) TOKEN $,n [ ' (dolist) ] LITERAL CALL, ] ;
        COLON   colon, ":", NORMAL
        .long   token, dollar_comma_n
        .long   dolit, 0xdecaffe, DEBUG, drop
;;;     [ ' (dolist) ] LITERAL CALL,
        .long   right_bracket, exit
;;;
;;; : IMMEDIATE ( -- ) [ =IMED ] LITERAL LAST @ C@ OR LAST @ C! ;
;;;
;;; .( Defining Words )
;;;
;;; : USER ( u -- \ <string> ) TOKEN $,n OVERT COMPILE (douser) , ;
;;;
;;; : CREATE ( -- \ <string> ) TOKEN $,n OVERT COMPILE (dovar) ;
;;;
;;; : VARIABLE ( -- \ <string> ) CREATE 0 , ;
;;;
;;; .( Tools )
;;;
;;; : (dump_ascii) ( b u -- ) FOR AFT COUNT >CHAR EMIT THEN NEXT DROP ;
        COLON   dump_ascii,"(dump_ascii)", NORMAL
        .long   to_r
        .long   branch, dump_ascii_l2
dump_ascii_l1:
        .long   count, to_char, emit
dump_ascii_l2:
        .long   donext, dump_ascii_l1
        .long   drop, exit
;;;
;;; : (dump) ( b u -- b )
;;;   OVER 4 U.R SPACE FOR AFT COUNT 3 U.R THEN NEXT ;
        COLON   paren_dump, "(dump)", NORMAL
        .long   over, dolit, 8, u_dot_r, space
        .long   to_r
        .long   branch, paren_dump_l2
paren_dump_l1:
        .long   count, dolit, 3, u_dot_r
paren_dump_l2:
        .long   donext, paren_dump_l1
        .long   exit
;;;
;;; : DUMP ( b u -- )
;;;   BASE @ >R HEX  16 /
;;;   FOR CR 16 2DUP (dump) -ROT 2 SPACES _TYPE ENOUGH? 0= WHILE
;;;   NEXT ELSE R> DROP THEN DROP  R> BASE ! ;
        COLON   dump, "dump", NORMAL
        .long   base, fetch, to_r, hex, dolit, 16, divide
        .long   to_r
dump_l1:
        .long   cr, dolit, 16, twodup, paren_dump, minus_rot, dolit, 2, spaces, dump_ascii
        .long   enoughq, zero_equal, qbranch, dump_l2
        .long   donext, dump_l1
        .long   branch, dump_l3
dump_l2:
        .long   r_from, drop
dump_l3:
        .long   drop, r_from, base, store, exit

;;; : .S ( -- ) CR DEPTH FOR AFT R@ PICK . THEN NEXT ."  <tos" ;
        COLON   dot_s, ".s", NORMAL
        .long   cr, depth, to_r
        .long   branch, dot_s_l2
dot_s_l1:
        .long   r_fetch, pick, dot
dot_s_l2:
        .long   donext, dot_s_l1
        .long   do_dot_quote
        FSTRING " <tos"
        .long   exit

;;; : !CSP ( -- ) SP@ CSP ! ;
;;; : ?CSP ( -- ) SP@ CSP @ XOR ABORT" stack depth" ;
;;;
;;; : CODE>NAME ( ca -- na | F )
;;;   CURRENT
;;;   BEGIN CELL+ @ ?DUP WHILE 2DUP
;;;     BEGIN @ DUP WHILE 2DUP NAME>CODE @ XOR
;;;     WHILE NAME>LINK
;;;     REPEAT      THEN NIP ?DUP
;;;   UNTIL NIP NIP EXIT THEN DROP FALSE ;
;*        COLON   code_to_name, "code>name", NORMAL
;*        .long   current
;*to_name_l1:
;*        .long   cell_plus, fetch, qdup
;*        .long   qbranch, to_name_l4
;*        .long   twodup
;*to_name_l2:
;*        .long   fetch, dup
;*        .long   qbranch, to_name_l3
;*        .long   twodup, name_to_code, fetch, _xor
;*        .long   qbranch, to_name_l3
;*        .long   name_to_link
;*        .long   branch, to_name_l2
;*to_name_l3:
;*        .long   nip, qdup
;*        .long   qbranch, to_name_l1
;*        .long   nip, nip, exit
;*to_name_l4:
;*        .long   drop, dolit, FALSE, exit

;**;;; hacked version - the vocabulary structure is not workable yet
;**        COLON   code_to_name, "code>name", NORMAL
;**        .long   current
;**;to_name_l1:
;**        .long   cell_plus, fetch, qdup
;**        .long   qbranch, to_name_l4
;**        .long   twodup
;**to_name_l2:
;**        .long   fetch, dup
;**        .long   qbranch, to_name_l3
;**        .long   twodup, name_to_code, fetch, _xor
;**        .long   qbranch, to_name_l3
;**        .long   name_to_link
;**        .long   branch, to_name_l2
;**to_name_l3:
;**        .long   nip, qdup
;**       ;.long   qbranch, to_name_l1
;**        .long   qbranch, to_name_l5
;**        .long   nip, nip, exit
;**to_name_l4:
;**        .long   drop, dolit, FALSE, exit
;**to_name_l5:
;**        .long   twodrop, dolit, FALSE, exit


;;; disassembler for colon definitions
;;; does no know how to stop - press enter twice to stop
;;; SEE ( -- ) \  token
;;;   ' CR ALIGNED  \ skip over the "xcall dolist"
;;;   BEGIN
;;;     CELL+ DUP @ DUP IF CODE>NAME THEN
;;;     NAME>CODE ?DUP
;;;       IF    SPACE .ID
;;;       ELSE  DUP @ U. \ number
;;;       THEN
;;;   ENOUGH? UNTIL DROP ;

	COLON see, "see", NORMAL
	.long	tick
	.long	cr, aligned
see_l1:
	.long	cell_plus, dup, fetch, dup
	.long	qbranch, see_l2
	.long	code_to_name
see_l2:
	.long	qdup
	.long	qbranch, see_l3
	.long	space, dot_id
	.long	branch, see_l4
see_l3:
	.long	dup, fetch, u_dot
see_l4:
	.long	enoughq, qbranch, see_l1
	.long	drop, exit

;;; : .ID ( na -- )
;;;   ?DUP IF COUNT $001F AND TYPE EXIT THEN ." {noName}" ;
        COLON   dot_id, ".id", NORMAL
        .long   qdup, qbranch, dot_id_l1
        .long   count, type, exit
dot_id_l1:
	.long	do_dot_quote
	FSTRING	"{no-name}"
        .long   exit

;;; : WORDS ( -- )
;;;   CR  CONTEXT @
;;;   BEGIN @ ?DUP
;;;   WHILE DUP SPACE .ID NAME>LINK  ENOUGH?
;;;   UNTIL DROP THEN ;
        COLON   words, "words", NORMAL
        .long   cr, context, fetch
words_l1:
        .long   fetch, qdup
        .long   qbranch, words_l2
        .long   dup, space, dot_id, name_to_link
        .long   enoughq, qbranch, words_l1
        .long   drop
words_l2:
        .long   exit


;;; .( Hardware reset )
;;;
;;; \ version
;;;
;;; $100 CONSTANT VER ( -- u )
;;;   \ hi byte = major revision in decimal
;;;   \ lo byte = minor revision in decimal
;;;
;;; BANNER ( -- ) CR <message> CR
        COLON   banner, "banner", NORMAL
        .long   cr
	.long	do_dot_quote
	FSTRING	"S33 forth interpreter"
        .long   cr, exit

;;; CREATE 'BOOT  ' hi , \ application vector
;;;
;;; : COLD ( -- )
;;;   \ init CPU
;;;   \ init stacks
;;;   \ init user area
;;;   \ init IP
;;;   PRESET  'BOOT @EXECUTE
;;;   FORTH CONTEXT @ DUP CURRENT D! OVERT
;;;   QUIT ;
        COLON   cold, "cold", NORMAL
        .long   preset
        .long   forth, context, fetch, dup, current, dstore, overt
        ;.long   tboot, atexecute, quit
        .long   banner
        .long   quit

        COLON   nop, "nop", NORMAL
        .long   exit

        CODE    BREAKPOINT, "(brk)", NORMAL     ;debug
        xcall   xdebug                          ;debug
        xld.w   %r0, bpt
        xcall   sio_put_string
s1:     jp      s1                              ;debug
bpt:    .asciz  "STOPPED\r\n"
        .balign 4

        CODE    DEBUG, "(debug)", NORMAL        ;debug
        xcall   xdebug                          ;debug
        NEXT                                    ;debug

        COLON   led, "led", NORMAL              ;debug
        .long   invert, dolit, 0x04000000, hstore, exit

        COLON   delay, "delay", NORMAL          ;debug
        .long   dolit, 50000
delay_l1:
        .long   decrement, qdup, qbranch, delay_l2
        .long   branch, delay_l1
delay_l2:
        .long   exit

        COLON   flash, "flash", NORMAL          ;debug
        .long   dolit, 0, led, delay
        .long   dolit, 1, led, delay
        .long   dolit, 2, led, delay
        .long   dolit, 3, led, delay
        .long   dolit, 4, led, delay
        .long   dolit, 5, led, delay
        .long   dolit, 6, led, delay
        .long   dolit, 7, led, delay
        .long   dolit, 0, led
        .long   exit


;;; finish off the dictionary
        .data
        .balign 4

end_of_dictionary:
	.long	0,0

        .space  65536                           ; space for more names
end_of_dictionary_memory:

last_name = __last_name                         ; should be the final name


;;; finish off the code
        .text

        .balign 4

end_of_code:
        .space  65536                           ; space for more code
end_of_code_memory:
