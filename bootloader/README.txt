
	OpenMoko Dictionary hardware bootloader
	Copyright (c) 7/8/2008 by Daniel Mack <daniel@caiaq.de>


(0) Preamble

Epson's S1C33E07 CPU has the possibility to boot from the following media:

	* NAND flash
	* NOR flash
	* SPI EEPROM
	* RS232

For the Wikipedia reader, we only support SPI EEPROM (for normal operation)
and RS232 (for initial bootstrapping and developent). This two options are
selected by the CE10# line which is available at the PCB. The default is
CE10# = low which make the CPU boot from EEPROM.

The goal is to have the kernel for the application located on the SD card
and keep the bootloader as simple as possible.

On each power cycle, the CPU reads 512 bytes of instruction code, copies
them to address 0x0 (internal RAM) and then vectors to this address. As
this is not enough space to store a bootloader which communicates with
the SD card, there is need for some more fuzzy logic.

The bootloader is thus split into three parts which are described in
detail here.


(1) The first stage bootloader (rs232)

This image is needed for bootstrapping a naked device and for debugging
only. It is loaded via the serial port and has the following tasks to do:

	* Set up the serial console to 57600/8N1
	* Take commands from serial line to toggle the EEPROM's CS# signal
	  and to read and write from the EEPROM

Hence, it is more or less a remote control for the EEPROM to allow the
host to write an initial EEPROM image.


(2) The first stage EEPROM bootloader (eeprom-1st)

Once the device booted from EEPROM, we run into the same problem like
before - the CPU only copied 512 bytes from EEPROM page 0 to address 0x0
and then vectors to it. The job of this image is to load more bytes from
the EEPROM and call the next bootloader. These are the functions:

	* Initialize the SPI interface
	* Set up the serial console to 57600/8N1
	* Initialize the external SDRAM (mapped to area 19, 0x10000000)
	* Read a fix number (determined at compile time) of bytes from
	  EEPROM's logical address 0x300 and copy the to the external
	  RAM @0x10000000
	* Outputs the character '!' to indicate the following jump
	* Jump to the location of the newly loaded code

For development purposes (to avoid time intensive EEPROM flashing/
rebooting cycles all the time), this loader also has a second use case
which can be selected at compile time with a simple #define switch in
the first lines. When compiled for serial mode, it does the following:

	* Set up the serial console to 57600/8N1
	* Initialize the external SDRAM (mapped to area 19, 0x10000000)
	* Loads a fix number (hardcoded in this case, 30000 for now)
	  from the RS232 line and copies them to the external RAM location
	* Outputs the character '!' to indicate the following jump
	* Jump to the location of the newly loaded code

With this mode, development became a lot easier, even though it is not
the usual case for deployment later.


(3) The second stage EEPROM bootloader (eeprom-2nd)

This is where SD card handling and file loading is done and we don't
have this dramatic space problem here. The code is based on efsl and
has some abstraction functions for low-level SPI communication tasks
specific to our hardware design.

 ...



(4) The e07load utility

To flash the device and bootstrap the bootloaders, there is a little
utility called e07load. It assumes the CPU is in RS232 boot mode and
does the following things when started:

	* Opens the serial console with 57600/8N1
	* Sends out 4 sync bytes (0x80, 0x80, 0x80, 0x80)
	* Reads back the CPU ID (supposed to be 0x06, 0x0e, 0x07, 0x00)
	* Sends out the 512 bytes of bootstrap code (read from file 'rs232')
	* Reads back the 512 bytes of bootstrap code for verification
	* By now, the CPU has already vectored to our 'remote control' code
	  so we can use it now
	* Use the 'SPI remote control' code implemented in the first stage
	  bootloader and write the 512 bytes EEPROM code
	* Read back the 512 bytes EEPROM code for verification
	* Write the 2nd level EEPROM bootloader at address 0x300 where it
	  will be read by the first stage EEPROM loader later
	* Read back this image for verification


(*) Links

efsl:
S1C33E07 datasheet:

