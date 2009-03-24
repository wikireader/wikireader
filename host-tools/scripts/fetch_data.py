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


def run_command(cmd):
    subprocess.check_call(cmd)

def execute(hash, url):
    print "Getting %s" % url
    file_base = os.path.join("articles", hash.left(1), hash.left(2))
    render_text = "%s.blib" % os.path.join(file_base, hash)
    render_link = "%s.link" % os.path.join(file_base, hash)

    run_command(["GtkLauncher", url])
    run_command(["extract_spacing.py", "render_text.blib"])
    run_command(["mkdir", "-p", file_base])
    run_command(["mv", "-f", "render_text.blib", render_text])
    run_command(["mv", "-f", "render_text.links", render_link])


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

    # mark it as done
    subprocess.check_call(["touch", "%s.complete" % file])


