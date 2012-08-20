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