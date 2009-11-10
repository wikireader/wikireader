\ production testing

\ signal to testudo
cr .( *START-TEST*) cr

include touch.4th
test-touch-main
include susptest.4th
test-suspend-main

\ signal to testudo
cr .( *END-TEST*) cr
