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
        print "Getting %s" % line[:-1]
        os.environ['file'] = line[:-1]
        os.system("$HOME/source/webkit-pedia.git/WebKitBuild/Release/Programs/GtkLauncher $file")

        os.environ['file'] = line[:-1].replace("http://127.0.0.1/mediawiki/index.php/", "")
        os.system("mv -f render_text.blib articles/$file")

