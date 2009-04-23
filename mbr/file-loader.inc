/*
    e07 bootloader suite - generic file loader
    Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "application.h"
#include "eeprom.h"
#include "elf32.h"


int main(void)
{
	APPLICATION_INITIALISE();

	// boot an elf file
	{
		unsigned int i;
		for (i = 0; 0 != LoadList[i]; ++i) {
			int error = - elf_exec(LoadList[i]);
			print("load '");
			print(LoadList[i]);
			print("' error = ");
			print_u32(error);
			print_char('\n');
		}
	}

	APPLICATION_FINALISE(0);
}
