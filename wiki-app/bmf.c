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
#include <string.h>
#include <errno.h>

#ifdef WIKIPCF
#include <assert.h>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <file-io.h>
#include <malloc-simple.h>
#include <msg.h>
#endif

#include "bmf.h"

int load_bmf(pcffont_bmf_t *font)
{
	int fd,file_size,read_size;
	font_bmf_header header;

	fd = openfile(font->file,0);

	if(fd<0)
		return -1;

	file_size = 256 * sizeof(charmetric_bmf)+sizeof(font_bmf_header);
	font->charmetric = (char*)Xalloc(file_size);

	read_size = readfile(fd, font->charmetric, file_size);

	memcpy(&header,font->charmetric,sizeof(font_bmf_header));

	font->Fmetrics.linespace = header.linespace;
	font->Fmetrics.ascent    = header.ascent;
	font->Fmetrics.descent   = header.descent;

#ifdef WIKIPCF
	font->file_size = lseek(fd, 0, SEEK_END);
#else
	wl_fsize(fd, &(font->file_size));
#endif

	return fd;
}

int
pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics)
{
	int size = 0;
	int read_size = 0, offset = 0;
	char buffer[1024];
	int font_header;

	memset(buffer,0,1024);

	if(font==NULL || font->fd < 0)
		return -1;

	font_header =  sizeof(font_bmf_header);
	size = val*sizeof(charmetric_bmf)+font_header;

	if(val <= 256)
	{
		memcpy(Cmetrics,font->charmetric+val*sizeof(charmetric_bmf)+font_header,sizeof(charmetric_bmf));
	}
	else
	{
		offset = val*sizeof(charmetric_bmf)+font_header;
		if (offset <= font->file_size - sizeof(charmetric_bmf))
		{
#ifdef WIKIPCF
			lseek(font->fd,offset,SEEK_SET);
#else
			//closefile(font->fd);
			//font->fd = openfile(font->file,0);
			//wl_seek(font->fd,0);
			wl_seek(font->fd,offset);

#endif
		}
		else
		{
			if (font->bPartialFont)
			{ // character not defined in the current font file (and it is intended to include partial characters)
				font = font->supplement_font;
				return pres_bmfbm(val, font, bitmap, Cmetrics);
				//return -1;
			}
			else
				return -1;
		}

		size = 1024; // Due to the nature of wl_read, the read size needs to be 1024.

		read_size = readfile(font->fd,buffer,size);
		memcpy(Cmetrics,buffer,sizeof(charmetric_bmf));
		if((Cmetrics->height*Cmetrics->widthBytes)==0)
		{

			font = font->supplement_font;
			return pres_bmfbm(val, font, bitmap, Cmetrics);
			//return -1;
		}
	}

	if(Cmetrics->width>0)
		*bitmap = (bmf_bm_t*)Cmetrics+8;
	else
		return -1;

	return 1;
}

unsigned long *
Xalloc (int m)
{
#ifdef WIKIPCF
	void * mem = malloc(m);
	return (unsigned long *)mem;
#else
	void * mem = malloc_simple(m,MEM_TAG_ARTICLE_F1);
	return (unsigned long *)mem;
#endif
}
/*int load_bmf(pcffont_bmf_t *font)
  {
  int fd,file_size,read_size;
  char buffer[1024];
  font_bmf_header header;

  fd = openfile(font->file,0);

  if(fd<0)
  return -1;

  file_size = readfile(fd, buffer, 1024);
  memcpy(&header,buffer,sizeof(font_bmf_header));
  closefile(fd);

  fd = openfile(font->file,0);

  if(fd<0)
  return -1;

  file_size = 65535*sizeof(charmetric_bmf_header) + sizeof(font_bmf_header)+header.bmp_buffer_len*256;
  font->charmetric = (char*)Xalloc(file_size);
  read_size = readfile(fd, font->charmetric, file_size);

  font->Fmetrics.linespace = header.linespace;
  font->Fmetrics.ascent    = header.ascent;
  font->Fmetrics.descent   = header.descent;
  font->bmp_buffer_len     = header.bmp_buffer_len;

  #ifdef WIKIPCF
  font->file_size = lseek(fd, 0, SEEK_END);
  #else
  wl_fsize(fd, &(font->file_size));
  #endif

  return fd;
  }*/
/*
  int
  pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics)
  {
  int size = 0;
  int read_size = 0, offset = 0;
  char buffer[1024];
  charmetric_bmf_header bmf_header;
  int font_header;

  font_header =  sizeof(font_bmf_header);
  if(val <= 256)
  {
  offset = val*sizeof(charmetric_bmf_header)+font_header;

  memcpy(&bmf_header,font->charmetric+val*sizeof(charmetric_bmf_header)+font_header,sizeof(charmetric_bmf_header));

  memcpy(Cmetrics,&bmf_header,sizeof(charmetric_bmf)-font->bmp_buffer_len);
  memcpy(Cmetrics->bitmap,font->charmetric+bmf_header.pos,font->bmp_buffer_len);
  *bitmap = (bmf_bm_t*)Cmetrics+8;
  return 1;
  }
  else
  {
  offset = val*sizeof(charmetric_bmf_header)+font_header;
  if (offset <= font->file_size - sizeof(charmetric_bmf))
  #ifdef WIKIPCF
  lseek(font->fd,offset,SEEK_SET);
  #else
  wl_seek(font->fd,offset);
  #endif
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

//              size = sizeof(charmetric_bmf);
size = 1024; // Due to the nature of wl_read, the read size needs to be 1024.

read_size = readfile(font->fd,buffer,size);
memcpy(&bmf_header,buffer,sizeof(charmetric_bmf_header));
if(bmf_header.pos<=0)
return -1;
#ifdef WIKIPCF
lseek(font->fd,bmf_header.pos,SEEK_SET);
#else
wl_seek(font->fd,bmf_header.pos);
#endif
memset(buffer,0,1024);
read_size = readfile(font->fd,buffer,size);

memcpy(Cmetrics,&bmf_header,sizeof(charmetric_bmf)-font->bmp_buffer_len);

}

if(Cmetrics->height>0 && Cmetrics->widthBytes>0)
{
memcpy(Cmetrics->bitmap,buffer,font->bmp_buffer_len);
*bitmap = (bmf_bm_t*)Cmetrics+8;

}
else
return -1;

return 1;
}*/

 /*int load_bmf(pcffont_bmf_t *font)
   {
   int fd,file_size,read_size;
   char buffer[1024];
   font_bmf_header header;

   fd = openfile(font->file,0);

   if(fd<0)
   return -1;

   file_size = readfile(fd, buffer, 1024);
   memcpy(&header,buffer,sizeof(font_bmf_header));
   closefile(fd);

   fd = openfile(font->file,0);

   if(fd<0)
   return -1;

   file_size = 65535*sizeof(charmetric_bmf_header) + sizeof(font_bmf_header)+64*256;
   font->charmetric = (char*)Xalloc(file_size);
   read_size = readfile(fd, font->charmetric, file_size);

   font->Fmetrics.linespace = header.linespace;
   font->Fmetrics.ascent    = header.ascent;
   font->Fmetrics.descent   = header.descent;

   #ifdef WIKIPCF
   font->file_size = lseek(fd, 0, SEEK_END);
   #else
   wl_fsize(fd, &(font->file_size));
   #endif

   return fd;
   }

   int
   pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics)
   {
   int size = 0;
   int read_size = 0, offset = 0;
   char buffer[1024];
   charmetric_bmf_header bmf_header;
   int font_header;

   font_header =  sizeof(font_bmf_header);
   if(val <= 256)
   {
   offset = val*sizeof(charmetric_bmf_header)+font_header;

   memcpy(&bmf_header,font->charmetric+val*sizeof(charmetric_bmf_header)+font_header,sizeof(charmetric_bmf_header));

   memcpy(Cmetrics,&bmf_header,sizeof(charmetric_bmf)-64);
   memcpy(Cmetrics->bitmap,font->charmetric+bmf_header.pos,64);
   *bitmap = (bmf_bm_t*)Cmetrics+8;
   return 1;
   }
   else
   {
   offset = val*sizeof(charmetric_bmf_header)+font_header;
   if (offset <= font->file_size - sizeof(charmetric_bmf))
   #ifdef WIKIPCF
   lseek(font->fd,offset,SEEK_SET);
   #else
   wl_seek(font->fd,offset);
   #endif
   else
   {
   if (font->bPartialFont)
   { // character not defined in the current font file (and it is intended to include partial characters)
   font++;
   return pres_bmfbm(val, font, bitmap, Cmetrics);
   }
   else
   return -1;
   }

//              size = sizeof(charmetric_bmf);
size = 1024; // Due to the nature of wl_read, the read size needs to be 1024.

read_size = readfile(font->fd,buffer,size);
memcpy(&bmf_header,buffer,sizeof(charmetric_bmf_header));
if(bmf_header.pos<=0)
return -1;
#ifdef WIKIPCF
lseek(font->fd,bmf_header.pos,SEEK_SET);
#else
wl_seek(font->fd,bmf_header.pos);
#endif
memset(buffer,0,1024);
read_size = readfile(font->fd,buffer,size);

memcpy(Cmetrics,&bmf_header,sizeof(charmetric_bmf)-64);

}

if(Cmetrics->height>0 && Cmetrics->widthBytes>0)
{
memcpy(Cmetrics->bitmap,buffer,64);
*bitmap = (bmf_bm_t*)Cmetrics+8;

}
else
return -1;

return 1;
}
unsigned long *
Xalloc (int m)
{
#ifdef WIKIPCF
void * mem = malloc(m);
return (unsigned long *)mem;
#else
void * mem = malloc_simple(m,MEM_TAG_ARTICLE_F1);
return (unsigned long *)mem;
#endif
}*/

/*
  int load_bmf(pcffont_bmf_t *font)
  {
  int fd,file_size,read_size;

  fd = openfile(font->file,0);

  if(fd<0)
  return -1;

  file_size = (256+1) * sizeof(charmetric_bmf);
  font->charmetric = (char*)Xalloc(file_size);

  read_size = readfile(fd, font->charmetric, file_size);
  #ifdef WIKIPCF
  font->file_size = lseek(fd, 0, SEEK_END);
  #else
  wl_fsize(fd, &(font->file_size));
  #endif

  return fd;
  }

  int
  pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics)
  {
  int size = 0;
  int read_size = 0, offset = 0;
  char buffer[1024];

  if(val <= 256)
  memcpy(Cmetrics,font->charmetric+(val+1)*sizeof(charmetric_bmf),sizeof(charmetric_bmf));
  else
  {
  offset = (val+1)*sizeof(charmetric_bmf);
  if (offset <= font->file_size - sizeof(charmetric_bmf))
  #ifdef WIKIPCF
  lseek(font->fd,offset,SEEK_SET);
  #else
  wl_seek(font->fd,offset);
  #endif
  else
  {
  if (font->bPartialFont)
  { // character not defined in the current font file (and it is intended to include partial characters)
  font++;
  return pres_bmfbm(val, font, bitmap, Cmetrics);
  }
  else
  return -1;
  }

//              size = sizeof(charmetric_bmf);
size = 1024; // Due to the nature of wl_read, the read size needs to be 1024.

read_size = readfile(font->fd,buffer,size);
memcpy(Cmetrics,buffer,sizeof(charmetric_bmf));
}

if(Cmetrics->width>0)
*bitmap = (bmf_bm_t*)Cmetrics+8;
else
return -1;

return 1;
}
unsigned long *
Xalloc (int m)
{
#ifdef WIKIPCF
void * mem = malloc(m);
return (unsigned long *)mem;
#else
void * mem = malloc_simple(m,MEM_TAG_ARTICLE_F1);
return (unsigned long *)mem;
#endif
}*/
