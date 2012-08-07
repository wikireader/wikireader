#!/bin/sh

py2applet --make-setup --iconfile="WikiReader.icns" -S --resources=images "Wikireader Update.py" ./tools/7za << EOF
y
EOF
rm -rf build dist
python setup.py py2app

codesign -f -s "Developer ID Application" "dist/Wikireader Update.app/" --timestamp=none

# check code sign
spctl -a -v "dist/Wikireader Update.app"

open "dist/WikiReader Update.app"

rm -rf build
