#!/usr/bin/env python

import glob, os, sys, subprocess
job_dir = sys.argv[1]


os.chdir(job_dir)
try:
    os.mkdir("articles")
except:
    pass

display = 99 - int(job_dir.rsplit('/', 1)[1])
os.system("Xvfb :%d -noreset &" % display)
os.environ['DISPLAY'] = ":%d" % display

failed_urls = open("failed.urls", "w")

class TimeOutException(Exception):
    pass


def execute(hash, url):
    print "Getting %s" % url
    file = os.path.join("articles", url.replace("http://127.0.0.1/mediawiki/index.php/", url))
    subprocess.check_call(["GtkLauncher", url])
    subprocess.check_call(["extract_spacing.py", "render_text.blib"])
    subprocess.check_call(["mv", "-f", "render_text.blib", file])


for work in glob.glob("*.work"):
    file = open(work)
    for line in file:
        data = line[:-1].split(" ", 1)
        try:
            execute(data[0], data[1])
        except subprocess.CalledProcessError:
            print >> failed_urls, "ProcessError: %s %s" % (data[0], data[1])
	except TimeOutException:
	    print >> failed_urls, "Timeout: %s %s" % (data[0], data[1])


