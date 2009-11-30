\ program the on-board flash then run some tests

base @ decimal

: get-image-file  ( b u addr -- flag )
    >r
    r/o bin open-file ?dup
    if
        lcd-." open error = " lcd-dec. lcd-cr
        r> 2drop
        false exit
    then

    r> \ file-id buffer
    swap >r \ buffer

    flash-rom-size

    begin
        \ b u1
        2dup
        r@ read-file ?dup  \ b u1 u2 ior ior?
        if
            lcd-." read error = " lcd-dec. lcd-cr
            2drop drop
            r> close-file drop
            false exit
        then
        \ b u1 u2
        swap over -  \ b u2 (u1-u2)
        >r + r>      \ (b+u2) (u1-u2)
        dup 0=
    until
    2drop
    r> close-file drop
    true
;

create serial-number-buffer
flash-serial-number-length allot

: program-rom  ( b u -- )

    flash-select-internal

    lcd-." Load File: " 2dup lcd-type lcd-cr

    here get-image-file 0=
    if
        lcd-." Read file failed" lcd-cr
        exit
    then

    lcd-." S/N: "

    \ display the serial number
    serial-number-buffer flash-serial-number-offset +
    flash-serial-number-length
    flash-serial-number-offset flash-read
    if
        flash-serial-number-length 0
        ?do
            serial-number-buffer flash-serial-number-offset + i + c@
            dup bl 127 within
            if
                lcd-emit
            else
                drop
            then
        loop
    else
        lcd-." FAIL" lcd-cr
        exit
    then

    lcd-cr lcd-." Erase: "

    flash-write-enable
    if
        flash-chip-erase
        if
            lcd-." Done"
        else
            lcd-." FAIL"
            lcd-cr exit
        then
    else
        lcd-." FAIL"
        lcd-cr exit
    then

    lcd-cr lcd-." Program: "
    flash-rom-size 0 ?do

        flash-write-enable 0=
        if
            lcd-." FAIL" lcd-cr
            unloop
            exit
        then
        here i + flash-page-size i flash-write 0=
        if
            lcd-." FAIL" lcd-cr
            unloop
            exit
        then
        i flash-sector-size 1- and 0=
        if
            [char] . lcd-emit
        then
    flash-page-size +loop

    lcd-cr lcd-." Verify:  "
    flash-rom-size 0 ?do
        here i + flash-sector-size i flash-verify
        if
            [char] . lcd-emit
        else
            [char] E lcd-emit
        then
    flash-sector-size +loop
    lcd-cr
;


\ ===========================================


: within-box ( x y x0 y0 x1 y1 -- flag )
    swap >r rot >r   \ x y y0 y1
    within           \ x flag
    swap r> r>       \ flag x x0 x1
    within and
;


variable sector
variable error-count

: scan-sd-sector ( -- )
    sector @ 1+ $fffff and dup sector !
    8 lcd-text-rows 3 - lcd-at-xy
    dup 8 lcd-u.r
    >r here 1024 + 1 r> read-sectors ?dup
    if
        drop
        8 lcd-text-rows 2 - lcd-at-xy
        1 error-count +! error-count @ 8 lcd-u.r
        filesystem-init
    then
;

50 constant box-width
50 constant box-height

lcd-width-pixels 2/ 15 - dup
constant touch-x0
box-width +
constant touch-x1

lcd-height-pixels 2/ 15 - dup
constant touch-y0
box-height +
constant touch-y1

1 constant event-no-touch
2 constant event-touch
3 constant event-release
4 constant event-k1
5 constant event-k2
6 constant event-k3
7 constant event-timeout

variable touched
variable time-limit

: read-event ( -- e )
    0 time-limit !
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                touched @
                if
                    false touched !
                    touch-x0 touch-y0 lcd-move-to
                    2 2 lcd-move-rel
                    box-width 4 - box-height 4 - lcd-white lcd-box
                    lcd-black
                    event-no-touch exit
                then
            else
                touch-x0 touch-y0
                touch-x1 touch-y1
                within-box
                touched @ 0= and
                if
                    true touched !
                    touch-x0 touch-y0 lcd-move-to
                    2 2 lcd-move-rel
                    box-width 4 - box-height 4 - lcd-box
                    event-touch exit
                then
            then
        then

        button? if
            button
            case
                button-none of
                    event-release exit
                endof
                button-left of
                    event-k1 exit
                endof
                button-centre of
                    event-k2 exit
                endof
                button-right of
                    event-k3 exit
                endof
            endcase
        then

        key? if
            key-flush
        then

        scan-sd-sector

        1 time-limit +!
        time-limit @ 500 >
        if
            event-timeout exit
        then

    again
;


variable bitset
variable in-ok


: cob-clear-log ( -- )
    12 6 ?do
        0 i lcd-at-xy 11 lcd-spaces
    loop
;


: cob-ok ( -- )
    true in-ok !
    21  7 lcd-at-xy lcd-."  OO  k  "
    21  8 lcd-at-xy lcd-." O  O k k"
    21  9 lcd-at-xy lcd-." O  O kk "
    21 10 lcd-at-xy lcd-."  OO  k k"
;

: cob-ok-off ( -- )
    in-ok @
    if
        11 7 ?do
            21 i lcd-at-xy 8 lcd-spaces
        loop
        false in-ok !
    then
;


: cob-test ( -- )
    false touched !
    0 bitset !
    begin
        read-event
        case
            event-timeout of
                0 bitset !
                cob-clear-log
                cob-ok-off
            endof

            event-no-touch of
                0 7 lcd-at-xy lcd-." CTP release"
                $01 bitset @ or bitset !
            endof

            event-touch of
                0 6 lcd-at-xy lcd-." CTP touch  "
                0 7 lcd-at-xy lcd-."            "
                $02 bitset @ or bitset !
                $01 invert bitset @ and bitset !
                cob-ok-off
            endof

            event-release of
                0 8 lcd-at-xy lcd-." Key release"
                $04 bitset @ or bitset !
            endof

            event-k1 of
                0 8 lcd-at-xy lcd-."            "
                0 9 lcd-at-xy lcd-." Key 1      "
                $08 bitset @ or bitset !
                $04 invert bitset @ and bitset !
                cob-ok-off
            endof

            event-k2 of
                0 8 lcd-at-xy lcd-."            "
                0 10 lcd-at-xy lcd-." Key 2     "
                $10 bitset @ or bitset !
                $04 invert bitset @ and bitset !
                cob-ok-off
            endof

            event-k3 of
                0 8 lcd-at-xy lcd-."            "
                0 11 lcd-at-xy lcd-." Key 3     "
                $20 bitset @ or bitset !
                $04 invert bitset @ and bitset !
                cob-ok-off
            endof
        endcase
        bitset @ $3f and $3f =
    until
    cob-ok
;


: cob-main ( -- )
    button-flush
    key-flush
    ctp-flush
    lcd-cls
    lcd-." COB Testing" lcd-cr

    s" flash.rom" program-rom

    10 lcd-text-rows 1- lcd-at-xy lcd-." Key1"
    17 lcd-text-rows 1- lcd-at-xy lcd-." Key2"
    24 lcd-text-rows 1- lcd-at-xy lcd-." Key3"


    0 lcd-text-rows 3 - lcd-at-xy
    lcd-." sector:         " lcd-cr
    lcd-." errors:         "

    touch-x0 touch-y0 lcd-move-to
    box-width box-height lcd-box

    begin
        cob-clear-log
        cob-test
    again
;

base !

