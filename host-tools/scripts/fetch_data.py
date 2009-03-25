#!/usr/bin/env python

import glob, os, sys, subprocess, signal, time
job_dir = sys.argv[1]


os.chdir(job_dir)
try:
    os.mkdir("articles")
except:
    pass

display = 99 - int(job_dir.rsplit('/', 1)[1])
os.system("Xvfb :%d -noreset -ac &" % display)
os.environ['DISPLAY'] = ":%d" % display

# wait for the x server to start
time.sleep(6)

failed_urls = open("failed.urls", "w")
current_pid = None

class TimeOutException(Exception):
    pass


def alarm_handler(signum, frame):
    """
    Kill the process with current_pid... this should help to get
    run_command unstuck...
    """
    os.kill(current_pid, signal.SIGKILL)

def run_command(cmd):
    """
    Execute a command. If the command is not finished with 120 seconds
    the program will be killed with SIGKILL. If the return value of the
    program is smaller than < 0 a timeout exception will be raised, if it
    is > 0 a CalledProcessError will be raised.
    """
    global current_pid

    # Launch and set an alarm... alarm_hnalder will kill
    # current_pid
    signal.alarm(120)
    proc = subprocess.Popen(cmd)
    current_pid = proc.pid
    pid, sts = os.waitpid(proc.pid, 0)

    # cancel timer and error checking
    signal.alarm(0)
    if sts > 0:
        raise subprocess.CalledProcessError(sts, cmd)
    elif sts < 0:
        raise TimeOutException()

def execute(hash, url):
    print "Getting %s" % url
    file_base = os.path.join("articles", hash[0], hash[0:1])
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
    subprocess.check_call(["touch", "%s.complete" % work])


