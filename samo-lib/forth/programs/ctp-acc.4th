.( display and ctp accuracy )

base @ decimal

.( ctp-accuracy )

: ctp-accuracy ( -- )
    lcd-cls
    s" CTP Accuracy Testing" lcd-type

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
            else
                20 32 lcd-move-to
                swap
                lcd-number
                s"   " lcd-type
                lcd-number
            then
        then

        P6_P6D p@ $07 and
        case
            $02 of   \ left button
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                exit
            endof
        endcase
    again
;

ctp-accuracy

.( complete )
base !
