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

#include <types.h>
#include <misc.h>
#include "regs.h"
#include "wikireader.h"
#include "lcd.h"


#if LCD_USES_SPI
u8 spi_transmit_lcd(u8 out)
{
	REG_SPI_TXD = out;
	do {} while (REG_SPI_STAT & (1 << 6));
	return REG_SPI_RXD;
}
#endif

void init_lcd(void)
{
	LCD_DISPLAY_OFF();

	// set up LCD clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK0 |=
		LCDCAHBIF_CKE |
		LCDCSAPB_CKE |
		LCDC_CKE |
		0;
#if 0
	REG_CMU_CLKCNTL = (REG_CMU_CLKCNTL & ~LCDCDIV_MASK) | LCDCDIV_16;
#endif
	REG_CMU_PROTECT = CMU_PROTECT_ON;


#if LCD_USES_SPI
	LCD_CS_LO();

	/* SPI setup - should move somewhere else */
	REG_SPI_WAIT = 0x00000000;
	REG_SPI_RXMK = 0x00000000;
	REG_SPI_INT = 0x00000014;
	REG_SPI_CTL1 = 0x00001C73;

	LCD_CS_HI();
	delay(10);
	LCD_CS_LO();
#endif

	/* power down LCDC */
	REG_LCDC_PS =
		//INTF |
		//VNDPF |
		//PSAVE_NORMAL |
		//PSAVE_DOZE |
		PSAVE_POWER_SAVE |
		0;

	/* HT = (47+1) * 8 = 384 characters, HDP = (39+1) * 8 = 320 characters */
#define HDP (LCD_VRAM_WIDTH_BYTES - 1)
#define HT (HDP + 8)

	REG_LCDC_HD =
		(HT << HTCNT_SHIFT) |
		(HDP << HDPCNT_SHIFT) |
		0;

	/* VT = 244 + 1 = 255 lines, VDP = 239 + 1 = 480 lines */
#define VDP (LCD_VRAM_HEIGHT_LINES - 1)

	REG_LCDC_VD =
		((VDP + 1) << VTCNT_SHIFT) |
		(VDP << VDPCNT_SHIFT) |
		0;

	/* wf counter = 0 */
	REG_LCDC_MR = 0;

	/* LCDC Display Mode Register, grayscale */
#if LCD_MONOCHROME
	REG_LCDC_DMD =
		//TFTSEL |
		//COLOR |
		FPSMASK |

		//DWD_8_BIT_2 |
		//DWD_8_BIT_1 |
		DWD_4_BIT |

		//SWINV |
		//BLANK |
		//FRMRPT |
		//DITHEN |
		LUTPASS |

		//BPP_16 |
		//BPP_12 |
		//BPP_8 |
		//BPP_4 |
		//BPP_2 |
		BPP_1 |
		0;

#else
	REG_LCDC_DMD =
		//TFTSEL |
		//COLOR |
		FPSMASK |

		//DWD_8_BIT_2 |
		//DWD_8_BIT_1 |
		DWD_4_BIT |

		//SWINV |
		//BLANK |
		//FRMRPT |
		//DITHEN |
		LUTPASS |

		//BPP_16 |
		//BPP_12 |
		//BPP_8 |
		BPP_4 |
		//BPP_2 |
		//BPP_1 |
		0;
#endif

	/* relocate the frame buffer RAM */
	REG_LCDC_MADD = LCD_VRAM;

#if LCD_USES_SPI
	/* LCDC on */
	spi_transmit_lcd(0xa8);

	LCD_CS_HI();
	delay(10);
	LCD_CS_LO();
#endif

	/* set reg_power_save = 11b (normal mode) */
	REG_LCDC_PS =
		//INTF |
		//VNDPF |
		PSAVE_NORMAL |
		//PSAVE_DOZE |
		//PSAVE_POWER_SAVE |
		0;

	delay_us(1000);

	LCD_DISPLAY_ON();
}
