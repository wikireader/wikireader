#
# (C) Copyright 2008 OpenMoko, Inc.
# Author: xiangfu liu <xiangfu@openmoko.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

PROGRAM = det-wikird.exe
CC = gcc
#INCLUDEDIRS = .
CPPSOURCES = det-wikird.cpp
CPPOBJECTS = det-wikird.obj
CPPFLAGS = -mwindows

$(PROGRAM): $(CPPSOURCES)
	$(CC) $(CPPFLAGS) -o $@ $(CPPSOURCES)

det-wikird.obj: det-wikird.cpp det-wikird.h

clean:
	del $(CPPOBJECTS) $(PROGRAM)

run:
	$(PROGRAM)

