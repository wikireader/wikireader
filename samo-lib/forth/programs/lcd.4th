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

: lcd-set-pixel ( x y -- )
  LCD_VRAM_WIDTH_BYTES * LCD_VRAM + >r
  8 /mod r> + swap    ( c-addr bit-number )
  $80 swap rshift     ( c-addr bit )
  over c@ or swap c!
;

: lcd-clear-pixel ( x y -- )
  LCD_VRAM_WIDTH_BYTES * LCD_VRAM + >r
  8 /mod r> + swap    ( c-addr bit-number )
  $80 swap rshift     ( c-addr bit )
  $ff xor over c@     ( c-addr ~bit byte )
  and swap c!
;

: lcd-set-point ( x y -- )
  2dup lcd-set-pixel
  2dup 1+ lcd-set-pixel
  2dup 1- lcd-set-pixel
  2dup 2 + lcd-set-pixel
  2dup 2 - lcd-set-pixel
  2dup swap 1+ swap lcd-set-pixel
  2dup swap 1- swap lcd-set-pixel
  2dup swap 2 + swap lcd-set-pixel
  2dup swap 2 - swap lcd-set-pixel
  2drop ;

.( font-8x13 )

base @ hex
create font-8x13
( 0000 ) 00 c, 00 c, AA c, 00 c, 82 c, 00 c, 82 c, 00 c, 82 c, 00 c, AA c, 00 c, 00 c,
( 0001 ) 00 c, 00 c, 00 c, 10 c, 38 c, 7C c, FE c, 7C c, 38 c, 10 c, 00 c, 00 c, 00 c,
( 0002 ) AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c, 55 c, AA c,
( 0003 ) 00 c, 00 c, A0 c, A0 c, E0 c, A0 c, AE c, 04 c, 04 c, 04 c, 04 c, 00 c, 00 c,
( 0004 ) 00 c, 00 c, E0 c, 80 c, C0 c, 80 c, 8E c, 08 c, 0C c, 08 c, 08 c, 00 c, 00 c,
( 0005 ) 00 c, 00 c, 60 c, 80 c, 80 c, 80 c, 6C c, 0A c, 0C c, 0A c, 0A c, 00 c, 00 c,
( 0006 ) 00 c, 00 c, 80 c, 80 c, 80 c, 80 c, EE c, 08 c, 0C c, 08 c, 08 c, 00 c, 00 c,
( 0007 ) 00 c, 00 c, 18 c, 24 c, 24 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0008 ) 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 7C c, 00 c, 00 c, 00 c,
( 0009 ) 00 c, 00 c, C0 c, A0 c, A0 c, A0 c, A8 c, 08 c, 08 c, 08 c, 0E c, 00 c, 00 c,
( 000A ) 00 c, 00 c, 88 c, 88 c, 50 c, 50 c, 2E c, 04 c, 04 c, 04 c, 04 c, 00 c, 00 c,
( 000B ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, F0 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 000C ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, F0 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 000D ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 1F c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 000E ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 1F c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 000F ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, FF c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0010 ) FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0011 ) 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0012 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0013 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 00 c, 00 c, 00 c,
( 0014 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c,
( 0015 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 1F c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0016 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, F0 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0017 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, FF c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0018 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FF c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 0019 ) 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c,
( 001A ) 00 c, 00 c, 00 c, 00 c, 0E c, 30 c, C0 c, 30 c, 0E c, 00 c, FE c, 00 c, 00 c,
( 001B ) 00 c, 00 c, 00 c, 00 c, E0 c, 18 c, 06 c, 18 c, E0 c, 00 c, FE c, 00 c, 00 c,
( 001C ) 00 c, 00 c, 00 c, 00 c, 00 c, FE c, 44 c, 44 c, 44 c, 44 c, 44 c, 00 c, 00 c,
( 001D ) 00 c, 00 c, 00 c, 04 c, 04 c, 7E c, 08 c, 10 c, 7E c, 20 c, 20 c, 00 c, 00 c,
( 001E ) 00 c, 00 c, 1C c, 22 c, 20 c, 70 c, 20 c, 20 c, 20 c, 62 c, DC c, 00 c, 00 c,
( 001F ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0020 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0021 ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 10 c, 00 c, 00 c,
( 0022 ) 00 c, 00 c, 24 c, 24 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0023 ) 00 c, 00 c, 00 c, 24 c, 24 c, 7E c, 24 c, 7E c, 24 c, 24 c, 00 c, 00 c, 00 c,
( 0024 ) 00 c, 00 c, 10 c, 3C c, 50 c, 50 c, 38 c, 14 c, 14 c, 78 c, 10 c, 00 c, 00 c,
( 0025 ) 00 c, 00 c, 22 c, 52 c, 24 c, 08 c, 08 c, 10 c, 24 c, 2A c, 44 c, 00 c, 00 c,
( 0026 ) 00 c, 00 c, 00 c, 00 c, 30 c, 48 c, 48 c, 30 c, 4A c, 44 c, 3A c, 00 c, 00 c,
( 0027 ) 00 c, 00 c, 10 c, 10 c, 10 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0028 ) 00 c, 00 c, 04 c, 08 c, 08 c, 10 c, 10 c, 10 c, 08 c, 08 c, 04 c, 00 c, 00 c,
( 0029 ) 00 c, 00 c, 20 c, 10 c, 10 c, 08 c, 08 c, 08 c, 10 c, 10 c, 20 c, 00 c, 00 c,
( 002A ) 00 c, 00 c, 24 c, 18 c, 7E c, 18 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 002B ) 00 c, 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 00 c, 00 c, 00 c,
( 002C ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 38 c, 30 c, 40 c, 00 c,
( 002D ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 7C c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 002E ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c,
( 002F ) 00 c, 00 c, 02 c, 02 c, 04 c, 08 c, 10 c, 20 c, 40 c, 80 c, 80 c, 00 c, 00 c,
( 0030 ) 00 c, 00 c, 18 c, 24 c, 42 c, 42 c, 42 c, 42 c, 42 c, 24 c, 18 c, 00 c, 00 c,
( 0031 ) 00 c, 00 c, 10 c, 30 c, 50 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 0032 ) 00 c, 00 c, 3C c, 42 c, 42 c, 02 c, 04 c, 18 c, 20 c, 40 c, 7E c, 00 c, 00 c,
( 0033 ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 1C c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0034 ) 00 c, 00 c, 04 c, 0C c, 14 c, 24 c, 44 c, 44 c, 7E c, 04 c, 04 c, 00 c, 00 c,
( 0035 ) 00 c, 00 c, 7E c, 40 c, 40 c, 5C c, 62 c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0036 ) 00 c, 00 c, 1C c, 20 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0037 ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 08 c, 10 c, 10 c, 20 c, 20 c, 00 c, 00 c,
( 0038 ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 3C c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0039 ) 00 c, 00 c, 3C c, 42 c, 42 c, 46 c, 3A c, 02 c, 02 c, 04 c, 38 c, 00 c, 00 c,
( 003A ) 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c,
( 003B ) 00 c, 00 c, 00 c, 00 c, 10 c, 38 c, 10 c, 00 c, 00 c, 38 c, 30 c, 40 c, 00 c,
( 003C ) 00 c, 00 c, 02 c, 04 c, 08 c, 10 c, 20 c, 10 c, 08 c, 04 c, 02 c, 00 c, 00 c,
( 003D ) 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 00 c, 00 c, 7E c, 00 c, 00 c, 00 c, 00 c,
( 003E ) 00 c, 00 c, 40 c, 20 c, 10 c, 08 c, 04 c, 08 c, 10 c, 20 c, 40 c, 00 c, 00 c,
( 003F ) 00 c, 00 c, 3C c, 42 c, 42 c, 02 c, 04 c, 08 c, 08 c, 00 c, 08 c, 00 c, 00 c,
( 0040 ) 00 c, 00 c, 3C c, 42 c, 42 c, 4E c, 52 c, 56 c, 4A c, 40 c, 3C c, 00 c, 00 c,
( 0041 ) 00 c, 00 c, 18 c, 24 c, 42 c, 42 c, 42 c, 7E c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0042 ) 00 c, 00 c, 78 c, 44 c, 42 c, 44 c, 78 c, 44 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 0043 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 40 c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0044 ) 00 c, 00 c, 78 c, 44 c, 42 c, 42 c, 42 c, 42 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 0045 ) 00 c, 00 c, 7E c, 40 c, 40 c, 40 c, 78 c, 40 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 0046 ) 00 c, 00 c, 7E c, 40 c, 40 c, 40 c, 78 c, 40 c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 0047 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 4E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0048 ) 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 7E c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0049 ) 00 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 004A ) 00 c, 00 c, 1F c, 04 c, 04 c, 04 c, 04 c, 04 c, 04 c, 44 c, 38 c, 00 c, 00 c,
( 004B ) 00 c, 00 c, 42 c, 44 c, 48 c, 50 c, 60 c, 50 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 004C ) 00 c, 00 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 004D ) 00 c, 00 c, 82 c, 82 c, C6 c, AA c, 92 c, 92 c, 82 c, 82 c, 82 c, 00 c, 00 c,
( 004E ) 00 c, 00 c, 42 c, 42 c, 62 c, 52 c, 4A c, 46 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 004F ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0050 ) 00 c, 00 c, 7C c, 42 c, 42 c, 42 c, 7C c, 40 c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 0051 ) 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 42 c, 52 c, 4A c, 3C c, 02 c, 00 c,
( 0052 ) 00 c, 00 c, 7C c, 42 c, 42 c, 42 c, 7C c, 50 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 0053 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 3C c, 02 c, 02 c, 42 c, 3C c, 00 c, 00 c,
( 0054 ) 00 c, 00 c, FE c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 0055 ) 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0056 ) 00 c, 00 c, 82 c, 82 c, 44 c, 44 c, 44 c, 28 c, 28 c, 28 c, 10 c, 00 c, 00 c,
( 0057 ) 00 c, 00 c, 82 c, 82 c, 82 c, 82 c, 92 c, 92 c, 92 c, AA c, 44 c, 00 c, 00 c,
( 0058 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 10 c, 28 c, 44 c, 82 c, 82 c, 00 c, 00 c,
( 0059 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 005A ) 00 c, 00 c, 7E c, 02 c, 04 c, 08 c, 10 c, 20 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 005B ) 00 c, 00 c, 3C c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 20 c, 3C c, 00 c, 00 c,
( 005C ) 00 c, 00 c, 80 c, 80 c, 40 c, 20 c, 10 c, 08 c, 04 c, 02 c, 02 c, 00 c, 00 c,
( 005D ) 00 c, 00 c, 78 c, 08 c, 08 c, 08 c, 08 c, 08 c, 08 c, 08 c, 78 c, 00 c, 00 c,
( 005E ) 00 c, 00 c, 10 c, 28 c, 44 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 005F ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, FE c, 00 c,
( 0060 ) 00 c, 10 c, 08 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 0061 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0062 ) 00 c, 00 c, 40 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 62 c, 5C c, 00 c, 00 c,
( 0063 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0064 ) 00 c, 00 c, 02 c, 02 c, 02 c, 3A c, 46 c, 42 c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 0065 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 0066 ) 00 c, 00 c, 1C c, 22 c, 20 c, 20 c, 7C c, 20 c, 20 c, 20 c, 20 c, 00 c, 00 c,
( 0067 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3A c, 44 c, 44 c, 38 c, 40 c, 3C c, 42 c, 3C c,
( 0068 ) 00 c, 00 c, 40 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 0069 ) 00 c, 00 c, 00 c, 10 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 006A ) 00 c, 00 c, 00 c, 04 c, 00 c, 0C c, 04 c, 04 c, 04 c, 04 c, 44 c, 44 c, 38 c,
( 006B ) 00 c, 00 c, 40 c, 40 c, 40 c, 44 c, 48 c, 70 c, 48 c, 44 c, 42 c, 00 c, 00 c,
( 006C ) 00 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 006D ) 00 c, 00 c, 00 c, 00 c, 00 c, EC c, 92 c, 92 c, 92 c, 92 c, 82 c, 00 c, 00 c,
( 006E ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 006F ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 0070 ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 62 c, 42 c, 62 c, 5C c, 40 c, 40 c, 40 c,
( 0071 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3A c, 46 c, 42 c, 46 c, 3A c, 02 c, 02 c, 02 c,
( 0072 ) 00 c, 00 c, 00 c, 00 c, 00 c, 5C c, 22 c, 20 c, 20 c, 20 c, 20 c, 00 c, 00 c,
( 0073 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 30 c, 0C c, 42 c, 3C c, 00 c, 00 c,
( 0074 ) 00 c, 00 c, 00 c, 20 c, 20 c, 7C c, 20 c, 20 c, 20 c, 22 c, 1C c, 00 c, 00 c,
( 0075 ) 00 c, 00 c, 00 c, 00 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 0076 ) 00 c, 00 c, 00 c, 00 c, 00 c, 44 c, 44 c, 44 c, 28 c, 28 c, 10 c, 00 c, 00 c,
( 0077 ) 00 c, 00 c, 00 c, 00 c, 00 c, 82 c, 82 c, 92 c, 92 c, AA c, 44 c, 00 c, 00 c,
( 0078 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 24 c, 18 c, 18 c, 24 c, 42 c, 00 c, 00 c,
( 0079 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
( 007A ) 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 04 c, 08 c, 10 c, 20 c, 7E c, 00 c, 00 c,
( 007B ) 00 c, 00 c, 0E c, 10 c, 10 c, 08 c, 30 c, 08 c, 10 c, 10 c, 0E c, 00 c, 00 c,
( 007C ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 007D ) 00 c, 00 c, 70 c, 08 c, 08 c, 10 c, 0C c, 10 c, 08 c, 08 c, 70 c, 00 c, 00 c,
( 007E ) 00 c, 00 c, 24 c, 54 c, 48 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A0 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A1 ) 00 c, 00 c, 10 c, 00 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00A2 ) 00 c, 00 c, 10 c, 38 c, 54 c, 50 c, 50 c, 54 c, 38 c, 10 c, 00 c, 00 c, 00 c,
( 00A3 ) 00 c, 00 c, 1C c, 22 c, 20 c, 70 c, 20 c, 20 c, 20 c, 62 c, DC c, 00 c, 00 c,
( 00A4 ) 00 c, 00 c, 00 c, 00 c, 42 c, 3C c, 24 c, 24 c, 3C c, 42 c, 00 c, 00 c, 00 c,
( 00A5 ) 00 c, 00 c, 82 c, 82 c, 44 c, 28 c, 7C c, 10 c, 7C c, 10 c, 10 c, 00 c, 00 c,
( 00A6 ) 00 c, 00 c, 10 c, 10 c, 10 c, 10 c, 00 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00A7 ) 00 c, 18 c, 24 c, 20 c, 18 c, 24 c, 24 c, 18 c, 04 c, 24 c, 18 c, 00 c, 00 c,
( 00A8 ) 00 c, 24 c, 24 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00A9 ) 00 c, 38 c, 44 c, 92 c, AA c, A2 c, AA c, 92 c, 44 c, 38 c, 00 c, 00 c, 00 c,
( 00AA ) 00 c, 00 c, 38 c, 04 c, 3C c, 44 c, 3C c, 00 c, 7C c, 00 c, 00 c, 00 c, 00 c,
( 00AB ) 00 c, 00 c, 00 c, 12 c, 24 c, 48 c, 90 c, 48 c, 24 c, 12 c, 00 c, 00 c, 00 c,
( 00AC ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 7E c, 02 c, 02 c, 02 c, 00 c, 00 c, 00 c,
( 00AD ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00AE ) 00 c, 38 c, 44 c, 92 c, AA c, AA c, B2 c, AA c, 44 c, 38 c, 00 c, 00 c, 00 c,
( 00AF ) 00 c, 00 c, 7E c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B0 ) 00 c, 00 c, 18 c, 24 c, 24 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B1 ) 00 c, 00 c, 00 c, 10 c, 10 c, 7C c, 10 c, 10 c, 00 c, 7C c, 00 c, 00 c, 00 c,
( 00B2 ) 00 c, 30 c, 48 c, 08 c, 30 c, 40 c, 78 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B3 ) 00 c, 30 c, 48 c, 10 c, 08 c, 48 c, 30 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B4 ) 00 c, 08 c, 10 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B5 ) 00 c, 00 c, 00 c, 00 c, 00 c, 42 c, 42 c, 42 c, 42 c, 66 c, 5A c, 40 c, 00 c,
( 00B6 ) 00 c, 00 c, 3E c, 74 c, 74 c, 74 c, 34 c, 14 c, 14 c, 14 c, 14 c, 00 c, 00 c,
( 00B7 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 18 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00B8 ) 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c, 08 c, 18 c,
( 00B9 ) 00 c, 20 c, 60 c, 20 c, 20 c, 20 c, 70 c, 00 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00BA ) 00 c, 00 c, 30 c, 48 c, 48 c, 30 c, 00 c, 78 c, 00 c, 00 c, 00 c, 00 c, 00 c,
( 00BB ) 00 c, 00 c, 00 c, 90 c, 48 c, 24 c, 12 c, 24 c, 48 c, 90 c, 00 c, 00 c, 00 c,
( 00BC ) 00 c, 40 c, C0 c, 40 c, 40 c, 42 c, E6 c, 0A c, 12 c, 1A c, 06 c, 00 c, 00 c,
( 00BD ) 00 c, 40 c, C0 c, 40 c, 40 c, 4C c, F2 c, 02 c, 0C c, 10 c, 1E c, 00 c, 00 c,
( 00BE ) 00 c, 60 c, 90 c, 20 c, 10 c, 92 c, 66 c, 0A c, 12 c, 1A c, 06 c, 00 c, 00 c,
( 00BF ) 00 c, 00 c, 10 c, 00 c, 10 c, 10 c, 20 c, 40 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00C0 ) 00 c, 10 c, 08 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C1 ) 00 c, 08 c, 10 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C2 ) 00 c, 18 c, 24 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C3 ) 00 c, 32 c, 4C c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C4 ) 00 c, 24 c, 24 c, 00 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C5 ) 00 c, 18 c, 24 c, 18 c, 18 c, 24 c, 42 c, 42 c, 7E c, 42 c, 42 c, 00 c, 00 c,
( 00C6 ) 00 c, 00 c, 6E c, 90 c, 90 c, 90 c, 9C c, F0 c, 90 c, 90 c, 9E c, 00 c, 00 c,
( 00C7 ) 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 40 c, 40 c, 40 c, 42 c, 3C c, 08 c, 10 c,
( 00C8 ) 00 c, 10 c, 08 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00C9 ) 00 c, 08 c, 10 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CA ) 00 c, 18 c, 24 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CB ) 00 c, 24 c, 24 c, 00 c, 7E c, 40 c, 40 c, 78 c, 40 c, 40 c, 7E c, 00 c, 00 c,
( 00CC ) 00 c, 20 c, 10 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CD ) 00 c, 08 c, 10 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CE ) 00 c, 18 c, 24 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00CF ) 00 c, 44 c, 44 c, 00 c, 7C c, 10 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00D0 ) 00 c, 00 c, 78 c, 44 c, 42 c, 42 c, E2 c, 42 c, 42 c, 44 c, 78 c, 00 c, 00 c,
( 00D1 ) 00 c, 64 c, 98 c, 00 c, 82 c, C2 c, A2 c, 92 c, 8A c, 86 c, 82 c, 00 c, 00 c,
( 00D2 ) 00 c, 20 c, 10 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D3 ) 00 c, 08 c, 10 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D4 ) 00 c, 18 c, 24 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D5 ) 00 c, 64 c, 98 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D6 ) 00 c, 44 c, 44 c, 00 c, 7C c, 82 c, 82 c, 82 c, 82 c, 82 c, 7C c, 00 c, 00 c,
( 00D7 ) 00 c, 00 c, 00 c, 00 c, 42 c, 24 c, 18 c, 18 c, 24 c, 42 c, 00 c, 00 c, 00 c,
( 00D8 ) 00 c, 02 c, 3C c, 46 c, 4A c, 4A c, 52 c, 52 c, 52 c, 62 c, 3C c, 40 c, 00 c,
( 00D9 ) 00 c, 20 c, 10 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DA ) 00 c, 08 c, 10 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DB ) 00 c, 18 c, 24 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DC ) 00 c, 24 c, 24 c, 00 c, 42 c, 42 c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00DD ) 00 c, 08 c, 10 c, 00 c, 44 c, 44 c, 28 c, 10 c, 10 c, 10 c, 10 c, 00 c, 00 c,
( 00DE ) 00 c, 00 c, 40 c, 7C c, 42 c, 42 c, 42 c, 7C c, 40 c, 40 c, 40 c, 00 c, 00 c,
( 00DF ) 00 c, 00 c, 38 c, 44 c, 44 c, 48 c, 50 c, 4C c, 42 c, 42 c, 5C c, 00 c, 00 c,
( 00E0 ) 00 c, 00 c, 10 c, 08 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E1 ) 00 c, 00 c, 04 c, 08 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E2 ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E3 ) 00 c, 00 c, 32 c, 4C c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E4 ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E5 ) 00 c, 18 c, 24 c, 18 c, 00 c, 3C c, 02 c, 3E c, 42 c, 46 c, 3A c, 00 c, 00 c,
( 00E6 ) 00 c, 00 c, 00 c, 00 c, 00 c, 6C c, 12 c, 7C c, 90 c, 92 c, 6C c, 00 c, 00 c,
( 00E7 ) 00 c, 00 c, 00 c, 00 c, 00 c, 3C c, 42 c, 40 c, 40 c, 42 c, 3C c, 08 c, 10 c,
( 00E8 ) 00 c, 00 c, 10 c, 08 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00E9 ) 00 c, 00 c, 08 c, 10 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EA ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EB ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 42 c, 7E c, 40 c, 42 c, 3C c, 00 c, 00 c,
( 00EC ) 00 c, 00 c, 20 c, 10 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00ED ) 00 c, 00 c, 10 c, 20 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00EE ) 00 c, 00 c, 30 c, 48 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00EF ) 00 c, 00 c, 48 c, 48 c, 00 c, 30 c, 10 c, 10 c, 10 c, 10 c, 7C c, 00 c, 00 c,
( 00F0 ) 00 c, 24 c, 18 c, 28 c, 04 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F1 ) 00 c, 00 c, 32 c, 4C c, 00 c, 5C c, 62 c, 42 c, 42 c, 42 c, 42 c, 00 c, 00 c,
( 00F2 ) 00 c, 00 c, 20 c, 10 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F3 ) 00 c, 00 c, 08 c, 10 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F4 ) 00 c, 00 c, 18 c, 24 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F5 ) 00 c, 00 c, 32 c, 4C c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F6 ) 00 c, 00 c, 24 c, 24 c, 00 c, 3C c, 42 c, 42 c, 42 c, 42 c, 3C c, 00 c, 00 c,
( 00F7 ) 00 c, 00 c, 00 c, 10 c, 10 c, 00 c, 7C c, 00 c, 10 c, 10 c, 00 c, 00 c, 00 c,
( 00F8 ) 00 c, 00 c, 00 c, 00 c, 02 c, 3C c, 46 c, 4A c, 52 c, 62 c, 3C c, 40 c, 00 c,
( 00F9 ) 00 c, 00 c, 20 c, 10 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FA ) 00 c, 00 c, 08 c, 10 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FB ) 00 c, 00 c, 18 c, 24 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FC ) 00 c, 00 c, 28 c, 28 c, 00 c, 44 c, 44 c, 44 c, 44 c, 44 c, 3A c, 00 c, 00 c,
( 00FD ) 00 c, 00 c, 08 c, 10 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
( 00FE ) 00 c, 00 c, 00 c, 40 c, 40 c, 5C c, 62 c, 42 c, 42 c, 62 c, 5C c, 40 c, 40 c,
( 00FF ) 00 c, 00 c, 24 c, 24 c, 00 c, 42 c, 42 c, 42 c, 46 c, 3A c, 02 c, 42 c, 3C c,
base !

8  constant font-width
13 constant font-height

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
  lcd-y @ font-height + dup LCD_HEIGHT_LINES 1- > if
    drop 0
  then
  lcd-y !
;


.( lcd-char )

: lcd-char ( c -- )
  lcd-y @ LCD_VRAM_WIDTH_BYTES * LCD_VRAM +
  lcd-x @ 3 rshift +           ( c c-addr )
  swap                         ( c-addr c )
  font-height * font-8x13 +    ( lcd-addr font-addr )
  font-height for aft
    2dup c@ swap c!
    char+ swap LCD_VRAM_WIDTH_BYTES + swap
  then next 2drop
  lcd-x @ font-width + dup LCD_WIDTH_PIXELS 1- > if
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
