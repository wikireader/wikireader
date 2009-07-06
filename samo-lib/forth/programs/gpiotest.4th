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


\ read battery voltage

: battery-adc ( -- w )

  $0f AD_CLKCTL p!
  0 AD_TRIG_CHNL p!
  $01 P7_03_CFP p!
  $0304 AD_EN_SMPL_STAT p!

  AD_EN_SMPL_STAT p@ $02 or AD_EN_SMPL_STAT p!

  begin
    AD_EN_SMPL_STAT  p@ $08 and 0= while
  repeat

  AD_ADD p@ \ -- w

  0 P7_03_CFP p!
  0 AD_EN_SMPL_STAT p!
  0 AD_CLKCTL p!

;

\ values for ADC full scale
3000 constant AVDD_MILLIVOLTS
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

\ display battery mV
: battery-millivolts
    0 8 lcd-at-xy
    s" battery = " lcd-type
    battery-adc dup lcd-number
    s"  counts" lcd-type lcd-cr
    s"           " lcd-type
    AVDD_MILLIVOLTS * MULTIPLIER *
    ADC_FULL_SCALE / DIVISOR /
    lcd-number s"  mV" lcd-type
;

variable measure-count

: test-gpio-main ( -- )
    lcd-cls
    s" GPIO TESTS" lcd-type
    begin
        P6_P6D p@ $07 and
        0=
    until

    10 lcd-text-rows 1- lcd-at-xy
    s" WDT   OFF    EXIT" lcd-type

    begin
        1 measure-count +!
        measure-count @ 2000 = if
            0 measure-count !
            battery-millivolts
        then
        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                wdt-off
                exit
            endof
            $04 of   \ centre button
                off
            endof
            $01 of   \ right button
                exit
            endof
        endcase
    again
;

base !
