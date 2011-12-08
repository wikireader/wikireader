/*
 * Copyright (c) 2009 Openmoko Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <grifo.h>

#include "ustring.h"
#include "bmf.h"
#include "wikilib.h"

int load_bmf(pcffont_bmf_t *font)
{
	int fd;
	font_bmf_header header;

	if (NULL == font || NULL == font->file) {
		fatal_error("font is NULL");
	}

	fd = file_open(font->file, FILE_OPEN_READ);
	if(fd < 0) {
		panic("failed to open font: %s", font->file);
		//debug_printf("failed to open font: %s\n", font->file);
		return -1;
	}

	file_size(font->file, &(font->file_size));
	if (0 == font->file_size) {
		fatal_error("zero size font: %s", font->file);
	}
	font->charmetric = (char*)memory_allocate(font->file_size, "bmf");
	if (!font->charmetric) {
		fatal_error("load_bmf malloc error on: %s", font->file);
	}
	memset(font->charmetric, 0, font->file_size);

	file_read(fd, font->charmetric, 256 * sizeof(charmetric_bmf)+sizeof(font_bmf_header));

	memcpy(&header,font->charmetric,sizeof(font_bmf_header));

	font->Fmetrics.linespace = header.linespace;
	font->Fmetrics.ascent    = header.ascent;
	font->Fmetrics.descent   = header.descent;
	font->Fmetrics.default_char = header.default_char;

	return fd;
}

int
pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics)
{
	int size = 0;
	int offset = 0;
	char buffer[1024];
	int font_header;
	int bFound = 0;

	memset(buffer,0,1024);

	if(font==NULL || font->fd < 0)
		return -1;

	if (font->fd == FONT_FD_NOT_INITED)
	{
		font->fd = load_bmf(font);
		if(font->fd < 0)
			return -1;
	}
	font_header =  sizeof(font_bmf_header);

	if(val <= 256)
	{
		memcpy(Cmetrics,font->charmetric+val*sizeof(charmetric_bmf)+font_header,sizeof(charmetric_bmf));
	}
	else
	{
		offset = val*sizeof(charmetric_bmf)+font_header;
		if (offset <= (long)font->file_size - (long)sizeof(charmetric_bmf))
		{
			memcpy(Cmetrics,font->charmetric+val*sizeof(charmetric_bmf)+font_header,sizeof(charmetric_bmf));
			if (Cmetrics->width)
			{
				bFound = 1;
			}
			else
			{
				file_lseek(font->fd,offset);
			}
		}
		else
		{
			if (font->bPartialFont)
			{ // character not defined in the current font file (and it is intended to include partial characters)
				font = font->supplement_font;
				return pres_bmfbm(val, font, bitmap, Cmetrics);
			}
			else
				return -1;
		}

		if (!bFound)
		{
			size = sizeof(charmetric_bmf);

			file_read(font->fd,buffer,size);
			memcpy(Cmetrics,buffer,sizeof(charmetric_bmf));
			memcpy(font->charmetric+val*sizeof(charmetric_bmf)+font_header,Cmetrics,sizeof(charmetric_bmf));
		}
	}

	if(Cmetrics->width>0)
		*bitmap = (bmf_bm_t*)Cmetrics+8;
	else
	{
		if (val > 256 && offset <= (long)font->file_size - (long)sizeof(charmetric_bmf))
		{
			if (font->Fmetrics.default_char && val != (ucs4_t)font->Fmetrics.default_char)
			{
				pres_bmfbm(font->Fmetrics.default_char, font, bitmap, Cmetrics);
			}
			if (!Cmetrics->width)
			{
				Cmetrics->width = 1;
				Cmetrics->height = 0;
			}
			memcpy(font->charmetric+val*sizeof(charmetric_bmf)+font_header,Cmetrics,sizeof(charmetric_bmf));
		}
	}

	return 1;
}
