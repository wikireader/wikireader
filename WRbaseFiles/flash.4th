\ flash.4th

base @ decimal

: get-image-file  ( b u addr -- flag )
    >r
    r/o bin open-file ?dup
    if
        ." open error = " dec. cr
        r> 2drop
        false exit
    then

    r> \ file-id buffer
    swap >r \ buffer

    flash-rom-size

    begin
        \ b u1
        2dup
        r@ read-file ?dup  \ b u1 u2 ior ior?
        if
            ." read error = " dec. cr
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


: program-rom  ( b u -- )

    cr ." Loading: " 2dup type cr

    here get-image-file 0=
    if
        ." Read file failed" cr
        exit
    then

    cr ." Serial Number: "

    \ save serial number
    here flash-serial-number-offset +
    flash-serial-number-length
    flash-serial-number-offset flash-read
    if
        flash-serial-number-length 0
        ?do
            here flash-serial-number-offset + i + c@
            dup bl 127 within
            if
                emit
            else
                drop
            then
        loop
    else
        ." FAIL" cr
        exit
    then

    cr ." Erase: "

    flash-write-enable
    if
        flash-chip-erase
        if
            ." Done"
        else
            ." FAIL"
            cr exit
        then
    else
        ." FAIL"
        cr exit
    then

    cr ." Program: "
    flash-rom-size 0 ?do

        flash-write-enable 0=
        if
            ." FAIL" cr
            unloop
            exit
        then
        here i + flash-page-size i flash-write 0=
        if
            ." FAIL" cr
            unloop
            exit
        then
        i flash-sector-size 1- and 0=
        if
            ." ."
        then
    flash-page-size +loop

    cr ." Verify: "
    flash-rom-size 0 ?do
        here i + flash-sector-size i flash-verify
        if
            ." ."
        else
            ." E"
        then
    flash-sector-size +loop
    cr
;


: save-rom ( b u -- )
    cr ." Read ROM: "
    here flash-rom-size 0 flash-read
    if
        ." PASS"
    else
        ." FAIL" cr
        2drop
        exit
    then

    cr ." Write to: "  2dup type cr
    w/o create-file ?dup
    if
        ." create file error = " dec. cr
        drop
        exit
    then
    >r \ save fileid

    here flash-rom-size r@ write-file ?dup
    if
        ." write file error = " dec. cr
    then
    r> close-file drop
;


: burn (  "<spaces><filename>" -- )
    bl parse program-rom
;

: save ( "<spaces><filename>" --  )
    bl parse save-rom
;

base !
