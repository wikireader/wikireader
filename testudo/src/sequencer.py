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


# display a functions documentation string
def displayDoc(message):
    wrapper = textwrap.TextWrapper(initial_indent = '    \ ', subsequent_indent = '    | ')
    print wrapper.fill(message)

def info(x):
    print 'INFO:', x

# the main script running application
# this can throw exceptions if compile or setUp fail
# If setUp succeeds the tearDown will be run
def runTests(name, debug):
    global verbose

    module_name = name
    if name.endswith('.py'):
        module_name = name[:-3]
    elif name.endswith('.pyc'):
        module_name = name[:-4]

    if verbose:
        print 'TEST: Load Module:', module_name

    global_variables = {
        'debug': debug,
        'info': lambda x : info(x),
        }
    eval('0', global_variables) # populate global_variables

    execfile(name, global_variables)

    s = filter(lambda name: name.startswith('test'), global_variables.iterkeys())
    s.sort()

    if verbose:
        print 'TEST: Run: %s.setUp' % module_name
        displayDoc(inspect.getdoc(global_variables['setUp']))

    eval('setUp()', global_variables)

    try:
        for f in s:
            if verbose:
                print 'TEST: Run: %s.%s' % (module_name, f)
                displayDoc(inspect.getdoc(global_variables[f]))
            eval(f + '()', global_variables)
        if verbose:
            print 'PASS: all tests completed'
    except AssertionError, e:
        print 'FAIL:', e
    finally:
        if verbose:
            print 'TEST: Run: %s.tearDown' % module_name
            displayDoc(inspect.getdoc(global_variables['tearDown']))
        eval('tearDown()', global_variables)


# run one test script catching errors
def runOneTest(name, debug):
    if verbose:
        print 'TEST: %s' % name
    try:
        runTests(name, debug)
    except SyntaxError, s:
        print 'Test module compile failed: ', s
    except Exception, e:
        print 'Test module run failed', e


# process a directory tree
def processTree(top_dir, debug):
    for root, dirs, files in os.walk(top_dir):
        if verbose:
            print 'Directory:', root
        for r in DIRECTORY_IGNORE_LIST:
            if r in dirs:
                dirs.remove(r)
        dirs.sort()
        tests = filter(lambda f: f.startswith('test') and f.endswith('.py'), files)
        tests.sort()
        for t in tests:
            runOneTest(os.path.join(root, t), debug)


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
            processTree(arg, debug)
        else:
            runOneTest(arg, debug)

# execute the main program if run as a script
if __name__ == '__main__':
    main()
