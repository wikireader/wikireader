\ run some tests for mtbf test

base @ decimal

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
    dup lcd-number
    >r here 1024 + 1 r> read-sectors ?dup
    if
        drop
        8 lcd-text-rows 2 - lcd-at-xy
        1 error-count +! error-count @ lcd-number
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

variable touched

: read-event ( -- e )
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
    again
;

variable bitset
variable time-limit

: mtbf-clear-log ( -- )
    12 6 ?do
        0 i lcd-at-xy s"            " lcd-type
    loop
;

: mtbf-test ( -- )
    false touched !
    0 bitset !
    0 time-limit !
    begin
        1 time-limit +!
        time-limit @ 500 >
        if
            0 bitset !
            0 time-limit !
            mtbf-clear-log
        then

        read-event
        case
            event-no-touch of
                0 time-limit !
                0 7 lcd-at-xy s" CTP release" lcd-type
                $01 bitset @ or bitset !
            endof

            event-touch of
                0 time-limit !
                0 6 lcd-at-xy s" CTP touch  " lcd-type
                0 7 lcd-at-xy s"            " lcd-type
                $02 bitset @ or bitset !
                $01 invert bitset @ and bitset !
            endof

            event-release of
                0 time-limit !
                0 8 lcd-at-xy s" Key release" lcd-type
                $04 bitset @ or bitset !
            endof

            event-k1 of
                0 time-limit !
                0 8 lcd-at-xy s"            " lcd-type
                0 9 lcd-at-xy s" Key 1      " lcd-type
                $08 bitset @ or bitset !
                $04 invert bitset @ and bitset !
            endof

            event-k2 of
                0 time-limit !
                0 8 lcd-at-xy s"            " lcd-type
                0 10 lcd-at-xy s" Key 2     " lcd-type
                $10 bitset @ or bitset !
                $04 invert bitset @ and bitset !
            endof

            event-k3 of
                0 time-limit !
                0 8 lcd-at-xy s"            " lcd-type
                0 11 lcd-at-xy s" Key 3     " lcd-type
                $20 bitset @ or bitset !
                $04 invert bitset @ and bitset !
            endof
        endcase
        bitset @ $3f and $3f =
    until
    25 6 lcd-at-xy s" PASS" lcd-type
    800000 delay-us
    25 6 lcd-at-xy s"     " lcd-type
;

: mtbf-main ( -- )
    button-flush
    key-flush
    ctp-flush
    lcd-cls
    s" MTBF Testing" lcd-type
    lcd-cr lcd-cr
    s" 1. Check errors" lcd-type lcd-cr
    s" 2. Touch square" lcd-type lcd-cr
    s" 3. Press/release each key" lcd-type lcd-cr

    10 lcd-text-rows 1- lcd-at-xy s" Key1" lcd-type
    17 lcd-text-rows 1- lcd-at-xy s" Key2" lcd-type
    24 lcd-text-rows 1- lcd-at-xy s" Key3" lcd-type


    0 lcd-text-rows 3 - lcd-at-xy
    s" sector: 00000000" lcd-type lcd-cr
    s" errors: 00000000" lcd-type

    touch-x0 touch-y0 lcd-move-to
    box-width box-height lcd-box

    begin
        mtbf-clear-log
        mtbf-test
    again
;

base !

