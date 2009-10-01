#
# (C) Copyright 2008 Openmoko, Inc.
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

#CC = g++  # for Mingw under Windows
CC = i586-mingw32msvc-g++ # for Mingw under Linux
SRC_DIR = ../src

CPP_SRCS = ./det-wikird.cpp
CPP_OBJS = $(patsubst %.cpp,%.obj, $(CPP_SRCS))

C_SRCS = $(SRC_DIR)/check-wikireader.c
C_OBJS = $(patsubst %.c,%.o, $(C_SRCS))

C_FLAGS = -Wall -I. -I$(SRC_DIR) -mwindows -DWINDOWS -g

TARGETS = det-wikird.exe

%.o : %.c
	$(CC) $(C_FLAGS) -c $< -o $@

%.obj : %.cpp
	$(CC) $(C_FLAGS) -c $< -o $@

$(C_OBJS): $(C_SRCS)
$(CPP_OBJS): $(CPP_SRCS)

all: $(TARGETS)

$(TARGETS): $(CPP_OBJS) $(C_OBJS)
	$(CC) $(C_FLAGS) -o $(TARGETS) $(CPP_OBJS) $(C_OBJS)

clean:
	rm $(C_OBJS) $(CPP_OBJS) $(TARGETS)

run:
	$(TARGETS)
