.( gpio tests )

base @ decimal


.( wdt-high - force wdt pin high )

: wdt-high ( -- )
  $08 dup P6_P6D p! P6_IOC6 p! ;


.( wdt-low - force wdt pin low )

: wdt-low ( -- )
  wdt-high
  0 P6_P6D p! ;


.( wdt-off - wdt pin high / short delay / then low )

: wdt-off ( -- )
  wdt-high
  500000 0 ?do loop
  wdt-low
  500000 0 ?do loop ;


.( off - normal auto power off )

: off ( -- )
$08 dup P0_IOC0 p! P0_P0D p! ;


.( battery_adc - read battery voltage )

: battery_adc ( -- w )

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

.( bv = display battery mV )
: bv
  cr ." battery = "
  battery_adc dup . ." counts == "
  AVDD_MILLIVOLTS * MULTIPLIER *
  ADC_FULL_SCALE / DIVISOR /
  . ." mV"
;

.( complete )
base !
