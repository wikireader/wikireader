\ draw with dots

base @ decimal


variable pixel

: dot-draw ( -- )
    lcd-cls
    s" CTP Dot Plotting" lcd-type
    lcd-cr lcd-cr
    s" Touch screen to place dot" lcd-type
    9 lcd-text-rows 1- lcd-at-xy
    s" Clear" lcd-type
    10 lcd-spaces
    s" Exit" lcd-type

    button-flush
    key-flush
    ctp-flush
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

        button? if
            button
            case
                button-left of
                    lcd-clear-all
                    1 pixel !
                endof
                button-centre of
                    lcd-set-all
                    0 pixel !
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

