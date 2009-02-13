/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

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

#include "regs.h"
#include "wikireader.h"
#include "lcd.h"
#include "types.h"

extern void delay(u32);

u8 spi_transmit_lcd(u8 out)
{
	REG_SPI_TXD = out;
	do {} while (REG_SPI_STAT & (1 << 6));
	return REG_SPI_RXD;
}

void init_lcd(void)
{
	LCD_CS_LO();
	TFT_CTL1_LO();

        /* disable write protection of clock registers */
        REG_CMU_PROTECT = 0x96;
	REG_CMU_GATEDCLK0 |= 0x7;
#if 0
	/* set the LCDC_CLK to 1/16 */
	REG_CMU_CLKCNTL |= 0xF0000;
#endif

	/* re-enable write protection of clock registers */
	REG_CMU_PROTECT = 0x00;

	/* SPI setup - should move somewhere else */
	REG_SPI_WAIT = 0x00000000;
	REG_SPI_RXMK = 0x00000000;
	REG_SPI_INT = 0x00000014;
	REG_SPI_CTL1 = 0x00001C73;

	LCD_CS_HI();
	delay(10);
	LCD_CS_LO();

	/* power down LCDC */
  	REG_LCDC_PS = 0x00000000;

  	/* HT = (47+1) * 8 = 384 characters, HDP = (39+1) * 8 = 320 characters */
  	REG_LCDC_HD = 0x002f0027;

  	/* VT = 244 + 1 = 255 lines, VDP = 239 + 1 = 480 lines */
  	REG_LCDC_VD = 0x00f400ef;

  	/* wf counter = 0 */
  	REG_LCDC_MR = 0x0;

	/* LCDC Display Mode Register, grayscale */
#if LCD_MONOCHROME
	REG_LCDC_DMD = 0x22000010;
#else
  	REG_LCDC_DMD = 0x22000012;
#endif

	/* relocate the frame buffer RAM */
  	REG_LCDC_MADD = LCD_VRAM;

	/* LCDC on */
	spi_transmit_lcd(0xa8);

	LCD_CS_HI();
	delay(10);
	LCD_CS_LO();

  	/* set reg_power_save = 11b (normal mode) */
  	REG_LCDC_PS |= 0x00000003;

	delay(0x10000);
	TFT_CTL1_HI();
}

