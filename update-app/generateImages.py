#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2012
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#          Hank Wang <hank@openmoko.com>

"""
Generate all imgs to python binary file.
img2py command required.
$ python generateImages.py
"""

from commands import getoutput
import os
from Utilities import getProgramFolder

imagePath = getProgramFolder() + '/images'
outputFile = 'images.py'
imageFile = getProgramFolder() + '/' + outputFile

if os.path.exists(imageFile):
    os.remove(imageFile)

for filename in os.listdir(imagePath):
    print getoutput('img2py -a -f %s/%s %s' % (imagePath, filename, imageFile))