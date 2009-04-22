.( lcd tests )
include regs.4th

base @ decimal

320 constant LCD-WIDTH
240 constant LCD-HEIGHT

$00080000 constant LCD-VRAM

LCD-WIDTH LCD-HEIGHT * 8 / constant LCD-VRAM-SIZE

: TFT-CTL1-LOW ( -- )
   P8_P8D p@ $f7 and P8_P8D p!
;
: TFT-CTL1-HIGH ( -- )
   P8_P8D p@ $08 or P8_P8D p!
;


.( lcd-init )

: lcd-init ( -- )
  TFT-CTL1-LOW

  \ disable write protection of clock registers
  $96 CMU_PROTECT p!
  CMU_GATEDCLK0 p@ $07 or CMU_GATEDCLK0 p!

  \ re-enable write protection of clock registers
  $00 CMU_PROTECT p!

  \ power down LCDC
  $00000000 LCDC_PS p!

  \ HT = (47+1) * 8 = 384 characters, HDP = (39+1) * 8 = 320 characters
  $002f0027 LCDC_HD p!

  \ VT = 244 + 1 = 255 lines, VDP = 239 + 1 = 480 lines
  $00f400ef LCDC_VD p!

  \ wf counter = 0
  $0 LCDC_MR p!

  \ LCDC Display Mode Register
  \ monochrome
  $22000010 LCDC_DMD p!

  \ greyscale
  \ $22000012 LCDC_DMD p!

  \ frame buffer RAM
  LCD-VRAM LCDC_MADD p!

  \ set reg_power_save = 11b (normal mode)
  LCDC_PS p@ $00000003 or LCDC_PS p!

  10000 delay-us

  \ LCDC on
  TFT-CTL1-HIGH
;


.( lcd-zero )

: lcd-zero ( -- )
  LCD-VRAM LCD-VRAM-SIZE 0 fill
;

.( lcd-ff )

: lcd-ff ( -- )
  LCD-VRAM LCD-VRAM-SIZE $ff fill
;

.( lcd-flash )
: lcd-flash ( -- )
  cr lcd-init
  10 for aft
    lcd-zero
    400000 delay-us
    lcd-ff
    400000 delay-us
    [char] . emit
  then next
;

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
  \ dup bl > if dup emit then
;

variable ctp-x
variable ctp-y

: ctp-clear ( -- )
  0 dup ctp-x ! ctp-y !
;

: ctp-digit ( a -- f )
  ctp-get 16 digit? if
    ( a u ) over @ 16 * + swap !
    true
  else 2drop false
  then
;

: ctp-packet ( -- x y )
  0
  begin
    dup
    case
      0 of ctp-clear ctp-get [char] A = if 1+ then endof
      1 of ctp-get [char] A = if 1+ else 1- then endof

      2 of ctp-x ctp-digit if 1+ else drop 0 then endof
      3 of ctp-x ctp-digit if 1+ else drop 0 then endof
      4 of ctp-x ctp-digit if 1+ else drop 0 then endof
      5 of ctp-x ctp-digit if 1+ else drop 0 then endof

      6 of ctp-y ctp-digit if 1+ else drop 0 then endof
      7 of ctp-y ctp-digit if 1+ else drop 0 then endof
      8 of ctp-y ctp-digit if 1+ else drop 0 then endof
      9 of ctp-y ctp-digit if 1+ else drop 0 then endof

      10 of ctp-get [char] 0 = if 1+ else drop 0 then endof
      11 of ctp-get [char] 1 = if drop
                                  ctp-x @
                                  ctp-y @
                                  exit then endof
      0 swap
    endcase
  again
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


.( cursor )

: cursor ( -- )
  ctp-init
  lcd-init
  lcd-ff
  begin
    ctp-packet 2dup swap . . cr
    32 * swap 8 / + $3fff and
    LCD-VRAM + $00 swap c!
  enough? until
;


.( complete )
base !
