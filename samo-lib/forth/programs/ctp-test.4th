\ ctp testing

base @ decimal

20 constant box-width-small
20 constant box-height-small

40 constant box-width-large
40 constant box-height-large


: box-origin-large ( u -- x y w h )
    case
        0 of  \ top left
            0 0
        endof
        1 of  \ top right
            lcd-width-pixels box-width-large - 0
        endof
        2 of  \ bottom right
            lcd-width-pixels box-width-large - lcd-height-pixels box-height-large - 1-
        endof
        3 of  \ bottom left
            0 lcd-height-pixels box-height-large - 1-
        endof
        4 of  \ centre
            lcd-width-pixels 2/ box-width-large 2/ -
            lcd-height-pixels 2/ box-height-large 2/ -
        endof
    endcase
    box-width-large box-height-large  \ x y w h
;


: box-origin-small ( u -- x y w h )
    case
        0 of  \ top left
            0 0
        endof
        1 of  \ top right
            lcd-width-pixels box-width-small - 0
        endof
        2 of  \ bottom right
            lcd-width-pixels box-width-small - lcd-height-pixels box-height-small - 1-
        endof
        3 of  \ bottom left
            0 lcd-height-pixels box-height-small - 1-
        endof
        4 of  \ centre
            lcd-width-pixels 2/ box-width-small 2/ -
            lcd-height-pixels 2/ box-height-small 2/ -
        endof
    endcase
    box-width-small box-height-small  \ x y w h
;


: left-origin-small ( u -- x y w h )
    case
        0 of  \ top left
            0 box-origin-small
        endof
        1 of  \ centre
            4 box-origin-small
        endof
        2 of  \ bottom right
            2 box-origin-small
        endof
    endcase
;


: right-origin-small ( u -- x y w h )
    case
        0 of  \ top right
            1 box-origin-small
        endof
        1 of  \ centre
            4 box-origin-small
        endof
        2 of  \ bottom left
            3 box-origin-small
        endof
    endcase
;


variable 'origin

: inside-box ( x y u -- flag )
    'origin @ execute   \ x y x0 y0 w0 h0

    >r >r >r            \ x y x0         R: h0 w0 y0
    swap r> r>          \ x x0 y y0 w0   R: h0
    -rot r>             \ x x0 w0 y y0 h0

    over + within >r    \ x x0 w0        R: f
    over + within r> and
;


variable check-boxes
variable required-lines

: draw-boxes ( u -- )
    case
        1 of
            3 check-boxes !
            2 required-lines !
            ['] left-origin-small 'origin !
        endof
        2 of
            3 check-boxes !
            2 required-lines !
            ['] right-origin-small 'origin !
        endof
        4 check-boxes !
        4 required-lines !
        ['] box-origin-large 'origin !
    endcase

    lcd-cls
    check-boxes @ 0
    ?do
        i 'origin @ execute  \ x y w h
        2>r
        lcd-move-to lcd-black
        2r> lcd-box
        8 4 lcd-move-rel
        i [char] 1 + lcd-emit
    loop
;


variable down
variable check-counter
variable inside
variable in-sequence
variable flag

: draw-lines ( u -- flag )
    draw-boxes

    button-flush
    key-flush
    ctp-flush
    false down !
    false inside !
    true in-sequence !
    0 check-counter !
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                down @ if
                    check-counter @ required-lines @ =
                    inside @ and
                    in-sequence @ and
                    exit
                then
            else
                down @
                if
                    false flag !
                    check-boxes @ 0
                    ?do
                        2dup
                        i inside-box if
                            true flag !
                            i check-counter @ check-boxes @ mod <>
                            if
                                false in-sequence !
                            then
                        then
                    loop

                    flag @ if  \ into box
                        inside @ 0= if
                            true inside !
                        then
                    else       \ out of box
                        inside @ if
                            false inside !
                            1 check-counter +!
                        then
                    then
                    lcd-line-to
                else
                    lcd-move-to
                    true down !
                then
            then
        then

        button? if
            button
            case
                button-left of
                endof
                button-centre of
                endof
                button-right of
                    false exit
                endof
            endcase
        then

        key? if
            key-flush
        then

\        wait-for-event
    again
;

variable retry-count

: test-ctp-tries ( u -- flag )
    3 0 ?do
        i 0<>
        if
            lcd-cls
            s" CTP Test" lcd-type
            lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
            s" RETRY " lcd-type i lcd-.
            500000 delay-us
        then
        dup draw-lines
        if
            unloop
            drop
            true exit
        then
    loop
    drop false
;

: test-ctp-sequence ( -- flag )
    true
    3 0 ?do
        i test-ctp-tries dup
        if
            s" PASS"
        else
            s" FAIL"
        then
        type ." : CTP item " i 1+ . cr
        and
    loop
;

: test-ctp-main ( -- )
    lcd-cls
    test-ctp-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    s" CTP Test" lcd-type
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : CTP test" cr
;

base !
