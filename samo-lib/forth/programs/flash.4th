\ flash.4th

base @ decimal
also c33

65536 constant rom-size
256 constant page-size
4096 constant sector-size

: get-image-file  ( addr b u -- flag )
    cr r/o open-file ?dup
    if
        cr ." open error = " dec.
        2drop
        false exit
    then

    >r \ save fileid

    rom-size

    begin
        \ b u1
        2dup
        r@ read-file ?dup  \ b u1 u2 ior ior?
        if
            cr ." read error = " dec.
            2drop drop
            r> close-file drop
            false exit
        then
        \ b u1 u2
        swap over -  \ b u2 (u1-u2)
        >r + r>      \ (b+u2) (u1-u2)
        dup 0=
    until
    2drop
    r> close-file drop
    true
;


: EEPROM-CS-LOW ( -- )
    P5_P5D p@
    $fb and
    P5_P5D p!
;

: EEPROM-CS-HIGH ( -- )
    P5_P5D p@
    $04 or
    P5_P5D p!
;


: Disable-SD-CARD ( -- )
    EEPROM-CS-HIGH
    P3_P3D p@
    $f7 and
    P3_P3D p!
;

: Enable-SD-CARD ( -- )
    EEPROM-CS-HIGH
    P3_P3D p@
    $08 and
    P3_P3D p!
    100 delay-us
    filesystem-init
    250 delay-us
;


$01 constant spi-write-status
$02 constant spi-page-program
$03 constant spi-read-data
$04 constant spi-write-disable
$05 constant spi-read-status
$06 constant spi-write-enable
$0b constant spi-fast-read
$c7 constant spi-chip-erase

: spi-put ( c -- )
    begin
        SPI_STAT p@ TDEF and
    until
    SPI_TXD p!
;


: spi-get ( -- c )
    0 SPI_TXD p!
    begin
        SPI_STAT p@ RDFF and
    until
    SPI_RXD p@
;

: send-command ( c -- )
    EEPROM-CS-LOW
    spi-put
    EEPROM-CS-HIGH
;

: wait-ready ( -- )
    EEPROM-CS-LOW
    spi-read-status spi-put
    begin
        spi-get
        $01 and
        0=
    until
    EEPROM-CS-HIGH
;

: write-enable ( -- )
    spi-write-enable send-command
;

: write-disable ( -- )
    spi-write-disable send-command
;


: program-block ( b u addr -- )
    wait-ready
    write-enable
    EEPROM-CS-LOW
    spi-page-program spi-put
    dup 16 rshift spi-put  \ A23..A16
    dup 8 rshift spi-put   \ A15..A08
    spi-put                \ A07..A00
    0 ?do
        dup c@ spi-put
        char+
    loop
    EEPROM-CS-HIGH
    wait-ready
;

: verify-block ( b u addr -- flag )
    EEPROM-CS-LOW
    spi-fast-read spi-put
    dup 16 rshift spi-put  \ A23..A16
    dup 8 rshift spi-put   \ A15..A08
    spi-put                \ A07..A00
    spi-get drop           \ dummy
    0 ?do
        dup c@ spi-get <> if
            drop
            EEPROM-CS-HIGH
            false
            exit
        then
        char+
    loop
    EEPROM-CS-HIGH
    drop
    true
;

: read-block ( b u addr -- )
    EEPROM-CS-LOW
    spi-fast-read spi-put
    dup 16 rshift spi-put  \ A23..A16
    dup 8 rshift spi-put   \ A15..A08
    spi-put                \ A07..A00
    spi-get drop           \ dummy
    0 ?do
        spi-get over c!
        char+
    loop
    EEPROM-CS-HIGH
    drop
;

: chip-erase ( -- )
    wait-ready
    write-enable
    spi-chip-erase send-command
    wait-ready
;


: burn (  "<spaces>name" -- )
    here   \ buffer address
    cr ." Loading: "
    bl parse 2dup type
    get-image-file 0=
    if
        cr ." Read file failed" cr
        exit
    then

    Disable-SD-CARD

    cr ." Erase"

    chip-erase

    cr ." Program: "
    rom-size 0 ?do

        here i + page-size i program-block

        i $fff and 0=
        if
            ." ."
        then
    page-size +loop
    cr

    write-disable

    cr ." Verify: "
    rom-size 0 ?do
        here i + sector-size i verify-block
        if
            ." ."
        else
            ." E"
        then
    sector-size +loop
    Enable-SD-CARD
    cr
;

: read-all-rom ( b -- )
    Disable-SD-CARD
    rom-size 0 read-block
    Enable-SD-CARD
;


: save-rom ( "<spaces><filename>" --  )
    cr ." Read ROM"
    here read-all-rom
    cr ." Write to: "
    bl parse 2dup type
    w/o create-file ?dup
    if
        cr ." create file error = " dec.
        drop
        exit
    then
    >r \ save fileid

    here rom-size r@ write-file ?dup
    if
        cr ." write file error = " dec.
    then
    r> close-file drop
;

base !
