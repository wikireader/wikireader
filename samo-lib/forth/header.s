;;; header.s
;;;
;;; Copyright 2009 Christopher Hall <hsw@openmoko.com>
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

;;; some character constants
backspace = 0x08
line_feed = 0x0a
carriage_return = 0x0d
delete = 0x7f

;;; some special constants
BYTES_PER_CELL = 4
BITS_PER_BYTE = 8
BITS_PER_CELL = (BITS_PER_BYTE * BYTES_PER_CELL)

;;; as above but for shift instead of multiply/divide
LOG2_BITE_PER_BYTE = 3
LOG2_BYTES_PER_CELL = 2

;;; boolean - to match Forth standard
FALSE = 0
TRUE = -1

;;; header flags
FLAG_IMMEDIATE = 0x01
FLAG_COMPILE_ONLY = 0x02
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
;        xcall   xdebug
        jp      %r3                             ; execute the code
        .endm


;;; inline forth counted strings

        .macro  LSTRING, text
        .long   str_\@_finish - str_\@_start
str_\@_start:
        .ascii  "\text"
str_\@_finish:
        .balign 4
        .endm


;;; macro to create offsets in bytes and cells

        .macro  MAKE_OFFSET, label, value
        .ifnotdef \label\()_BYTES
\label\()_BYTES = \value
\label\()_CELLS = \value
        .endif
        .endm


;;; the header
;;;  0: code address
;;;  4: param address
;;;  8: flags
;;; 12: link address
;;; 16: count (name address points here)
;;; 20: name string (byte count)
;;; 24+count: (zeros as required to .balign 4)

        .section .forth_dict, "wa"
        .balign 4

__last_name = 0                                       ; to link the list

        .macro  HEADER, label, name, flags, code

        .section .forth_dict
        .balign 4
        .global \label
\label\():
        .long   \code                                 ; code
l_param_\@:
        .long   param_\label                          ; param
l_flags_\@:
        .long   \flags                                ; flags

prev_\label = __last_name
l_link_\@:
        .long   prev_\label                           ; link

        .global name_\label
name_\label\():
__last_name = .
        LSTRING "\name"

        .balign 4

        .global param_\label
param_\label\():

;        MAKE_OFFSET DICTIONARY_HEADER,       "( name_\label - \label )"

        MAKE_OFFSET DICTIONARY_CODE_TO_NAME_OFFSET,  "( name_\label - \label )"
        MAKE_OFFSET DICTIONARY_CODE_TO_PARAM_OFFSET, "( l_param_\@ - \label )"
        MAKE_OFFSET DICTIONARY_CODE_TO_LINK_OFFSET,  "( l_link_\@ - \label )"
        MAKE_OFFSET DICTIONARY_CODE_TO_FLAGS_OFFSET, "( l_flags_\@ - \label )"

;;; these are for find
        MAKE_OFFSET DICTIONARY_CODE_OFFSET,  "( name_\label - \label )"
        MAKE_OFFSET DICTIONARY_PARAM_OFFSET, "( name_\label - l_param_\@ )"
        MAKE_OFFSET DICTIONARY_FLAGS_OFFSET, "( name_\label - l_flags_\@ )"
        MAKE_OFFSET DICTIONARY_LINK_OFFSET,  "( name_\label - l_link_\@ )"

        .endm


;;; code definitions

        .macro  CODE, sequence, name, label, flags
        HEADER  \label, "\name", \flags, param_\label
        .endm

        .macro  END_CODE
        .endm


;;; colon definitions

        .macro  COLON, sequence, name, label, flags
        HEADER  \label, "\name", \flags, param_paren_colon_paren
        .endm

        .macro  END_COLON
        .long   exit
        .long   0
        .endm


;;; variable definitions

        .macro  VARIABLE, sequence, name, label, flags
        HEADER  \label, "\name", \flags, param_paren_var_paren
        .endm


        .macro  CREATE, sequence, name, label, flags
        HEADER  \label, "\name", \flags, param_paren_var_paren
        .endm


;;; constant definitions

        .macro  CONSTANT, sequence, name, label, flags
        HEADER  \label, "\name", \flags, param_paren_const_paren
        .endm


;;; miscellaneous variables

        .section .bss

        .balign 4
terminal_buffer_start:
        .space   65536
terminal_buffer_length = . -terminal_buffer_start


stack_pointer_low_limit:
        .space   65536
        .global initial_stack_pointer
initial_stack_pointer:                                ; NOTE: stack underflows over return space!

return_pointer_low_limit:
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

        ld.w    %r0, 0                                ; clear status register
        ld.w    %psr, %r0

        xcall   Vector_initialise                     ; must be first
        xcall   Serial_initialise
        xcall   Button_initialise
        xcall   CTP_initialise

        xld.w   %r0, cold_start                       ; initial ip value
;;;       xcall   xdebug
        NEXT


;;; headerless code to initially boot the system
        .balign 4                                     ; forth byte code must be aligned
cold_start:
        .long   cold                                  ; initial forth code
        .long   branch, cold_start                    ; just run cold_start in a loop

