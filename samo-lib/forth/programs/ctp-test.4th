\ ctp testing

base @ decimal

40 constant box-width
40 constant box-height


: within-box ( x y x0 y0 x1 y1 -- flag )
    swap >r rot >r   \ x y y0 y1
    within           \ x flag
    swap r> r>       \ flag x x0 x1
    within and
;


: box-origin ( u -- x y )
    case
        0 of  \ top left
            0 0
        endof
        1 of  \ top right
            lcd-width-pixels box-width - 0
        endof
        2 of  \ bottom right
            lcd-width-pixels box-width - lcd-height-pixels box-height - 1-
        endof
        3 of  \ bottom left
            0 lcd-height-pixels box-height - 1-
        endof
        4 of  \ centre
            lcd-width-pixels 2/ box-width 2/ -
            lcd-height-pixels 2/ box-height 2/ -
        endof
    endcase
;


: left-origin ( u -- x y )
    case
        0 of  \ top left
            0 box-origin
        endof
        1 of  \ centre
            4 box-origin
        endof
        2 of  \ bottom right
            2 box-origin
        endof
    endcase
;


: right-origin ( u -- x y )
    case
        0 of  \ top right
            1 box-origin
        endof
        1 of  \ centre
            4 box-origin
        endof
        2 of  \ bottom left
            3 box-origin
        endof
    endcase
;


variable 'origin

: inside-box ( x y u -- flag )
    'origin @ execute   \ x y x0 y0
    over box-width +    \ x y x0 y0 x1
    over box-height +   \ x y x0 y0 x1 yi
    within-box
;


variable check-boxes
variable required-lines

: draw-boxes ( u -- )
    case
        1 of
            3 check-boxes !
            2 required-lines !
            ['] left-origin 'origin !
        endof
        2 of
            3 check-boxes !
            2 required-lines !
            ['] right-origin 'origin !
        endof
        4 check-boxes !
        4 required-lines !
        ['] box-origin 'origin !
    endcase

    lcd-cls
    check-boxes @ 0
    ?do
        i 'origin @ execute
        lcd-move-to lcd-black
        box-width box-height lcd-box
        8 4 lcd-move-rel
        i [char] 1 + lcd-emit
    loop
;


variable down
variable check-counter
variable inside
variable in-sequence
variable flag

: draw-lines ( -- flag )
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

: test-ctp-sequence ( -- flag )
    true
    3 0 ?do
        i draw-boxes
        draw-lines dup
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
