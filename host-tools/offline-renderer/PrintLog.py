#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Article Rendering
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>

import sys, os

# overcome Pythons buggy unicode handling in print statements

def message(data):
    if type(data) == unicode:
        sys.stdout.write(data.encode('utf-8'))
    else:
        sys.stdout.write(data)
    sys.stdout.write('\n')
    sys.stdout.flush

