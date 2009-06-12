\ ansi-forth.fs
\
\ Copyright 2009 Christopher Hall <hsw@openmoko.com>
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

include meta.fs

meta-compile

\ possible formats:
\   <colon>   word <double-colon> alt-name ( -- )
\   <c-o-d-e> word <double-colon> alt-name ( -- )

6
constant build-number     :: build-number            ( -- n )

code !                    :: store                   ( x a-addr -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.w    [%r4], %r5
        NEXT
end-code

\ **not really correct, it will not work with 'ud' i.e.64 bit numbers
: #                       :: number-sign             ( ud1 -- ud2 )
  base @ um/mod ( ur uq )
  swap 9 over < if
    [ char A char 9 - 1- ] literal +
  then
  [char] 0 + hold 0 ;

: #>                      :: number-sign-greater     ( xd -- c-addr u )
  2drop hld @ pad over - ;

: #s                      :: number-sign-s           ( ud1 -- ud2 )
  begin # 2dup or while repeat ;

: '                       :: tick                    ( "<spaces>name" -- xt )
  parse-word search-wordlists if exit then throw ;

: (                       :: paren                   ( "ccc<paren>" -- )
  [char] ) parse 2drop ; immediate

code (colon)              :: paren-colon-paren       ( R: -- nest-sys )
        pushn   %r0                                  ; save previous ip
        ld.w    %r0, [%r2]                           ; ip = param address
        NEXT
end-code
compile-only


code (const)              :: paren-const-paren       ( %r2: address -- )
        ld.w    %r4, [%r2]                           ; %r4 = parameter address
        ld.w    %r4, [%r4]                           ; read the constant value
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code
compile-only

code (?do)                :: paren-question-do-paren ( limit index -- ) ( R: -- limit index )
        ld.w    %r4, [%r1]+                          ; index
        ld.w    %r5, [%r1]+                          ; limit

        cmp     %r4, %r5                             ; equal? => skip the loop
        jreq    qdo_l1                               ; ...yes

        sub     %sp, 1
        ld.w    [%sp], %r5                           ; limit
        sub     %sp, 1
        ld.w    [%sp], %r4                           ; index
        add     %r0, BYTES_PER_CELL                  ; skip the branch address
        NEXT

qdo_l1:
        ld.w    %r0, [%r0]                           ; branch over loop
        NEXT
end-code

code (do)                 :: paren-do-paren          ( limit index -- ) ( R: -- limit index )
        ld.w    %r4, [%r1]+                          ; index
        ld.w    %r5, [%r1]+                          ; limit
        sub     %sp, 1
        ld.w    [%sp], %r5                           ; limit
        sub     %sp, 1
        ld.w    [%sp], %r4                           ; index
        add     %r0, BYTES_PER_CELL                  ; skip the branch address
        NEXT
end-code

code (lit)                :: paren-lit-paren         ( -- x)
        ld.w    %r3, [%r0]+
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r3
        NEXT
end-code

\ (local)                 :: paren-local-paren       ( c-addr u -- )

code (+loop)              :: paren-plus-loop-paren   ( x -- ) ( R: stop count+x -- )
        ld.w    %r4, [%r1]+                          ; increment
        ld.w    %r5, [%sp]                           ; count
        add     %r4, %r5                             ; count + increment
        ld.w    [%sp], %r4                           ; count + increment

        xld.w   %r6, [%sp + BYTES_PER_CELL]          ; stop

        sub     %r5, %r6                             ; count - stop
        sub     %r4, %r6                             ; count+increment - stop
        xor     %r4, %r5                             ; if negative then loop is complete
        jrlt    loop_done                            ; ...yes, skip the branch offset
        ld.w    %r0, [%r0]                           ; ...no, branch back
        NEXT
end-code

code (loop)               :: paren-loop-paren        ( x -- ) ( R: stop count+x -- )
        ld.w    %r5, [%sp]                           ; count
        add     %r5, 1
        ld.w    [%sp], %r5                           ; count + increment
        xld.w   %r4, [%sp + BYTES_PER_CELL]          ; stop
        cmp     %r4, %r5                             ; count == stop?
        jreq    loop_done                            ; ...yes, skip the branch offset
loop_continue:
        ld.w    %r0, [%r0]                           ; ...no, branch back
        NEXT
loop_done:
        add     %sp, 2                               ; drop 2 stack words
        add     %r0, BYTES_PER_CELL                  ; skip branch offset
        NEXT
end-code

: (s")                    :: paren-s-quote-paren     ( -- c-addr u )
  r> count         ( c-addr u )
  2dup + aligned   ( c-addr u r-addr )
  >r
; compile-only

code (var)                :: paren-var-paren         ( %r2: address -- )
        ld.w    %r4, [%r2]                           ; %r4 = parameter address
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code
compile-only

code *                    :: star                    ( n1|u1 n2|u2 -- n3|u3 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mlt.w   %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        NEXT
end-code

\ */                      :: star-slash              ( n1 n2 n3 -- n4 )
\  */mod nip ;

\ */mod                   :: star-slash-mod          ( n1 n2 n3 -- n4:r n5:q )

code +                    :: plus                    ( n1|u1 n2|u2 -- n3|u3 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

code +!                   :: plus-store              ( n|u a-addr -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.w    %r6, [%r4]
        add     %r5, %r6
        ld.w    [%r4], %r5
        NEXT
end-code

: +loop                   :: plus-loop               ( C: do-sys -- ) ( -- ) ( R: loop-sys1 --  | loop-sys2 )
  postpone (+loop) compile, here swap ! ; immediate compile-only

: ,                       :: comma                   ( x -- )
  align here dup cell+ cp ! ! ;

code -                    :: minus                   ( n1|u1 n2|u2 -- n3|u3 )
        ld.w    %r5, [%r1]+
        ld.w    %r4, [%r1]
        sub     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

: -rot                    :: minus-rote              ( x1 x2 x3 -- x3 x1 x2 )
  rot rot ;

: -trailing               :: dash-trailing           ( c-addr u1 -- c-addr u2 )
  dup 1- swap 0 ?do
    2dup i cr - + c@  bl xor
    if i - char+ unloop exit then
  loop drop 0 ;

: .                       :: dot                     ( n -- )
  s>d d. ;

: ."                      :: dot-quote               ( "ccc<quote>" -- )
  postpone s" postpone type ; immediate compile-only

: .(                      :: dot-paren               ( "ccc<paren>" -- )
  [char] ) parse cr type ; immediate

: .r                      :: dot-r                   ( n1 n2 -- )
  >r s>d r> d.r ;

: .s                      :: dot-s                   ( -- )
 cr ." [" depth dup 0 .r ." ]: "
 dup 0 ?do dup i - pick . loop ."  <-top"
 drop ;

\ .vocab                  :: dot-vocab               ( wid -- )

: /                       :: slash                   ( n1 n2 -- n3 )
  /mod nip ;

: /mod                    :: slash-mod               ( n1 n2 -- n3:r n4:q )
  >r s>d r> fm/mod ;

: /string                 :: slash-string            ( c-addr1 u1 n -- c-addr2 u2 )
  2dup < if 2drop 0
  else
    swap over + ( c-addr1 n u1+n )
    rot rot +
  then ;

code 0<                   :: zero-less               ( n -- flag )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jrlt    set_true_flag
set_false_flag:
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4
        NEXT
set_true_flag:
        ld.w    %r4, TRUE
        ld.w    [%r1], %r4
        NEXT
end-code

code 0<>                  :: zero-not-equals         ( x -- flag )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jrne    set_true_flag
        jp      set_false_flag
end-code

code 0=                   :: zero-equals             ( x -- flag )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jreq    set_true_flag
        jp      set_false_flag
end-code

code 0>                   :: zero-greater            ( n -- flag )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jrgt    set_true_flag
        jp      set_false_flag
end-code

code 1+                   :: one-plus                ( n1|u1 -- n2|u2 )
        ld.w    %r4, [%r1]
        add     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code 1-                   :: one-minus               ( n1|u1 -- n2|u2 )
        ld.w    %r4, [%r1]
        sub     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code 2!                   :: two-store               ( x1 x2 a-addr -- )
        ld.w    %r6, [%r1]+                          ; a-addr
        ld.w    %r5, [%r1]+                          ; x2
        ld.w    %r4, [%r1]+                          ; x1
        ld.w    [%r6]+, %r4                          ; x1
        ld.w    [%r6]+, %r5                          ; x2
        NEXT
end-code

code 2*                   :: two-star                ( x1 -- x2 )
        ld.w    %r4, [%r1]
        sla     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code 2/                   :: two-slash               ( x1 -- x2 )
        ld.w    %r4, [%r1]
        sra     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code 2>r                  :: two-to-r                ( x1 x2 -- ) ( R: -- x1 x2 )
        ld.w    %r4, [%r1]+                          ; x2
        ld.w    %r5, [%r1]+                          ; x1
        sub     %sp, 1
        ld.w    [%sp], %r5
        sub     %sp, 1
        ld.w    [%sp], %r4
        NEXT
end-code

code 2@                   :: two-fetch               ( a-addr -- x1 x2 )
        ld.w    %r4, [%r1]                           ; a-addr
        ld.w    %r5, [%r4]+
        ld.w    [%r1], %r5
        sub     %r1, BYTES_PER_CELL
        ld.w    %r5, [%r4]
        ld.w    [%r1], %r5
        NEXT
end-code

\ 2constant               :: two-constant            ( x1 x2 "<spaces>name" -- )

: 2drop                   :: two-drop                ( x1 x2 -- )
  drop drop ;

: 2dup                    :: two-dupe                ( x1 x2 -- x1 x2 x1 x2 )
  over over ;

\ 2literal                :: two-literal             ( x1 x2 -- )
\ 2over                   :: two-over                ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )

code 2r>                  :: two-r-from              ( -- x1 x2 ) ( R: x1 x2 -- )
        ld.w    %r4, [%sp]                           ; x2
        add     %sp, 1
        ld.w    %r5, [%sp]                           ; x1
        add     %sp, 1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code 2r@                  :: two-r-fetch             ( -- x1 x2 ) ( R: x1 x2 -- x1 x2 )
        ld.w    %r4, [%sp]                           ; x2
        xld.w   %r5, [%sp + BYTES_PER_CELL]          ; x1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code


\ 2rot                    :: two-rote                ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )

code 2swap                :: two-swap                ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
        ld.w    %r4, [%r1]                           ; x4
        xld.w    %r5, [%r1 + BYTES_PER_CELL]         ; x3
        xld.w    %r6, [%r1 + BYTES_PER_CELL * 2]     ; x2
        xld.w    %r7, [%r1 + BYTES_PER_CELL * 3]     ; x1
        ld.w    [%r1], %r6                           ; x2
        xld.w    [%r1 + BYTES_PER_CELL], %r7         ; x1
        xld.w    [%r1 + BYTES_PER_CELL * 2], %r4; x4
        xld.w    [%r1 + BYTES_PER_CELL * 3], %r5; x3
        NEXT
end-code

: 2variable               :: two-variable            ( "<spaces>name" -- )
  create 0 , 0 , ;

: :                       :: colon                   ( C: "<spaces>name" -- colon-sys )
  create
  ['] (colon) @ last-definition @ !
  ]
;

\ :noname                 :: colon-no-name           ( C: -- colon-sys )  ( S:  -- xt )

: ;                       :: semicolon               ( C: colon-sys -- )
  postpone exit
  0 compile,     \ zero for see
  postpone [
; immediate compile-only

\ ;code                   :: semicolon-code          ( C: colon-sys -- )

code <                    :: less-than               ( n1 n2 -- flag )
        ld.w    %r5, [%r1]+                          ; n2
        ld.w    %r4, [%r1]                           ; n1
        cmp     %r4, %r5                             ; n1 < n2
        jrlt    set_true_flag_1                      ; ...yes
        jp      set_false_flag_1                     ; ...no
end-code

: <#                      :: less-number-sign        ( -- )
  pad hld ! ;

code <>                   :: not-equals              ( x1 x2 -- flag )
        ld.w    %r5, [%r1]+                          ; x1
        ld.w    %r4, [%r1]                           ; x2
        cmp     %r4, %r5                             ; x1 <> x2
        jrne    set_true_flag_1                      ; ...yes
        jp      set_false_flag_1                     ; ...no
end-code

code =                    :: equals                  ( x1 x2 -- flag )
        ld.w    %r5, [%r1]+                          ; x1
        ld.w    %r4, [%r1]                           ; x2
        cmp     %r4, %r5                             ; x1 = x2
        jreq    set_true_flag_1                      ; ...yes
        jp      set_false_flag_1                     ; ...no
end-code

code >                    :: greater-than            ( n1 n2 -- flag )
        ld.w    %r5, [%r1]+                          ; n2
        ld.w    %r4, [%r1]                           ; n1
        cmp      %r4, %r5                            ; n1 > n2 ?
        jrgt    set_true_flag_1                      ; ...yes
set_false_flag_1:
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4
        NEXT
set_true_flag_1:
        ld.w    %r4, TRUE
        ld.w    [%r1], %r4
        NEXT
end-code

code >body                :: to-body                 ( xt -- a-addr )
        ld.w    %r4, [%r1]                           ; xt
        xld.w   %r5, DICTIONARY_CODE_TO_PARAM_OFFSET_BYTES
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

code >code                :: to-code                 ( name-a-addr -- xt )
        ld.w    %r4, [%r1]                           ; xt
        xld.w   %r5, DICTIONARY_CODE_TO_NAME_OFFSET_BYTES
        sub     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

code >flags               :: to-flags                ( xt -- a-addr )
        ld.w    %r4, [%r1]                           ; xt
        xld.w   %r5, DICTIONARY_CODE_TO_FLAGS_OFFSET_BYTES
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

variable  >in             :: to-in                   ( -- a-addr )

code >link                :: to-link                 ( xt -- a-addr )
        ld.w    %r4, [%r1]                           ; xt
        xld.w   %r5, DICTIONARY_CODE_TO_LINK_OFFSET_BYTES
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

code >name                :: to-name                 ( xt -- a-addr )
        ld.w    %r4, [%r1]                           ; xt
        xld.w   %r5, DICTIONARY_CODE_TO_NAME_OFFSET_BYTES
        add     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

: >number                 :: to-number               ( ud1 c-addr1 u1 -- ud2 c-addr2 u2 )
  base @ >r 0 >r
  dup if
    over c@
    case
      [char] + of >number-skip endof
      [char] - of >number-skip r> drop 1 >r endof
      [char] % of >number-skip  2 base ! endof
      [char] & of >number-skip  8 base ! endof
      [char] # of >number-skip 10 base ! endof
      [char] $ of >number-skip 16 base ! endof
    endcase
    >number-digits
    r> r> base !
    if
      2>r dnegate 2r>
    then
  then
;

: >number-digits          :: to-number-digits        ( ud1 c-addr u -- ud2 c-addr+n u- )
    begin
      dup while
      over c@ digit? if  ( ud c-addr u digit36 )
        dup base @ > if drop exit then
        rot rot          ( ud digit c-addr u )
        2>r >r
        drop base @ um* r> s>d d+
        2r>              ( ud*base+digit c-addr u )
        >number-skip
      else
        exit
      then
    repeat
;

: >number-skip            :: to-number-skip          ( c-addr u -- c-addr+1 u-1 )
  1- swap char+ swap ;

code >r                   :: to-r                    ( x -- ) ( R: -- x )
        ld.w    %r4, [%r1]+
        sub     %sp, 1
        ld.w    [%sp], %r4
        NEXT
end-code

: ?                       :: question                ( a-addr -- )
  @ . ;

code ?branch              :: question-branch         ( -- )
        ld.w    %r4, [%r1]+
        or      %r4, %r4
        jrne    no_branch
        ld.w    %r0, [%r0]
        NEXT
no_branch:
        add     %r0, BYTES_PER_CELL
        NEXT
end-code

: ?do                     :: question-do             ( C: -- do-sys ) ( n1|u1 n2|u2 -- ) ( R: --  | loop-sys )
  postpone (?do) here 0 compile, here ; immediate compile-only

code ?dup                 :: question-dupe           ( x -- 0 | x x )
        ld.w    %r4, [%r1]
        or      %r4, %r4
        jreq    qdup_l1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
qdup_l1:
        NEXT
end-code

: ?stack                  :: question-stack          ( -- )
  depth 0< if -4 throw then \ underflow
;

code @                    :: fetch                   ( a-addr -- x )
        ld.w    %r4, [%r1]
        ld.w    %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
end-code

: abort                   :: abort                   ( i*x -- ) ( R: j*x -- )
  quit-reset
  quit ;

: abort"                  :: abort-quote             ( "ccc<quote>" -- )
  postpone if
  postpone cr
  postpone ."
  postpone abort
  postpone then ; immediate

: abs                     :: abs                     ( n -- u )
  dup 0 < if negate then ;

: accept                  :: accept                  ( c-addr +n1 -- +n2 )
  over + over        ( begin end+1 current )
  begin
    key dup
    [ctrl] m = over [ctrl] j = or 0=
  while
    ( begin end+1 current char )
    dup bl 127 within if
      ( begin end+1 current char )
      >r 2dup xor if
        r> dup emit
        over c!
        char+
      else
        r> drop
      then
    else
      dup [ctrl] h = over 127 = or if
        drop   ( begin end+1 current )
        2 pick over <> if
          char-
          [ctrl] h emit bl emit [ctrl] h emit
        then
      else
        drop
      then
    then
  repeat drop
  nip over -
  cr
;

: again                   :: again                   ( C: dest -- ) ( -- )
  postpone  branch compile, ; immediate compile-only

: ahead                   :: ahead                   ( C: -- orig ) ( -- )
  postpone branch here 0 compile, ; immediate compile-only

: align                   :: align                   ( -- )
  here aligned cp ! ;

code aligned              :: aligned                 ( addr -- a-addr )
        ld.w    %r4, [%r1]
        add     %r4, BYTES_PER_CELL - 1
        and     %r4, ~(BYTES_PER_CELL - 1)           ; must be power of 2
        ld.w    [%r1], %r4
        NEXT
end-code

\ allocate                :: allocate                ( u -- a-addr ior )

: allot                   :: allot                   ( n -- )
  here + cp ! ;

: also                    :: also                    ( -- )
  get-order ?dup if  over swap 1+ set-order then ;

code and                  :: and                     ( x1 x2 -- x3 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        and     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

\ assembler               :: assembler               ( -- )
\ at-xy                   :: at-x-y                  ( u1 u2 -- )

variable base             :: base                    ( -- a-addr )

: begin                   :: begin                   ( -- )
  here ; immediate compile-only

code bin                  :: bin                     ( fam1 -- fam2 )
        ld.w    %r6, [%r1]                           ; fam
        xcall   FileSystem_bin
        ld.w    [%r1], %r4                           ; fam2
        NEXT
end-code

: bl                      :: b-l                     ( -- char )
  32 ;

: blank                   :: blank                   ( c-addr u -- )
  bl fill ;

\ blk                     :: b-l-k                   ( -- a-addr )
\ block                   :: block                   ( u -- a-addr )

code branch               :: branch                  ( -- )
        ld.w    %r0, [%r0]
        NEXT
end-code

\ buffer                  :: buffer                  ( u -- a-addr )
\ bye                     :: bye                     ( -- )

code c!                   :: c-store                 ( char c-addr -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.b    [%r4], %r5
        NEXT
end-code

: c,                      :: c-comma                 ( char -- )
  here dup char+ cp ! c! ;

code c@                   :: c-fetch                 ( c-addr -- char )
        ld.w    %r4, [%r1]
        ld.ub   %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
end-code

: case                    :: case                    ( C: -- case-sys ) ( -- )
  0 ; immediate compile-only

: catch                   :: catch                   ( i*x xt -- j*x 0 | i*x n )
  sp@ >r  handler @ >r  rp@ handler !
  execute
  r> handler !  r> drop  0 ;

code cell+                :: cell-plus               ( a-addr1 -- a-addr2 )
        ld.w    %r4, [%r1]
        add     %r4, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code cell-                :: cell-minus              ( a-addr1 -- a-addr2 )
        ld.w    %r4, [%r1]
        sub     %r4, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code cells                :: cells                   ( n1 -- n2 )
        ld.w    %r4, [%r1]                           ; value
        sla     %r4, LOG2_BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: char                    :: char                    ( "<spaces>name" -- char )
  bl parse drop c@ ;

code char+                :: char-plus               ( c-addr1 -- c-addr2 )
        ld.w    %r4, [%r1]
        add     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code char-                :: char-minus              ( c-addr1 -- c-addr2 )
        ld.w    %r4, [%r1]
        sub     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

: chars                   :: chars                   ( n1 -- n2 )
  ;

code close-file           :: close-file              ( fileid -- ior )
        ld.w    %r6, [%r1]                           ; fileid
        xcall   FileSystem_close
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

code cmove                :: c-move                  ( c-addr1 c-addr2 u -- )
        ld.w    %r4, [%r1]+                          ; count
        ld.w    %r5, [%r1]+                          ; dst
        ld.w    %r6, [%r1]+                          ; src
        or      %r4, %r4
        jreq    cmove_done
cmove_loop:
        ld.ub   %r7, [%r6]+
        ld.b    [%r5]+, %r7
        xsub    %r4, 1
        jrne    cmove_loop
cmove_done:
        NEXT
end-code

code cmove>               :: c-move-up               ( c-addr1 c-addr2 u -- )
        ld.w    %r4, [%r1]+                          ; count
        ld.w    %r5, [%r1]+                          ; dst
        ld.w    %r6, [%r1]+                          ; src
        or      %r4, %r4
        jreq    cmove_up_done
        add     %r5, %r4
        add     %r6, %r4
cmove_up_loop:
        xsub    %r5, 1
        xsub    %r6, 1
        ld.ub   %r7, [%r6]
        ld.b    [%r5], %r7
        xsub    %r4, 1
        jrne    cmove_up_loop
cmove_up_done:
        NEXT
end-code

\ code                    :: code                    ( C: "<spaces>name" -- )

: cold                    :: cold                    ( i*x -- )
  0 >in !
  10 base !
  cold-cp0 cp !
  0 current !
  0 handler !
  pad hld !
  cold-rp0 rp0 !
  cold-sp0 sp0 !
  0 source-id !
  false state !
  0 terminal-count !
  cold-last-name forth-wordlist !
  rp0 @ rp!
  sp0 @ sp!

  only forth definitions

  cr  ." moko forth interpreter for S33C (build:"
  build-number 0 u.r
  ." )" cr
  quit-reset

  \ initial code to run
  s" forth.ini"  r/o open-file 0= if  \ ignore any errors
    include-file
  then

  quit
;

code cold-cp0             :: cold-c-p-zero           ( -- a-addr )
        xld.w   %r4, dictionary_end
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code cold-last-name       :: cold-last-name          ( -- a-addr )
        xld.w   %r4, last_name                       ; name of last word
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code cold-rp0             :: cold-r-p-zero           ( -- a-addr )
        xld.w   %r4, initial_return_pointer
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code cold-sp0             :: cold-s-p-zero           ( -- a-addr )
        xld.w   %r4, initial_stack_pointer
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code compare              :: compare                 ( c-addr1 u1 c-addr2 u2 -- n )
        ld.w    %r4, [%r1]+                          ; count 2
        ld.w    %r5, [%r1]+                          ; address 2
        ld.w    %r6, [%r1]+                          ; count 1
        ld.w    %r7, [%r1]                           ; address 1

compare_loop:
        or      %r4,%r4                              ; count 2 == 0?
        jrne    compare_l1                           ; ...no

        or      %r6,%r6                              ; count 1 == 0?
        jrne    compare_plus_one                     ; ...no

compare_zero:
        ld.w    %r4, 0
        ld.w    [%r1], %r4
        NEXT

compare_l1:
        or      %r6,%r6                              ; count 1 == 0?
        jreq    compare_minus_one                    ; ...yes

        ld.ub   %r8, [%r7]+                          ; byte 1
        ld.ub   %r9, [%r5]+                          ; byte 2
        cmp     %r8, %r9                             ; counts must be equal
        jrgt    compare_plus_one                     ; byte 1 > byte 2
        jrlt    compare_minus_one                    ; byte 1 < byte 2
        sub     %r4, 1
        sub     %r6, 1
        jp      compare_loop                         ; progess next

compare_plus_one:
        ld.w    %r4, 1
        ld.w    [%r1], %r4
        NEXT

compare_minus_one:
        ld.w    %r4, -1
        ld.w    [%r1], %r4
        NEXT
end-code

: compile,                :: compile-comma           ( xt -- )
  , ;

: compile-only            :: compile-only            ( -- )
  last-definition @
  dup 0= if
    -22 throw
  then
  >flags @ flag-compile-only or
  last-definition @ >flags ! ;

: constant                :: constant                ( C: x "<spaces>name" -- ) ( -- x )
  create ,
  ['] (const) @ last-definition @ ! ;

create context            :: context                 ( -- addr )
16 dup cells allot
constant #vocs            :: number-sign-vocs        ( -- u )
variable #order           :: number-sign-order       ( -- a-addr )

: count                   :: count                   ( c-addr1 -- c-addr2 u )
  dup cell+ swap @ ;

variable cp               :: cp                      ( -- addr )

: cr                      :: c-r                     ( -- )
  13 emit 10 emit ;

: create                  :: create                  ( "<spaces>name" -- )
  parse-word 2dup
  search-wordlists if drop cr ." duplicate definition of: " 2dup type then
  ( c-addr u )
  align                                              \ ensure cp is aligned
  here last-definition !                             \ the last definition cp for immediate etc.
  ['] (var) @ ,                                      \ code pointer
  0 ,                                                \ param pointer
  0 ,                                                \ flags
  0 ,                                                \ link
  dup ,                                              \ name length
  >r here r@  cmove                                  \ name string
  r> allot                                           \ (skip over name)
  align                                              \ endure aligned
  here last-definition @ >body !                     \ set the param pointer
  get-current @                                      \ previous name address
  last-definition @ >link !                          \ store in link
  last-definition @ >name                            \ current name address
  get-current !                                      \ current vocab points to this name
;

code create-file          :: create-file             ( c-addr u fam -- fileid ior )
        ld.w    %r8, [%r1]+                          ; fam
        ld.w    %r7, [%r1]                           ; count
        xld.w   %r6, [%r1 + BYTES_PER_CELL]          ; string
        xcall   FileSystem_create
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + BYTES_PER_CELL], %r4          ; fd
        NEXT
end-code

\ cs-pick                 :: c-s-pick                ( C: destu ... orig0|dest0 -- destu ... orig0|dest0 destu ) ( S: u -- )
\ cs-roll                 :: c-s-roll                ( C: origu|destu origu-1|destu-1 ... orig0|dest0 -- origu-1|destu-1 ... orig0|dest0 origu|destu )( S: u -- )

variable current          :: current                 ( -- addr )

code d+                   :: d-plus                  ( d1|ud1 d2|ud2 -- d3|ud3 )
        ld.w    %r4, [%r1]+                          ; d2-h
        ld.w    %r5, [%r1]+                          ; d2-l
        ld.w    %r6, [%r1]                           ; d1-h
        xld.w   %r7, [%r1 + BYTES_PER_CELL]          ; d2-l
        add     %r7, %r5
        adc     %r6, %r4
        ld.w    [%r1], %r6                           ; high
        xld.w   [%r1 + BYTES_PER_CELL], %r7          ; low
        NEXT
end-code

code d-                   :: d-minus                 ( d1|ud1 d2|ud2 -- d3|ud3 )
        ld.w    %r4, [%r1]+                          ; d2-h
        ld.w    %r5, [%r1]+                          ; d2-l
        ld.w    %r6, [%r1]                           ; d1-h
        xld.w   %r7, [%r1 + BYTES_PER_CELL]          ; d2-l
        sub     %r7, %r5
        sbc     %r6, %r4
        ld.w    [%r1], %r6                           ; high
        xld.w   [%r1 + BYTES_PER_CELL], %r7          ; low
        NEXT
end-code

: d.                      :: d-dot                   ( d -- )
  swap over dabs <# #s rot sign #> type space ;

: d.r                     :: d-dot-r                 ( d n -- )
  >r swap over dabs <# #s rot sign #> r> over - spaces type ;

\ d0<                     :: d-zero-less             ( d -- flag )
\ d0=                     :: d-zero-equals           ( xd -- flag )
\ d2*                     :: d-two-star              ( xd1 -- xd2 )
\ d2/                     :: d-two-slash             ( xd1 -- xd2 )
\ d<                      :: d-less-than             ( d -- flag )
\ d=                      :: d-equals                ( xd -- flag )
\ d>s                     :: d-to-s                  ( d -- n )

: dabs                    :: d-abs                   ( d -- ud )
  dup 0< if dnegate then ;

: decimal                 :: decimal                 ( -- )
  10 base ! ;

: definitions             :: definitions             ( -- )
  get-order  over set-current discard ;

code delete-file          :: delete-file             ( c-addr u -- ior )
        ld.w    %r7, [%r1]+                          ; count
        ld.w    %r6, [%r1]                           ; string
        xcall   FileSystem_delete
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

: depth                   :: depth                   ( -- +n )
  sp@ sp0 @ swap - 1 cells / ;

: digit?                  :: digit-question          ( c -- False | base36-digit True )
  dup [char] 0 < if drop false exit then
  dup [ char 9 1+ ] literal < if [char] 0 - true exit then
  dup [char] A < if drop false exit then
  dup [ char Z 1+ ] literal < if [char] A - 10 + true exit then
  dup [char] a < if drop false exit then
  dup [ char z 1+ ] literal < if [char] a - 10 + true exit then
  drop false ;

: discard                 :: discard                 ( x1 .. xu u -- )
  0 ?do drop loop ;

\ dmax                    :: d-max                   ( d1 d2 -- d3 )
\ dmin                    :: d-min                   ( d1 d2 -- d3 )

: dnegate                 :: d-negate                ( d -- d)
  invert >r invert 1 um+ r> + ;

: do                      :: do                      ( C: -- do-sys ) ( n1|u1 n2|u2 -- ) ( R: -- loop-sys )
  postpone (do) here 0 compile, here ; immediate compile-only

\ do-vocabulary           :: do-vocabulary           ( -- )
\    does>  @ >r           (  ) ( R: widnew )
\     get-order  swap drop ( wid1 ... widn-1 n )
\     r> swap set-order
\ ;

\ does>                   :: does                    ( C: colon-sys1 -- colon-sys2 ) ( -- ) ( R: nest-sys1 -- )

code drop                 :: drop                    ( x -- )
        ld.w    %r4, [%r1]+
        NEXT
end-code

\ du<                     :: d-u-less                ( ud1 ud2 -- flag )

: dump                    :: dump                    ( c-addr u -- )
  base @ >r hex 16 /
  0 ?do
    cr dup 4 u.r space
    dup
    2 0 do
      8 0 do
        dup c@ 3 u.r char+
      loop
      2 spaces
    loop
    drop 2 spaces
    2 0 do
      8 0 do
        dup c@
        dup 127 bl within if drop [char] . then
        emit char+
      loop
      2 spaces
    loop
  loop
  drop
  r> base !
;

code dup                  :: dupe                    ( x -- x x )
        ld.w    %r4, [%r1]
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

\ editor                  :: editor                  ( -- )
\ ekey                    :: e-key                   ( -- u )
\ ekey>char               :: e-key-to-char           ( u -- u false | char true )
\ ekey?                   :: e-key-question          ( -- flag )

: else                    :: else                    ( C: orig1 -- orig2 ) ( -- )
  postpone ahead swap postpone then ; immediate compile-only

code emit                 :: emit                    ( char -- )
        ld.w    %r6, [%r1]+
        xcall   Serial_PutChar
        NEXT
end-code

code emit?                :: emit-question           ( -- flag )
        xcall   Serial_PutReady
        or      %r4, %r4
        jreq    emit_question_no_space
        ld.w    %r4, TRUE
emit_question_no_space:
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

\ empty-buffers           :: empty-buffers           ( -- )

: endcase                 :: end-case                ( C: case-sys -- ) ( x -- )
  postpone drop
  begin
    ?dup
  while
    postpone then
  repeat
; immediate compile-only

: endof                   :: end-of                  ( C: case-sys1 of-sys -- case-sys2 ) ( -- )
  postpone else
; immediate compile-only

\ if key is pressed, wait for second key press
\ return true if the second key is enter
: enough?                 :: enough-question         ( -- flag )
  key? if key drop key 13 = else false then ;

\ environment?            :: environment-query       ( c-addr u -- false | i*x true )

: erase                   :: erase                   ( addr u -- )
  0 ?do dup 0 ! cell+ loop drop ;

: evaluate                :: evaluate                ( i*x c-addr u -- j*x )
  source-id @ >r
  -1 source-id !
  quit-evaluate
  r> source-id ! ;

code execute              :: execute                 ( i*x xt -- j*x )
        ld.w    %r2, [%r1]+                          ; point to code ptr
        ld.w    %r3, [%r2]+                          ; code / param address
        jp      %r3                                  ; execute the code
end-code

code exit                 :: exit                    ( -- ) ( R: nest-sys -- )
        popn    %r0                                  ; restore ip
        NEXT
end-code

0
constant false            :: false                   ( -- false )

code file-position        :: file-position           ( fileid -- ud ior )
        ld.w    %r6, [%r1]                           ; fileid
        xcall   FileSystem_ltell
        ld.w    [%r1], %r4                           ; pos
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

code file-size            :: file-size               ( fileid -- ud ior )
        ld.w    %r6, [%r1]                           ; fileid
        xcall   FileSystem_lsize
        ld.w    [%r1], %r4                           ; size
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

\ file-status             :: file-status             ( c-addr u -- x ior )


20 dup
create fileid-stack       :: fileid-stack            ( -- a-addr )
, 0 ,  cells allot

code filesystem-close-all :: filesystem-close-all    ( -- )
        xcall   FileSystem_CloseAll
        NEXT
end-code

code filesystem-init      :: filesystem-init         ( -- )
        xcall   FileSystem_initialise
        NEXT
end-code

: fill                    :: fill                    ( c-addr u char -- )
  rot rot \ char c-addr u
  0 ?do 2dup c! 1+ loop 2drop ;

\ find                    :: find                    ( c-addr -- c-addr 0  |  xt 1  |  xt -1 )

code flag-compile-only    :: flag-compile-only       ( -- u )
        ld.w    %r4, FLAG_COMPILE_ONLY
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code flag-immediate       :: flag-immediate          ( -- u )
        ld.w    %r4, FLAG_IMMEDIATE
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

\ flush                   :: flush                   ( -- )

code flush-file           :: flush-file              ( fileid -- ior )
        ld.w    %r6, [%r1]                           ; fileid
        xcall   FileSystem_sync
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

: fm/mod                  :: f-m-slash-mod           ( d1 n1 -- n2:r n3:q )
   dup 0<  dup >r
   if negate >r dnegate r>
   then >r dup 0< if r@ + then r> um/mod r>
   if swap negate swap then ;

\ forget                  :: forget                  ( "<spaces>name" -- )

: forth                   :: forth                   ( -- )
  get-order dup 0> if nip else 1+ then
  forth-wordlist swap ;

variable forth-wordlist   :: forth-wordlist          ( -- wid )

\ free                    :: free                    ( a-addr -- ior )

: get-current             :: get-current             ( -- wid )
  current @ ;

: get-order               :: get-order               ( -- widn ... wid1 n )
  #order @ 0 ?do
    #order @  i - 1- cells context + @
  loop
  #order @
;

code h!                   :: half-word-store         ( u h-addr -- )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        ld.h    [%r4], %r5
        NEXT
end-code

code h@                   :: half-word-fetch         ( h-addr -- u )
        ld.w    %r4, [%r1]
        ld.uh   %r4, [%r4]
        ld.w    [%r1], %r4
        NEXT
end-code

variable handler          :: handler                 ( -- a-addr )

: here                    :: here                    ( -- addr )
  cp @ ;

: hex                     :: hex                     ( -- )
  16 base ! ;

variable hld              :: hld                     ( -- addr )

: hold                    :: hold                    ( char -- )
  hld @ char- dup hld ! c! ;

code i                    :: i                       ( -- n|u ) ( R: loop-sys -- loop-sys )
        ld.w    %r4, [%sp]
        xsub    %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: if                      :: if                      ( C: -- orig ) ( x -- )
  postpone ?branch here 0 compile, ; immediate compile-only

: immediate               :: immediate               ( -- )
  last-definition @
  dup 0= if
    -22 throw
  then
  >flags @ flag-immediate or
  last-definition @ >flags ! ;

: include                 :: include                 ( <filename> -- )
  bl parse included ;

: include-file            :: include-file            ( i*x fileid -- j*x )
  source-id @ fileid-stack stack-push
  source-id !
;

: included                :: included                ( i*x c-addr u -- j*x )
  r/o open-file  \ fileid ior
  ?dup if    cr ." open error = " . drop
       else include-file
  then ;

code invert               :: invert                  ( x1 -- x2 )
        ld.w    %r4, [%r1]
        not     %r4, %r4
        ld.w    [%r1], %r4
        NEXT
end-code

code j                    :: j                       ( -- n|u ) ( R: loop-sys1 loop-sys2 -- loop-sys1 loop-sys2 )
        xld.w   %r4, [%sp + 3 * BYTES_PER_CELL]
        xsub    %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code key                  :: key                     ( -- char )
        xcall   Serial_GetChar
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code key?                 :: key-question            ( -- flag )
        xcall   Serial_InputAvailable
        or      %r4, %r4
        jreq    key_question_no_character
        ld.w    %r4, TRUE
key_question_no_character:
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

\ for create to store the last definitions xt
variable last-definition  :: last-definition         ( -- a-addr )

\ leave                   :: leave                   ( -- ) ( R: loop-sys -- )
\ list                    :: list                    ( u -- )

: literal                 :: literal                 ( C: x -- ) ( -- x )
  postpone (lit) compile, ; immediate compile-only

\ load                    :: load                    ( i*x u -- j*x )
\ locals|                 :: locals-bar              ( "<spaces>name1" "<spaces>name2" ... "<spaces>namen" | -- ) ( xn ... x2 x1 -- )

: loop                    :: loop                    ( C: do-sys -- ) ( -- ) ( R: loop-sys1 -- | loop-sys2 )
  postpone (loop) compile, here swap ! ; immediate compile-only


code lshift               :: l-shift                 ( x1 u -- x2 )
        ld.w    %r4, [%r1]+                          ; shift
        ld.w    %r5, [%r1]                           ; value
        sla     %r5, %r4
        ld.w    [%r1], %r5
        NEXT
end-code

code m*                   :: m-star                  ( n1 n2 -- d )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mlt.w   %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        ld.w    %r4, %ahr
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

\ m*/                     :: m-star-slash            ( d1 n1 +n2 -- d2 )

code m+                   :: m-plus                  ( d1|ud1 n -- d2|ud2 )
        ld.w    %r4, [%r1]+                          ; n
        ld.w    %r5, [%r1]                           ; d1.low
        xld.w   %r6, [%r1 + BYTES_PER_CELL]          ; d1.high
        ld.w    %r7, 0
        add     %r5, %r4
        adc     %r6, %r7
        xld.w   [%r1 + BYTES_PER_CELL], %r6          ; d1.high
        ld.w    [%r1], %r5                           ; d1.low
        NEXT
end-code

\ marker                  :: marker                  ( "<spaces>name" -- ) ( -- )

: max                     :: max                     ( n1 n2 -- n3 )
  2dup      < if swap then drop ;

: min                     :: min                     ( n1 n2 -- n3 )
  2dup swap < if swap then drop ;

: mod                     :: mod                     ( n1 n2 -- n3 )
  >r s>d r> fm/mod drop ;

\ move                    :: move                    ( addr1 addr2 u -- )

\ ms                      :: ms                      ( u -- )

code negate               :: negate                  ( n1 -- n2 )
        ld.w    %r4, [%r1]
        not     %r4, %r4
        add     %r4, 1
        ld.w    [%r1], %r4
        NEXT
end-code

code nip                  :: nip                     ( x1 x2 -- x2 )
        ld.w    %r4, [%r1]+                          ; x2
        ld.w    [%r1], %r4
        NEXT
end-code

: of                      :: of                      ( C: -- of-sys ) ( x1 x2 --   | x1 )
     postpone over
     postpone =
     postpone if
     postpone drop
; immediate compile-only


: only                    :: only                    ( -- )
  -1 set-order ;

code open-file            :: open-file               ( c-addr u fam -- fileid ior )
        ld.w    %r8, [%r1]+                          ; fam
        ld.w    %r7, [%r1]                           ; count
        xld.w   %r6, [%r1 + BYTES_PER_CELL]          ; string
        xcall   FileSystem_open
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + BYTES_PER_CELL], %r4          ; fd
        NEXT
end-code

code or                   :: or                      ( x1 x2 -- x3 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        or      %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

\ order                   :: order                   ( -- )
\  get-order 0 ?do
\    cr 2 spaces .vocab
\  loop
\  cr [char] * emit space get-current .vocab ;

code over                 :: over                    ( x1 x2 -- x1 x2 x1 )
        xld.w   %r4, [%r1 + BYTES_PER_CELL]
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: pad                     :: pad                     ( -- c-addr )
  here 80 + ;

\ page                    :: page                    ( -- )

: parse                   :: parse                   ( char "ccc<char>" -- c-addr u )
  >r
  >in @                     ( start )
  begin
    source                  ( start c-addr length )
    >in @                   ( start c-addr length index )
    > while                 ( start c-addr )
    >in @ + c@              ( start char )
    r@ = if                 ( start )
       >in @ over -         ( start index-start )
       swap source drop +   ( count start+c-addr )
       swap r> drop         ( c-addr u )
       1 >in +!             \ skip terminator char
       exit
    then
    1 >in +!
  repeat
  r> drop                   ( start c-addr )
  over +                    ( start c-addr+start )
  swap >in @ swap -         ( c-addr u )
;

: parse-word              :: parse-word              ( <spaces>name -- c-addr u )
  begin
    source                  ( c-addr length )
    >in @                   ( c-addr length index )
    > while                 ( c-addr )
    >in @ + c@              ( char )
    bl = if                 ( )
      1 >in +!
    else
      bl parse exit
    then
  repeat
  0                         ( c-addr 0 )
;

: pick                    :: pick                    ( xu ... x1 x0 u -- xu ... x1 x0 xu )
  1+ cells sp@ + @ ;

: postpone                :: postpone                ( "<spaces>name" -- )
  ' dup >flags @
  flag-immediate and if
    compile,
  else
    postpone literal
    postpone compile,
  then ; immediate compile-only

: previous                :: previous                ( -- )
  get-order ?dup if swap drop 1- set-order then ;

: quit                    :: quit                    ( -- )  ( R: i*x -- )
  rp0 @ rp!
  begin postpone [
    begin
      refill if
        ['] quit-evaluate catch ?dup
      else
        false
      then
    until ( a)
    ?dup
    if
      cr ." error " . cr
      cr source type cr
      >in @ 1-
      begin
        dup source drop over +     \ u u c-addr+u
        c@ bl = and                \ u flag
      while
        1-
      repeat
      dup
      begin
        dup source drop over +     \ u0 u u c-addr+u
        c@ bl <> and               \ u0 u flag
      while
        1-
      repeat
      source drop over + c@ bl = if 1+ then
      dup spaces - 1+
      0 ?do [char] ^ emit loop
    then
    quit-reset
   again ;

: quit-evaluate           :: quit-evaluate           ( i*x -- j*x )
  begin
    parse-word ?dup if
      2dup search-wordlists
      case
        0 of   \ number
          2>r 0 0 2r>
          >number if
            -13 throw   \ undefined-word
          then
          2drop
          state @ if
            postpone literal
          then
        endof
        1 of   \  immediate
          >r 2drop r>
          state @ 0= if
            dup >flags @ flag-compile-only and if
              -14 throw               \ interpreting compile-only word
            then
          then
          execute
        endof
       -1 of   \  normal
          >r 2drop r>
          state @ if
            compile,
          else
            execute
          then
        endof
      endcase
    else
      drop exit
    then
    ?stack
  again
;

: quit-reset              :: quit-reset              ( i*x -- )
  sp0 @ sp!
  false state !
  filesystem-close-all
  fileid-stack stack-clear
  0 source-id !
;

code r/o                  :: r-o                     ( -- fam )
        xcall   FileSystem_ReadOnly
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code r/w                  :: r-w                     ( -- fam )
        xcall   FileSystem_ReadWrite
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code r>                   :: r-from                  ( -- x ) ( R: x -- )
        ld.w    %r4, [%sp]
        add     %sp, 1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code r@                   :: r-fetch                 ( -- x ) ( R: x -- x )
        ld.w    %r4, [%sp]
        xsub    %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code read-file            :: read-file               ( c-addr u1 fileid -- u2 ior )
        ld.w    %r6, [%r1]+                          ; fileid
        ld.w    %r8, [%r1]                           ; count
        xld.w   %r7, [%r1 + BYTES_PER_CELL]          ; buffer
        xcall   FileSystem_read
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + BYTES_PER_CELL], %r4          ; count2
        NEXT
end-code

: read-line               :: read-line               ( c-addr u1 fileid -- u2 flag ior )
\ eof:                             0  t    0
      >r >r dup r> r> swap         \ b0 b fileid u
      0 ?do                        \ b0 b fileid
        begin
          2dup 1 swap read-file    \ b0 b fileid 0/1 ior
          ?dup if                  \ b0 b fileid 0/1 ior
            >r 2drop               \ b0 b
            swap -                 \ u2
            false r>               \ u2 false ior
            unloop exit
          then
                                   \ b0 b fileid 0/1
          0= if
            drop swap - dup 0      \ u2 flag 0
            unloop exit            \ u2 f 0
          then
                                   \ b0 b fileid
          over c@ [ctrl] m xor     \ b0 b fileid f
        until
        \ here have a non cr character
        over c@ [ctrl] j = if
          drop swap - true 0
          unloop  exit
        then
        >r 1+ r>                   \ b0 b+1 fileid
      loop
\ filled buffer without cr/lf      \ b0 b' fileid
      drop swap - true 0           \ u2 t 0
;

\ recurse                 :: recurse                 ( -- )

: refill                  :: refill                  ( -- flag )
  0 >in !
  source-id @ if
    terminal-buffer source-id @ read-line ( u flag ior )
    ?dup if
       cr ." read error = " . cr  \ b u2
       2drop
       source-id @ close-file drop
       abort" file error"
    then
    if
       terminal-count !
       true
\       cr source type   \ ***DEBUG***
    else
       drop
       source-id @ close-file drop
       fileid-stack stack-pop source-id !
       false
    then
  else \ console
    ."  Ok " cr
    terminal-buffer accept terminal-count ! drop true
  then
;

code rename-file          :: rename-file             ( c-addr1 u1 c-addr2 u2 -- ior )
        ld.w    %r9, [%r1]+                          ; count2
        ld.w    %r8, [%r1]+                          ; name2
        ld.w    %r7, [%r1]+                          ; count1
        ld.w    %r6, [%r1]                           ; name1
        xcall   FileSystem_rename
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

: repeat                  :: repeat                  ( C: orig dest -- ) ( -- )
  postpone again here swap ! ; immediate compile-only

code reposition-file      :: reposition-file         ( ud fileid -- ior )
        ld.w    %r6, [%r1]+                          ; fileid
        ld.w    %r7, [%r1]                           ; pos
        xcall   FileSystem_lseek
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

\ resize                  :: resize                  ( a-addr1 u -- a-addr2 ior )

\ resize-file             :: resize-file             ( ud fileid -- ior )

\ restore-input           :: restore-input           ( xn ... x1 n -- flag )

\ roll                    :: roll                    ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )

: rot                     :: rote                    ( x1 x2 x3 -- x2 x3 x1 )
  >r swap r> swap ;


code rp!                  :: r-p-store               ( addr -- )
        ld.w    %r4, [%r1]+
        ld.w    %sp, %r4
        NEXT
end-code
compile-only

variable rp0              :: r-p-zero                ( -- addr )

code rp@                  :: r-p-fetch               ( -- addr )
        ld.w    %r4, %sp
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

code rshift               :: r-shift                 ( x1 u -- x2 )
        ld.w    %r4, [%r1]+                          ; shift
        ld.w    %r5, [%r1]                           ; value
        srl     %r5, %r4
        ld.w    [%r1], %r5
        NEXT
end-code

: s"                      :: s-quote                 ( C: "ccc<quote>" -- ) ( -- c-addr u )
  postpone (s")
  [char] " parse     ( c-addr u)
  dup ,              \ save length
  2dup here          ( c-addr u c-addr u c-addr2 )
  swap cmove         ( c-addr u -- )
  allot drop
; immediate compile-only

: s'                      :: s-apostrophe            ( C: "ccc<quote>" -- ) ( -- c-addr u )
  postpone (s")
  [char] ' parse     ( c-addr u)
  dup ,              \ save length
  2dup here          ( c-addr u c-addr u c-addr2 )
  swap cmove         ( c-addr u -- )
  allot drop
; immediate compile-only

: s>d                     :: s-to-d                  ( n -- d )
  dup 0< if -1 else 0 then ;

\ save-buffers            :: save-buffers            ( -- )
\ save-input              :: save-input              ( -- xn ... x1 n )
\ scr                     :: s-c-r                   ( -- a-addr )

code search               :: search                  ( c-addr1 u1 c-addr2 u2 -- c-addr3 u3 flag )
        ld.w    %r4, [%r1]+                          ; u2
        ld.w    %r5, [%r1]+                          ; c-addr2
        ld.w    %r6, [%r1]                           ; u1
        xld.w   %r7, [%r1 + BYTES_PER_CELL]          ; c-addr1

search_loop:
        cmp     %r4, %r6                             ; u2 > u1
        jrugt   search_not_found                     ; ...yes

        ld.w    %r8, %r4                             ; count
        ld.w    %r9, %r7                             ; string
        ld.w    %r10, %r5                            ; match

search_compare:
        ld.ub   %r13, [%r9]+                         ; get byte of string
        ld.ub   %r14, [%r10]+                        ; get byte of match
        cmp     %r13, %r14
        jrne    search_next_position                 ; loop back if no match
        sub     %r8, 1
        jrne    search_compare                       ; loop to comare bytes

search_found:
        ld.w    [%r1], %r6                           ; u1
        xld.w   [%r1 + BYTES_PER_CELL], %r7          ; c-addr1
        sub     %r1, BYTES_PER_CELL
        ld.w    %r4, TRUE
        ld.w    [%r1], %r4
        NEXT

search_next_position:
        add     %r7, 1                               ; next address
        sub     %r6, 1                               ; decrement length
        jp      search_loop                          ; back for next compare

search_not_found:
        sub     %r1, BYTES_PER_CELL
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4
        NEXT                                         ; c-addr1 u1 false
end-code

code search-wordlist      :: search-wordlist         ( c-addr u wid -- 0 | xt 1 | xt -1 )
        ld.w    %r4, [%r1]+                          ; wid

find_loop:
        ld.w    %r4, [%r4]                           ; address of name
        or      %r4, %r4                             ; or zero if end of list
        jreq    find_not_found

        ;; comparison of counted strings is inlined for speed

        xld.w   %r5, [%r1]                           ; u = name length

        ld.w    %r7, %r4                             ; address of name in dictionary
        ld.w    %r8, [%r7]+                          ; dictionary name length

        cmp     %r5, %r8                             ; counts must be equal
        jrne    find_next                            ; ...no

        xld.w   %r6, [%r1 + BYTES_PER_CELL]          ; c-addr = name to find

find_cmp_loop:
        ld.ub   %r8,[%r6]+                           ; get 1 byte from string 1
        ld.ub   %r9,[%r7]+                           ; get 1 byte from string 2
        cmp     %r8, %r9                             ; check if equal
        jrne    find_next                            ; ..not equal => false result
        sub     %r5, 1                               ; decrement counter
        jrne    find_cmp_loop                        ; go back for more bytes

find_found:
        ld.w    %r5, %r4                             ; NAME>FLAGS
        xld.w   %r6, DICTIONARY_FLAGS_OFFSET_BYTES
        sub     %r5, %r6

        ld.w    %r6, 1                               ; immediate == 1
        ld.w    %r5, [%r5]
        and     %r5, FLAG_IMMEDIATE
        jrne    find_is_immediate

        not     %r6, %r6                             ; nomal == -1
        add     %r6, 1

find_is_immediate:
        ld.w    [%r1], %r6                           ; 1 (immediate) | -1 (normal)

        xld.w   %r5, DICTIONARY_CODE_OFFSET_BYTES
        sub     %r4, %r5                             ; NAME>CODE
        xld.w   [%r1 + BYTES_PER_CELL], %r4          ; ca
        NEXT

find_next:
        xld.w   %r5, DICTIONARY_LINK_OFFSET_BYTES
        jp.d    find_loop                            ; try next word (delayed)
        sub     %r4, %r5                             ; NAME>LINK

find_not_found:
        ld.w    %r4, [%r1]+                          ; drop u
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4                           ; FALSE
        NEXT
end-code

: search-wordlists        :: search-wordlists        ( c-addr u -- 0 | xt 1 | xt -1 )
  #order @ 0 ?do
    2dup                ( c-addr u c-addr u )
    i cells context + @ ( c-addr u c-addr u wid)
    search-wordlist     ( c-addr u; 0 | xt 1 | xt -1 )
    ?dup if             ( c-addr u; w 1 | w -1 )
      2swap 2drop unloop exit ( w 1 | w -1 )
    then                ( c-addr u )
  loop                  ( c-addr u )
  2drop 0
;

: see                     :: see                     ( "<spaces>name" -- )
  base @
  ' dup @ ['] (colon) @ <> if
    drop
    ." not a colon definition"
    base ! exit
  then
  dup >flags @ ." flags = " hex . cr
  >body @
  aligned
  begin
    dup  [char] $ emit hex 1 u.r [char] : emit space
    dup @ ?dup
  while
      dup >name count type
      dup  ['] (lit) =       ( xt flag )
      over ['] branch = or   ( xt flag )
      over ['] ?branch = or  ( xt flag )
      over ['] (do) = or     ( xt flag )
      over ['] (?do) = or    ( xt flag )
      over ['] (loop) = or   ( xt flag )
      over ['] (+loop) = or  ( xt flag )
      if
        drop cell+
        dup @ dup decimal u.
        space [char] / emit space
        [char] $ emit
        hex 1 u.r \ number
      else
        ['] (s") = if
          space
          cell+ dup count type
          count + aligned cell-
        then
      then
      cr cell+
   repeat drop base ! ;

: set-current             :: set-current             ( wid -- )
  current ! ;

: set-order               :: set-order               ( widn ... wid1 n -- )
  dup -1 = if
    drop
    \ root-wordlist dup 2  \ do not have this yet, so use forth instead
    forth-wordlist dup 2
  then
  \ **********************VALIDATE context size*************************
  dup #order !
  0 ?do  i cells context + ! loop
;

: sign                    :: sign                    ( n -- )
  0< if [char] - hold then ;

\ sliteral                :: sliteral                ( C: c-addr1 u -- ) ( -- c-addr1 u )
\ sm/rem                  :: s-m-slash-rem           ( d1 n1 -- n2 n3 )

: source                  :: source                  ( -- c-addr u )
  terminal-buffer drop terminal-count @ ;

variable source-id        :: source-i-d              ( -- 0 | -1 | fileid )

code sp!                  :: s-p-store               ( a-addr -- )
        ld.w    %r1, [%r1]
        NEXT
end-code

variable sp0              :: s-p-zero                ( -- a-addr )

code sp@                  :: s-p-fetch               ( -- a-addr )
        ld.w    %r4, %r1
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: space                   :: space                   ( -- )
  bl emit ;

: spaces                  :: spaces                  ( n -- )
  dup 0> if
    0 ?do space loop
  else
    drop
  then ;

\ usage example:
\ 25 ( stack-size-in-cells )
\ dup create my-stack , 0 , cells allot
\ stack = {size(N), ptr, value1, value2, ..., valueN}

: stack-clear             :: stack-clear             ( stack-addr -- )
  cell+ 0 swap ! ;

: stack-pop               :: stack-pop               ( stack-addr -- w )
  cell+ >r r@ @ 1- dup 0< abort" stack underflow"
  dup r@ ! 1+ cells r> + @
;

: stack-push              :: stack-push              ( w stack-addr -- )
  dup        \ w a a
  @          \ w a size
  swap cell+ \ w size ptr
  >r r@ @ 1+ \ w size index
  swap over  \ w index size index
  < abort" stack overflow"
             \ w index
  dup r@ !   \ w index
  cells r> + !
;

variable state            :: state                   ( -- a-addr )

code swap                 :: swap                    ( x1 x2 -- x2 x1 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]+
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5
        NEXT
end-code

code terminal-buffer      :: terminal-buffer         ( -- c-addr buffer-length )
        xld.w   %r4, terminal_buffer_start
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        xld.w   %r4, terminal_buffer_length
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

variable terminal-count   :: terminal-count          ( -- a-addr )

: then                    :: then                    ( C: orig -- ) ( -- )
  here swap ! ; immediate compile-only

: throw                   :: throw                   ( k*x n -- k*x | i*x n )
  handler @ rp!  r> handler !  r> swap >r sp! drop r> ;

\ thru                    :: thru                    ( i*x u1 u2 -- j*x )
\ time&date               :: time-and-date           ( -- +n1 +n2 +n3 +n4 +n5 +n6 )
\ to                      :: to                      ( I: x "<spaces>name" -- )  ( C: "<spaces>name" -- ) ( x -- )

-1
constant true             :: true                    ( -- true )

: tuck                    :: tuck                    ( x1 x2 -- x2 x1 x2 )
  swap over ;

: type                    :: type                    ( c-addr u -- )
  0 ?do dup c@ emit char+ loop drop ;

: u.                      :: u-dot                   ( u -- )
  0 <# #s #> type space ;

: u.r                     :: u-dot-r                 ( u n -- )
  >r 0 <# #s #> r> over - spaces type ;

code u<                   :: u-less-than             ( u1 u2 -- flag )
        ld.w    %r5, [%r1]+                          ; u2
        ld.w    %r4, [%r1]                           ; u1
        cmp     %r4, %r5                             ; u1 < u2 ?
        jrult   set_true_flag_u                      ; ...yes
        jp      set_false_flag_u                     ; ...no
end-code

code u>                   :: u-greater-than          ( u1 u2 -- flag )
        ld.w    %r5, [%r1]+                          ; u2
        ld.w    %r4, [%r1]                           ; u1
        cmp     %r4, %r5                             ; u1 > u2 ?
        jrugt   set_true_flag_u                      ; ...yes
set_false_flag_u:
        ld.w    %r4, FALSE
        ld.w    [%r1], %r4
        NEXT
set_true_flag_u:
        ld.w    %r4, TRUE
        ld.w    [%r1], %r4
        NEXT
end-code

code um*                  :: u-m-star                ( u1 u2 -- ud )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        mltu.w  %r4, %r5
        ld.w    %r4, %alr
        ld.w    [%r1], %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    %r4, %ahr
        ld.w    [%r1], %r4
        NEXT
end-code

code um+                  :: u-m-plus                ( u1 u2 -- ud )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        add     %r4, %r5
        ld.w    [%r1], %r4
        ld.w    %r4, 0
        adc     %r4, %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: um/mod                  :: u-m-slash-mod           ( ud u1 -- u2 u3 )
   2dup u<
   if negate  32 0
     ?do
       >r
       dup  um+  >r >r dup  um+  r> + dup r>
       r@ swap >r  um+  r> or
       if >r drop 1+ r> else drop then
       r>
     loop drop swap exit
   then drop 2drop  -1 dup ;

code unloop               :: unloop                  ( -- ) ( R: loop-sys -- )
        add     %sp, 2                               ; drop 2 stack words
        NEXT
end-code

: until                   :: until                   ( C: dest -- ) ( x -- )
  postpone ?branch compile, ; immediate compile-only

\ unused                  :: unused                  ( -- u )
\ update                  :: update                  ( -- )

\ value                   :: value                   ( x "<spaces>name" -- ) ( -- x )

: variable                :: variable                ( "<spaces>name" -- ) ( -- a-addr )
  create 0 , ;

\ vocabulary              :: vocabulary              ( name -- )
\  wordlist create ,  do-vocabulary ;

code w/o                  :: w-o                     ( -- fam )
        xcall   FileSystem_WriteOnly
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code

: while                   :: while                   ( C: dest -- orig dest ) ( x -- )
   postpone if swap ; immediate compile-only

: within                  :: within                  ( n1|u1 n2|u2 n3|u3 -- flag )
  over - >r - r> u< ;

\ word                    :: word                    ( char "<chars>ccc<char>" -- c-addr )
\ wordlist                :: wordlist                ( -- wid )

: words                   :: words                   ( -- )
  cr  context @
  begin @ ?dup
  while dup space count type >code >link  enough?
  until drop then ;

code write-file           :: write-file              ( c-addr u fileid -- ior )
        ld.w    %r6, [%r1]+                          ; fileid
        ld.w    %r8, [%r1]                           ; count
        xld.w   %r7, [%r1 + BYTES_PER_CELL]          ; buffer
        xcall   FileSystem_write
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + BYTES_PER_CELL], %r4          ; count2
        NEXT
end-code

\ write-line              :: write-line              ( c-addr u fileid -- ior )

code xor                  :: x-or                    ( x1 x2 -- x3 )
        ld.w    %r4, [%r1]+
        ld.w    %r5, [%r1]
        xor     %r4, %r5
        ld.w    [%r1], %r4
        NEXT
end-code

: [                       :: left-bracket            ( -- )
  false state ! ; immediate

: [']                     :: bracket-tick            ( "<spaces>name" -- ) ( -- xt )
  ' postpone literal ; immediate compile-only

: [char]                  :: bracket-char            ( "<spaces>name" -- ) ( -- char )
  char postpone literal ; immediate compile-only

: [compile]               :: bracket-compile         ( "<spaces>name" -- )
  -30 throw ;

: [ctrl] char and postpone literal ; immediate compile-only

\ [else]                  :: bracket-else            ( "<spaces>name" ... -- )
\ [if]                    :: bracket-if              ( flag | flag "<spaces>name" ... -- )
\ [then]                  :: bracket-then            ( -- )

: \                       :: backslash               ( "ccc<eol>"-- )
  refill drop ; immediate

: ]                       :: right-bracket           ( -- )
  true state ! ;


\ end of ANSI forth + some extra items to make it work
\ ====================================================


\ Directory access functions
\ ==========================

code open-directory       :: open-directory          ( b u -- dirid ior )
        ld.w    %r7, [%r1]                           ; count
        xld.w   %r6, [%r1 + 4]                       ; buffer
        xcall   FileSystem_OpenDirectory
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + 4], %r4                       ; count2
        NEXT
end-code

code close-directory      :: close-directory         ( dirid -- ior )
        ld.w    %r6, [%r1]                           ; dirid
        xcall   FileSystem_CloseDirectory
        ld.w    [%r1], %r5                           ; ior
        next
end-code

code read-directory       :: read-directory          ( b u dirid -- u2 ior )
        ld.w    %r6, [%r1]+                          ; dirid
        ld.w    %r8, [%r1]                           ; count
        xld.w   %r7, [%r1 + 4]                       ; buffer
        xcall   FileSystem_ReadDirectory
        ld.w    [%r1], %r5                           ; ior
        xld.w   [%r1 + 4], %r4                       ; count2
        NEXT
end-code


\ Access to absolute sectors on disk
\ ==================================

\ buffer size = count * 512 bytes
code read-sectors         :: read-sectors            ( b count sector -- ior )
        ld.w    %r6, [%r1]+                          ; sector
        ld.w    %r8, [%r1]+                          ; count
        xld.w   %r7, [%r1]                           ; buffer
        xcall   FileSystem_AbsoluteRead
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code

\ buffer size = count * 512 bytes
code write-sectors        :: write-sectors           ( b count sector -- ior )
        ld.w    %r6, [%r1]+                          ; sector
        ld.w    %r8, [%r1]+                          ; count
        xld.w   %r7, [%r1]                           ; buffer
        xcall   FileSystem_AbsoluteWrite
        ld.w    [%r1], %r5                           ; ior
        NEXT
end-code


\ convenience
\ ===========

: dec.                    :: dec-dot                 ( n -- )
  base @ decimal swap . base ! ;

: hex.                    :: hex-dot                 ( n -- )
  base @ hex swap u. base ! ;


\ peripheral port access
\ ======================

\ fetch a peripheral register value
: p@                      :: p-fetch                 ( reg-addr -- value )
  2@ \ address size
  case
    32 of  @ endof
    16 of h@ endof
     8 of c@ endof
  endcase
;

\ display peripheral register
: p?                      :: p-question              ( reg-addr -- )
  p@ . ;

\ store a value to a peripheral register
: p!                      :: p-store                 ( value reg-addr -- )
  2@ \ address size
  case
    32 of  ! endof
    16 of h! endof
     8 of c! endof
  endcase
;


\ font
\ ===

hex
create font-8x13          :: font-8x13               ( -- c-addr )
( 0000 ) 00 c, 00 c, AA c, 00 c, 82 c, 00 c, 82 c, 00 c, 82 c, 00 c, AA c, 00 c, 00 c,
( 0001 ) 00 c, 00 c, 00 c, 10 c, 38 c, 7C c, FE c, 7C c, 38 c, 10 c, 00 c, 00 c, 00 c,
( 0002 ) AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c,
( 0003 ) 00 c, 00 c, A0 c, A0 c, E0 c, A0 c, AE c, 04 c, 04 c, 04 c, 04 c, 00 c, 00 c,
( 0004 ) 00 c, 00 c, E0 c, 80 c, C0 c, 80 c, 8E c, 08 c, 0C c, 08 c, 08 c, 00 c, 00 c,
( 0005 ) 00 c, 00 c, 60 c, 80 c, 80 c, 80 c, 6C c, 0A c, 0C c, 0A c, 0A c, 00 c, 00 c,
( 0006 ) 00 c, 00 c, 80 c, 80 c, 80 c, 80 c, EE c, 08 c, 0C c, 08 c, 08 c, 00 c, 00 c,
( 0007 ) 00 c, 00 c, 18 c, 24 c, 24 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0008 ) 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 7C c, 00 c, 00 c, 00 c,
( 0009 ) 00 c, 00 c, C0 c, A0 c, A0 c, A0 c, A8 c, 08 c, 08 c, 08 c, 0E c, 00 c, 00 c,
( 000A ) 00 c, 00 c, 88 c, 88 c, 50 c, 50 c, 2E c, 04 c, 04 c, 04 c, 04 c, 00 c, 00 c,
( 000B ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, F0 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 000C ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, F0 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 000D ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 1F c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 000E ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 1F c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 000F ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, FF c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0010 ) FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0011 ) 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0012 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0013 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c,
( 0014 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c,
( 0015 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 1F c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0016 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, F0 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0017 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0018 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0019 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 001A ) 00 c, 00 c, 00 c, 00 c, 0E c, 30 c, C0 c, 30 c, 0E c, 00 c, FE c, 00 c, 00 c,
( 001B ) 00 c, 00 c, 00 c, 00 c, E0 c, 18 c, 06 c, 18 c, E0 c, 00 c, FE c, 00 c, 00 c,
( 001C ) 00 c, 00 c, 00 c, 00 c, 00 c, FE c, 44 c, 44 c, 44 c, 44 c, 44 c, 00 c, 00 c,
( 001D ) 00 c, 00 c, 00 c, 04 c, 04 c, 7E c, 08 c, 10 c, 7E c, 20 c, 20 c, 00 c, 00 c,
( 001E ) 00 c, 00 c, 1C c, 22 c, 20 c, 70 c, 20 c, 20 c, 20 c, 62 c, DC c, 00 c, 00 c,
( 001F ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0020 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0021 ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 10 c, 00 c, 00 c,
( 0022 ) 00 c, 00 c, 24 c, 24 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0023 ) 00 c, 00 c, 00 c, 24 c, 24 c, 7E c, 24 c, 7E c, 24 c, 24 c, 00 c, 00 c, 00 c,
( 0024 ) 00 c, 00 c, 10 c, 3C c, 50 c, 50 c, 38 c, 14 c, 14 c, 78 c, 10 c, 00 c, 00 c,
( 0025 ) 00 c, 00 c, 22 c, 52 c, 24 c, 08 c, 08 c, 10 c, 24 c, 2A c, 44 c, 00 c, 00 c,
( 0026 ) 00 c, 00 c, 00 c, 00 c, 30 c, 48 c, 48 c, 30 c, 4A c, 44 c, 3A c, 00 c, 00 c,
( 0027 ) 00 c, 00 c, 10 c, 10 c, 10 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0028 ) 00 c, 00 c, 04 c, 08 c, 08 c, 10 c, 10 c, 10 c, 08 c, 08 c, 04 c, 00 c, 00 c,
( 0029 ) 00 c, 00 c, 20 c, 10 c, 10 c, 08 c, 08 c, 08 c, 10 c, 10 c, 20 c, 00 c, 00 c,
( 002A ) 00 c, 00 c, 24 c, 18 c, 7E c, 18 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 002B ) 00 c, 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 00 c, 00 c, 00 c,
( 002C ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 38 c, 30 c, 40 c, 00 c,
( 002D ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 7C c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 002E ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c,
( 002F ) 00 c, 00 c, 02 c, 02 c, 04 c, 08 c, 10 c, 20 c, 40 c, 80 c, 80 c, 00 c, 00 c,
( 0030 ) 00 c, 00 c, 18 c, 24 c, 42 c, 42 c, 42 c, 42 c, 42 c, 24 c, 18 c, 00 c, 00 c,
( 0031 ) 00 c, 00 c, 10 c, 30 c, 50 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 0032 ) 00 c, 00 c, 3C c, 42 c, 42 c, 02 c, 04 c, 18 c, 20 c, 40 c, 7E c, 00 c, 00 c,
( 0033 ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 1C c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0034 ) 00 c, 00 c, 04 c, 0C c, 14 c, 24 c, 44 c, 44 c, 7E c, 04 c, 04 c, 00 c, 00 c,
( 0035 ) 00 c, 00 c, 7E c, 40 c, 40 c, 5C c, 62 c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0036 ) 00 c, 00 c, 1C c, 20 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0037 ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 08 c, 10 c, 10 c, 20 c, 20 c, 00 c, 00 c,
( 0038 ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 3C c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0039 ) 00 c, 00 c, 3C c, 42 c, 42 c, 46 c, 3A c, 02 c, 02 c, 04 c, 38 c, 00 c, 00 c,
( 003A ) 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c,
( 003B ) 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c, 00 c, 38 c, 30 c, 40 c, 00 c,
( 003C ) 00 c, 00 c, 02 c, 04 c, 08 c, 10 c, 20 c, 10 c, 08 c, 04 c, 02 c, 00 c, 00 c,
( 003D ) 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 00 c, 00 c, 7E c, 00 c, 00 c, 00 c, 00 c,
( 003E ) 00 c, 00 c, 40 c, 20 c, 10 c, 08 c, 04 c, 08 c, 10 c, 20 c, 40 c, 00 c, 00 c,
( 003F ) 00 c, 00 c, 3C c, 42 c, 42 c, 02 c, 04 c, 08 c, 08 c, 00 c, 08 c, 00 c, 00 c,
( 0040 ) 00 c, 00 c, 3C c, 42 c, 42 c, 4E c, 52 c, 56 c, 4A c, 40 c, 3C c, 00 c, 00 c,
( 0041 ) 00 c, 00 c, 18 c, 24 c, 42 c, 42 c, 42 c, 7E c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0042 ) 00 c, 00 c, 78 c, 44 c, 42 c, 44 c, 78 c, 44 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 0043 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 40 c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0044 ) 00 c, 00 c, 78 c, 44 c, 42 c, 42 c, 42 c, 42 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 0045 ) 00 c, 00 c, 7E c, 40 c, 40 c, 40 c, 78 c, 40 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 0046 ) 00 c, 00 c, 7E c, 40 c, 40 c, 40 c, 78 c, 40 c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 0047 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 4E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0048 ) 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 7E c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0049 ) 00 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 004A ) 00 c, 00 c, 1F c, 04 c, 04 c, 04 c, 04 c, 04 c, 04 c, 44 c, 38 c, 00 c, 00 c,
( 004B ) 00 c, 00 c, 42 c, 44 c, 48 c, 50 c, 60 c, 50 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 004C ) 00 c, 00 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 004D ) 00 c, 00 c, 82 c, 82 c, C6 c, AA c, 92 c, 92 c, 82 c, 82 c, 82 c, 00 c, 00 c,
( 004E ) 00 c, 00 c, 42 c, 42 c, 62 c, 52 c, 4A c, 46 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 004F ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0050 ) 00 c, 00 c, 7C c, 42 c, 42 c, 42 c, 7C c, 40 c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 0051 ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 42 c, 52 c, 4A c, 3C c, 02 c, 00 c,
( 0052 ) 00 c, 00 c, 7C c, 42 c, 42 c, 42 c, 7C c, 50 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 0053 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 3C c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0054 ) 00 c, 00 c, FE c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 0055 ) 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0056 ) 00 c, 00 c, 82 c, 82 c, 44 c, 44 c, 44 c, 28 c, 28 c, 28 c, 10 c, 00 c, 00 c,
( 0057 ) 00 c, 00 c, 82 c, 82 c, 82 c, 82 c, 92 c, 92 c, 92 c, AA c, 44 c, 00 c, 00 c,
( 0058 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 10 c, 28 c, 44 c, 82 c, 82 c, 00 c, 00 c,
( 0059 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 005A ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 10 c, 20 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 005B ) 00 c, 00 c, 3C c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 3C c, 00 c, 00 c,
( 005C ) 00 c, 00 c, 80 c, 80 c, 40 c, 20 c, 10 c, 08 c, 04 c, 02 c, 02 c, 00 c, 00 c,
( 005D ) 00 c, 00 c, 78 c, 08 c, 08 c, 08 c, 08 c, 08 c, 08 c, 08 c, 78 c, 00 c, 00 c,
( 005E ) 00 c, 00 c, 10 c, 28 c, 44 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 005F ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FE c, 00 c,
( 0060 ) 00 c, 10 c, 08 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0061 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0062 ) 00 c, 00 c, 40 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 62 c, 5C c, 00 c, 00 c,
( 0063 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0064 ) 00 c, 00 c, 02 c, 02 c, 02 c, 3A c, 46 c, 42 c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0065 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0066 ) 00 c, 00 c, 1C c, 22 c, 20 c, 20 c, 7C c, 20 c, 20 c, 20 c, 20 c, 00 c, 00 c,
( 0067 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3A c, 44 c, 44 c, 38 c, 40 c, 3C c, 42 c, 3C c,
( 0068 ) 00 c, 00 c, 40 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0069 ) 00 c, 00 c, 00 c, 10 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 006A ) 00 c, 00 c, 00 c, 04 c, 00 c, 0C c, 04 c, 04 c, 04 c, 04 c, 44 c, 44 c, 38 c,
( 006B ) 00 c, 00 c, 40 c, 40 c, 40 c, 44 c, 48 c, 70 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 006C ) 00 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 006D ) 00 c, 00 c, 00 c, 00 c, 00 c, EC c, 92 c, 92 c, 92 c, 92 c, 82 c, 00 c, 00 c,
( 006E ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 006F ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0070 ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 62 c, 42 c, 62 c, 5C c, 40 c, 40 c, 40 c,
( 0071 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3A c, 46 c, 42 c, 46 c, 3A c, 02 c, 02 c, 02 c,
( 0072 ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 22 c, 20 c, 20 c, 20 c, 20 c, 00 c, 00 c,
( 0073 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 30 c, 0C c, 42 c, 3C c, 00 c, 00 c,
( 0074 ) 00 c, 00 c, 00 c, 20 c, 20 c, 7C c, 20 c, 20 c, 20 c, 22 c, 1C c, 00 c, 00 c,
( 0075 ) 00 c, 00 c, 00 c, 00 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 0076 ) 00 c, 00 c, 00 c, 00 c, 00 c, 44 c, 44 c, 44 c, 28 c, 28 c, 10 c, 00 c, 00 c,
( 0077 ) 00 c, 00 c, 00 c, 00 c, 00 c, 82 c, 82 c, 92 c, 92 c, AA c, 44 c, 00 c, 00 c,
( 0078 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 24 c, 18 c, 18 c, 24 c, 42 c, 00 c, 00 c,
( 0079 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
( 007A ) 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 04 c, 08 c, 10 c, 20 c, 7E c, 00 c, 00 c,
( 007B ) 00 c, 00 c, 0E c, 10 c, 10 c, 08 c, 30 c, 08 c, 10 c, 10 c, 0E c, 00 c, 00 c,
( 007C ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 007D ) 00 c, 00 c, 70 c, 08 c, 08 c, 10 c, 0C c, 10 c, 08 c, 08 c, 70 c, 00 c, 00 c,
( 007E ) 00 c, 00 c, 24 c, 54 c, 48 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A0 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A1 ) 00 c, 00 c, 10 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00A2 ) 00 c, 00 c, 10 c, 38 c, 54 c, 50 c, 50 c, 54 c, 38 c, 10 c, 00 c, 00 c, 00 c,
( 00A3 ) 00 c, 00 c, 1C c, 22 c, 20 c, 70 c, 20 c, 20 c, 20 c, 62 c, DC c, 00 c, 00 c,
( 00A4 ) 00 c, 00 c, 00 c, 00 c, 42 c, 3C c, 24 c, 24 c, 3C c, 42 c, 00 c, 00 c, 00 c,
( 00A5 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 7C c, 10 c, 7C c, 10 c, 10 c, 00 c, 00 c,
( 00A6 ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 00 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00A7 ) 00 c, 18 c, 24 c, 20 c, 18 c, 24 c, 24 c, 18 c, 04 c, 24 c, 18 c, 00 c, 00 c,
( 00A8 ) 00 c, 24 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A9 ) 00 c, 38 c, 44 c, 92 c, AA c, A2 c, AA c, 92 c, 44 c, 38 c, 00 c, 00 c, 00 c,
( 00AA ) 00 c, 00 c, 38 c, 04 c, 3C c, 44 c, 3C c, 00 c, 7C c, 00 c, 00 c, 00 c, 00 c,
( 00AB ) 00 c, 00 c, 00 c, 12 c, 24 c, 48 c, 90 c, 48 c, 24 c, 12 c, 00 c, 00 c, 00 c,
( 00AC ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 02 c, 02 c, 02 c, 00 c, 00 c, 00 c,
( 00AD ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00AE ) 00 c, 38 c, 44 c, 92 c, AA c, AA c, B2 c, AA c, 44 c, 38 c, 00 c, 00 c, 00 c,
( 00AF ) 00 c, 00 c, 7E c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B0 ) 00 c, 00 c, 18 c, 24 c, 24 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B1 ) 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 7C c, 00 c, 00 c, 00 c,
( 00B2 ) 00 c, 30 c, 48 c, 08 c, 30 c, 40 c, 78 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B3 ) 00 c, 30 c, 48 c, 10 c, 08 c, 48 c, 30 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B4 ) 00 c, 08 c, 10 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B5 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 66 c, 5A c, 40 c, 00 c,
( 00B6 ) 00 c, 00 c, 3E c, 74 c, 74 c, 74 c, 34 c, 14 c, 14 c, 14 c, 14 c, 00 c, 00 c,
( 00B7 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B8 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 08 c, 18 c,
( 00B9 ) 00 c, 20 c, 60 c, 20 c, 20 c, 20 c, 70 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00BA ) 00 c, 00 c, 30 c, 48 c, 48 c, 30 c, 00 c, 78 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00BB ) 00 c, 00 c, 00 c, 90 c, 48 c, 24 c, 12 c, 24 c, 48 c, 90 c, 00 c, 00 c, 00 c,
( 00BC ) 00 c, 40 c, C0 c, 40 c, 40 c, 42 c, E6 c, 0A c, 12 c, 1A c, 06 c, 00 c, 00 c,
( 00BD ) 00 c, 40 c, C0 c, 40 c, 40 c, 4C c, F2 c, 02 c, 0C c, 10 c, 1E c, 00 c, 00 c,
( 00BE ) 00 c, 60 c, 90 c, 20 c, 10 c, 92 c, 66 c, 0A c, 12 c, 1A c, 06 c, 00 c, 00 c,
( 00BF ) 00 c, 00 c, 10 c, 00 c, 10 c, 10 c, 20 c, 40 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00C0 ) 00 c, 10 c, 08 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C1 ) 00 c, 08 c, 10 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C2 ) 00 c, 18 c, 24 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C3 ) 00 c, 32 c, 4C c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C4 ) 00 c, 24 c, 24 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C5 ) 00 c, 18 c, 24 c, 18 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C6 ) 00 c, 00 c, 6E c, 90 c, 90 c, 90 c, 9C c, F0 c, 90 c, 90 c, 9E c, 00 c, 00 c,
( 00C7 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 40 c, 40 c, 42 c, 3C c, 08 c, 10 c,
( 00C8 ) 00 c, 10 c, 08 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00C9 ) 00 c, 08 c, 10 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CA ) 00 c, 18 c, 24 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CB ) 00 c, 24 c, 24 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CC ) 00 c, 20 c, 10 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CD ) 00 c, 08 c, 10 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CE ) 00 c, 18 c, 24 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CF ) 00 c, 44 c, 44 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00D0 ) 00 c, 00 c, 78 c, 44 c, 42 c, 42 c, E2 c, 42 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 00D1 ) 00 c, 64 c, 98 c, 00 c, 82 c, C2 c, A2 c, 92 c, 8A c, 86 c, 82 c, 00 c, 00 c,
( 00D2 ) 00 c, 20 c, 10 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D3 ) 00 c, 08 c, 10 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D4 ) 00 c, 18 c, 24 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D5 ) 00 c, 64 c, 98 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D6 ) 00 c, 44 c, 44 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D7 ) 00 c, 00 c, 00 c, 00 c, 42 c, 24 c, 18 c, 18 c, 24 c, 42 c, 00 c, 00 c, 00 c,
( 00D8 ) 00 c, 02 c, 3C c, 46 c, 4A c, 4A c, 52 c, 52 c, 52 c, 62 c, 3C c, 40 c, 00 c,
( 00D9 ) 00 c, 20 c, 10 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DA ) 00 c, 08 c, 10 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DB ) 00 c, 18 c, 24 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DC ) 00 c, 24 c, 24 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DD ) 00 c, 08 c, 10 c, 00 c, 44 c, 44 c, 28 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00DE ) 00 c, 00 c, 40 c, 7C c, 42 c, 42 c, 42 c, 7C c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 00DF ) 00 c, 00 c, 38 c, 44 c, 44 c, 48 c, 50 c, 4C c, 42 c, 42 c, 5C c, 00 c, 00 c,
( 00E0 ) 00 c, 00 c, 10 c, 08 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E1 ) 00 c, 00 c, 04 c, 08 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E2 ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E3 ) 00 c, 00 c, 32 c, 4C c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E4 ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E5 ) 00 c, 18 c, 24 c, 18 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E6 ) 00 c, 00 c, 00 c, 00 c, 00 c, 6C c, 12 c, 7C c, 90 c, 92 c, 6C c, 00 c, 00 c,
( 00E7 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 42 c, 3C c, 08 c, 10 c,
( 00E8 ) 00 c, 00 c, 10 c, 08 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00E9 ) 00 c, 00 c, 08 c, 10 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EA ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EB ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EC ) 00 c, 00 c, 20 c, 10 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00ED ) 00 c, 00 c, 10 c, 20 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00EE ) 00 c, 00 c, 30 c, 48 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00EF ) 00 c, 00 c, 48 c, 48 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00F0 ) 00 c, 24 c, 18 c, 28 c, 04 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F1 ) 00 c, 00 c, 32 c, 4C c, 00 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 00F2 ) 00 c, 00 c, 20 c, 10 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F3 ) 00 c, 00 c, 08 c, 10 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F4 ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F5 ) 00 c, 00 c, 32 c, 4C c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F6 ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F7 ) 00 c, 00 c, 00 c, 10 c, 10 c, 00 c, 7C c, 00 c, 10 c, 10 c, 00 c, 00 c, 00 c,
( 00F8 ) 00 c, 00 c, 00 c, 00 c, 02 c, 3C c, 46 c, 4A c, 52 c, 62 c, 3C c, 40 c, 00 c,
( 00F9 ) 00 c, 00 c, 20 c, 10 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FA ) 00 c, 00 c, 08 c, 10 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FB ) 00 c, 00 c, 18 c, 24 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FC ) 00 c, 00 c, 28 c, 28 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FD ) 00 c, 00 c, 08 c, 10 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
( 00FE ) 00 c, 00 c, 00 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 62 c, 5C c, 40 c, 40 c,
( 00FF ) 00 c, 00 c, 24 c, 24 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
decimal

8
constant font-width       :: font-width              ( -- u )
13
constant font-height      :: font-height             ( -- u )


\ LCD driver
\ ==========

240
constant lcd-width-pixels :: lcd-width-pixels        ( -- u )

208
constant lcd-height-lines :: lcd-height-lines        ( -- u )

hex 80000 decimal
constant lcd-vram         :: lcd-vram                ( -- u )

lcd-width-pixels 31 + 32 / 32 *
constant lcd-vram-width-pixels :: lcd-vram-width-pixels  ( -- u )

lcd-height-lines
constant lcd-vram-height-lines :: lcd-vram-height-lines  ( -- u )

lcd-vram-width-pixels 8 /
constant lcd-vram-width-bytes  :: lcd-vram-width-bytes   ( -- u )

lcd-width-pixels 8 /
constant lcd-width-bytes  :: lcd-width-bytes         ( -- u )

lcd-vram-width-bytes lcd-vram-height-lines *
constant lcd-vram-size    :: lcd-vram-size           ( -- u )

: lcd-clear               :: lcd-clear               ( -- )
  lcd-vram lcd-vram-size 0 fill
;

: lcd-set-all             :: lcd-set-all             ( -- )
  lcd-vram lcd-vram-size 255 fill
;

\ line is 0 .. lcd-height-lines - 1; u is number of line to invert
: lcd-invert-lines        :: lcd-invert-lines        ( line u -- )
  swap lcd-vram-width-bytes * lcd-vram + swap lcd-vram-width-bytes *
  0 ?do
    dup c@ 255 xor over c! char+
  loop
  drop
;

: lcd-set-pixel           :: lcd-set-pixel           ( x y -- )
  lcd-vram-width-bytes * lcd-vram + >r
  8 /mod r> + swap    ( c-addr bit-number )
  128 swap rshift     ( c-addr bit )
  over c@ or swap c!
;

: lcd-clear-pixel         :: lcd-clear-pixel         ( x y -- )
  lcd-vram-width-bytes * lcd-vram + >r
  8 /mod r> + swap    ( c-addr bit-number )
  128 swap rshift     ( c-addr bit )
  255 xor over c@     ( c-addr ~bit byte )
  and swap c!
;

: lcd-set-point           :: lcd-set-point           ( x y -- )
  2dup lcd-set-pixel
  2dup 1+ lcd-set-pixel
  2dup 1- lcd-set-pixel
  2dup 2 + lcd-set-pixel
  2dup 2 - lcd-set-pixel
  2dup swap 1+ swap lcd-set-pixel
  2dup swap 1- swap lcd-set-pixel
  2dup swap 2 + swap lcd-set-pixel
  2dup swap 2 - swap lcd-set-pixel
  2drop ;

variable lcd-x1           :: lcd-x1                  ( -- a-addr )
variable lcd-y1           :: lcd-y1                  ( -- a-addr )

variable lcd-dx           :: lcd-dx                  ( -- a-addr )
variable lcd-dy           :: lcd-dy                  ( -- a-addr )

variable lcd-stepx        :: lcd-stepx               ( -- a-addr )
variable lcd-stepy        :: lcd-stepy               ( -- a-addr )

: lcd-line                :: lcd-line                ( x0 y0 x1 y1 -- )
  \ Bresenham Algorithm

  dup lcd-y1 !    ( x0 y0 x1 y1 )
  2 pick -                                   ( x0 y0 x1 y1-y0 )
  dup 0< if negate -1 else 1 then            ( x0 y0 x1 dy stepy )
  lcd-stepy ! 2* lcd-dy !                    ( x0 y0 x1 )

  dup lcd-x1 !                               ( x0 y0 x1 )
  2 pick -                                   ( x0 y0 x1-x0 )
  dup 0< if negate -1 else 1 then            ( x0 y0 dx stepx )
  lcd-stepx ! 2* lcd-dx !                    ( x0 y0 )

  2dup lcd-set-pixel                         ( x0 y0 )

  lcd-dx @ lcd-dy @ 2dup > if                ( x0 y0 dx dy )

    swap 2/ - >r                             ( x0 y0 ) ( R: fraction = dy - [dx >> 1] )

    begin
      over lcd-x1 @ <>                       \ x0 <> x1
    while
        r@ 0< 0= if                          \ fraction >= 0
          lcd-stepy @ +                      ( x0 y0+stepy )
          r> lcd-dx @ - >r                   \ fraction -= dx
        then
        swap lcd-stepx @ + swap              ( x0+stepx y0 )
        r> lcd-dy @ + >r                     \ fraction += dy
        2dup lcd-set-pixel                   ( x0 y0 )
    repeat

  else                                       ( x0 y0 dx dy )

    2/ - >r                                  ( x0 y0 ) ( R: fraction = dx - [dy >> 1] )
    swap                                     ( y0 x0 )

    begin
      over lcd-y1 @ <>                       \ y0 <> y1
    while
        r@ 0< 0= if                          \ fraction >= 0
          lcd-stepx @ +                      ( y0 x0+stepx )
          r> lcd-dy @ - >r                   \ fraction -= dy
        then
        swap lcd-stepy @ + swap              ( y0+stepy x0+stepx )
        r> lcd-dx @ + >r                     \ fraction += dx
        2dup swap lcd-set-pixel              ( y0 x0 )
    repeat

  then
  2drop r> drop ;                            ( -- )

variable lcd-x            :: lcd-x                   ( -- a-addr )
variable lcd-y            :: lcd-y                   ( -- a-addr )

: lcd-line-to             :: lcd-line-to             ( x y -- )
  2dup lcd-x @ lcd-y @ lcd-line lcd-move-to ;

: lcd-move-to             :: lcd-move-to             ( x y -- )
  lcd-y ! lcd-x ! ;

: lcd-at-xy               :: lcd-at-xy               ( x y -- )
  lcd-move-to ;

: lcd-home                :: lcd-home                ( -- )
  0 0 lcd-at-xy ;

: lcd-cls                 :: lcd-cls                 ( -- )
  lcd-clear lcd-home ;

: lcd-scroll              :: lcd-scroll              ( -- )
  font-height lcd-vram-width-bytes * dup dup   \ u u u
  lcd-vram + swap                              \ u c-addr u

  lcd-vram-size swap -                         \ u c-addr n
  lcd-vram swap cmove                          \ u

  lcd-vram lcd-vram-size + 1-                  \ u c-addr
  over - swap 0 fill

;

: lcd-scroll>             :: lcd-scroll-up           ( -- )
  font-height lcd-vram-width-bytes * dup dup   \ u u u
  lcd-vram + swap                              \ u c-addr u

  lcd-vram-size swap -                         \ u c-addr n
  lcd-vram -rot                                \ u c-addr2 c-addr n
  cmove>                                       \ u

  lcd-vram swap 0 fill
;

: lcd-cr                  :: lcd-cr                  ( -- )
  0 lcd-x !
  lcd-y @ font-height + dup lcd-height-lines 1- > if
    drop
    lcd-scroll
  else
    lcd-y !
  then
;

: lcd-emit                :: lcd-emit                ( c -- )
  lcd-x @ lcd-width-pixels 1- > if
    lcd-cr
  then
  lcd-y @ lcd-vram-width-bytes * lcd-vram +
  lcd-x @ 3 rshift +           ( c c-addr )
  swap                         ( c-addr c )
  font-height * font-8x13 +    ( lcd-addr font-addr )
  font-height 0 ?do
    2dup c@ swap c!
    char+ swap lcd-vram-width-bytes + swap
  loop 2drop
  font-width lcd-x +!
;

: lcd-type                :: lcd-type                ( caddr u -- )
  0 ?do
    dup c@ lcd-emit char+
  loop drop ;

: lcd-number              :: lcd-number              ( n -- )
  s>d <# # # # # # # # # #> lcd-type ;


\ CTP
\ ===

code ctp-pos              :: c-t-p-pos               ( -- x y )
        xcall   CTP_GetPosition
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r5
        NEXT
end-code

code ctp-pos?             :: c-t-p-pos-question      ( -- flag )
        xcall   CTP_PositionAvailable
        or      %r4, %r4
        jreq    ctp_pos_question_no_character
        ld.w    %r4, TRUE
ctp_pos_question_no_character:
        sub     %r1, BYTES_PER_CELL
        ld.w    [%r1], %r4
        NEXT
end-code


\ debugging
\ =========

code   (brk)              :: breakpoint              ( -- )
        xcall   xdebug                               ;debug
        xld.w   %r6, bpt
        xcall   Debug_PutString
s1:     jp      s1                                   ;debug
bpt:    .asciz  "STOPPED\r\n"
        .balign 4
        NEXT                                         ;debug
end-code

code   (debug)            :: debug                   ( -- )
        xcall   xdebug                               ;debug
        NEXT                                         ;debug
end-code

