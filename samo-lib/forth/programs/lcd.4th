.( LCD display  words )

base @ decimal

.( LCD constants )


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
lcd-clear-0

.( lcd-clear-1 )

: lcd-clear-1 ( -- )
  LCD_VRAM_SIZE for aft
    $ff r@ LCD_VRAM + c!
  then next
;
lcd-clear-1


.( lcd-set-pixel )

: lcd-set-pixel ( pixel x y -- )
  LCD_VRAM_WIDTH_BYTES * LCD_VRAM + >r
  8 /mod r> + swap    ( pixel c-addr bit-number )
  $80 swap rshift     ( pixel c-addr bit )
  rot 0= if
    $ff xor over c@   ( c-addr ~bit byte )
    and swap c!
  else
    over c@ or swap c!
  then
;


.( font-8x8 )

base @ hex
create font-8x8
( 0000 ) 00 c, A0 c, 10 c, 80 c, 10 c, 80 c, 50 c, 00 c,
( 0001 ) 00 c, 00 c, 20 c, 70 c, F8 c, 70 c, 20 c, 00 c,
( 0002 ) 50 c, A8 c, 50 c, A8 c, 50 c, A8 c, 50 c, A8 c,
( 0003 ) A0 c, A0 c, E0 c, A0 c, A0 c, 70 c, 20 c, 20 c,
( 0004 ) E0 c, 80 c, C0 c, B8 c, A0 c, 30 c, 20 c, 20 c,
( 0005 ) 60 c, 80 c, 80 c, 60 c, 30 c, 28 c, 30 c, 28 c,
( 0006 ) 80 c, 80 c, 80 c, E0 c, 38 c, 20 c, 30 c, 20 c,
( 0007 ) 00 c, 20 c, 50 c, 20 c, 00 c, 00 c, 00 c, 00 c,
( 0008 ) 00 c, 00 c, 20 c, 70 c, 20 c, 00 c, 70 c, 00 c,
( 0009 ) 90 c, D0 c, B0 c, 90 c, 20 c, 20 c, 20 c, 38 c,
( 000A ) A0 c, A0 c, A0 c, 40 c, 38 c, 10 c, 10 c, 10 c,
( 000B ) 20 c, 20 c, 20 c, E0 c, 00 c, 00 c, 00 c, 00 c,
( 000C ) 00 c, 00 c, 00 c, E0 c, 20 c, 20 c, 20 c, 20 c,
( 000D ) 00 c, 00 c, 00 c, 38 c, 20 c, 20 c, 20 c, 20 c,
( 000E ) 20 c, 20 c, 20 c, 38 c, 00 c, 00 c, 00 c, 00 c,
( 000F ) 20 c, 20 c, 20 c, F8 c, 20 c, 20 c, 20 c, 20 c,
( 0010 ) F8 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0011 ) 00 c, F8 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0012 ) 00 c, 00 c, 00 c, F8 c, 00 c, 00 c, 00 c, 00 c,
( 0013 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, F8 c, 00 c,
( 0014 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, F8 c,
( 0015 ) 20 c, 20 c, 20 c, 38 c, 20 c, 20 c, 20 c, 20 c,
( 0016 ) 20 c, 20 c, 20 c, E0 c, 20 c, 20 c, 20 c, 20 c,
( 0017 ) 20 c, 20 c, 20 c, F8 c, 00 c, 00 c, 00 c, 00 c,
( 0018 ) 00 c, 00 c, 00 c, F8 c, 20 c, 20 c, 20 c, 20 c,
( 0019 ) 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c,
( 001A ) 00 c, 10 c, 20 c, 40 c, 20 c, 10 c, 70 c, 00 c,
( 001B ) 00 c, 40 c, 20 c, 10 c, 20 c, 40 c, 70 c, 00 c,
( 001C ) 00 c, 00 c, 00 c, F8 c, 50 c, 50 c, 50 c, 00 c,
( 001D ) 00 c, 00 c, 20 c, F0 c, 60 c, F0 c, 40 c, 00 c,
( 001E ) 00 c, 20 c, 50 c, E0 c, 40 c, 50 c, A0 c, 00 c,
( 001F ) 00 c, 00 c, 00 c, 00 c, 20 c, 00 c, 00 c, 00 c,
( 0020 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0021 ) 00 c, 20 c, 20 c, 20 c, 20 c, 00 c, 20 c, 00 c,
( 0022 ) 00 c, 50 c, 50 c, 50 c, 00 c, 00 c, 00 c, 00 c,
( 0023 ) 50 c, 50 c, F8 c, 50 c, F8 c, 50 c, 50 c, 00 c,
( 0024 ) 20 c, 70 c, A0 c, 70 c, 28 c, 70 c, 20 c, 00 c,
( 0025 ) 00 c, 40 c, 50 c, 20 c, 50 c, 10 c, 00 c, 00 c,
( 0026 ) 40 c, A0 c, A0 c, 40 c, A0 c, A0 c, 50 c, 00 c,
( 0027 ) 00 c, 20 c, 20 c, 20 c, 00 c, 00 c, 00 c, 00 c,
( 0028 ) 00 c, 20 c, 40 c, 40 c, 40 c, 40 c, 20 c, 00 c,
( 0029 ) 00 c, 40 c, 20 c, 20 c, 20 c, 20 c, 40 c, 00 c,
( 002A ) 00 c, 00 c, 90 c, 60 c, F0 c, 60 c, 90 c, 00 c,
( 002B ) 00 c, 00 c, 20 c, 20 c, F8 c, 20 c, 20 c, 00 c,
( 002C ) 00 c, 00 c, 00 c, 00 c, 00 c, 30 c, 20 c, 40 c,
( 002D ) 00 c, 00 c, 00 c, 00 c, F0 c, 00 c, 00 c, 00 c,
( 002E ) 00 c, 00 c, 00 c, 00 c, 00 c, 20 c, 70 c, 20 c,
( 002F ) 00 c, 10 c, 10 c, 20 c, 40 c, 80 c, 80 c, 00 c,
( 0030 ) 00 c, 20 c, 50 c, 50 c, 50 c, 50 c, 20 c, 00 c,
( 0031 ) 00 c, 20 c, 60 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 0032 ) 00 c, 60 c, 90 c, 10 c, 60 c, 80 c, F0 c, 00 c,
( 0033 ) 00 c, F0 c, 20 c, 60 c, 10 c, 90 c, 60 c, 00 c,
( 0034 ) 00 c, 20 c, 60 c, A0 c, F0 c, 20 c, 20 c, 00 c,
( 0035 ) 00 c, F0 c, 80 c, E0 c, 10 c, 90 c, 60 c, 00 c,
( 0036 ) 00 c, 60 c, 80 c, E0 c, 90 c, 90 c, 60 c, 00 c,
( 0037 ) 00 c, F0 c, 10 c, 20 c, 20 c, 40 c, 40 c, 00 c,
( 0038 ) 00 c, 60 c, 90 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 0039 ) 00 c, 60 c, 90 c, 90 c, 70 c, 10 c, 60 c, 00 c,
( 003A ) 00 c, 00 c, 60 c, 60 c, 00 c, 60 c, 60 c, 00 c,
( 003B ) 00 c, 00 c, 30 c, 30 c, 00 c, 30 c, 20 c, 40 c,
( 003C ) 00 c, 10 c, 20 c, 40 c, 40 c, 20 c, 10 c, 00 c,
( 003D ) 00 c, 00 c, 00 c, F0 c, 00 c, F0 c, 00 c, 00 c,
( 003E ) 00 c, 40 c, 20 c, 10 c, 10 c, 20 c, 40 c, 00 c,
( 003F ) 00 c, 20 c, 50 c, 10 c, 20 c, 00 c, 20 c, 00 c,
( 0040 ) 30 c, 48 c, 98 c, A8 c, A8 c, 90 c, 40 c, 30 c,
( 0041 ) 00 c, 60 c, 90 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 0042 ) 00 c, E0 c, 90 c, E0 c, 90 c, 90 c, E0 c, 00 c,
( 0043 ) 00 c, 60 c, 90 c, 80 c, 80 c, 90 c, 60 c, 00 c,
( 0044 ) 00 c, E0 c, 90 c, 90 c, 90 c, 90 c, E0 c, 00 c,
( 0045 ) 00 c, F0 c, 80 c, E0 c, 80 c, 80 c, F0 c, 00 c,
( 0046 ) 00 c, F0 c, 80 c, E0 c, 80 c, 80 c, 80 c, 00 c,
( 0047 ) 00 c, 60 c, 90 c, 80 c, B0 c, 90 c, 60 c, 00 c,
( 0048 ) 00 c, 90 c, 90 c, F0 c, 90 c, 90 c, 90 c, 00 c,
( 0049 ) 00 c, 70 c, 20 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 004A ) 00 c, 70 c, 20 c, 20 c, 20 c, A0 c, 40 c, 00 c,
( 004B ) 00 c, 90 c, A0 c, C0 c, A0 c, A0 c, 90 c, 00 c,
( 004C ) 00 c, 80 c, 80 c, 80 c, 80 c, 80 c, F0 c, 00 c,
( 004D ) 00 c, 90 c, F0 c, F0 c, 90 c, 90 c, 90 c, 00 c,
( 004E ) 00 c, 90 c, D0 c, F0 c, B0 c, B0 c, 90 c, 00 c,
( 004F ) 00 c, 60 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 0050 ) 00 c, E0 c, 90 c, 90 c, E0 c, 80 c, 80 c, 00 c,
( 0051 ) 00 c, 60 c, 90 c, 90 c, D0 c, B0 c, 60 c, 10 c,
( 0052 ) 00 c, E0 c, 90 c, 90 c, E0 c, 90 c, 90 c, 00 c,
( 0053 ) 00 c, 60 c, 90 c, 40 c, 20 c, 90 c, 60 c, 00 c,
( 0054 ) 00 c, 70 c, 20 c, 20 c, 20 c, 20 c, 20 c, 00 c,
( 0055 ) 00 c, 90 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 0056 ) 00 c, 90 c, 90 c, 90 c, 90 c, 60 c, 60 c, 00 c,
( 0057 ) 00 c, 90 c, 90 c, 90 c, F0 c, F0 c, 90 c, 00 c,
( 0058 ) 00 c, 90 c, 90 c, 60 c, 60 c, 90 c, 90 c, 00 c,
( 0059 ) 00 c, 88 c, 88 c, 50 c, 20 c, 20 c, 20 c, 00 c,
( 005A ) 00 c, F0 c, 10 c, 20 c, 40 c, 80 c, F0 c, 00 c,
( 005B ) 00 c, 70 c, 40 c, 40 c, 40 c, 40 c, 70 c, 00 c,
( 005C ) 00 c, 80 c, 80 c, 40 c, 20 c, 10 c, 10 c, 00 c,
( 005D ) 00 c, 70 c, 10 c, 10 c, 10 c, 10 c, 70 c, 00 c,
( 005E ) 00 c, 20 c, 50 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 005F ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, F0 c,
( 0060 ) 00 c, 40 c, 20 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0061 ) 00 c, 00 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 0062 ) 00 c, 80 c, 80 c, E0 c, 90 c, 90 c, E0 c, 00 c,
( 0063 ) 00 c, 00 c, 00 c, 30 c, 40 c, 40 c, 30 c, 00 c,
( 0064 ) 00 c, 10 c, 10 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 0065 ) 00 c, 00 c, 00 c, 60 c, B0 c, C0 c, 60 c, 00 c,
( 0066 ) 00 c, 20 c, 50 c, 40 c, E0 c, 40 c, 40 c, 00 c,
( 0067 ) 00 c, 00 c, 00 c, 60 c, 90 c, 70 c, 10 c, 60 c,
( 0068 ) 00 c, 80 c, 80 c, E0 c, 90 c, 90 c, 90 c, 00 c,
( 0069 ) 00 c, 20 c, 00 c, 60 c, 20 c, 20 c, 70 c, 00 c,
( 006A ) 00 c, 10 c, 00 c, 10 c, 10 c, 10 c, 50 c, 20 c,
( 006B ) 00 c, 80 c, 80 c, 90 c, E0 c, 90 c, 90 c, 00 c,
( 006C ) 00 c, 60 c, 20 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 006D ) 00 c, 00 c, 00 c, D0 c, A8 c, A8 c, A8 c, 00 c,
( 006E ) 00 c, 00 c, 00 c, E0 c, 90 c, 90 c, 90 c, 00 c,
( 006F ) 00 c, 00 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 0070 ) 00 c, 00 c, 00 c, E0 c, 90 c, E0 c, 80 c, 80 c,
( 0071 ) 00 c, 00 c, 00 c, 70 c, 90 c, 70 c, 10 c, 10 c,
( 0072 ) 00 c, 00 c, 00 c, A0 c, D0 c, 80 c, 80 c, 00 c,
( 0073 ) 00 c, 00 c, 00 c, 30 c, 60 c, 10 c, 60 c, 00 c,
( 0074 ) 00 c, 40 c, 40 c, E0 c, 40 c, 50 c, 20 c, 00 c,
( 0075 ) 00 c, 00 c, 00 c, 90 c, 90 c, 90 c, 70 c, 00 c,
( 0076 ) 00 c, 00 c, 00 c, 50 c, 50 c, 50 c, 20 c, 00 c,
( 0077 ) 00 c, 00 c, 00 c, 88 c, A8 c, A8 c, 50 c, 00 c,
( 0078 ) 00 c, 00 c, 00 c, 90 c, 60 c, 60 c, 90 c, 00 c,
( 0079 ) 00 c, 00 c, 00 c, 90 c, 90 c, 70 c, 90 c, 60 c,
( 007A ) 00 c, 00 c, 00 c, F0 c, 20 c, 40 c, F0 c, 00 c,
( 007B ) 30 c, 40 c, 20 c, C0 c, 20 c, 40 c, 30 c, 00 c,
( 007C ) 00 c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 00 c,
( 007D ) C0 c, 20 c, 40 c, 30 c, 40 c, 20 c, C0 c, 00 c,
( 007E ) 00 c, 50 c, A0 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A0 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A1 ) 00 c, 20 c, 00 c, 20 c, 20 c, 20 c, 20 c, 00 c,
( 00A2 ) 00 c, 00 c, 20 c, 70 c, A0 c, A0 c, 70 c, 20 c,
( 00A3 ) 00 c, 20 c, 50 c, E0 c, 40 c, 50 c, A0 c, 00 c,
( 00A4 ) 00 c, 00 c, 88 c, 70 c, 50 c, 70 c, 88 c, 00 c,
( 00A5 ) 00 c, 88 c, 50 c, F8 c, 20 c, F8 c, 20 c, 00 c,
( 00A6 ) 20 c, 20 c, 20 c, 00 c, 20 c, 20 c, 20 c, 00 c,
( 00A7 ) 70 c, 80 c, E0 c, 90 c, 70 c, 10 c, E0 c, 00 c,
( 00A8 ) 00 c, 50 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A9 ) 00 c, 70 c, A8 c, C8 c, C8 c, A8 c, 70 c, 00 c,
( 00AA ) 30 c, 50 c, 30 c, 00 c, 70 c, 00 c, 00 c, 00 c,
( 00AB ) 00 c, 00 c, 00 c, 50 c, A0 c, 50 c, 00 c, 00 c,
( 00AC ) 00 c, 00 c, 00 c, 00 c, 70 c, 10 c, 10 c, 00 c,
( 00AD ) 00 c, 00 c, 00 c, 00 c, 70 c, 00 c, 00 c, 00 c,
( 00AE ) 00 c, 70 c, E8 c, D8 c, E8 c, D8 c, 70 c, 00 c,
( 00AF ) 00 c, 70 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B0 ) 00 c, 20 c, 50 c, 20 c, 00 c, 00 c, 00 c, 00 c,
( 00B1 ) 00 c, 00 c, 20 c, 70 c, 20 c, 00 c, 70 c, 00 c,
( 00B2 ) 20 c, 50 c, 10 c, 20 c, 70 c, 00 c, 00 c, 00 c,
( 00B3 ) 60 c, 10 c, 60 c, 10 c, 60 c, 00 c, 00 c, 00 c,
( 00B4 ) 00 c, 20 c, 40 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B5 ) 00 c, 00 c, 00 c, 90 c, 90 c, 90 c, E0 c, 80 c,
( 00B6 ) 00 c, 78 c, E8 c, E8 c, 68 c, 28 c, 28 c, 00 c,
( 00B7 ) 00 c, 00 c, 00 c, 00 c, 20 c, 00 c, 00 c, 00 c,
( 00B8 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 20 c, 40 c,
( 00B9 ) 20 c, 60 c, 20 c, 20 c, 70 c, 00 c, 00 c, 00 c,
( 00BA ) 20 c, 50 c, 20 c, 00 c, 70 c, 00 c, 00 c, 00 c,
( 00BB ) 00 c, 00 c, 00 c, A0 c, 50 c, A0 c, 00 c, 00 c,
( 00BC ) 80 c, 80 c, 80 c, A0 c, 60 c, F0 c, 20 c, 00 c,
( 00BD ) 80 c, 80 c, A0 c, D0 c, 10 c, 20 c, 70 c, 00 c,
( 00BE ) 80 c, 40 c, 80 c, 60 c, A0 c, F0 c, 20 c, 00 c,
( 00BF ) 00 c, 20 c, 00 c, 20 c, 40 c, 50 c, 20 c, 00 c,
( 00C0 ) 40 c, 20 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C1 ) 20 c, 40 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C2 ) 60 c, 90 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C3 ) 50 c, A0 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C4 ) 90 c, 00 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C5 ) 60 c, 90 c, 60 c, 90 c, F0 c, 90 c, 90 c, 00 c,
( 00C6 ) 00 c, 70 c, A0 c, A0 c, F0 c, A0 c, B0 c, 00 c,
( 00C7 ) 00 c, 60 c, 90 c, 80 c, 80 c, 90 c, 60 c, 40 c,
( 00C8 ) 40 c, 20 c, F0 c, 80 c, E0 c, 80 c, F0 c, 00 c,
( 00C9 ) 20 c, 40 c, F0 c, 80 c, E0 c, 80 c, F0 c, 00 c,
( 00CA ) 60 c, 90 c, F0 c, 80 c, E0 c, 80 c, F0 c, 00 c,
( 00CB ) 90 c, 00 c, F0 c, 80 c, E0 c, 80 c, F0 c, 00 c,
( 00CC ) 40 c, 20 c, 70 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 00CD ) 10 c, 20 c, 70 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 00CE ) 20 c, 50 c, 70 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 00CF ) 50 c, 00 c, 70 c, 20 c, 20 c, 20 c, 70 c, 00 c,
( 00D0 ) 00 c, 70 c, 48 c, E8 c, 48 c, 48 c, 70 c, 00 c,
( 00D1 ) 50 c, A0 c, 90 c, D0 c, B0 c, 90 c, 90 c, 00 c,
( 00D2 ) 40 c, 20 c, 60 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00D3 ) 20 c, 40 c, 60 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00D4 ) 60 c, 90 c, 60 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00D5 ) 50 c, A0 c, 60 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00D6 ) 90 c, 00 c, 60 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00D7 ) 00 c, 00 c, 00 c, 00 c, 50 c, 20 c, 50 c, 00 c,
( 00D8 ) 00 c, 70 c, B0 c, B0 c, D0 c, D0 c, E0 c, 00 c,
( 00D9 ) 40 c, 20 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00DA ) 20 c, 40 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00DB ) 60 c, 90 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00DC ) 90 c, 00 c, 90 c, 90 c, 90 c, 90 c, 60 c, 00 c,
( 00DD ) 10 c, 20 c, 88 c, 50 c, 20 c, 20 c, 20 c, 00 c,
( 00DE ) 00 c, 80 c, E0 c, 90 c, 90 c, E0 c, 80 c, 00 c,
( 00DF ) 00 c, 60 c, 90 c, A0 c, A0 c, 90 c, A0 c, 00 c,
( 00E0 ) 40 c, 20 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E1 ) 20 c, 40 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E2 ) 20 c, 50 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E3 ) 50 c, A0 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E4 ) 00 c, 50 c, 00 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E5 ) 60 c, 90 c, 60 c, 70 c, 90 c, 90 c, 70 c, 00 c,
( 00E6 ) 00 c, 00 c, 00 c, F0 c, 68 c, B0 c, 78 c, 00 c,
( 00E7 ) 00 c, 00 c, 00 c, 30 c, 40 c, 40 c, 30 c, 20 c,
( 00E8 ) 40 c, 20 c, 00 c, 60 c, B0 c, C0 c, 60 c, 00 c,
( 00E9 ) 20 c, 40 c, 00 c, 60 c, B0 c, C0 c, 60 c, 00 c,
( 00EA ) 60 c, 90 c, 00 c, 60 c, B0 c, C0 c, 60 c, 00 c,
( 00EB ) 00 c, 50 c, 00 c, 60 c, B0 c, C0 c, 60 c, 00 c,
( 00EC ) 40 c, 20 c, 00 c, 60 c, 20 c, 20 c, 70 c, 00 c,
( 00ED ) 10 c, 20 c, 00 c, 60 c, 20 c, 20 c, 70 c, 00 c,
( 00EE ) 20 c, 50 c, 00 c, 60 c, 20 c, 20 c, 70 c, 00 c,
( 00EF ) 00 c, 50 c, 00 c, 60 c, 20 c, 20 c, 70 c, 00 c,
( 00F0 ) A0 c, 40 c, A0 c, 10 c, 70 c, 90 c, 60 c, 00 c,
( 00F1 ) 50 c, A0 c, 00 c, E0 c, 90 c, 90 c, 90 c, 00 c,
( 00F2 ) 40 c, 20 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 00F3 ) 20 c, 40 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 00F4 ) 60 c, 90 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 00F5 ) 50 c, A0 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 00F6 ) 00 c, 90 c, 00 c, 60 c, 90 c, 90 c, 60 c, 00 c,
( 00F7 ) 00 c, 00 c, 20 c, 00 c, 70 c, 00 c, 20 c, 00 c,
( 00F8 ) 00 c, 00 c, 00 c, 70 c, B0 c, D0 c, E0 c, 00 c,
( 00F9 ) 40 c, 20 c, 00 c, 90 c, 90 c, 90 c, 70 c, 00 c,
( 00FA ) 20 c, 40 c, 00 c, 90 c, 90 c, 90 c, 70 c, 00 c,
( 00FB ) 60 c, 90 c, 00 c, 90 c, 90 c, 90 c, 70 c, 00 c,
( 00FC ) 00 c, 90 c, 00 c, 90 c, 90 c, 90 c, 70 c, 00 c,
( 00FD ) 20 c, 40 c, 00 c, 90 c, 90 c, 70 c, 90 c, 60 c,
( 00FE ) 00 c, 80 c, 80 c, E0 c, 90 c, E0 c, 80 c, 80 c,
( 00FF ) 00 c, 90 c, 00 c, 90 c, 90 c, 70 c, 90 c, 60 c,
base !


variable lcd-x
variable lcd-y


.( lcd-at-xy )

: lcd-at-xy ( x y -- )
  lcd-y ! lcd-x ! ;


.( lcd-home )

: lcd-home ( -- )
  0 0 lcd-at-xy ;


.( lcd-cls )

: lcd-cls ( -- )
  lcd-clear-0 lcd-home ;
lcd-cls


.( lcd-cr )

: lcd-cr ( -- )
  0 lcd-x !
  lcd-y @ 8 + dup LCD_HEIGHT_LINES 1- > if
    drop 0
  then
  lcd-y !
;


.( lcd-char )

: lcd-char ( c -- )
  lcd-y @ LCD_VRAM_WIDTH_BYTES * LCD_VRAM +
  lcd-x @ 3 rshift +   ( c c-addr )
  swap                 ( c-addr c )
  3 lshift font-8x8 +  ( lcd-addr font-addr )
  8 for aft
    2dup c@ swap c!
    char+ swap LCD_VRAM_WIDTH_BYTES + swap
  then next 2drop
  lcd-x @ 8 + dup LCD_WIDTH_PIXELS 1- > if
    drop lcd-cr
  else
    lcd-x !
  then
;


.( lcd-type )

: lcd-type ( caddr u -- )
  for aft
    dup c@ lcd-char char+
  then next drop ;


.( lcd-number )

: lcd-number ( x y u -- )
  <# # # # # # # # # #> lcd-type ;


.( complete )
base !
