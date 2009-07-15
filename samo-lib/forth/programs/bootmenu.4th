\ boot menu
decimal

lcd-cls

\ dos file name <count>8.3
8 1+ 3 + 1+ constant item-length

8 constant menu-item-length
2 constant first-menu-line
lcd-text-columns menu-item-length /mod dup constant items-per-line
1- / menu-item-length + constant display-item-width

\ 16 line screen = title + blank + 12 items + blank + soft-keys
12 items-per-line * constant max-items


: menu-data ( u -- )
    create
    item-length * allot
    align
  does>   ( index -- c-addr )
    swap item-length * +
;

max-items menu-data menu

variable menu-count

: read-items ( -- )

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
                    drop
                    menu-count @
                    dup max-items <
                    if
                        menu \ src count dest
                        2dup c! char+  \ save count (byte)
                        swap cmove
                        1 menu-count +!
                    else
                        drop
                    then
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

: item-position ( u -- x y )
    items-per-line /mod  \ col row
    >r
    display-item-width *
    r>
    first-menu-line +
;

: display-item ( u -- )
    dup item-position
    lcd-at-xy
    menu dup char+ swap c@
    4 - \ ignore the .4mu extension
    lcd-type
;

: display-highlighted-item ( u -- )
    lcd-white
    display-item
    lcd-black
;

: display-items ( -- )
    lcd-cls
    s" Forth programs" lcd-type

    menu-count @ 0
    ?do
        i display-item
    loop
;


: run-program ( c-addr u -- )
    lcd-cls s" Loading: " lcd-type
    2dup lcd-type
    included
    quit
;

variable menu-cursor

: boot-menu ( -- )
    read-items
    display-items
    22 lcd-text-rows 1- lcd-at-xy s" Console" lcd-type
    ctp-flush
    key-flush
    button-flush
    -1 menu-cursor !

    begin
        ctp-pos? if
            ctp-char dup 0<
            if
                2drop
                menu-cursor @ 0< 0= if
                    menu-cursor @ menu
                    dup char+ swap c@
                    ['] run-program catch
                    cold \ restart menu
                then
                -1 menu-cursor !
            else
                menu-cursor @ 0< 0=
                if
                    menu-cursor @ display-item
                    -1 menu-cursor !
                then
                first-menu-line - dup 0< 0=
                if
                    items-per-line *
                    swap
                    display-item-width /
                    +
                    dup menu-count @ <
                    if
                        dup menu-cursor !
                        display-highlighted-item
                    else
                        drop
                    then
                else
                    2drop
                then
            then
        then

        button? if
            button
            case
                button-power of
                    power-off
                endof
                button-left of
                endof
                button-centre of
                endof
                button-right of
                    lcd-cls
                    s" Serial Console Active" lcd-type lcd-cr lcd-cr
                    s" Connect Debug Cable at:" lcd-type lcd-cr
                    s"   19200 8N1" lcd-type lcd-cr
                    cold0 \ does not return
                endof
            endcase
        then

        key? if
            key-flush
        then

        wait-for-event
    again
;

\ start menu
boot-menu

\ ensure restart will occur if any program exits
cold
cold
cold
cold

decimal
