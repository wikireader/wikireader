\ gpio tests

base @ decimal


\ watchdog off

: wdt-high ( -- )
  $08 dup P6_P6D p! P6_IOC6 p! ;


: wdt-low ( -- )
  wdt-high
  0 P6_P6D p! ;


: wdt-off ( -- )
  wdt-high
  500000 0 ?do loop
  wdt-low
  500000 0 ?do loop ;


\ normal auto power off

: off ( -- )
$08 dup P0_IOC0 p! P0_P0D p! ;


\ read ADC channels 0..2

: read-adc ( -- u2 u1 u0 )

    \ configure adc
    $000f AD_CLKCTL p!
    $0100 AD_ADVMODE p!
    $1000 AD_TRIG_CHNL p!
    $0304 AD_EN_SMPL_STAT p!

    \ enable pins adc0..adc2
    $15 P7_03_CFP p!

    \ start the adc
    AD_EN_SMPL_STAT p@ $02 or AD_EN_SMPL_STAT p!

    \ wait for conversion complete
    \ begin
    \     AD_EN_SMPL_STAT p@ $08 and
    \ until

    begin
        AD_END p@ $04 and
    until

    \ read the data
    AD_CH2_BUF p@
    AD_CH1_BUF p@
    AD_CH0_BUF p@

    \ turn adc off
    0 P7_03_CFP p!
    0 AD_EN_SMPL_STAT p!
    0 AD_CLKCTL p!

;

\ values for ADC full scale
3150 constant AVDD_MILLIVOLTS
1023 constant ADC_FULL_SCALE

\ resistor divider in kilo ohms
150  constant ADC_LIMITING_RESISTOR
1000 constant ADC_SHUNT_RESISTOR

\ scale factor for fixed point conversion
\ must not overflow 32 bits signed value
100  constant DIVISOR

DIVISOR ADC_LIMITING_RESISTOR ADC_SHUNT_RESISTOR + *
ADC_SHUNT_RESISTOR /
constant MULTIPLIER

\ display battery
: battery-display ( u -- )
    dup
    0 3 lcd-at-xy
    s" battery = " lcd-type
    5 lcd-u.r
    s"  counts" lcd-type lcd-cr
    s"           " lcd-type
    AVDD_MILLIVOLTS * MULTIPLIER *
    ADC_FULL_SCALE / DIVISOR /
    5 lcd-u.r s"  mV" lcd-type
;

\ display thermistor

: thermistor-display ( u -- )
    0 6 lcd-at-xy
    s" thermistor = " lcd-type
    5 lcd-u.r
    s"  counts" lcd-type
;

\ display lcd_contrast

: contrast-display ( u -- )
    0 8 lcd-at-xy
    s" contrast = " lcd-type
    5 lcd-u.r
    s"  counts" lcd-type
;


\ configure timer1

: tm1-configure ( max -- )

    \ enable TM1 output
    \ P1_03_CFP p@ ~$0c and $04 or P1_03_CFP p!
    P1_03_CFP p@ $f3 and $04 or P1_03_CFP p!

    T16ADV T16_ADVMODE p!

    \ Stop timer
    0
    \ INITOLx or
    \ SELFMx or
    \ SELCRBx or
    \ OUTINVx or
    \ CKSLx or
    PTMx or
    \ PRESETx or
    \ PRUNx or
    T16_CTL1 p!

    \ Set prescale
    0
    P16TONx or
    \ P16TSx_MCLK_DIV_4096 or
    \ P16TSx_MCLK_DIV_1024 or
    \ P16TSx_MCLK_DIV_256 or
    \ P16TSx_MCLK_DIV_64 or
    \ P16TSx_MCLK_DIV_16 or
    \ P16TSx_MCLK_DIV_4 or
    \ P16TSx_MCLK_DIV_2 or
    P16TSx_MCLK_DIV_1 or
    T16_CLKCTL_1 p!

    \ Set count
    dup
    T16_CR1A p!
    T16_CR1B p!

    \ Reset
    T16_CTL1 p@ PRESETx or T16_CTL1 p!

    \ Set PAUSE On
    0
    \ PAUSE5 or
    \ PAUSE4 or
    \ PAUSE3 or
    \ PAUSE2 or
    PAUSE1 or
    \ PAUSE0 or
    T16_CNT_PAUSE p!

    \ Run
    T16_CTL1 p@ PRUNx or T16_CTL1 p!

    \ Set PAUSE Off
    0
    \ PAUSE5 or
    \ PAUSE4 or
    \ PAUSE3 or
    \ PAUSE2 or
    \ PAUSE1 or
    \ PAUSE0 or
    T16_CNT_PAUSE p!
;

: pwm-set ( u -- )
    dup
    0 T16_CR1B p@ within
    if
        T16_CR1A p!
    else
        drop
    then
;

: pwm-get ( -- u )
    T16_CR1A p@
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
    begin
        P6_P6D p@ $07 and
        0=
    until

    3 12 lcd-at-xy
    s" <-- change contrast -->" lcd-type

    10 lcd-text-rows 1- lcd-at-xy
    s" WDT   Off    Exit" lcd-type

    4095 tm1-configure 2048 pwm-set
    0 last-x !

    begin
        1 measure-count +!
        measure-count @ $3ff = if
            0 measure-count !
            read-adc
            battery-display
            thermistor-display
            contrast-display
            0 10 lcd-at-xy
            s" pwm = " lcd-type
            pwm-get 5 lcd-u.r
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
                    - pwm-get + pwm-set
                    last-x !
                then
            then
        then

        button? if
            button
            case
                button-left of
                    wdt-off
                endof
                button-centre of
                    off
                endof
                button-right of
                    exit
                endof
            endcase
        then
    again
;

base !
