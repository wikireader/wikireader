\ ctp line drawing

base @ decimal

variable down

: line-draw ( -- )
    lcd-cls
    button-flush
    key-flush
    ctp-flush
    false down !
    lcd-black
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                false down !
            else
                down @
                if
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
                    lcd-cls
                    false down !
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
