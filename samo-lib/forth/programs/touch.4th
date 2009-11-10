\ ctp testing

base @ decimal

80 constant touch-box-width
40 constant touch-box-height


: draw-patterns ( -- )
    lcd-black

    60 0 do
        i i lcd-move-to
        lcd-width-pixels i 2* - lcd-height-pixels i 2* - lcd-box
    5 +loop

    lcd-width-pixels 2/ touch-box-width 2/ -
    lcd-height-pixels 2/ touch-box-height 2/ -
    lcd-move-to
    touch-box-width touch-box-height lcd-box

    lcd-text-columns 2/ 3 - lcd-text-rows 2/ 1- lcd-at-xy
    s" TOUCH" lcd-type
;


variable touch-down

: test-touch-sequence  ( -- flag )
    lcd-cls
    draw-patterns

    button-flush
    key-flush
    ctp-flush
    false touch-down !

    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                touch-down @
                exit
            else
                2drop
                true touch-down !
                lcd-text-columns 2/ 4 - lcd-text-rows 2/ 1- lcd-at-xy
                s" RELEASE" lcd-type
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
                endof
            endcase
        then

        key? if
            key-flush
        then

\        wait-for-event
    again
;


: test-touch-main ( -- )
    test-touch-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    s" Touch Test" lcd-type
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : touch test" cr
;

base !
