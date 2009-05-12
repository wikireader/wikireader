.( led.4th - flash leds )

base @ decimal


.( delay )

: delay ( -- )
  50000 for next ;


.( led )

: led ( u -- )
  invert $04000000 h! ;


.( sweep )

: sweep ( -- )
  1 led delay
  2 led delay
  4 led delay
  2 led delay
;


.( flash )

: flash ( -- )
  1 led delay
  0 led delay
  2 led delay
  0 led delay
  4 led delay
  0 led delay
  2 led delay
  0 led delay
;


.( scan )

: scan ( n -- )
  for aft
    sweep
  then next ;


.( pulsate )

: pulsate ( n -- )
  for aft
    flash
  then next ;


.( run ... )
5 scan  5 pulsate  0 led

.( completed )
base !