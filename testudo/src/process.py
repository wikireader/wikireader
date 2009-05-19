#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Run a script on a pty
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import sys
import os
import re
import select

class Process:

    PROMPT_TIME = 100
    BUFFER_SIZE = 65536

    def __init__(self, cmd, callback):
        self.callback = callback
        self.cmd = cmd

    def __repr__(self):
        return "Process " + self.menu + "('" + self.cmd[0] + "')"

    def run(self):
        (pid, fd) = os.forkpty()

        # child process
        if pid == 0:
            try:
                #print 'exec:', self.cmd
                os.execvp(self.cmd[0], self.cmd)
            except OSError, e:
                print 'execution failed:', e
                print 'command was:', self.cmd
                sys.exit(os.EX_OSERR)

        # parent process
        selector = select.poll()

        selector.register(fd, select.POLLIN)


        run = True
        currentLine = ''
        while run:
            eventList = selector.poll(Process.PROMPT_TIME)
            # print eventList
            for (fd, e) in eventList:
                # print 'fd =', fd, 'e =', e
                if (e & select.POLLIN) != 0:
                    s = os.read(fd, Process.BUFFER_SIZE)
                    s = ''.join([currentLine, s])
                    (sLeft, sSep, sRight) = s.rpartition('\n')
                    data = ''.join([sLeft, sSep])
                    if data != '':
                        self.callback(data)
                    currentLine = sRight
                elif (e & (select.POLLOUT | select.POLLERR)) == select.POLLOUT:
                    os.write(fd, "no\n")
                if (e & select.POLLHUP) != 0:
                    run = False
                    break
        os.close(fd)
        (thePID, rc) = os.waitpid(pid, 0)
        return rc == 0


# main program
if __name__ == '__main__':
    result = ''
    def cap(s):
        sys.stdout.write('cap>> ')
        sys.stdout.write(s)
        sys.stdout.write('***')
        sys.stdout.flush()
        if 'Press Reset' == s.strip():
            sys.stdout.write('-=<>=-')
            sys.stdout.flush()


    p = Process(sys.argv[1:], cap)

    print 'start:', sys.argv[1:]
    flag = p.run()
    print 'rc =', flag
