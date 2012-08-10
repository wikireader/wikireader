#!/bin/sh

py2applet --make-setup --iconfile="WikiReader.icns" -S --resources=images "WikiReader Update.py" ./tools/7za << EOF
y
EOF
rm -rf build dist
python setup.py py2app

#codesign -f -s "Developer ID Application" "dist/WikiReader Update.app/" --timestamp=none
codesign -f -s "Developer ID Application" "dist/WikiReader Update.app/"

# check code sign
spctl -a -v "dist/WikiReader Update.app"

open "dist/WikiReader Update.app"

rm -rf build
