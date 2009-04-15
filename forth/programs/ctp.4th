.( ctp tests )
include regs.4th

base @ decimal


$01 constant RDBFx


38400 constant ctp-bps
48000000 constant cpu-clock
8 constant divmd
cpu-clock divmd /
ctp-bps 2* / 1-
constant ctp-brtrd

.( ctp-init )

: ctp-init ( -- )
  $c3 EFSIF1_CTL p!     \ 8-bit async, no parity, internal clock, 1 stop bit
  $10 EFSIF1_IRDA p!    \ DIVMD = 1/8, General I/F mode
  ctp-brtrd dup EFSIF1_BRTRDL p!
  256 / EFSIF1_BRTRDM p!
  $01 EFSIF1_BRTRUN p!  \ enable
  $01 P0_47_CFP p!      \ rx only ( tx => + 4 )
;


.( ctp-ready )

: ctp-ready ( -- f )
  EFSIF1_STATUS p@ RDBFx and 0<>
;

.( ctp-get )

: ctp-get ( -- w )
  begin ctp-ready until
  EFSIF1_RXD p@
;


.( ctp )

: ctp ( -- )
  ctp-init hex
  begin
    ctp-get emit
    \ 3 u.r
  enough? until
  decimal
;

ctp

.( complete )
base !