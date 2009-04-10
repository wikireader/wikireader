.( gpio tests )
base @


.( wdt-high - force wdt pin high )

: wdt-high ( -- )
  $08 dup P6_P6D p! P6_IOC6 p! ;


.( wdt-low - force wdt pin low )

: wdt-low ( -- )
  $08 P6_P6D p!
  0 P6_IOC6 p! ;


.( wdt-off - wdt pin high / short delay / then low )

: wdt-off ( -- )
  wdt-high
  500000 for next
  wdt-low
  500000 for next ;


.( off - normal auto power off )

: off ( -- )
$08 dup P0_IOC0 p! P0_P0D p! ;


.( complete )
base !
