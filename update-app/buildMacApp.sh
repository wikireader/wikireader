#! /usr/bin/env sh
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2012
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#          Hank Wang <hank@openmoko.com>

py2applet --make-setup --iconfile="WikiReader.icns" -S --resources=images "WikiReader Update.py" ./tools/7za << EOF
y
EOF
rm -rf build dist
python setup.py py2app

codesign -f -s "Developer ID Application" "dist/WikiReader Update.app/" --timestamp=none
#codesign -f -s "Developer ID Application" "dist/WikiReader Update.app/"

# check code sign
spctl -a -v "dist/WikiReader Update.app"

#open "dist/WikiReader Update.app"

cd dist

tar -zcvf "WikiReader.tar.gz" "WikiReader Update.app/"

cd -
