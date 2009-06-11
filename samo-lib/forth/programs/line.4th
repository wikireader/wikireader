.( ctp line drawing )

base @ decimal

variable down

: draw ( -- )
    lcd-clear
    false down !
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

        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                lcd-clear
                false down !
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                exit
            endof
        endcase
    again
;

draw

.( complete )
base !
