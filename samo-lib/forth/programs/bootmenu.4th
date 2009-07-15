\ boot menu
decimal

lcd-cls

\ 16 line screen = title + blank + 14 items
14 constant max-items
8 1+ 3 + 1+ constant item-length  \ 8.3\0


max-items item-length * constant items-size

create items items-size allot

variable menu-count

: read-items ( -- )
    items items-size 0 fill

    lcd-cls
    s" Forth programs" lcd-type lcd-cr lcd-cr

    s" /" open-directory ?dup
    if  cr ." open-directory error = "
        dec. drop exit
    then
    >r    \ save dirid
    begin
        here 256 r@ read-directory ?dup
        if  cr ." directory read error = "
            dec. drop
            r> close-directory drop exit
        then
        dup
    while
            here swap ( c-addr u )
            2dup s" .4MU" search if
                4 = if
                    drop 2dup
                    menu-count @
                    dup max-items < 0= if
                        cr ." too many menu items"
                        -1 throw
                    then
                    item-length * items + \ src count dest
                    swap cmove
                    4 -
                    bl lcd-emit
                    lcd-type lcd-cr
                    1 menu-count +!
                else
                    drop 2drop
                then
            else
                2drop 2drop
            then
    repeat
    drop
    r> close-directory drop
;

: menu-select ( -- n)
    button-flush
    key-flush
    ctp-flush
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop 0
            else
                nip font-height / 1+
            then
            exit
        then

        button? if
            button
            case
                button-left of
                    -2
                    exit
                endof
                button-centre of
                    -4
                    exit
                endof
                button-right of
                    -1
                    exit
                endof
            endcase
        then

        key? if
            key-flush
        then

        wait-for-event
    again
;


: run-program ( c-addr u -- )
    lcd-cls s" Loading: " lcd-type
    2dup lcd-type
    included
    quit
;

variable menu-cursor

: menu ( -- )
    read-items
    22 lcd-text-rows 1- lcd-at-xy s" Console" lcd-type
    ctp-flush
    key-flush
    button-flush
    begin
        P6_P6D p@ $07 and 0=
    until
    begin
        menu-select dup 2 >
        if
            dup 3 - menu-count @ < if
                menu-cursor @ ?dup if
                    1- font-height * font-height lcd-invert-lines
                then
                dup menu-cursor !
                1- font-height * font-height lcd-invert-lines
                0
            then
        else
            menu-cursor @ ?dup if
                3 -  item-length * items +
                dup
                begin
                    dup c@
                while
                        1+
                repeat
                over - ['] run-program catch
                cold \ restart menu
            then
            0 menu-cursor !
        then
        -1 =
    until
    lcd-cls
    s" serial console active" lcd-type
    -1 throw \ will abort menu and return to command prompt
;

\ start menu
menu

\ ensure restart will occur if any program exits
cold
cold
cold
cold

decimal
