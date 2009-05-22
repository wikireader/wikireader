# test main board
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 2 or later
# DESCRIPTION: Main board test
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import os.path

MASTER_TEST = 'test_SAMO_01.py'
execfile(os.path.join(os.path.dirname(module_name), MASTER_TEST))

# override MASTER_TEST settings
SUPPLY_STANDARD_VOLTAGE = 3.4
