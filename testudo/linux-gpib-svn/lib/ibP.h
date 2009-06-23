/***************************************************************************
                              lib/ibP.h
                             -------------------

    copyright            : (C) 2001,2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _IBP_H
#define _IBP_H

#include "ibConf.h"

/* Unit descriptor flag */

#define GPIB_CONFIGS_LENGTH 0x1000
#define FIND_CONFIGS_LENGTH 64	/* max number of devices we can read from config file */

extern ibBoard_t ibBoard[];
extern ibConf_t *ibConfigs[];
extern ibConf_t ibFindConfigs[];

#include <errno.h>
#include <fcntl.h>

static const int sad_offset = 0x60;

#endif	/* _IBP_H */
