\ ctp testing

decimal

30 constant box-width
30 constant box-height

: draw-box ( x y -- )

    2dup lcd-move-to

    over box-width + over lcd-line-to
    over box-width + over box-height + lcd-line-to
    over over box-height + lcd-line-to
    lcd-line-to
;


: within-box ( x y x0 y0 x1 y1 -- flag )
    swap >r rot >r   \ x y y0 y1
    within           \ x flag
    swap r> r>       \ flag x x0 x1
    within and
;


: box-origin ( u -- x y )
    case
        0 of
            0 0
        endof

        1 of
            lcd-width-pixels box-width - 0
        endof

        2 of
            lcd-width-pixels box-width - lcd-height-lines box-height - 1-
        endof

        3 of
            0 lcd-height-lines box-height - 1-
        endof
    endcase
;


: inside-box ( x y u -- flag )
    box-origin          \ x y x0 y0
    over box-width +    \ x y x0 y0 x1
    over box-height +   \ x y x0 y0 x1 yi
    within-box
;


variable down
variable check
variable inside
variable in-sequence
variable flag

: draw-lines ( -- flag )
    lcd-cls
    4 0 ?do
        i box-origin draw-box
    loop

    false down !
    false inside !
    true in-sequence !
    0 check !
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                down @ if
                    check @ 4 =
                    inside @ and
                    in-sequence @ and
                    exit
                then
            else
                down @
                if
                    false flag !
                    4 0 ?do
                        2dup
                        i inside-box if
                            true flag !
                            i check @ 3 and <> if
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
                            1 check +!
                            cr check ?
                        then
                    then

                    lcd-line-to
                else
                    lcd-move-to
                    true down !
                then
            then
        then

        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                true exit
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                false exit
            endof
        endcase
    again
;

: ctp-test ( -- )
    lcd-cls
    draw-lines if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    cr type ." : CTP" cr
;


ctp-test
