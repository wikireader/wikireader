#!/usr/bin/env python

import glob, os, sys
job_dir = sys.argv[1]


os.chdir(job_dir)
try:
    os.mkdir("articles")
except:
    pass

display = 99 - int(job_dir.rsplit('/', 1)[1])
os.system("Xvfb :%d -noreset &" % display)
os.environ['DISPLAY'] = ":%d" % display

for work in glob.glob("*.work"):
    file = open(work)
    for line in file:
        data = line[:-1].split(" ", 1)
        hash = data[0]
        url = data[1]
        print "Getting %s" % url
        os.environ['file'] = url
        os.system("$HOME/source/webkit-pedia.git/WebKitBuild/Release/Programs/GtkLauncher $file")

        os.environ['file'] = url.replace("http://127.0.0.1/mediawiki/index.php/", "")
        os.system("extract_spacing.py render_text.blib")
        os.system("mv -f render_text.blib articles/$file")

