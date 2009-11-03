#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Run all Python test scripts in a directory tree
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import sys
import os
import os.path
import getopt
import inspect
import textwrap


DIRECTORY_IGNORE_LIST = ['CVS', '.svn', '.git', '00IGNORE']

verbose = True


# display a functions documentation string
def displayDoc(fd, message):
    if None == message:
        message = '*empty documentation string*'
    wrapper = textwrap.TextWrapper(initial_indent = '    \ ', subsequent_indent = '    | ')
    fd.write(wrapper.fill(message))
    fd.write('\n')

def info(fd, message):
    global verbose
    if verbose:
        fd.write('INFO:%s\n' % message)

def request_action(fd, message):
    global verbose
    if verbose:
        fd.write('ACTION:%s\n' % message)

def fail_if(cond, message):
    if cond:
        raise AssertionError, message

# the main script running application
# this can throw exceptions if compile or setUp fail
# If setUp succeeds the tearDown will be run
def runTests(fd, name, debug, **kwargs):
    global verbose

    module_name = name
    if name.endswith('.py'):
        module_name = name[:-3]
    elif name.endswith('.pyc'):
        module_name = name[:-4]

    if verbose:
        fd.write('TEST: Load Module: %s\n' % module_name)

    global_variables = {
        'module_name': module_name,
        'debug': debug,
        'info': lambda message : info(fd, message),
        'request_action': lambda message : request_action(fd, message),
        'fail_unless': lambda cond, message : fail_if(not cond, message),
        'fail_if': lambda cond, message : fail_if(cond, message),
        'global_args': kwargs,
        }
    eval('0', global_variables) # populate global_variables

    execfile(name, global_variables)

    s = filter(lambda name: name.startswith('test'), global_variables.iterkeys())
    s.sort()

    if verbose:
        fd.write('TEST: %s.setUp\n' % module_name)
        displayDoc(fd, inspect.getdoc(global_variables['setUp']))

    eval('setUp()', global_variables)

    try:
        for f in s:
            if verbose:
                fd.write('TEST: %s.%s\n' % (module_name, f))
                displayDoc(fd, inspect.getdoc(global_variables[f]))
            eval(f + '()', global_variables)
            fd.write('PASS: %s.%s\n' % (module_name, f))
        if verbose:
            fd.write('PASS: all tests completed\n')
    except AssertionError, e:
        fd.write('FAIL: %s\n' % e)
    finally:
        if verbose:
            fd.write('TEST: %s.tearDown\n' % module_name)
            displayDoc(fd, inspect.getdoc(global_variables['tearDown']))
        eval('tearDown()', global_variables)


# run one test script catching errors
def runOneTest(fd, name, debug, **kwargs):
    if verbose:
        fd.write('FILE: %s\n' % name)
    try:
        runTests(fd, name, debug, **kwargs)
    except SyntaxError, s:
        fd.write('FAIL: Test module compile failed: %s\n' % s)
    except Exception, e:
        fd.write('FAIL: Test module run failed: %s\n' % e)


# process a directory tree
def processTree(fd, top_dir, debug):
    for root, dirs, files in os.walk(top_dir):
        if verbose:
            fd.write('Directory: %s\n' % root)
        for r in DIRECTORY_IGNORE_LIST:
            if r in dirs:
                dirs.remove(r)
        dirs.sort()
        tests = filter(lambda f: f.startswith('test') and f.endswith('.py'), files)
        tests.sort()
        for t in tests:
            runOneTest(fd, os.path.join(root, t), debug)


# display error and usage message; then exit
def usage(message):
    if message != None:
        print 'error:', message
    print 'usage:', sys.argv[0], '<options> <directories>'
    print '       --help        = this message'
    print '       --debug=n     = set global debug=level for test scripts'
    print '       --verbose     = set verbose for framework'
    sys.exit(1)


# main program to scan files and directories for test*.py files
# run scripts in the following order:
# 1. items from the command line in the order given
#    individual files are run as encountered.
# 2. Direcorries are handled as follows:
#  a. First all files of the form test*.py are run in sorted order.
#  b. Finally subdirectories are scanned in sorted order
#     and each directory is treated as 2.
def main():
    global verbose
    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], 'hd:v', ['help', 'debug=', 'verbose'])
    except getopt.GetoptError, err:
        usage(err)
    debug = 0
    verbose = False
    for o, a in opts:
        if o in ('-v', '--verbose'):
            verbose = True
        elif o in ('-h', '--help'):
            usage(None)
        elif o in ('-d', '--debug'):
            try:
                debug = int(a)
            except ValueError, e:
                debug = 1
        else:
            usage('invalid option: ' + o)

    if args == []:
        usage('missing arguments')

    # list of Python test files in sorted order
    for arg in args:
        if os.path.isdir(arg):
            processTree(sys.stdout, arg, debug)
        else:
            #runOneTest(sys.stdout, arg, debug)
            runOneTest(sys.stdout, arg, debug)

# execute the main program if run as a script
if __name__ == '__main__':
    main()
