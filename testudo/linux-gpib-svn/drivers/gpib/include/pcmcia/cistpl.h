/*
 * pcmcia/cistpl.h compatibility header
 */
/*
    Copyright (C) 2005-2006 Ian Abbott
    Copyright (C) 2009 Frank Mori Hess <fmhess@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __COMPAT_PCMCIA_CISTPL_H_
#define __COMPAT_PCMCIA_CISTPL_H_

#include_next <pcmcia/cistpl.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)

/* pcmcia_parse_tuple() macro only has 2 params for 2.6.28 and later. */
#define PCMCIA_PARSE_TUPLE(tuple, parse) pccard_parse_tuple(tuple, parse)

#else

#define PCMCIA_PARSE_TUPLE(tuple, parse) pcmcia_parse_tuple(tuple, parse)

#endif

#endif // __COMPAT_PCMCIA_CISTPL_H_
