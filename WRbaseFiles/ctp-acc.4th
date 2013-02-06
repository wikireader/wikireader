\ display and ctp accuracy

base @ decimal

: ctp-accuracy ( -- )
    button-flush
    key-flush
    ctp-flush
    lcd-cls
    s" CTP Accuracy Testing" lcd-type
    24 lcd-text-rows 1- lcd-at-xy s" Exit" lcd-type

    0  67 lcd-move-to s"  67" lcd-type
    0 135 lcd-move-to s" 135" lcd-type

    80  160 lcd-move-to s" 79" lcd-type
    160 160 lcd-move-to s" 159" lcd-type

    79  67  lcd-set-point
    159 67  lcd-set-point
    79  135 lcd-set-point
    159 135 lcd-set-point

    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                20 32 lcd-move-to
                2 lcd-spaces
            else
                20 32 lcd-move-to
                [char] * lcd-emit
                lcd-space
                [char] X lcd-emit
                swap
                4 lcd-u.r
                4 lcd-spaces
                [char] Y lcd-emit
                4 lcd-u.r
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
                    exit
                endof
            endcase
        then

        key? if
            key-flush
        then

        wait-for-event
    again
;

base !

