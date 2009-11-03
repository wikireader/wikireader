\ production testing

\ signal to testudo
cr .( *START-TEST*) cr

include ctp-test.4th
test-ctp-main
include susptest.4th
test-suspend-main

\ signal to testudo
cr .( *END-TEST*) cr
