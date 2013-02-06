\ draw a test pattern

: eye ( -- )
  lcd-clear
  76 0 ?do 75 i -
    120 over - 100 lcd-move-to
    120 over 25 + lcd-line-to
    120 over + 100 lcd-line-to
    120 i 100 + lcd-line-to
    120 over - 100 lcd-line-to
    drop
    5 +loop ;


eye
