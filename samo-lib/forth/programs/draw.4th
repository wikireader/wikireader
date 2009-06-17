.( ctp tests )

base @ decimal

.( draw )

variable pixel

: draw ( -- )
    lcd-clear-all
    1 pixel !
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
            else
                pixel @ if
                    lcd-set-pixel
                else
                    lcd-clear-pixel
                then
            then
        then

        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                lcd-clear-all
                1 pixel !
            endof
            $04 of   \ centre button
                lcd-set-all
                0 pixel !
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
