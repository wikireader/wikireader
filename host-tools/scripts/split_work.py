#!/usr/bin/env python

"""
Simple sccript to split a urls.fetch file generated
by the wiki-tools into jobs for a given number of slaves
and given number of threads per slave...

The names of the slave directories is zecke-%d and must
be created before calling this script. The urls.fetch file
must be in the current directory.

Yes this is a poor man's job dispatcher...
"""

import os

ITEMS_PER_PACKAGE = 1000
NUMBER_OF_SLAVES = 4
NUMBER_OF_JOBS = 6


file = open("urls.fetch")
current_number = 0
current_file = None
file_no = 0

last_thread = {}
for thread in range(1, NUMBER_OF_SLAVES + 1):
    last_thread[thread] = 0


for line in file:
   if not current_file:
       file = "%.5d.work" % file_no
       slave_nr = ((file_no % NUMBER_OF_SLAVES) + 1)
       slave = "zecke-%d" % slave_nr
       thread = "%d" % (last_thread[slave_nr] + 1)
       path = os.path.join(slave, thread, file)
       current_file = open(path, "w")
       file_no = file_no + 1
       last_thread[slave_nr] = (last_thread[slave_nr] + 1) % NUMBER_OF_JOBS

   print >> current_file, line.replace(" ", "_")[:-1] 
   current_number = current_number + 1
   if current_number % ITEMS_PER_PACKAGE == 0:
       current_file.close()
       current_file = None
