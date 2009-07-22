\ gpio tests

base @ decimal

\ display battery
: battery-display ( u -- )
    0 2 lcd-at-xy
    s" battery    = " lcd-type
    battery-mv 5 lcd-u.r
    s"  mV" lcd-type
;

\ display thermistor

: thermistor-display ( u -- )
    0 4 lcd-at-xy
    s" thermistor = " lcd-type
    temperature-c 5 lcd-u.r
    s"  C" lcd-type
;

\ display lcd_contrast

: contrast-display ( u -- )
    0 8 lcd-at-xy
    s" contrast   = " lcd-type
    contrast-mv 5 lcd-u.r
    s"  mV" lcd-type
;


\ main
variable measure-count

variable last-x

: test-gpio-main ( -- )
    key-flush
    ctp-flush
    button-flush

    lcd-cls
    s" GPIO TESTS" lcd-type

    3 12 lcd-at-xy
    s" <-- change contrast -->" lcd-type

    10 lcd-text-rows 1- lcd-at-xy
    s" Off   Norm    Exit" lcd-type

    0 last-x !

    begin
        1 measure-count +!
        measure-count @ $3ff = if
            0 measure-count !
            analog-scan
            battery-display
            thermistor-display
            contrast-display
            0 10 lcd-at-xy
            s" pwm        = " lcd-type
            get-contrast-pwm 5 lcd-u.r
        then

        ctp-pos? if
            ctp-pos 0< if
                drop 0 last-x !
            else
                last-x @ 0= if
                    last-x !
                else
                    dup
                    last-x @
                    - get-contrast-pwm + set-contrast-pwm
                    last-x !
                then
            then
        then

        button? if
            button
            case
                button-left of
                    power-off
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
    again
;

base !
