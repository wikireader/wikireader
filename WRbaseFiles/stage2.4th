\ production testing

\ signal to testudo
cr .( *START-TEST*) cr

include version.4th
test-version-main
include key-test.4th
test-keys-main
include lcd-test.4th
test-lcd-main
include ctp-test.4th
test-ctp-main
include susptest.4th
test-suspend-main

\ signal to testudo
cr .( *END-TEST*) cr
