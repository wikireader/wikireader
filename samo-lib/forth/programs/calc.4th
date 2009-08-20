\ simple calculator

base @ decimal

30 constant box-width
30 constant box-height

: draw-box ( x y -- )
    lcd-move-to lcd-black
    box-width box-height lcd-box
;

: (highlight-box) ( x y -- )
    lcd-move-to
    1 1 lcd-move-rel
    box-width 2 - box-height 2 - lcd-box
    1 1 lcd-move-rel
    box-width 4 - box-height 4 - lcd-box
;

: highlight-box ( x y -- )
    lcd-black (highlight-box)
;

: unhighlight-box ( x y -- )
    lcd-white (highlight-box)
    lcd-black
;

: within-box ( x y x0 y0 x1 y1 -- flag )
    swap >r rot >r   \ x y y0 y1
    within           \ x flag
    swap r> r>       \ flag x x0 x1
    within and
;

48 constant y-offset
4 font-width * constant x-offset

create positions
\ x is multiple of font-width
 0 font-width * x-offset + c,  0 y-offset + c, char 1 c, bl c,
 4 font-width * x-offset + c,  0 y-offset + c, char 2 c, bl c,
 8 font-width * x-offset + c,  0 y-offset + c, char 3 c, bl c,
12 font-width * x-offset + c,  0 y-offset + c, char + c, bl c,
16 font-width * x-offset + c,  0 y-offset + c, char C c, bl c,
 0 font-width * x-offset + c, 32 y-offset + c, char 4 c, bl c,
 4 font-width * x-offset + c, 32 y-offset + c, char 5 c, bl c,
 8 font-width * x-offset + c, 32 y-offset + c, char 6 c, bl c,
12 font-width * x-offset + c, 32 y-offset + c, char - c, bl c,
16 font-width * x-offset + c, 32 y-offset + c, char M c, char C c,
 0 font-width * x-offset + c, 64 y-offset + c, char 7 c, bl c,
 4 font-width * x-offset + c, 64 y-offset + c, char 8 c, bl c,
 8 font-width * x-offset + c, 64 y-offset + c, char 9 c, bl c,
12 font-width * x-offset + c, 64 y-offset + c, char * c, bl c,
16 font-width * x-offset + c, 64 y-offset + c, char M c, char + c,
 0 font-width * x-offset + c, 96 y-offset + c, char . c, bl c,
 4 font-width * x-offset + c, 96 y-offset + c, char 0 c, bl c,
 8 font-width * x-offset + c, 96 y-offset + c, char = c, bl c,
12 font-width * x-offset + c, 96 y-offset + c, char / c, bl c,
16 font-width * x-offset + c, 96 y-offset + c, char M c, char R c,
20 constant position-count


: box-data ( u -- x y c1 c2 )
    dup 0< if drop 0 then
    dup position-count < 0= if
        drop 0
    then
    2* 2* positions + dup c@
    swap char+ dup c@
    swap char+ dup c@
    swap char+ c@
;


: inside-box ( x y u -- flag )
    box-data 2drop      \ x y x0 y0
    over box-width +    \ x y x0 y0 x1
    over box-height +   \ x y x0 y0 x1 yi
    within-box
;

variable v-sign
variable v-int
variable v-frac

variable v-point

variable v-op

variable total-sign
variable total-int
variable total-frac

variable mem-sign
variable mem-int
variable mem-frac

variable auto-clear

\ define 8.8 number
99999999 constant v-max
v-max 1+ constant v-overflow

8 constant int-digits
8 constant frac-digits

: half-add ( carry u1 u2 -- u carry )
    + + dup v-max > if
        v-max - 1- 1
    else
        0
    then
;

: half-sub ( borrow u1 u2 -- u borrow )
    - swap - dup 0< if
        v-max + 1+ 1
    else
        0
    then
;


: add ( -- )
    v-sign @ total-sign @ =
    if
        0 total-frac @ v-frac @ half-add    \ frac carry
        total-int @ v-int @ half-add        \ overflow
        if
            2drop v-max v-max
        then
    else
        total-int @ v-int @ 2dup < if
            2drop true
        else
            = if
                total-frac @ v-frac @ <
            then
        then
        if
            0 v-frac @ total-frac @ half-sub    \ frac borrow
            v-int @ total-int @ half-sub
            v-sign @ total-sign !
        else
            0 total-frac @ v-frac @ half-sub    \ frac borrow
            total-int @ v-int @ half-sub
        then
        drop
    then
    2dup or 0= if
        false total-sign !
    then
    total-int !
    total-frac !
;

: mem-add ( -- )
    total-sign @ mem-sign @ =
    if
        0 mem-frac @ total-frac @ half-add    \ frac carry
        mem-int @ total-int @ half-add        \ overflow
        if
            2drop v-max v-max
        then
    else
        mem-int @ total-int @ 2dup < if
            2drop true
        else
            = if
                mem-frac @ total-frac @ <
            then
        then
        if
            0 total-frac @ mem-frac @ half-sub    \ frac borrow
            total-int @ mem-int @ half-sub
            total-sign @ mem-sign !
        else
            0 mem-frac @ total-frac @ half-sub    \ frac borrow
            mem-int @ total-int @ half-sub
        then
        drop
    then
    2dup or 0= if
        false mem-sign !
    then
    mem-int !
    mem-frac !
;


: sub ( -- )
    v-sign @ dup invert v-sign !
    add
    v-sign !
;

: mul ( -- )
    total-sign @ v-sign @ <> total-sign !

    total-frac @ v-frac @ um*
    v-overflow fm/mod

    total-int @ v-frac @ um*
    v-overflow fm/mod
    >r +

    total-frac @ v-int @ um*
    v-overflow fm/mod
    >r +

    s>d     v-overflow fm/mod

    total-int @ v-int @ um*
    v-overflow fm/mod

    >r + r> swap
    r> + r> +

    s>d     v-overflow fm/mod
    rot +
    if
        2drop
        v-max v-max
    then

    total-int !
    total-frac !
    v-overflow 2/ > if
        total-frac @ 1+ v-max > if
            total-int @ 1+ v-max > if
                v-max v-max
                total-int !
                total-frac !
            else
                0 total-frac !
                1 total-int +!
            then
        else
            1 total-frac +!
        then
    then
;

variable r-int
variable r-frac
variable rs-int
variable rs-frac
variable q-int
variable q-frac

: div-gr ( -- flag )
    r-int @ total-int @ >
    if
        true exit
    then
    r-int @ total-int @ =
    r-frac @ total-frac @ > and
;

: div-q*10 ( -- )
    q-frac @ 10 *
    s>d v-overflow fm/mod
    q-int @ 10 * +
    q-int ! q-frac !
;

: div-r*10 ( -- )
    r-frac @ 10 *
    s>d v-overflow fm/mod
    r-int @ 10 * +
    r-int ! r-frac !
;

: div-t*10 ( -- )
    total-frac @ 10 *
    s>d v-overflow fm/mod
    total-int @ 10 * +
    total-int ! total-frac !
;


: div ( -- )
    v-int @ v-frac @ or 0=
    if
        v-max v-max
        total-int !
        total-frac !
        exit
    then
    total-int @ total-frac @ or 0=
    if
        exit
    then

    v-sign @ total-sign @ xor total-sign !
    v-int  @ dup r-int  ! rs-int  !
    v-frac @ dup r-frac ! rs-frac !
    0 q-int ! 0 q-frac !

    \ shift count
    div-gr if
        1
    else
        0
    then
    begin
        div-gr 0=
    while
            r-frac @ rs-frac !
            r-int  @ rs-int  !
            div-r*10
            1+
    repeat
    rs-frac @ r-frac !
    rs-int  @ r-int  !
    \ loop count on stack
    frac-digits +
    0 ?do
        div-q*10
        begin
            0 total-frac @ r-frac @ half-sub
            total-int @ r-int @ half-sub 0=
        while
                total-int !
                total-frac !
                1 q-frac +!  \ never gets bigger than 9 so no need for carry
        repeat
        2drop
        div-t*10
    loop
    q-frac @ total-frac !
    q-int  @ total-int  !
;

: output-number ( frac int sign x y -- )
    lcd-at-xy
    if
        [char] -
    else
        bl
    then
    lcd-emit
    8 lcd-u.r
    [char] . lcd-emit
    s>d <# # # # # # # # # #> lcd-type
;

: refresh-display ( -- )
    total-frac @
    total-int @
    total-sign @
    5 1 output-number
    lcd-space [char] T lcd-emit

    v-frac @
    v-int @
    v-sign @
    5 2 output-number
    26 1 lcd-at-xy

    v-op @
    case
       -1 of bl endof
        0 of bl endof
        1 of [char] + endof
        2 of [char] - endof
        3 of [char] * endof
        4 of [char] / endof
    endcase
    lcd-emit

    mem-frac @
    mem-int @
    mem-sign @
    5 lcd-text-rows 2 - output-number
    lcd-space [char] M lcd-emit
;

: clear-mem ( -- )
    false mem-sign !
    0 mem-int !
    0 mem-frac !
;

: clear-entry ( -- )
    false v-sign !
    0 v-int !
    0 v-frac !
    0 v-point !
    false auto-clear !
    v-op @ -1 = if
        0 v-op !
    then
;

: clear-all ( -- )
    clear-entry
    0 v-op !
    false total-sign !
    0 total-int !
    0 total-frac !
;

: recall-mem ( -- )
    clear-entry
    mem-sign @ v-sign !
    mem-int  @ v-int  !
    mem-frac @ v-frac !
;

: copy-to-total ( -- )
    v-sign @ total-sign !
    v-int  @ total-int  !
    v-frac @ total-frac !
;

: new-digit ( u -- )
    auto-clear @ if
        clear-entry
    then
    v-point @ if
        v-point @ 0> if
            v-point @ 10 / dup
            0= if
                drop -1 v-point !
            else
                dup v-point !
                * v-frac +!
            then
        then
     else
        v-int @ 10 * + dup v-max > if
            drop v-max
        then
        v-int !
    then
;

: operator-set ( u -- )
    v-op @
    case
        0 of copy-to-total endof
        1 of add endof
        2 of sub endof
        3 of mul endof
        4 of div endof
    endcase
    v-op !
    true auto-clear !
;

: process ( u -- )
    case
        0 of     \ 1
            1 new-digit
        endof
        1 of     \ 2
            2 new-digit
        endof
        2 of     \ 3
            3 new-digit
        endof
        3 of     \ +
            1 operator-set
        endof
        4 of     \ C
            clear-all
        endof
        5 of     \ 4
            4 new-digit
        endof
        6 of     \ 5
            5 new-digit
        endof
        7 of     \ 6
            6 new-digit
        endof
        8 of     \ -
            2 operator-set
        endof
        9 of     \ MC
            clear-mem
        endof
        10 of    \ 7
            7 new-digit
        endof
        11 of    \ 8
            8 new-digit
        endof
        12 of    \ 9
            9 new-digit
        endof
        13 of    \ *
            3 operator-set
        endof
        14 of    \ M+
            mem-add
        endof
        15 of    \ .
            auto-clear @ if
                clear-entry
            then
            v-overflow v-point !
        endof
        16 of    \ 0
            0 new-digit
        endof
        17 of    \ =
            -1 operator-set
        endof
        18 of    \ /
            4 operator-set
        endof
        19 of    \ MR
            recall-mem
        endof
    endcase
    refresh-display
;


variable down
variable box-number
variable can-exit

: calculator ( -- )
    lcd-cls
    button-flush
    key-flush
    ctp-flush

    can-exit @ if
        24 lcd-text-rows 1- lcd-at-xy s" Exit" lcd-type
    then

    position-count 0 ?do
        i box-data >r >r 2dup draw-box
        8 + >r font-width + r> lcd-move-to
        r> lcd-emit r> lcd-emit
    loop

    false down !
    refresh-display
    -1 box-number !
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                down @ if
                    box-number @ dup
                    box-data 2drop
                    unhighlight-box
                    1 1 lcd-at-xy 2 lcd-spaces
                    0 0 unhighlight-box
                    process
                    -1 box-number !
                then
                false down !
            else
                position-count 0 ?do
                    2dup i inside-box if
                        true down !
                        box-number @ 0< 0=
                        box-number @ i <> and
                        if
                            box-number @
                            box-data 2drop
                            1 1 lcd-at-xy 2 lcd-spaces
                            unhighlight-box
                            0 0 unhighlight-box
                        then
                        i dup box-number !
                        box-data
                        swap 1 1 lcd-at-xy lcd-emit lcd-emit
                        highlight-box
                        0 0 highlight-box
                    then
                loop
                2drop
            then
        then

        key? if
            key-flush
        then

        button? if
            button
            case
                button-left of
                endof
                button-centre of
                endof
                button-right of
                    can-exit @ if
                        exit
                    then
                endof
                button-power of
                    power-off
                endof
            endcase
        then

        wait-for-event
    again
;

base !
