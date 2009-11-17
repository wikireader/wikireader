/*
 * init - the first user process to run
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "grifo.h"


int main(int argc, char **argv)
{
	debug_print("init starting\n");

	//panic("init broken %d\n", 42);
	//chain("hello.app one two 'three words here' --four='some \"quoted words\"'\" also 'quoted here'\" five");
	chain("events.app wait");

	return 0;
}
