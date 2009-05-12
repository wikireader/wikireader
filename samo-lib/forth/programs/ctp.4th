.( ctp tests )
include regs.4th

base @ decimal


$01 constant RDBFx


\ 38400 constant ctp-bps
9600 constant ctp-bps
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


.( ctp-test )

: ctp-test ( -- )
  ctp-init hex
  begin
    ctp-get \ emit
    3 u.r
  enough? until
  decimal
;


.( ctp-read )

variable ctp-x
variable ctp-y


: ctp-read ( -- x y )
  0 0
  begin
    2drop
    begin
      ctp-get $aa =
    until
    ctp-get 8 lshift ctp-get or 2/
    ctp-get 8 lshift ctp-get or 2/
    ctp-get 1 =
  until
;


.( LCD functions )


240 constant LCD_WIDTH_PIXELS
208 constant LCD_HEIGHT_LINES

LCD_WIDTH_PIXELS 31 + 32 / 32 * constant LCD_VRAM_WIDTH_PIXELS
LCD_HEIGHT_LINES constant LCD_VRAM_HEIGHT_LINES
LCD_VRAM_WIDTH_PIXELS 8 / constant LCD_VRAM_WIDTH_BYTES
LCD_WIDTH_PIXELS 8 / constant LCD_WIDTH_BYTES

$80000 constant LCD_VRAM
LCD_VRAM_WIDTH_BYTES LCD_VRAM_HEIGHT_LINES * constant LCD_VRAM_SIZE


.( lcd-clear-0 )

: lcd-clear-0 ( -- )
  LCD_VRAM_SIZE for aft
    0 r@ LCD_VRAM + c!
  then next
;


.( lcd-clear-1 )

: lcd-clear-1 ( -- )
  LCD_VRAM_SIZE for aft
    $ff r@ LCD_VRAM + c!
  then next
;


.( lcd-set-pixel )

: lcd-set-pixel ( x y c -- )
  >r                  ( x y )
  LCD_VRAM_WIDTH_BYTES * LCD_VRAM + >r
  8 /mod r> + swap    ( c-addr bit-number )
  $80 swap rshift     ( c-addr bit )
  r> 0= if
    $ff xor over c@   ( c-addr ~bit byte )
    and swap c!
  else
    over c@ or swap c!
  then
;


.( draw )

variable pixel

: draw ( -- )
  ctp-init
  lcd-clear-0
  1 pixel !
  begin
    ctp-read
    pixel @ lcd-set-pixel
    P6_P6D p@ $01 and 0<> if
      lcd-clear-0
      1 pixel !
    then
    P6_P6D p@ $02 and 0<> if
      lcd-clear-1
      0 pixel !
    then
  enough? until
;

draw

.( complete )
base !
