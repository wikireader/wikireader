\ meta.fs
\
\ Copyright (c) 2009 Openmoko Inc.
\
\ Authors   Christopher Hall <hsw@openmoko.com>
\
\ Redistribution and use in source and binary forms, with or without
\ modification, are permitted provided that the following conditions are
\ met:
\
\  1. Redistributions of source code must retain the above copyright
\     notice, this list of conditions and the following disclaimer.
\
\  2. Redistributions in binary form must reproduce the above copyright
\     notice, this list of conditions and the following disclaimer in
\     the documentation and/or other materials provided with the
\     distribution.
\
\ THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
\ EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
\ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
\ PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
\ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
\ CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
\ SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
\ BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
\ WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
\ OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
\ IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

forth definitions

vocabulary meta-compiler  immediate
vocabulary meta-words     immediate
vocabulary meta-interpret immediate
vocabulary meta-assemble  immediate


\ word for meta compiler

only forth
also meta-compiler definitions

variable label-count
variable meta-state
variable suppress-once

: tab 9 emit ;

: getline
  refill 0= abort" premature EOF"
;

variable cross-dict-flag

: cross-dict-name ( -- )
  cross-dict-flag @
  0 cross-dict-flag !
  case
    0 of
      ." forth_dict"
    endof
    1 of
      ." root_dict"
    endof
  endcase
  space ;

: gen-label ( -- n )
  1 label-count +! label-count @ ;

: type-nodash ( c-addr u -- )
  0 ?do
    dup c@ dup [char] - = if
      drop [char] _
    then
    emit
    char+
  loop drop ;

: escaped-type ( c-addr u -- )
  0 ?do
    dup c@ dup case
      [char] " of
        drop
        [char] \ emit [char] 0 emit
        [char] 4 emit [char] 2 emit
      endof
      [char] \ of
        drop
        [char] \ dup emit emit
      endof
      [char] ; of
        drop
        ." \073"
      endof
      emit
    endcase
    char+
  loop drop ;

: hex. ( u -- ) base @ >r hex ." 0x" u. r> base ! ;

: .long ( -- )    tab ." .long" tab ;

: .byte ( -- )    tab ." .byte" tab ;

: suppress true suppress-once ! ;

: output-symbol-pre ( -- f )
    suppress-once @ 0= dup if
      .long
    then ;

: output-symbol-post ( f -- )
    if cr then
    false suppress-once ! ;

: output-symbol" ( string<quote> -- )
    postpone output-symbol-pre
    postpone ."
    postpone output-symbol-post
; immediate

: .lstring  ( -- \ "<string>" )
  tab ." LSTRING" tab 34 emit
  [char] " parse escaped-type 34 emit cr ;

: _number ( s-addr -- u \ number )
    base @ >r          \ R: base
    >r 0 dup r> count  \ ud c-addr u
    over c@            \ ud c-addr u
    0 >r               \ R: 0  (positive)
    case
        [char] + of swap char+ swap 1-  endof
        [char] - of swap char+ swap 1- r> drop 1 >r endof
        [char] % of swap char+ swap 1-   2 base ! endof
        [char] & of swap char+ swap 1-   8 base ! endof
        [char] # of swap char+ swap 1-  10 base ! endof
        [char] $ of swap char+ swap 1-  16 base ! endof
    endcase
    \ ud c-addr u  R: base sign
    >number ( d c-addr u )
    ?dup if
        ." .error " 34 emit ." ***INVALID: " type 34 emit cr -1
        \ ." >>" type 2drop true abort" invalid number"
    then
    2drop
    r> if negate then
    r> base !
;

: _interpret ( -- )
      false meta-state ! ;

: _compile ( -- )
      true meta-state ! ;

: _literal ( u -- )
  .long ." paren_lit_paren, " . cr ;

variable last-parsed-word-xt

: quoted-parse-word ( flag -- )
  0 last-parsed-word-xt !
  parse-word 2dup
  34 emit
  escaped-type
  34 emit
  space
  ['] meta-words >body
  search-wordlist if
    suppress dup last-parsed-word-xt ! execute
  else
    ." !!ERROR: not found in symbol.fi!!"
  then
  space
  if
    last-parsed-word-xt @ ?dup if
      ." flags_"
      suppress execute
    then
  else
    ." 0"
  then
  cr
;

: set-flags-to-zero ( -- )
  last-parsed-word-xt @ ?dup if
    ." flags_"
    suppress execute
    ."  = 0" cr
  then
;


: meta-constant  ( C: x "<spaces>name" -- ) ( -- x )
 >r get-order get-current
 only postpone forth also postpone meta-interpret
 definitions
 r> constant
 set-current set-order ;

: meta-compile ( -- )
  ." ;;; Meta Compiler starting" cr
  begin
    \ cr ." >> "
    bl word dup count nip if
      \ dup count 34 emit type 34 emit bl emit
      meta-state @ if \ compiling
        only [compile] meta-words
        also [compile] meta-assemble
        find if
          execute
        else
          _number _literal
        then
      else \ interpreting
        only [compile] forth
        also [compile] meta-interpret
        find if
          execute
        else
          _number
        then
      then
    else
      drop
      refill 0= if
        ." ;;; Meta Compiler exiting" cr cr
        only [compile] forth
        exit
      then
    then
  again
;

\ words that are more than just a simple print
\ these override the meta-words versions
\ used in interpret mode

only forth
also meta-interpret definitions
meta-compiler

\ the next definition will be in this dictionary
: cross-root-definition ( -- ) 1 cross-dict-flag ! ;

: :: ( -- \ word )
  parse-word 2drop ;

: code ( -- \ string )
  cr
  tab ." CODE" tab cross-dict-name
  true quoted-parse-word
  \ rest of line is ignored
  begin
    getline
    tib #tib @ s" end-code" str= 0= while
    tib #tib @ type cr
  repeat
  getline
  tab ." END_CODE" cr
  set-flags-to-zero
;

: ] ( -- )  _compile ;

: : ( -- \ word )
  cr
  tab ." COLON" tab cross-dict-name
  true quoted-parse-word
  _compile
;


: constant ( x -- \ word )
  cr
  tab ." CONSTANT" tab cross-dict-name
  dup constant
  latestxt >name cell+ dup cell+ swap @ 255 and
  34 emit
  escaped-type
  34 emit
  parse-word 2drop parse-word 2drop \ ignore :: <word>
  space
  latestxt >name cell+ dup cell+ swap @ 255 and
  ['] meta-words >body
  search-wordlist if
    suppress execute ."  0"
  else
    ." !!ERROR: not found in symbol.fi!!
  then
  cr .long . cr
;

: forth ;
: c33 ;
: only ;
: also ;

: variable ( -- \ word )
  cr
  tab ." VARIABLE" tab cross-dict-name
  false quoted-parse-word
  .long 0 . cr
;

: create ( -- \ word )
  cr
  tab ." CREATE" tab cross-dict-name
  false quoted-parse-word
;


: <',> ( -- \ word)
  get-order
  only postpone meta-words
  bl word
  find if
    execute
  else
    ." .error ***unknown***" cr
  then
  set-order
;

: allot ( u -- )
  3 + 4 /
  tab ." .rept" tab . cr
  .long 0 . cr
  tab ." .endr" cr
;

: , ( u -- )
  .long hex. cr
;

: c, ( u -- )
  .byte hex. cr
;

: immediate ( -- )
  last-parsed-word-xt @ ?dup if
    dup
    ." flags_" suppress execute ."  = "
    ." flags_" suppress execute ."  + FLAG_IMMEDIATE"
    cr
  then ;

: compile-only ( -- )
  last-parsed-word-xt @ ?dup if
    dup
    ." flags_" suppress execute ."  = "
    ." flags_" suppress execute ."  + FLAG_COMPILE_ONLY"
    cr
  then ;


\ should not be here **************************************************
: literal ( u -- ) _literal ;


: cells ( u -- u ) 4 * ;
: cell+ ( u -- u ) 4 + ;
: cell- ( u -- u ) 4 - ;


\ word that are more than just a simple print
\ these override the meta-words versions
\ used in compile/assembly generation mode

only forth
also meta-assemble definitions
meta-compiler

: :: ( -- \ word )
  parse-word 2drop ;

: .( ( -- \ string )
  [char] ) parse type ;

: [char] ( -- c \ word)
  char _literal ;

: [ctrl] ( -- c \ word)
  char 31 and _literal ;

: literal ( u -- ) _literal ;

: ; .long ." exit" cr
  tab ." END_COLON" cr
  set-flags-to-zero
  _interpret ;

: [ ( -- )
      _interpret ;

: do ( -- dest label )
  gen-label dup
  .long ." paren_do_paren, L" . cr
  ." L" gen-label dup . [char] : emit cr ;

: ?do ( -- dest label )
  gen-label dup
  .long ." paren_question_do_paren, L" . cr
  ." L" gen-label dup . [char] : emit cr ;

: loop ( dest label -- )
  .long ." paren_loop_paren, L" . cr
  ." L" . [char] : emit cr ;

: +loop ( dest label -- )
  .long ." paren_plus_loop_paren, L" . cr
  ." L" . [char] : emit cr ;


: begin ( -- label )
  ." L" gen-label dup . [char] : emit cr ;

: again ( label -- )
  .long ." branch, L" . cr ;

: while ( dest -- origin dest )
  .long ." question_branch, L" gen-label dup . cr swap ;

: until ( dest -- )
  .long ." question_branch, L" . cr ;


: repeat ( origin dest -- )
  .long ." branch, L" . cr
  ." L" . [char] : emit cr ;

: if ( -- label )
  .long ." question_branch, L" gen-label dup . cr ;


: then ( -- label )
  ." L" . [char] : emit cr ;


: else ( label -- label )
  .long ." branch, L" gen-label dup . cr
  swap
  ." L" . [char] : emit cr ;

: case ( -- 0 )
  0 ;

: of ( -- <if> )
     .long ." over, equals" cr
     .long ." question_branch, L" gen-label dup . cr
     .long ." drop" cr
;

: endof ( <if> -- <else> )
  .long ." branch, L" gen-label dup . cr
  swap
  ." L" . [char] : emit cr
;


: endcase ( 0 <if>*n -- )
  .long ." drop" cr
  begin
    ?dup
  while
    ." L" . [char] : emit cr
  repeat
;

: ['] ( -- \ word)
  get-order
  only postpone meta-words
  bl word
  find if
    .long ." paren_lit_paren, "
    suppress
    execute
    cr
  else
    ." .error ***unknown***" cr
  then
  set-order
;

: postpone ( -- \ word)
  get-order
  only [compile] meta-words
  bl word
  find case
    1 of
      execute
    endof
    -1 of
      .long ." paren_lit_paren, "
      suppress
      execute
      ." , compile_comma" cr
    endof
    ." .error ****unknown***" cr
  endcase
  set-order
;

: ( ( -- \ comment )
  [char] ) parse 2drop ;

: \ ( -- \ comment )
  getline ;

: ." ( -- \ "<string>" )
  .long ." paren_s_quote_paren" cr
  .lstring
  .long ." type" cr
;

: lcd-." ( -- \ "<string>" )
  .long ." paren_s_quote_paren" cr
  .lstring
  .long ." lcd_type" cr
;

: s" ( "string" -- )
  .long ." paren_s_quote_paren" cr
  .lstring
;


: abort" ( -- \ "<string>" )
  .long ." question_branch, L" gen-label dup . cr
  .long ." paren_s_quote_paren" cr
  .lstring
  .long ." type, abort" cr
  ." L" . [char] : emit cr
;
