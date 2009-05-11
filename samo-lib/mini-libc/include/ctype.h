/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: ctype.h,v 1.7 2006/11/15 14:34:56 coppice Exp $
 */

#if !defined(__CTYPE_H_)
#define __CTYPE_H_

extern inline int isalpha(int c)		{ return ((c>='A'&&c<='Z')||(c>='a'&&c<='z')); }

extern inline int iscntrl(int c)		{ return ((c>=0&&c<=27)||c==127);}

extern inline int isdigit(int c)		{ return (((c)>='0')&&((c)<='9'));}

extern inline int isalnum(int c)		{ return (isalpha(c)||isdigit(c));}

extern inline int isspace(int c)		{ return ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'));}

extern inline int isascii(int c)		{ return (((c) & ~0x7f) == 0);}

extern inline int isupper(int c)		{ return ((c) >= 'A' && (c) <= 'Z');}

extern inline int islower(int c)		{ return ((c) >= 'a' && (c) <= 'z');}

extern inline int isprint(int c)		{ return (c >= ' ' && c < 127);}

extern inline int isblank(int c)		{ return (c == '\t' || c == ' ');}

extern inline int isxdigit(int c)     
{
	return (isdigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'));
}

extern inline int ispunct(int c)
{
	return (isprint (c) && !islower(c) && !isupper(c) && c != ' ' && !isdigit(c));
}

extern inline int toupper(int c)		{ return ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')));}

extern inline int tolower(int c)		{ return ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z'))) ;} 

extern inline int toascii(int c)		{ return (c & 0x7f); }

#endif
