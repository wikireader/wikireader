/*
    e07 bootloader suite
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

#define	APPLICATION_TITLE "keypad test"
#include "application.h"


// main() must be first as the loader executes from the first program address
int main(void)
{
	APPLICATION_INITIALISE();

	print("press buttons on keypad. [any serial input to exit]\n");

	while (!serial_input_available()) {
		print("keys = 0x");
		print_byte(REG_P6_P6D & 0x07);
		print("\n");
	}

	(void)serial_input_char();

	APPLICATION_FINALISE(0);
}
