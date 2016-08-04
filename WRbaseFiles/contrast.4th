\ contract check

base @ decimal

\ display battery
: battery-display ( -- )
    0 2 lcd-at-xy
    s" battery    = " lcd-type
    battery-mv 5 lcd-u.r
    s"  mV" lcd-type
;

\ display thermistor

: thermistor-display ( -- )
    0 4 lcd-at-xy
    s" thermistor = " lcd-type
    temperature-c dup 5 lcd-.r
    s"  C" lcd-type
    lcd-cr
    s" c.dmd      = " lcd-type
    21079 swap
    dup 25 > if
        25 - 46 * -
    else
        drop
    then
    5 lcd-u.r
;


\ display the current timer value

: timer-display ( -- )
    0 6 lcd-at-xy
    s" timer      = " lcd-type
    timer-read 12 lcd-u.r
;


\ display lcd_contrast

: contrast-display ( -- )
    0 8 lcd-at-xy
    s" contrast   = " lcd-type
    contrast-mv 5 lcd-u.r
    s"  mV" lcd-type
;

variable compensation-active

: pwm-display ( -- )
    0 10 lcd-at-xy
    s" pwm        = " lcd-type
    get-contrast-pwm 5 lcd-u.r
    5 lcd-spaces
    compensation-active @
    if   s" T-comp"
    else s" Off   "
    then
    lcd-type
;


\ compensation on/off control

: compensation-off ( -- )
    false compensation-active !
    10 lcd-text-rows 1- lcd-at-xy
    s" C-On " lcd-type
;

: compensation-on ( -- )
    true compensation-active !
    10 lcd-text-rows 1- lcd-at-xy
    s" C-Off" lcd-type
;

: compensation-toggle ( -- )
    compensation-active @
    if   compensation-off
    else compensation-on
    then
;


\ main

: test-contrast-main ( -- )
    key-flush
    ctp-flush
    button-flush
    false compensation-active !

    lcd-cls
    s" Contrast Check" lcd-type

    10 lcd-text-rows 1- lcd-at-xy
    s"       Norm    Exit" lcd-type

    compensation-off

    begin

        analog-scan
        battery-display
        thermistor-display
        timer-display
        contrast-display
        pwm-display

        ctp-pos? if
            ctp-pos 2drop
        then

        button? if
            button
            case
                button-left of
                    compensation-toggle
                endof
                button-centre of
                    nominal-contrast-pwm set-contrast-pwm
                endof
                button-right of
                    exit
                endof
            endcase
        then

        key? if
            key-flush
        then

        \ wait-for-event

        compensation-active @ if (temperature-comp) then
    again
;

base !
