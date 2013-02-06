\ serial.4th

base @ decimal

: display-serial-number ( -- )

    lcd-black
    lcd-cls
    s" Read Serial Number from FLASH" lcd-type
    24 lcd-text-rows 1- lcd-at-xy
    s" Exit" lcd-type

    0 2 lcd-at-xy
    here flash-serial-number-length flash-serial-number-offset flash-read
    if
        s" Serial Number:" lcd-type lcd-cr

        flash-serial-number-length 0
        ?do
            here i + c@
            dup bl 127 within
            if
                lcd-emit
            else
                drop
            then
        loop
    else
        s" Error: No Serial Number Found" lcd-type
    then

    button-flush
    key-flush
    ctp-flush
    begin
        ctp-pos? if
            ctp-flush
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
