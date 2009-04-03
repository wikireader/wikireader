;;; forth.s
;;; based on the public domain eforth implementations
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

;;; Version (no minor values, just increment)
BUILD_NUMBER = 1

;;; some character constants
backspace = 0x08
line_feed = 0x0a
carriage_return = 0x0d
delete = 0x7f

;;; some special constants
BYTES_PER_CELL = 4
BITS_PER_BYTE = 8
BITS_PER_CELL = (BITS_PER_BYTE * BYTES_PER_CELL)

FALSE = 0
TRUE = -1

;;; header flags
FLAG_IMMEDIATE = 0x80
FLAG_COMPILE_ONLY = 0x40
FLAG_NORMAL = 0


;;; registers (C preserves r0..r3)
;;; r0     forth ip
;;; r1     forth sp
;;; r2     forth pp
;;; r3     forth w
;;; r4     C result low
;;; r5     C result high
;;; r6     C argument 1
;;; r7     C argument 2
;;; r8     C argument 3
;;; r9     C argument 4
;;; r10..r14  used by C and/or extended asm instructions
;;; r15    __dp for C

        .macro  NEXT                            ; inner interpreter
        ld.w    %r2, [%r0]+                     ; incr IP (%r0)
        ld.w    %r3, [%r2]+                     ; %r2 -> param address
        jp      %r3                             ; execute the code
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
;;;  0: code address
;;;  4: param address
;;;  8: flags
;;; 12: link address
;;; 16: count (byte)  (name adress points here)
;;; 17: name string (count bytes)
;;; 17+count: (zeros as required to .balign 4)

        .section .forth_dict, "wa"
        .balign 4
        .section .forth_param, "wax"
        .balign 4

__last_name = 0                                 ; to link the list

        .macro  HEADER, label, name, flags, code

        .section .forth_dict
        .balign 4
        .global \label
\label\():
        .long   \code                           ; code
l_param_\@:
        .long   param_\label                    ; param
l_flags_\@:
        .long   \flags                          ; flags

prev_\label = __last_name
l_link_\@:
        .long   prev_\label                     ; link

        .global name_\label
name_\label\():
__last_name = .
        FSTRING "\name"

DICTIONARY_HEADER_CELLS  = ( name_\label - \label ) / BYTES_PER_CELL
DICTIONARY_CODE_OFFSET   = ( name_\label - \label ) / BYTES_PER_CELL
DICTIONARY_PARAM_OFFSET  = ( name_\label - l_param_\@ ) / BYTES_PER_CELL
DICTIONARY_FLAGS_OFFSET  = ( name_\label - l_flags_\@ ) / BYTES_PER_CELL
DICTIONARY_LINK_OFFSET   = ( name_\label - l_link_\@ ) / BYTES_PER_CELL

        .section .forth_param
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


;;; constant definitions

        .macro  CONSTANT, label, name, flags
        HEADER  \label, "\name", \flags, param_doconst
        .endm


;;; user variables sections

        .section .user_defaults, "a"
        .balign 4
user_defaults:

        .section .user_variables, "wa"
        .balign 4
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

        .balign 4
terminal_buffer:
        .space   65536
        .global initial_stack_pointer
initial_stack_pointer:     ; NOTE: stack underflows over return space!
        .space   65536
        .global initial_return_pointer
initial_return_pointer:


;;; Program Code
        .section .text
        .global main
main:
        xld.w   %r15, __dp
        xld.w   %r1, initial_stack_pointer
        xld.w   %r4, initial_return_pointer
        ld.w    %sp, %r4
        xld.w   %r0, cold_start                 ; initial ip value
        NEXT

        .balign 4                               ; forth byte code must be aligned
cold_start:
        .long   cold, branch, cold_start        ; just run cold in a loop


;;; .( Special interpreters )

        CODE    dolit, "(dolit)", FLAG_COMPILE_ONLY ; ( -- w ) COMPILE-ONLY
        ld.w    %r3, [%r0]+
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r3
        NEXT
        END_CODE

        CODE    docolon, "(docolon)", FLAG_COMPILE_ONLY ; ( -- )
        pushn   %r0                             ; save previous ip
        ld.w    %r0, [%r2]                      ; ip = param address
        NEXT
        END_CODE

        CODE    execute, "execute", FLAG_NORMAL ; ( a -- )
        ld.w    %r2, [%r1]+                     ; point to code ptr
        ld.w    %r3, [%r2]+                     ; code / param address
        jp      %r3                             ; execute the code
        END_CODE

        CODE    exit, "exit", FLAG_NORMAL       ; ( -- )
        popn    %r0                             ; restore ip
        NEXT
        END_CODE


;;; .( Loop & Branch - absolute address )

;;; : (next) ( -- ) \ hiLevel model  16bit absolute branch
;;;   r> r> dup if 1- >r @ >r exit then drop cell+ >r ;
        CODE    donext, "(next)",  FLAG_COMPILE_ONLY
        ld.w    %r4, [%sp]
        or      %r4, %r4
        jreq    donext_l1
        xsub    %r4, 1
        ld.w    [%sp], %r4
        ld.w    %r0, [%r0]
        NEXT
donext_l1:
        add     %sp, 1
no_branch:
        add     %r0, BYTES_PER_CELL
        NEXT
        END_CODE

        CODE    qbranch, "?branch", FLAG_COMPILE_ONLY ; ( f -- ) COMPILE-ONLY
        ld.w    %r4, [%r1]+
        or      %r4, %r4
        jrne    no_branch
        ld.w    %r0, [%r0]
        NEXT
        END_CODE

        CODE    branch, "branch", FLAG_COMPILE_ONLY  ; ( -- ) COMPILE-ONLY
        ld.w    %r0, [%r0]
        NEXT
        END_CODE


;;; .( Memory fetch & store )

        CODE    store, "!", FLAG_NORMAL              ; ( w a -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.w    [%r4], %r5
        NEXT
        END_CODE

        CODE    fetch, "@", FLAG_NORMAL              ; ( a -- w )
        ld.w    %r4, [%r1]
        ld.w    %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    hstore, "h!", FLAG_NORMAL            ; ( c h -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.h    [%r4], %r5
        NEXT
        END_CODE

        CODE    hfetch, "h@", FLAG_NORMAL            ; ( h -- c )
        ld.w    %r4, [%r1]
        ld.uh   %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    cstore, "c!", FLAG_NORMAL            ; ( c b -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.b    [%r4], %r5
        NEXT
        END_CODE

        CODE    cfetch, "c@", FLAG_NORMAL            ; ( b -- c )
        ld.w    %r4, [%r1]
        ld.ub   %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
        END_CODE


;;; .( Return Stack )

        CODE    rp_fetch, "rp@", FLAG_NORMAL         ; ( -- a )
        ld.w    %r4, %sp
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    rp_store, "rp!", FLAG_COMPILE_ONLY  ; ( a -- ) COMPILE-ONLY
        ld.w    %r4, [%r1]+
        ld.w    %sp, %r4
        NEXT
        END_CODE

        CODE    r_from, "r>", FLAG_COMPILE_ONLY     ; ( -- w ) COMPILE-ONLY
        ld.w    %r4, [%sp]
        add     %sp, 1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    r_fetch, "r@", FLAG_NORMAL           ; ( -- w )
        ld.w    %r4, [%sp]
        xsub    %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    to_r, ">r", FLAG_COMPILE_ONLY       ; ( w -- ) COMPILE-ONLY
        ld.w    %r4, [%r1]+
        sub     %sp, 1
        ld.w    [%sp], %r4
        NEXT
        END_CODE


;;; .( Data Stack )

        CODE    sp_fetch, "sp@", FLAG_NORMAL         ; ( -- a )
        ld.w    %r4, %r1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    sp_store, "sp!", FLAG_NORMAL         ; ( a -- )
        ld.w    %r1, [%r1]
        NEXT
        END_CODE

        CODE    drop, "drop", FLAG_NORMAL            ; ( w -- )
        ld.w    %r4, [%r1]+
        NEXT
        END_CODE

        CODE    dup, "dup", FLAG_NORMAL              ; ( w -- w w )
        ld.w    %r4, [%r1]
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    swap, "swap", FLAG_NORMAL            ; ( w1 w2 -- w2 w1 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5
        NEXT
        END_CODE

        CODE    over, "over", FLAG_NORMAL           ; ( w1 w2 -- w1 w2 w1 )
        xld.w   %r4, [%r1 + 4]
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE


;;; : ?DUP ( w -- w w, 0 ) DUP IF DUP THEN ;
	COLON   qdup, "?dup", FLAG_NORMAL
	.long	dup, qbranch, qdup_l1
	.long	dup
qdup_l1:
	.long	exit

;;; : NIP ( w1 w2 -- w2 ) SWAP DROP ;
        COLON   nip, "nip", FLAG_NORMAL
        .long   swap, drop, exit

;;; : ROT ( w1 w2 w3 -- w2 w3 w1 ) >R SWAP R> SWAP ;
        COLON   rot, "rot", FLAG_NORMAL
        .long   to_r, swap, r_from, swap, exit

;;; : -ROT ( w1 w2 w3 -- w3 w1 w2 ) ROT ROT ;
        COLON   minus_rot, "-rot", FLAG_NORMAL
        .long   rot, rot, exit

;;; : 2DROP ( w w  -- ) DROP DROP ;
        COLON   twodrop, "2drop", FLAG_NORMAL
        .long   drop, drop, exit

;;; : 2DUP ( w1 w2 -- w1 w2 w1 w2 ) OVER OVER ;
        COLON   twodup, "2dup", FLAG_NORMAL
        .long   over, over, exit


;;; .( Logic )

        CODE    zero_less, "0<", FLAG_NORMAL         ; ( n -- t )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jrlt    zero_less_l1
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4
        NEXT
zero_less_l1:
        ld.w    %r4, TRUE
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    _and, "and", FLAG_NORMAL             ; ( w w -- w )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        and     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    _or, "or", FLAG_NORMAL               ; ( w w -- w )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        or      %r4, %r5
        ld.w    [%r1], %r4
        NEXT
        END_CODE

        CODE    _xor, "xor", FLAG_NORMAL             ; ( w w -- w )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        xor     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : INVERT ( w -- w ) -1 XOR ;
        COLON   invert, "invert", FLAG_NORMAL
        .long   dolit, -1, _xor, exit


;;; .( Arithmetic )

        CODE    umplus, "um+", FLAG_NORMAL ; ( u u -- u cy ) \ or ( u u -- ud )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        add     %r4, %r5
        ld.w    [%r1], %r4
        ld.w    %r4, 0
        adc     %r4, %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : + ( u u -- u ) UM+ DROP ;
        CODE    plus, "+", FLAG_NORMAL               ; ( w w -- w )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : 1+ ( w -- w+1 ) 1 + ;
        CODE    increment, "1+", FLAG_NORMAL
        ld.w    %r4, [%r1]
        xadd    %r4, 1
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : 1- ( w -- w-1 ) 1 - ;
        CODE    decrement, "1-", FLAG_NORMAL
        ld.w    %r4, [%r1]
        xsub    %r4, 1
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; :  NEGATE ( n -- -n ) INVERT 1+ ;
	COLON   negate "negate",0
	.long	invert, increment, exit

;;; : DNEGATE ( d -- -d ) INVERT >R INVERT 1 UM+ R> + ;
	COLON   dnegate, "dnegate", FLAG_NORMAL
	.long	invert, to_r, invert
	.long	dolit, 1, umplus
	.long	r_from, plus, exit

;;; : - ( w w -- w ) NEGATE + ;
        CODE    minus, "-", FLAG_NORMAL         ; ( w w -- w )
        ld.w    %r5, [%r1]+
        ld.w    %r4, [%r1]
        sub     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
        END_CODE


;;; : ABS ( n -- +n ) DUP 0< IF NEGATE THEN ;

        COLON   abs, "abs", FLAG_NORMAL
        .long   dup, zero_less, qbranch, abs_l1
        .long   negate
abs_l1:
        .long   exit


;;; .( User variables )

;;; : (douser) ( -- a ) R> @ UP @ + ; COMPILE-ONLY  ( address passed via %r2 not stack )
        CODE    douser, "(douser)", FLAG_COMPILE_ONLY
        ld.w    %r4, [%r2]
        ld.w    %r4, [%r4]                   ; user is another pointer!
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : (dovar) ( -- a ) R> ; COMPILE-ONLY ( address passed via %r2 not stack )
        CODE   dovar, "(dovar)", FLAG_COMPILE_ONLY
        ld.w    %r4, [%r2]                   ; %r4 = parameter address
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : (doconst) ( -- a ) R> @ ; COMPILE-ONLY ( address passed via %r2 not stack )
        CODE   doconst, "(doconst)", FLAG_COMPILE_ONLY
        ld.w    %r4, [%r2]                   ; %r4 = parameter address
        ld.w    %r4, [%r4]                   ; read the constant value
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; 8 \ start offset
        USER    user_reserved, "(ureserved)", FLAG_NORMAL, "0,0,0,0"

;;; DUP USER SP0      1 CELL+ \ initial data stack pointer
        USER    sp0, "sp0", FLAG_NORMAL, initial_stack_pointer

;;; DUP USER RP0      1 CELL+ \ initial return stack pointer
        USER    rp0, "rp0", FLAG_NORMAL, initial_return_pointer

;;; DUP USER 'KEY?    1 CELL+ \ character input ready vector
        USER    tkey_query, "\047key?", FLAG_NORMAL, rx_query

;;; DUP USER 'EMIT    1 CELL+ \ character output vector
        USER    temit, "\047emit", FLAG_NORMAL, tx_store

;;; DUP USER 'EXPECT  1 CELL+ \ line input vector
        USER    texpect, "\047expect", FLAG_NORMAL, accept

;;; DUP USER 'TAP     1 CELL+ \ input case vector
        USER    ttap, "\047tap", FLAG_NORMAL, ktap

;;; DUP USER 'ECHO    1 CELL+ \ input echo vector
        USER    techo, "\047echo", FLAG_NORMAL, tx_store

;;; DUP USER 'PROMPT  1 CELL+ \ operator prompt vector
        USER    tprompt, "\047prompt", FLAG_NORMAL, dot_ok

;;; DUP USER BASE     1 CELL+ \ number base
        USER    base, "base", FLAG_NORMAL, 10

;;; DUP USER temp     1 CELL+ \ scratch
        USER    temp, "temp", FLAG_COMPILE_ONLY, 0

;;; DUP USER SPAN     1 CELL+ \ #chars input by EXPECT
        USER    span, "span", FLAG_NORMAL, 0

;;; DUP USER >IN      1 CELL+ \ input buffer offset
        USER to_in, ">in", FLAG_NORMAL, 0

;;; DUP USER #TIB     1 CELL+ \ #chars in the input buffer
;;;       1 CELLS ALLOT \   address  of input buffer
        USER    hash_tib, "#tib", FLAG_NORMAL, "0,terminal_buffer"

;;; DUP USER UP       1 CELL+ \ user base pointer
        ;; not needed

;;; DUP USER CSP      1 CELL+ \ save stack pointers
        ;; not needed

;;; DUP USER 'EVAL    1 CELL+ \ interpret/compile vector
        USER    teval, "\047eval", FLAG_NORMAL, dollar_interpret

;;; DUP USER 'NUMBER  1 CELL+ \ numeric input vector
        USER    tnumber, "\047number", FLAG_NORMAL, numberq

;;; DUP USER HLD      1 CELL+ \ formated numeric string
        USER    hld, "hld", FLAG_NORMAL, 0

;;; DUP USER HANDLER  1 CELL+ \ error frame pointer
        USER    handler, "handler", FLAG_NORMAL, 0

;;; DUP USER CONTEXT  1 CELL+ \ first search vocabulary
;;;   =VOCS CELL+ \ vocabulary stack
        USER    context, "context", FLAG_NORMAL, "0, 0,0,0,0, 0,0,0,0"

;;; DUP USER CURRENT  1 CELL+ \ definitions vocabulary
;;;       1 CELL+ \ newest vocabulary
        USER    current, "current", FLAG_NORMAL, "0,0"

;;; DUP USER CP       1 CELL+ \ dictionary code pointer
;;;       1 CELL+ \ dictionary name pointer
;;;       1 CELL+ \ last name compiled
        USER    cp, "cp", FLAG_NORMAL, "end_of_code, end_of_dictionary, last_name"


;;; .( Comparison )

;;; : 0= ( w -- t ) IF FALSE EXIT THEN TRUE ;
        COLON   zero_equal, "0=", FLAG_NORMAL
        .long   qbranch, zero_equal_l1
        .long   dolit, FALSE, exit
zero_equal_l1:
        .long   dolit, TRUE, exit

;;; : = ( w w -- t ) XOR 0= ;
        COLON   equal, "=", FLAG_NORMAL
        .long   _xor, zero_equal, exit

;;; : U< ( u u -- t ) 2DUP XOR 0< IF  NIP 0< EXIT THEN - 0< ;
        COLON   uless, "u<", FLAG_NORMAL
        .long   twodup, _xor, zero_less, qbranch, uless_l1
        .long   nip, zero_less, exit
uless_l1:
        .long   minus, zero_less, exit

;;; :  < ( n n -- t ) 2DUP XOR 0< IF DROP 0< EXIT THEN - 0< ;
        COLON   less, "<", FLAG_NORMAL
        .long   twodup, _xor, zero_less, qbranch, less_l1
        .long   drop, zero_less, exit
less_l1:
        .long   minus, zero_less, exit

;;; : MAX ( n n -- n ) 2DUP      < IF SWAP THEN DROP ;
        COLON   max, "max", FLAG_NORMAL
        .long   twodup, less, qbranch, max_l1
        .long   swap
max_l1:
        .long   drop, exit

;;; : MIN ( n n -- n ) 2DUP SWAP < IF SWAP THEN DROP ;
        COLON   min, "min", FLAG_NORMAL
        .long   twodup, swap, less, qbranch, min_l1
        .long   swap
min_l1:
        .long   drop, exit

;;; : WITHIN ( u ul uh -- t ) OVER - >R - R> U< ;
        COLON   within, "within", FLAG_NORMAL
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
        COLON   um_slash_mod, "um/mod", FLAG_NORMAL
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
        COLON   slash_mod, "/mod", FLAG_NORMAL
        .long   over, zero_less, swap, m_slash_mod, exit

;;; : MOD ( n n -- r ) /MOD DROP ;
        COLON   mod, "/", FLAG_NORMAL
        .long   slash_mod, drop, exit

;;; : / ( n n -- q ) /MOD NIP ;
        COLON   divide, "/", FLAG_NORMAL
        .long   slash_mod, nip, exit


;;; .( Multiply )

;;; : UM* ( u1 u2 -- ud )
;;;   0 SWAP ( u1 0 u2 ) 15
;;;   FOR DUP  UM+  >R >R DUP  UM+  R> + R>
;;;     IF >R OVER  UM+  R> + THEN
;;;   NEXT ROT DROP ;
        CODE    umult, "um*", FLAG_NORMAL
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mltu.w  %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    %r4, %ahr
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : * ( n n -- n ) UM* DROP ;
        CODE    times, "*", FLAG_NORMAL
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mlt.w   %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : M* ( n n -- d )
;;;   2DUP XOR 0< >R  ABS SWAP ABS UM*  R> IF DNEGATE THEN ;
        CODE    multd, "m*", FLAG_NORMAL
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mlt.w   %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        ld.w    %r4, %ahr
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : */MOD ( n n n -- r q ) >R M* R> M/MOD ;
;;; : */ ( n n n -- q ) */MOD NIP ;


;;; .( Bits & Bytes )

;;; : BYTE+ ( b -- b ) [ =BYTE ] LITERAL + ;
        COLON   byte_plus, "byte+", FLAG_NORMAL
        .long   increment, exit

;;; : BYTE- ( b -- b ) [ =BYTE ] LITERAL - ;
        COLON   byte_minus, "byte-", FLAG_NORMAL
        .long   decrement, exit

;;; : CELL+ ( a -- a ) [ =CELL ] LITERAL + ;
        COLON   cell_plus, "cell+", FLAG_NORMAL
        .long   dolit, BYTES_PER_CELL, plus, exit

;;; : CELL- ( a -- a ) [ =CELL ] LITERAL - ;
        COLON   cell_minus, "cell-", FLAG_NORMAL
        .long   dolit, BYTES_PER_CELL, minus, exit

;;; : CELLS ( n -- n ) [ =CELL ] LITERAL * ;
        COLON   cells, "cells", FLAG_NORMAL
        .long   dolit, BYTES_PER_CELL, times, exit

;;; : BL ( -- 32 ) 32 ;
        COLON   blank, "bl", FLAG_NORMAL
        .long   dolit, 32, exit

;;; subsitute unprintable character with '.'
;;; : >CHAR ( c -- c )
;;;   127 AND DUP 127 BL WITHIN IF DROP [CHAR] . THEN ;
	COLON   to_char, ">char", FLAG_NORMAL
	.long	dolit, 0x7f, _and, dup
	.long	dolit, 0x7f, blank, within
	.long	qbranch, to_char_l1
	.long	drop, dolit, '.'
to_char_l1:
	.long	exit

;;; : DEPTH ( -- n ) SP@ SP0 @ SWAP - 1 CELLS / ;
        COLON   depth, "depth", FLAG_NORMAL
        .long   sp_fetch, sp0, fetch, swap, minus
        .long   dolit, 1, cells, divide, exit

;;; : PICK ( +n -- w ) 1+ CELLS SP@ + @ ;
        COLON   pick, "pick", FLAG_NORMAL
        .long   increment, cells, sp_fetch, plus, fetch, exit

;;; align to a CELL boundary
;;; : ALIGNED ( b -- a ) ; IMMEDIATE
        COLON   aligned, "aligned", FLAG_IMMEDIATE
        .long   dolit, BYTES_PER_CELL - 1, plus
        .long   dolit, -BYTES_PER_CELL, _and, exit


;;; .( Memory access )

;;; : +! ( n a -- ) SWAP OVER @ + SWAP ! ;
        COLON   plus_store, "+!", FLAG_NORMAL
        .long   swap, over, fetch, plus, swap, store, exit

;;; : 2! ( d a -- ) SWAP OVER ! CELL+ ! ;
        COLON   dstore, "2!", FLAG_NORMAL
        .long   swap, over, store, cell_plus, store, exit

;;; : 2@ ( a -- d ) DUP CELL+ @ SWAP @ ;
        COLON   dfetch, "2@", FLAG_NORMAL
        .long   dup, cell_plus, fetch, swap, fetch, exit

;;; : COUNT ( b -- b +n ) DUP BYTE+ SWAP C@ ;
        COLON   count, "count", FLAG_NORMAL
        .long   dup, byte_plus, swap, cfetch, exit

;;; : HERE ( -- a ) CP @ ;
        COLON   here, "here", FLAG_NORMAL
        .long   cp, fetch, exit

;;; : PAD ( -- a ) HERE 80 + ;
        COLON   pad, "pad", FLAG_NORMAL
        .long   here, dolit, 80, plus, exit

;;; : TIB ( -- a ) #TIB CELL+ @ ;
        COLON   tib, "tib", FLAG_NORMAL
        .long   hash_tib, cell_plus, fetch, exit

;;; : NP ( -- a ) CP CELL+ ;
        COLON   np, "np", FLAG_NORMAL
        .long   cp, cell_plus, exit

;;; : LAST ( -- a ) NP CELL+ ;
        COLON   last, "last", FLAG_NORMAL
        .long   np, cell_plus, exit


;;; : (dovoc)	( -- )  R> CONTEXT ! ; COMPILE-ONLY
	COLON   dovoc, "(dovoc)", FLAG_COMPILE_ONLY
	.long	r_from, context, store, exit

;;; \ Make FORTH the context vocabulary
;;; : FORTH	( -- ) COMPILE (dovoc) [ =HEAD ] , [ =LINK ] , ;
	COLON   forth, "forth", FLAG_NORMAL
	.long	dovoc
	.long	last_name                       ; vocabulary head pointer
	.long	last_name                       ; vocabulary link pointer

;;; : @EXECUTE ( a -- ) @ ?DUP IF EXECUTE THEN ;
        COLON   atexecute, "@execute", FLAG_NORMAL
        .long   fetch, qdup, qbranch, atexecute_l1
        .long   execute
atexecute_l1:
        .long   exit

;;; : CMOVE ( b b u -- )
;;;   FOR AFT >R COUNT R@ C! R> 1+ THEN NEXT 2DROP ;
        CODE    cmove, "cmove", FLAG_NORMAL
        ld.w    %r4, [%r1]+                     ; count
        ld.w    %r5, [%r1]+                     ; dst
        ld.w    %r6, [%r1]+                     ; src
        or      %r4, %r4
        jreq    cmove_done
cmove_loop:
        ld.ub   %r7, [%r6]+
        ld.b    [%r5]+, %r7
        xsub    %r4, 1
        jrne    cmove_loop
cmove_done:
        NEXT
        END_CODE


;;; : -TRAILING ( b u -- b u )
;;;   FOR AFT DUP R@ + C@  BL XOR
;;;     IF R> BYTE+ EXIT THEN THEN
;;;   NEXT 0 ;
        COLON   minus_trailing, "-trailing", FLAG_NORMAL
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
        COLON   fill, "fill", FLAG_NORMAL
        .long   swap, to_r
fill_l1:
        .long   twodup, cstore, byte_plus
        .long   donext, fill_l1
        .long   twodrop, exit

;;; : ERASE ( b u -- ) 0 FILL ;
        COLON   erase, "erase", FLAG_NORMAL
        .long   dolit, 0, fill, exit

;;; : PACK$ ( b u a -- a ) \ null terminated
;;;   DUP >R  2DUP C! BYTE+ SWAP CMOVE  R> ;
        COLON   pack_dollar, "pack$", FLAG_NORMAL
	.long	aligned, dup, to_r
	.long	twodup, cstore, byte_plus
	.long	swap, cmove, r_from, exit


;;; .( Numeric Output ) \ single precision

;;; : DIGIT ( u -- c ) 9 OVER < 7 AND + [CHAR] 0 + ;
        COLON   digit, "digit", FLAG_NORMAL
        .long   dolit, 9, over, less, dolit, 7, _and, plus, dolit, '0', plus, exit

;;; : EXTRACT ( n base -- n c ) 0 SWAP UM/MOD SWAP DIGIT ;
        COLON   extract, "extract", FLAG_NORMAL
        .long   dolit, 0, swap, um_slash_mod, swap, digit, exit

;;; : <# ( -- ) PAD HLD ! ;
        COLON   less_hash, "<#", FLAG_NORMAL
        .long   pad, hld, store, exit

;;; : HOLD ( c -- ) HLD @ BYTE- DUP HLD ! C! ;
        COLON   hold, "hold", FLAG_NORMAL
        .long   hld, fetch, byte_minus, dup, hld, store, cstore, exit

;;; : # ( u -- u ) BASE @ EXTRACT HOLD ;
        COLON   hash, "#", FLAG_NORMAL
        .long   base, fetch, extract, hold, exit

;;; : #S ( u -- 0 ) BEGIN # DUP WHILE REPEAT ;
        COLON   hash_s, "#s", FLAG_NORMAL
hash_s_l1:
        .long   hash, dup
        .long   qbranch, hash_s_l2
        .long   branch, hash_s_l1
hash_s_l2:
        .long   exit

;;; : SIGN ( n -- ) 0< IF [CHAR] - HOLD THEN ;
        COLON   sign, "sign", FLAG_NORMAL
        .long   zero_less, qbranch, sign_l1
        .long   dolit, '-', hold
sign_l1:
        .long   exit

;;; : #> ( w -- b u ) DROP HLD @ PAD OVER - ;
        COLON   hash_greater, "#>", FLAG_NORMAL
        .long   drop, hld, fetch, pad, over, minus, exit

;;; : (str) ( w -- b u ) DUP >R ABS <# #S R> SIGN #> ;
        COLON   paren_str, "(str)"
        .long   dup, to_r, abs, less_hash, hash_s, r_from, sign, hash_greater, exit

;;; : HEX ( -- ) 16 BASE ! ;
        COLON   hex, "hex", FLAG_NORMAL
        .long   dolit, 16, base, store, exit

;;; : DECIMAL ( -- ) 10 BASE ! ;
        COLON   decimal, "decimal", FLAG_NORMAL
        .long   dolit, 10, base, store, exit


;;; .( Numeric Input ) \ single precision

;;; : DIGIT? ( c base -- u t )
;;;   >R [CHAR] 0 - 9 OVER <
;;;   IF 7 - DUP 10 < OR THEN DUP R> U< ;
        COLON   digitq, "digit?", FLAG_NORMAL
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
;;;   OVER C@ [CHAR] $ =
;;;   IF HEX SWAP BYTE+ SWAP 1- THEN ( a 0 b' n')
;;;   OVER C@ [CHAR] - = >R ( a 0 b n)
;;;   SWAP R@ - SWAP R@ + ( a 0 b" n") ?DUP
;;;   IF 1- ( a 0 b n)
;;;     FOR DUP >R C@ BASE @ DIGIT?
;;;       WHILE SWAP BASE @ * +  R> BYTE+
;;;     NEXT R@ ( ?sign) NIP ( b) IF NEGATE THEN SWAP
;;;       ELSE R> R> ( b index) 2DROP ( digit number) 2DROP 0
;;;       THEN DUP
;;;   THEN R> ( n ?sign) 2DROP R> BASE ! ;
        COLON   numberq, "number?", FLAG_NORMAL
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

;;; : KEY? ( -- c T | F ) 'KEY? @EXECUTE ;
        COLON   key_query, "key?", FLAG_NORMAL
        .long   tkey_query, atexecute, exit

;;; : KEY ( -- c ) BEGIN KEY? UNTIL ;
        COLON   key, "key", FLAG_NORMAL
key_l1:
        .long   key_query, qbranch, key_l1, exit

;;; : EMIT ( c -- ) 'EMIT @EXECUTE ;
        COLON   emit, "emit", FLAG_NORMAL
        .long   temit, atexecute, exit

;;; if key is pressed, wait for second key press
;;; return true if the second key is enter
;;; : ENOUGH? ( -- f ) KEY? DUP IF 2DROP KEY 13 = THEN ;
        COLON   enoughq, "enough?", FLAG_NORMAL
        .long   key_query, dup
        .long   qbranch, enoughq_l1
        .long   twodrop, key, dolit, 13, equal
enoughq_l1:
        .long   exit

;;; :  PACE ( -- ) 11 EMIT ;
;;; : SPACE ( -- ) BL EMIT ;
        COLON   space, "space", FLAG_NORMAL
        .long   blank, emit, exit

;;; : CHARS ( +n c -- ) SWAP 0 MAX FOR AFT DUP EMIT THEN NEXT DROP ;
        COLON   chars, "chars", FLAG_NORMAL
        .long   swap, dolit, 0, max
        .long   to_r, branch, chars_l2
chars_l1:
        .long   dup, emit
chars_l2:
        .long   donext, chars_l1
        .long   drop, exit

;;; : SPACES ( +n -- ) BL CHARS ;
        COLON   spaces, "spaces", FLAG_NORMAL
        .long   blank, chars, exit

;;; : do$ ( -- a )
;;;   R> R@ R> COUNT + ALIGNED >R SWAP >R ; COMPILE-ONLY
	COLON   do_dollar, "do$", FLAG_COMPILE_ONLY
	.long	r_from, r_fetch, r_from, count, plus
	.long	aligned, to_r, swap, to_r, exit

;;; : ($") ( -- a ) do$ ; COMPILE-ONLY
	COLON   do_dollar_quote, "($\042)", FLAG_COMPILE_ONLY
	.long	do_dollar, exit

;;; : TYPE ( b u -- ) FOR AFT COUNT EMIT THEN NEXT DROP ;
	COLON   type, "type", FLAG_NORMAL
	.long	to_r
	.long	branch, type_l2
type_l1:
	.long	dup, cfetch, emit, increment
type_l2:
	.long	donext, type_l1
	.long	drop, exit

;;; : .$ ( a -- ) COUNT TYPE ;
        COLON   dot_dollar, ".$", FLAG_NORMAL
        .long   count, type, exit

;;; : (.") ( -- ) do$ .$ ; COMPILE-ONLY
        COLON   do_dot_quote, "(.\042)", FLAG_COMPILE_ONLY
        .long   do_dollar, dot_dollar, exit

;;; : CR ( -- ) 13 EMIT 10 EMIT ;
        COLON   cr, "cr", FLAG_NORMAL
        .long   dolit, carriage_return, emit
        .long   dolit, line_feed, emit, exit

;;; :  .R ( n +n -- ) >R (str)      R> OVER - SPACES TYPE ;
        COLON   dot_r, ".r", FLAG_NORMAL
        .long   to_r, paren_str, r_from, over, minus, spaces, type, exit

;;; : U.R ( u +n -- ) >R <# #S #> R> OVER - SPACES TYPE ;
        COLON   u_dot_r, "u.", FLAG_NORMAL
        .long   to_r, less_hash, hash_s, hash_greater, r_from, over, minus, spaces, type, exit

;;; : U. ( u -- ) <# #S #> SPACE TYPE ;
        COLON   u_dot, "u.", FLAG_NORMAL
        .long   less_hash, hash_s, hash_greater, space, type, exit

;;; :  . ( w -- ) BASE @ 10 XOR IF U. EXIT THEN (str) SPACE TYPE ;
        COLON   dot, ".", FLAG_NORMAL
        .long   base, fetch, dolit, 10, _xor
        .long   qbranch, dot_l1
        .long   u_dot, exit
dot_l1:
        .long   paren_str, space, type, exit

;;; : ? ( a -- ) @ . ;
        COLON   question, "?", FLAG_NORMAL
        .long   fetch, dot, exit

;;; : H? ( h -- ) H@ . ;
        COLON   hquestion, "h?", FLAG_NORMAL
        .long   hfetch, dot, exit

;;; : C? ( b -- ) C@ . ;
        COLON   cquestion, "c?", FLAG_NORMAL
        .long   cfetch, dot, exit


;;; .( Parsing )

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
	COLON   paren_parse, "(parse)", FLAG_NORMAL
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
	COLON   parse, "parse", FLAG_NORMAL
	.long	to_r, tib, to_in, fetch, plus
	.long	hash_tib, fetch, to_in, fetch, minus
	.long	r_from, paren_parse
	.long	to_in, plus_store, exit

;;; : .( ( -- ) [CHAR] ) PARSE TYPE ; IMMEDIATE
        COLON   dot_paren, ".(", FLAG_IMMEDIATE
        .long   dolit, ')', parse, type, exit

;;; : ( ( -- ) [CHAR] ) PARSE 2DROP ; IMMEDIATE
        COLON   paren, "(", FLAG_IMMEDIATE
        .long   dolit, ')', parse, twodrop, exit

;;; : \ ( -- ) #TIB @ >IN ! ; IMMEDIATE
        COLON   backslash, "\\", FLAG_IMMEDIATE
        .long   hash_tib, fetch, to_in, store, exit

;;; : CHAR ( -- c ) BL PARSE DROP C@ ;
        COLON   char, "char", FLAG_NORMAL
        .long   blank, parse, drop, cfetch, exit

;;; : [CHAR] ( -- c ) CHAR LITERAL ; FLAG_IMMEDIATE | FLAG_COMPILE_ONLY
        COLON   bracket_char, "[char]", FLAG_NORMAL
        .long   char, literal, exit

;;; : CTRL ( -- c ) CHAR $001F AND ;
        COLON   ctrl, "ctrl", FLAG_NORMAL
        .long   char, dolit, 0x1f, _and, exit

;;; : [CTRL] ( -- c ) CTRL LITERAL ; FLAG_IMMEDIATE | FLAG_COMPILE_ONLY
        COLON   bracket_ctrl, "[ctrl]", FLAG_NORMAL
        .long   ctrl, literal, exit

;;; this puts the name in the right place for being the next defined item
;;; : TOKEN ( -- a \ <string> ) \ and reserve space for dictionary header
;;;   BL PARSE 31 MIN NP @ [ =DICTIONARY-HEADER-CELLS ] CELLS + PACK$ ;
        COLON   token, "token", FLAG_NORMAL
	.long	blank, parse
	.long	dolit, 31, min
	.long	np, fetch, dolit, DICTIONARY_HEADER_CELLS, cells, plus, pack_dollar
	.long	exit

;;; : WORD ( c -- a \ <string> ) PARSE HERE PACK$ ;
	COLON   word, "word", FLAG_NORMAL
	.long	parse, here, pack_dollar, exit


;;; .( Dictionary Search )

;;; : NAME>CODE ( na -- ca ) [ =DICTIONARY-CODE-OFFSET ] LITERAL CELLS - ;
        COLON   name_to_code, "name>code", FLAG_NORMAL
        .long   dolit, DICTIONARY_CODE_OFFSET, cells, minus, exit

;;; : NAME>PARAM ( na -- pa ) [ =DICTIONARY-PARAM-OFFSET ] LITERAL CELLS - ;
        COLON   name_to_param, "name>param", FLAG_NORMAL
        .long   dolit, DICTIONARY_PARAM_OFFSET, cells, minus, exit

;;; : NAME>FLAGS ( na -- fa ) [ =DICTIONARY-FLAGS-OFFSET ] LITERAL CELLS - ;
        COLON   name_to_flags, "name>flags", FLAG_NORMAL
        .long   dolit, DICTIONARY_FLAGS_OFFSET, cells, minus, exit

;;; : NAME>LINK ( na -- la ) [ =DICTIONARY-LINK-OFFSET ] LITERAL CELLS - ;
        COLON   name_to_link, "name>link", FLAG_NORMAL
        .long   dolit, DICTIONARY_LINK_OFFSET, cells, minus, exit

;;; : CODE>NAME ( ca -- na )  [ =DICTIONARY-CODE-OFFSET ] LITERAL CELLS + ;
        COLON   code_to_name, "code>name", FLAG_NORMAL
        .long   dolit, DICTIONARY_CODE_OFFSET, cells, plus, exit

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
        COLON   sameq, "same?", FLAG_NORMAL
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
        COLON   find, "find", FLAG_NORMAL
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
	COLON   nameq, "name?", FLAG_NORMAL
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

;;; : ^H ( b b b -- b b b ) \ backspace
;;;   >R OVER R@ < DUP
;;;   IF [ CTRL H ] LITERAL 'ECHO @EXECUTE THEN R> + ;
	COLON   bksp, "^h", FLAG_NORMAL
	.long	to_r, over, r_from, swap, over, _xor
	.long	qbranch, bksp_l1
	.long	dolit, backspace, techo, atexecute, dolit, 1, minus
	.long	blank, techo, atexecute
	.long	dolit, backspace, techo, atexecute
bksp_l1:
	.long	exit


;;; : TAP ( bot eot cur key -- bot eot cur )
;;;   DUP 'ECHO @EXECUTE OVER C! 1+ ;
        COLON   tap, "tap", FLAG_NORMAL
        .long   dup, techo, atexecute, over, cstore, increment, exit

;;; : kTAP ( bot eot cur key -- bot eot cur )
;;;   DUP 13 XOR
;;;   IF [ CTRL H ] LITERAL XOR IF BL TAP ELSE ^H THEN EXIT
;;;   THEN DROP NIP DUP ;
        COLON   ktap, "ktap", FLAG_NORMAL
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

        COLON   accept, "accept", FLAG_NORMAL
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
        colon   expect, "expect", FLAG_NORMAL
	.long	texpect, atexecute, span, store, drop, exit

;;; : QUERY ( -- )
;;;   TIB 80 'EXPECT @EXECUTE #TIB !  0 NIP >IN ! ;
	COLON   query, "query", FLAG_NORMAL
	.long	tib, dolit, 80, texpect, atexecute, hash_tib, store
	.long	drop, dolit, 0, to_in, store, exit


;;; .( Error handling )

;;; : CATCH ( ca -- err#/0 )
;;;   SP@ >R  HANDLER @ >R  RP@ HANDLER !
;;;   EXECUTE
;;;   R> HANDLER !  R> DROP  0 ;
	COLON   catch, "catch", FLAG_NORMAL
	.long	sp_fetch
        .long   to_r, handler, fetch, to_r
	.long	rp_fetch, handler, store, execute
	.long	r_from, handler, store
	.long	r_from, drop, dolit, 0, exit

;;; : THROW ( err# -- err# )
;;;   HANDLER @ RP!  R> HANDLER !  R> SWAP >R SP! DROP R> ;
	COLON   throw, "throw", FLAG_NORMAL
	.long	handler, fetch, rp_store
	.long	r_from, handler, store
	.long	r_from, swap, to_r, sp_store
	.long	drop, r_from, exit

;;; VARIABLE NULL$
        VARIABLE null_dollar, "null$", FLAG_NORMAL
        .long   0

;;; : ABORT ( -- ) NULL$ THROW ;
        COLON   abort, "abort", FLAG_NORMAL
        .long   null_dollar, throw, exit

;;; : (abort") ( f -- ) IF do$ THROW THEN do$ DROP ; COMPILE-ONLY
	COLON   do_abort_quote, "(abort\042)", FLAG_COMPILE_ONLY
	.long	qbranch, do_abort_quote_l1
	.long	do_dollar, throw
do_abort_quote_l1:
	.long	do_dollar, drop, exit


;;; .( Interpret )

;;; : $INTERPRET ( a -- )
;;;   NAME?  ?DUP
;;;   IF @ [ =COMP ] LITERAL AND
;;;     ABORT" compile ONLY" EXECUTE EXIT
;;;   THEN
;;;   'NUMBER @EXECUTE
;;;   IF EXIT THEN THROW ;
	COLON   dollar_interpret, "$interpret", FLAG_NORMAL
	.long	nameq, qdup
	.long	qbranch, dollar_interpret_l1
	.long	name_to_flags, fetch, dolit, FLAG_COMPILE_ONLY, _and
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
	COLON   left_bracket, "[", FLAG_IMMEDIATE
	.long	dolit, dollar_interpret, teval, store, exit

;;; : .OK ( -- ) [ ' $INTERPRET ] LITERAL 'EVAL @ = IF ."  ok" THEN CR ;
	COLON   dot_ok, ".ok", FLAG_NORMAL
	.long	dolit, dollar_interpret, teval, fetch, equal
	.long	qbranch, dot_ok_l1
	.long	do_dot_quote
	FSTRING	" Ok"
dot_ok_l1:
	.long	cr, exit


;;; : ?STACK ( -- ) DEPTH 0< IF $" underflow" THROW THEN ;
	COLON   qstack, "?stack", FLAG_NORMAL
	.long	depth, zero_less
	.long	do_abort_quote
	FSTRING	" underflow"
	.long	exit

;;; : EVAL ( -- )
;;;   BEGIN TOKEN DUP C@
;;;   WHILE 'EVAL @EXECUTE ?STACK
;;;   REPEAT DROP 'PROMPT @EXECUTE ;
        COLON   eval, "eval", FLAG_NORMAL
eval_l1:
	.long	token, dup, cfetch
	.long	qbranch, eval_l2
	.long	teval, atexecute, qstack
	.long	branch, eval_l1
eval_l2:
	.long	drop, tprompt, atexecute, exit


;;; .( Device I/O )

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
        CODE    rx_query, "rx?", FLAG_NORMAL         ; ( -- c T | F )
        xcall   Serial_InputAvailable
        or      %r4, %r4
        jreq    rx_query_no_character
        xcall   Serial_GetChar
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        ld.w    %r4, TRUE
rx_query_no_character:
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; output a character
        CODE    tx_store, "tx!", FLAG_NORMAL         ; ( c -- )
        ld.w    %r6, [%r1]+
        xcall   Serial_PutChar
        NEXT
        END_CODE

;;; : !IO ( -- ) ; IMMEDIATE \ initialize I/O device


;;; .( Shell )

;;; : PRESET ( -- ) SP0 @ SP!  [ =TIB ] LITERAL #TIB CELL+ ! ;
        COLON   preset, "preset", FLAG_NORMAL
	.long	sp0, fetch, sp_store
	.long	dolit, terminal_buffer, hash_tib, cell_plus, store
	.long	exit

;;; : XIO ( a a a -- ) \ reset 'EXPECT 'TAP 'ECHO 'PROMPT
;;;   [ ' accept ] LITERAL 'EXPECT !
;;;   'TAP !  'ECHO !  'PROMPT ! ;
        COLON   xio, "xio", FLAG_NORMAL
        .long   dolit, accept, texpect, store
        .long   ttap, store
        .long   techo, store
        .long   tprompt, store
        .long   exit

;;; : FILE ( -- )
;;;   [ ' PACE ] LITERAL [ ' DROP ] LITERAL [ ' kTAP ] LITERAL XIO ;
;;; ** Missing **

;;; : HAND ( -- )
;;;   [ ' .OK  ] LITERAL 'EMIT @ [ ' kTAP ] LITERAL XIO ;
        COLON   hand, "hand", FLAG_NORMAL
        .long   dolit, dot_ok, temit, fetch, dolit, ktap, xio, exit

;;; CREATE I/O  ' ?RX , ' TX! , \ defaults
;;; ** Missing **

;;; : CONSOLE ( -- ) I/O 2@ 'KEY? 2! HAND ;
        COLON   console, "console", FLAG_NORMAL
        .long   dolit, rx_query, tkey_query, store
        .long   dolit, tx_store, temit, store
        .long   hand, exit

;;; : que ( -- ) QUERY EVAL ;
;        COLON   que, "que", FLAG_NORMAL
;        .long   query, eval, exit

;;; : QUIT ( -- ) \ clear return stack ONLY
;;;   RP0 @ RP!
;;;   BEGIN [COMPILE] [
;;;     BEGIN [ ' que ] LITERAL CATCH ?DUP
;;;     UNTIL ( a)
;;;     CONSOLE  NULL$ OVER XOR
;;;     IF CR TIB #TIB @ TYPE
;;;        CR >IN @ [CHAR] ^ CHARS
;;;        CR .$ ."  ? "
;;;     THEN PRESET
;;;   AGAIN ;
        COLON   quit, "quit", FLAG_NORMAL
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
        COLON   tick, "'", FLAG_NORMAL
        .long   token, nameq
        .long   qbranch, tick_l1
        .long   exit
tick_l1:
        .long   throw, exit

;;; : ALLOT ( n -- ) CP +! ;
        COLON   allot, "allot", FLAG_NORMAL
        .long   cp, plus_store, exit

;;; : , ( w -- ) HERE ALIGNED DUP CELL+ CP ! ! ;
        COLON   comma, ",", FLAG_NORMAL
        .long   here, aligned, dup, cell_plus, cp
        .long   store, store, exit

;;; : [COMPILE] ( -- \ <string> ) ' , ; IMMEDIATE
        COLON   bracket_compile, "[compile]", FLAG_IMMEDIATE
        .long   tick, comma, exit

;;; : COMPILE ( -- ) R> DUP @ , CELL+ >R ; COMPILE-ONLY
        COLON   compile, "compile", FLAG_COMPILE_ONLY
        .long   r_from, dup, fetch, comma, cell_plus, to_r, exit

;;; : LITERAL ( w -- ) COMPILE (dolit) , ; IMMEDIATE
        COLON   literal, "literal", FLAG_IMMEDIATE
        .long   compile, dolit, comma, exit

;;; : $," ( -- ) [CHAR] " PARSE HERE PACK$ C@ 1+ ALLOT ;
        COLON   dollar_comma_quote, "$,\042", FLAG_NORMAL
        .long   dolit, '\"', parse, here, pack_dollar, cfetch, increment, allot, exit

;;; : RECURSE ( -- ) LAST @ CURRENT @ ! ; IMMEDIATE
        COLON   recurse, "recurse", FLAG_IMMEDIATE
        .long   last, fetch, current, fetch, store, exit


;;; .( Structures )

;;; : FOR ( -- a ) COMPILE >R HERE ; IMMEDIATE
        COLON   for, "for", FLAG_IMMEDIATE
        .long   compile, to_r, here, exit

;;; : BEGIN ( -- a ) HERE ; IMMEDIATE
        COLON   begin, "begin", FLAG_IMMEDIATE
        .long   here, exit

;;; : NEXT ( a -- ) COMPILE (next) , ; IMMEDIATE
        COLON   next, "next", FLAG_IMMEDIATE
	.long	compile, donext, comma, exit

;;; : UNTIL ( a -- ) COMPILE ?branch , ; IMMEDIATE
        COLON   until, "until", FLAG_IMMEDIATE
        .long   compile, qbranch, comma, exit

;;; : AGAIN ( a -- ) COMPILE  branch , ; IMMEDIATE
        COLON   again, "again", FLAG_IMMEDIATE
        .long   compile, branch, comma, exit

;;; : IF ( -- A )   COMPILE ?branch HERE 0 , ; IMMEDIATE
        COLON   if, "if", FLAG_IMMEDIATE
        .long   compile, qbranch, here, dolit, 0, comma, exit

;;; : AHEAD ( -- A ) COMPILE branch HERE 0 , ; IMMEDIATE
        COLON   ahead, "ahead", FLAG_IMMEDIATE
        .long   compile, branch, here, dolit, 0, comma, exit

;;; : REPEAT ( A a -- ) [COMPILE] AGAIN HERE SWAP ! ; IMMEDIATE
        COLON   repeat, "repeat", FLAG_IMMEDIATE
        .long   again, here, swap, store, exit

;;; : THEN ( A -- ) HERE SWAP ! ; IMMEDIATE
        COLON   then, "then", FLAG_IMMEDIATE
        .long  here, swap, store, exit

;;; : AFT ( a -- a A ) DROP [COMPILE] AHEAD [COMPILE] BEGIN SWAP ; IMMEDIATE
        COLON   aft, "aft", FLAG_IMMEDIATE
        .long   drop, ahead, begin, swap, exit

;;; : ELSE ( A -- A )  [COMPILE] AHEAD SWAP [COMPILE] THEN ; IMMEDIATE
        COLON   else, "else", FLAG_IMMEDIATE
        .long   ahead, swap, then, exit

;;; : WHILE ( a -- A a )    [COMPILE] IF SWAP ; IMMEDIATE
        COLON   while, "while", FLAG_IMMEDIATE,
        .long   if, swap, exit

;;; : ABORT" ( -- \ <string> ) COMPILE (abort") $," ; IMMEDIATE
	COLON   abortquote, "abort\042", FLAG_IMMEDIATE
	.long	compile, do_abort_quote, dollar_quote, exit

;;; : $" ( -- \ <string> ) COMPILE ($") $," ; IMMEDIATE
        COLON   dollar_quote, "$\042", FLAG_IMMEDIATE
	.long	compile, do_dollar_quote, dollar_comma_quote, exit

;;; : ." ( -- \ <string> ) COMPILE ."| $," ; IMMEDIATE
        COLON   dot_quote, ".\042", FLAG_IMMEDIATE
	.long	compile, do_dot_quote, dollar_comma_quote, exit


;;; .( Name Compiler )

;;; : ?UNIQUE ( a -- a )
;;;   DUP NAME? IF ."  redefined " OVER .$ THEN DROP ;
        COLON   qunique, "?unique", FLAG_NORMAL
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
        COLON   dollar_comma_n, "$,n", FLAG_NORMAL
        .long   dup, cfetch                     ; cfetch for string count (strlen ??)
        .long   qbranch, dollar_comma_n_l1
        .long   qunique
        .long   dup, last, store
        .long   here, aligned, swap
        .long   dup, name_to_link
        .long   current, fetch, fetch
        .long   swap, store
        .long   dup
        .long   count, plus, aligned            ; skip over the name
        .long   np, store
        .long   name_to_param
        .long   store
        .long   exit
dollar_comma_n_l1:
        .long   do_dollar_quote
        FSTRING "name"
        .long   throw


;;; .( FORTH Compiler )

;;; : $COMPILE ( a -- )
;;;   NAME? ?DUP
;;;   IF NAME>FLAGS @ [ =FLAG-IMMEDIATE ] LITERAL AND
;;;     IF EXECUTE ELSE , THEN EXIT
;;;   THEN
;;;   'NUMBER @EXECUTE
;;;   IF [COMPILE] LITERAL EXIT
;;;   THEN THROW ;
	colon   dollar_compile, "$compile", FLAG_NORMAL
	.long	nameq, qdup
	.long	qbranch, dollar_compile_l2
	.long	name_to_flags, fetch, dolit, FLAG_IMMEDIATE, _and
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
        COLON   overt, "overt", FLAG_NORMAL
        .long   last, fetch, current, fetch, store, exit

;;; : ; ( -- )
;;;   COMPILE EXIT [COMPILE] [ OVERT ; COMPILE-ONLY IMMEDIATE
        COLON   semicolon, "\073", FLAG_COMPILE_ONLY + FLAG_IMMEDIATE
        .long   compile, exit
        .long   left_bracket, overt, exit

;;; : ] ( -- ) [ ' $COMPILE ] LITERAL 'EVAL ! ;
	COLON   right_bracket, "]", FLAG_NORMAL
	.long	dolit, dollar_compile, teval, store, exit

;;; \ basic defining word call like: ' (doXXX) (DEFINE) THING
;;; \ and it will place the address of the actual code for (doXXX)
;;; \ into the code pointer for thing
;;; : (DEFINE) ( code -- \ <string> ) TOKEN DUP $,n
;;;            SWAP OVER NAME>CODE !
;;;            [ =FLAG-NORMAL ] LITERAL SWAP NAME>FLAGS ! ;
        COLON   paren_define, "(define)", FLAG_NORMAL
        .long   token, dup, dollar_comma_n
        .long   swap, over, name_to_code, store
        .long   dolit, FLAG_NORMAL, swap, name_to_flags, store
        .long   exit

;;; : : ( -- \ <string> )  [ ' (docolon) @ ] (DEFINE) ] ;
        COLON   colon, ":", FLAG_NORMAL
        .long   dolit, param_docolon, paren_define
        .long   right_bracket, exit

;;; : IMMEDIATE ( -- )
;;;             LAST @ NAME>FLAGS DUP @
;;;             [ =FLAG-IMMEDIATE ] LITERAL OR SWAP ! ;
        COLON   _immediate, "immediate", FLAG_NORMAL
        .long   last, fetch, name_to_flags, dup, fetch
        .long   dolit, FLAG_IMMEDIATE, _or, swap, store, exit

;;; : COMPILE-ONLY ( -- )
;;;             LAST @ NAME>FLAGS DUP @
;;;             [ =FLAG-COMPILE-ONLY ] LITERAL OR SWAP ! ;
        COLON   _compile_only, "compile-only", FLAG_NORMAL
        .long   last, fetch, name_to_flags, dup, fetch
        .long   dolit, FLAG_COMPILE_ONLY, _or, swap, store, exit


;;; .( Defining Words )

;;; : USER ( -- \ <string> )  [ ' (douser) @ ] LITERAL (DEFINE) OVERT ;
        COLON   user, "user", FLAG_NORMAL
        .long   dolit, param_douser, paren_define, overt, exit

;;; : CREATE ( -- \ <string> )  [ ' (dovar) @ ] LITERAL (DEFINE) OVERT ;
        COLON   create, "create", FLAG_NORMAL
        .long   dolit, param_dovar, paren_define, overt, exit

;;; : VARIABLE ( -- \ <string> ) CREATE 0 , ;
        COLON   variable, "variable", FLAG_NORMAL
        .long   create, dolit, 0, comma, exit

;;; : CONSTANT ( u -- \ <string> )  [ ' (doconst) @ ] LITERAL (DEFINE) OVERT ;
        COLON   constant, "constant", FLAG_NORMAL
        .long   dolit, param_doconst, paren_define, comma, overt, exit


;;; .( special constants )

        CONSTANT true, "true", FLAG_NORMAL
        .long   TRUE

        CONSTANT false, "false", FLAG_NORMAL
        .long   FALSE


;;; .( Tools )

;;; : (dump_ascii) ( b u -- ) FOR AFT COUNT >CHAR EMIT THEN NEXT DROP ;
        COLON   dump_ascii,"(dump_ascii)", FLAG_NORMAL
        .long   to_r
        .long   branch, dump_ascii_l2
dump_ascii_l1:
        .long   count, to_char, emit
dump_ascii_l2:
        .long   donext, dump_ascii_l1
        .long   drop, exit

;;; : (dump) ( b u -- b )
;;;   OVER 4 U.R SPACE FOR AFT COUNT 3 U.R THEN NEXT ;
        COLON   paren_dump, "(dump)", FLAG_NORMAL
        .long   over, dolit, 8, u_dot_r, space
        .long   to_r
        .long   branch, paren_dump_l2
paren_dump_l1:
        .long   count, dolit, 3, u_dot_r
paren_dump_l2:
        .long   donext, paren_dump_l1
        .long   exit

;;; : DUMP ( b u -- )
;;;   BASE @ >R HEX  16 /
;;;   FOR CR 16 2DUP (dump) -ROT 2 SPACES _TYPE ENOUGH? 0= WHILE
;;;   NEXT ELSE R> DROP THEN DROP  R> BASE ! ;
        COLON   dump, "dump", FLAG_NORMAL
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
        COLON   dot_s, ".s", FLAG_NORMAL
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
;;; ** Missing **

;;; : ?CSP ( -- ) SP@ CSP @ XOR ABORT" stack depth" ;
;;; ** Missing **

;;; \ search to see if an unknown address is really forth code
;;; : CODE? ( ca -- na | F )
;;;   CURRENT
;;;   BEGIN CELL+ @ ?DUP WHILE 2DUP
;;;     BEGIN @ DUP WHILE 2DUP NAME>CODE XOR
;;;     WHILE NAME>LINK
;;;     REPEAT      THEN NIP ?DUP
;;;   UNTIL NIP NIP EXIT THEN DROP FALSE ;
;;; hacked version - the vocabulary structure is not workable yet
        COLON   code_query, "code?", FLAG_NORMAL
        .long   current
;code_query_l1:
        .long   cell_plus, fetch, qdup
        .long   qbranch, code_query_l4
        .long   twodup
code_query_l2:
        .long   fetch, dup
        .long   qbranch, code_query_l3
        .long   twodup, name_to_code, _xor
        .long   qbranch, code_query_l3
        .long   name_to_link
        .long   branch, code_query_l2
code_query_l3:
        .long   nip, qdup
       ;.long   qbranch, code_query_l1
        .long   qbranch, code_query_l5
        .long   nip, nip, exit
code_query_l4:
        .long   drop, dolit, FALSE, exit
code_query_l5:
        .long   twodrop, dolit, FALSE, exit


;;; disassembler for colon definitions
;;; does no know how to stop - press enter twice to stop
;;; SEE ( -- ) \  token
;;; BASE @
;;;   ' CODE>NAME NAME>PARAM DUP
;;;  CR [CHAR] $ EMIT HEX 1 U.R [ CHAR : ] EMIT
;;;   @ CR ALIGNED CELL-
;;;   BEGIN
;;;     CELL+ DUP @ DUP IF CODE? THEN
;;;     ?DUP
;;;       IF    SPACE .ID
;;;       ELSE  DUP @ DUP DECIMAL U.
;;;             [CHAR] / EMIT
;;;             [CHAR] $ EMIT
;;;             HEX 1 U.R \ number
;;;       THEN
;;;   ENOUGH? UNTIL DROP BASE !;

	COLON see, "see", FLAG_NORMAL
        .long   base, fetch
	.long	tick, code_to_name, name_to_param, dup
        .long   cr
        .long   dolit, '$', emit
        .long   hex, dolit, 1, u_dot_r
        .long   dolit, ':', emit

        .long   fetch
	.long	cr, aligned, cell_minus
see_l1:
	.long	cell_plus, dup, fetch, dup
	.long	qbranch, see_l2
	.long	code_query
see_l2:
	.long	qdup
	.long	qbranch, see_l3
	.long	space, dot_id
	.long	branch, see_l4
see_l3:
	.long	dup, fetch, dup, decimal, u_dot
        .long   dolit, '/', emit, dolit, '$', emit
        .long   hex, dolit, 1, u_dot_r, cr
see_l4:
	.long	enoughq, qbranch, see_l1
	.long	drop, base, store, exit

;;; : .ID ( na -- )
;;;   ?DUP IF COUNT $001F AND TYPE EXIT THEN ." {noName}" ;
        COLON   dot_id, ".id", FLAG_NORMAL
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
        COLON   words, "words", FLAG_NORMAL
        .long   cr, context, fetch
words_l1:
        .long   fetch, qdup
        .long   qbranch, words_l2
        .long   dup, space, dot_id, name_to_link
        .long   enoughq, qbranch, words_l1
        .long   drop
words_l2:
        .long   exit


;;; .( File I/O )
;;; : R/O              (  -- fam )
        CODE    readonly, "r/o", FLAG_NORMAL
        xcall   FileSystem_ReadOnly
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : W/O              (  -- fam )
        CODE    writeonly, "w/o", FLAG_NORMAL
        xcall   FileSystem_WriteOnly
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : R/W              (  -- fam )
        CODE    readwrite, "r/w", FLAG_NORMAL
        xcall   FileSystem_ReadWrite
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
        END_CODE

;;; : BIN              ( fam -- fam2 )
        CODE    bin, "bin", FLAG_NORMAL
        ld.w    %r6, [%r1]                      ; fam
        xcall   FileSystem_bin
        ld.w    [%r1], %r4                      ; fam2
        NEXT
        END_CODE

;;; : S" ( -- \ <string> ) [COMPILE] $" COMPILE COUNT ; IMMEDIATE
;;;      \ runtime (  -- b u )
        COLON   s_quote, "s\042", FLAG_IMMEDIATE
	.long	dollar_quote, compile, count, exit


;;; : DELETE-FILE      ( b u -- ior )
        CODE    delete_file, "delete-file", FLAG_NORMAL
        ld.w    %r7, [%r1]+                     ; count
        ld.w    %r6, [%r1]                      ; string
        xcall   FileSystem_delete
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : RENAME-FILE      ( b1 u1 b2 u2 -- ior )
        CODE    rename_file, "rename-file", FLAG_NORMAL
        ld.w    %r9, [%r1]+                     ; count2
        ld.w    %r8, [%r1]+                     ; name2
        ld.w    %r7, [%r1]+                     ; count1
        ld.w    %r6, [%r1]                      ; name1
        xcall   FileSystem_rename
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : CREATE-FILE      ( b u fam -- fileid ior )
        CODE    create_file, "create-file", FLAG_NORMAL
        ld.w    %r8, [%r1]+                     ; fam
        ld.w    %r7, [%r1]                      ; count
        xld.w   %r6, [%r1 + 4]                  ; string
        xcall   FileSystem_create
        ld.w    [%r1], %r5                      ; ior
        xld.w   [%r1 + 4], %r4                  ; fd
        NEXT
        END_CODE

;;; : OPEN-FILE        ( b u fam -- fileid ior )
        CODE    open_file, "open-file", FLAG_NORMAL
        ld.w    %r8, [%r1]+                     ; fam
        ld.w    %r7, [%r1]                      ; count
        xld.w   %r6, [%r1 + 4]                  ; string
        xcall   FileSystem_open
        ld.w    [%r1], %r5                      ; ior
        xld.w   [%r1 + 4], %r4                  ; fd
        NEXT
        END_CODE

;;; : CLOSE-FILE       ( fileid -- ior )
        CODE    close_file, "close-file", FLAG_NORMAL
        ld.w    %r6, [%r1]                      ; fileid
        xcall   FileSystem_close
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : READ-FILE        ( b u fileid -- u2 ior )
        CODE    read_file, "read-file", FLAG_NORMAL
        ld.w    %r6, [%r1]+                     ; fileid
        ld.w    %r8, [%r1]                      ; count
        xld.w   %r7, [%r1 + 4]                  ; buffer
        xcall   FileSystem_read
        ld.w    [%r1], %r5                      ; ior
        xld.w   [%r1 + 4], %r4                  ; count2
        NEXT
        END_CODE

;;; : READ-LINE        ( b u fileid -- u2 f ior )
;;; \ EOF:                             0  T    0
;;;       >R >R DUP R> R> SWAP         \ b0 b fileid u
;;;       FOR AFT                      \ b0 b fileid
;;;         BEGIN
;;;           2DUP 1 SWAP READ-FILE    \ b0 b fileid 0/1 ior
;;;           ?DUP IF >R 2DROP SWAP - R> R> DROP EXIT THEN
;;;           0= IF DROP SWAP - DUP 0 R> DROP EXIT THEN \ u2 f 0
;;;                                    \ b0 b fileid
;;;           OVER C@ [CTRL] M XOR     \ b0 b fileid f
;;;         UNTIL
;;;         \ here have a non CR character
;;;         OVER C@ [CTRL] J = IF DROP SWAP - TRUE 0 R> DROP EXIT THEN
;;;         >R 1+ R>                   \ b0 b+1 fileid
;;;       THEN NEXT
;;; \ filled buffer without CR/LF      \ b0 b' fileid
;;;       DROP SWAP - TRUE 0           \ u2 T 0
;;; ;
        COLON   read_line, "read-line", FLAG_NORMAL
        .long   to_r, to_r, dup, r_from, r_from, swap

        .long   to_r
        .long   branch, read_line_l5
read_line_l1:
        .long   twodup, dolit, 1, swap, read_file
        .long   qdup, qbranch, read_line_l2
        .long   to_r, twodrop, swap, minus, true, r_from, r_from, drop, exit
read_line_l2:
        .long   zero_equal, qbranch, read_line_l3
        .long   drop, swap, minus, dup, dolit, 0, r_from, drop, exit
read_line_l3:
        .long   over, cfetch, dolit, 13, _xor, qbranch, read_line_l1
        .long   over, cfetch, dolit, 10, equal, qbranch, read_line_l4
        .long   drop, swap, minus, true, dolit, 0, r_from, drop, exit
read_line_l4:
        .long   to_r, increment, r_from
read_line_l5:
        .long   donext, read_line_l1
        .long   drop, swap, minus, true, dolit, 0, exit

;;; : WRITE-FILE       ( b u fileid -- u2 ior )
        CODE    write_file, "write-file", FLAG_NORMAL
        ld.w    %r6, [%r1]+                     ; fileid
        ld.w    %r8, [%r1]                      ; count
        xld.w   %r7, [%r1 + 4]                  ; buffer
        xcall   FileSystem_write
        ld.w    [%r1], %r5                      ; ior
        xld.w   [%r1 + 4], %r4                  ; count2
        NEXT
        END_CODE

;;; : FLUSH-FILE       ( fileid -- ior )
        CODE    flush_file, "flush-file", FLAG_NORMAL
        ld.w    %r6, [%r1]                      ; fileid
        xcall   FileSystem_sync
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : FILE-SIZE        ( fileid -- u ior )
        CODE    file_size, "file-size", FLAG_NORMAL
        ld.w    %r6, [%r1]                      ; fileid
        xcall   FileSystem_lsize
        ld.w    [%r1], %r4                      ; size
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : FILE-POSITION    ( fileid -- u ior )
        CODE    file_position, "file-position", FLAG_NORMAL
        ld.w    %r6, [%r1]                      ; fileid
        xcall   FileSystem_ltell
        ld.w    [%r1], %r4                      ; pos
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;; : REPOSITION-FILE  ( u fileid -- ior )
        CODE    reposition_file, "reposition-file", FLAG_NORMAL
        ld.w    %r6, [%r1]+                     ; fileid
        ld.w    %r7, [%r1]                      ; pos
        xcall   FileSystem_lseek
        ld.w    [%r1], %r5                      ; ior
        NEXT
        END_CODE

;;;  : FILESYSTEM-INIT (  --  )
        CODE   filesystem_init, "filesystem-init", FLAG_NORMAL
        xcall   FileSystem_initialise
        NEXT
        END_CODE

;;; \ simple test
;;; : print ( b u -- )
;;;         s" forth.s" r/o open-file
;;;         0= if fd !
;;;           buffer buffer-size fd @ read-file
;;;           0= if  buffer swap type
;;;           else   cr ." read error"
;;;           then
;;;           fd @ close-file drop
;;;         else  cr ." open error
;;;         then
;;;         cr ;
BUFFER_SIZE = 1024
        CONSTANT buffer_size, "buffer-size", FLAG_NORMAL
        .long   BUFFER_SIZE

        VARIABLE fd, "fd", FLAG_NORMAL
        .long   0

        VARIABLE buffer, "buffer", FLAG_NORMAL
        .space  BUFFER_SIZE
        .balign 4

        COLON   pf, "pf", FLAG_NORMAL
        .long   do_dollar_quote
        FSTRING "forth.s"
        .long   print, exit

        COLON   pt, "pt", FLAG_NORMAL
        .long   do_dollar_quote
        FSTRING "test.dat"
        .long   print, exit

        COLON   print, "print", FLAG_NORMAL
        .long   count, readonly, open_file
        .long   qdup, qbranch, print_l1
        .long   cr, do_dot_quote
        FSTRING "open error = "
        .long   dot, cr, exit

print_l1:
        .long   fd, store

print_l2:
        .long   buffer, buffer_size, decrement, decrement
        .long   fd, fetch, read_line
        .long   qdup, qbranch, print_l3
        .long   cr, do_dot_quote
        FSTRING "read error = "
        .long   dot, twodrop, cr, branch, print_l4

print_l3:                                       ; u2 f
        .long   qbranch, print_l4

        .long   buffer, swap, type, cr
        .long   branch, print_l2
print_l4:
        .long   fd, fetch, close_file, drop
        .long   cr, exit

some_text:
        .ascii  "This is a test for file write\n"
        .ascii  "with just a few lines of text\n"
        .ascii  "0123456789\n"
some_text_length = . - some_text
        .balign 4

        COLON   mkfile, "mkfile", FLAG_NORMAL
        .long   do_dollar_quote
        FSTRING "test.dat"
        .long   count, writeonly, create_file
        .long   qdup, qbranch, mkfile_l1
        .long   cr, do_dot_quote
        FSTRING "create error = "
        .long   dot, cr, exit
mkfile_l1:
        .long   fd, store
        .long   dolit, some_text, dolit, some_text_length, fd, fetch, write_file
        .long   qdup, qbranch, mkfile_l2
        .long   cr, do_dot_quote
        FSTRING "write error = "
        .long   dot, cr
mkfile_l2:
        .long   cr, do_dot_quote
        FSTRING "write count = "
        .long   dot, cr
        .long   fd, fetch, close_file, drop
mkfile_l3:
        .long   cr, exit


;;; .( Hardware reset )

;;; \ version

;;; =BUILD-NUMBER CONSTANT BUILD ( -- u )
        CONSTANT build, "build", FLAG_NORMAL
        .long   BUILD_NUMBER

;;; BANNER ( -- ) CR <message> CR
        COLON   banner, "banner", FLAG_NORMAL
        .long   cr
	.long	do_dot_quote
	FSTRING	"S33 forth interpreter (build:"
        .long   build, u_dot, do_dot_quote
        FSTRING ")"
        .long   cr, exit

;;; CREATE 'BOOT  ' BANNER , \ application vector
        VARIABLE tboot, "'boot", FLAG_NORMAL
        .long   banner

;;; : COLD ( -- )
;;;   \ init CPU
;;;   \ init stacks
;;;   \ init user area
;;;   \ init IP
;;;   PRESET
;;;   FORTH CONTEXT @ DUP CURRENT D! OVERT
;;;   'BOOT @EXECUTE
;;;   QUIT ;
        COLON   cold, "cold", FLAG_NORMAL
        .long   preset, filesystem_init
        .long   forth, context, fetch, dup, current, dstore, overt
        .long   tboot, atexecute
        .long   quit

        COLON   nop, "nop", FLAG_NORMAL              ;debug
        .long   exit

        CODE    BREAKPOINT, "(brk)", FLAG_NORMAL     ;debug
        xcall   xdebug                               ;debug
        xld.w   %r6, bpt
        xcall   Serial_PutString
s1:     jp      s1                                   ;debug
bpt:    .asciz  "STOPPED\r\n"
        .balign 4

        CODE    DEBUG, "(debug)", FLAG_NORMAL        ;debug
        xcall   xdebug                               ;debug
        NEXT                                         ;debug

        COLON   led, "led", FLAG_NORMAL              ;debug
        .long   invert, dolit, 0x04000000, hstore, exit

        COLON   delay, "delay", FLAG_NORMAL          ;debug
        .long   dolit, 50000
delay_l1:
        .long   decrement, qdup, qbranch, delay_l2
        .long   branch, delay_l1
delay_l2:
        .long   exit

        COLON   flash, "flash", FLAG_NORMAL          ;debug
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
        .section .forth_dict
        .balign 4

end_of_dictionary:
	.long	0,0

        .space  65536                           ; space for more names
end_of_dictionary_memory:

last_name = __last_name                         ; should be the final name


;;; finish off the code
        .section .forth_param
        .balign 4

end_of_code:
        .space  65536                           ; space for more code
end_of_code_memory:
