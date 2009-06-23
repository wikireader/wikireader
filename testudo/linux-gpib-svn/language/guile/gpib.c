/*
 * gpib.c - guile binding for LinuxGpib
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
 *
 * LinuxGpib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LinuxGpib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LinuxGpib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <guile/gh.h>
#include <gpib/ib.h>

#include "gpib.h"

#define FUNC_NAME "ibdev"
SCM
guile_ibdev (SCM board_index, SCM pad, SCM sad, SCM timeout, SCM eoi, SCM eos)
{
  int fd, _eoi;

  SCM_ASSERT_TYPE (SCM_EXACTP (board_index), board_index, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (pad), pad, 
		   SCM_ARG2, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (sad), sad, 
		   SCM_ARG3, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (timeout), timeout, 
		   SCM_ARG4, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (eoi) || SCM_BOOLP (eoi), eoi, 
		   SCM_ARG5, FUNC_NAME, "exact or bool");
  SCM_ASSERT_TYPE (SCM_EXACTP (eos), eos, 
		   SCM_ARG6, FUNC_NAME, "exact");

  if (SCM_BOOLP (eoi))
    _eoi = SCM_NFALSEP (eoi);
  else
    _eoi = SCM_NUM2INT (SCM_ARG5, eoi);


  if ((fd = ibdev (SCM_NUM2INT (SCM_ARG1, board_index), 
		   SCM_NUM2INT (SCM_ARG2, pad),
		   SCM_NUM2INT (SCM_ARG3, sad),
		   SCM_NUM2INT (SCM_ARG4, timeout),
		   _eoi,
		   SCM_NUM2INT (SCM_ARG6, eos))) < 0) {
    scm_syserror (FUNC_NAME);
  }
  return scm_int2num (fd);
}
#undef FUNC_NAME

#define FUNC_NAME "ibwrt"
SCM
guile_ibwrt (SCM ud, SCM data)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_STRINGP (data), data, 
		   SCM_ARG2, FUNC_NAME, "string");

  if ((ret = ibwrt (SCM_NUM2INT (SCM_ARG1, ud), 
		    SCM_STRING_CHARS (data),
		    SCM_NUM2INT (SCM_ARG2, 
				 scm_string_length (data)))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibcmd"
SCM
guile_ibcmd (SCM ud, SCM commands)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_STRINGP (commands), commands, 
		   SCM_ARG2, FUNC_NAME, "string");

  if ((ret = ibcmd (SCM_NUM2INT (SCM_ARG1, ud), 
		    SCM_STRING_CHARS (commands),
		    SCM_NUM2INT (SCM_ARG2,
				 scm_string_length (commands)))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibrd"
SCM
guile_ibrd (SCM ud, SCM bytes)
{
  int ret, len;
  char *data;
  SCM val;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (bytes), bytes, 
		   SCM_ARG2, FUNC_NAME, "exact");
  len = SCM_NUM2INT (SCM_ARG2, bytes);

  if ((data = (char *) malloc (len)) == NULL) {
    scm_memory_error (FUNC_NAME);
  }

  if ((ret = ibrd (SCM_NUM2INT (SCM_ARG1, ud), data, len)) & ERR) {
    free (data);
    return SCM_BOOL_F;
  }

  val = scm_mem2string (data, ibcnt);
  free (data);
  return val;
}
#undef FUNC_NAME

#define FUNC_NAME "ibfind"
SCM
guile_ibfind (SCM name)
{
  char *str;
  int ret;

  SCM_ASSERT_TYPE (SCM_STRINGP (name), name, 
		   SCM_ARG1, FUNC_NAME, "string");
  str = guile_to_string (name);
  
  if ((ret = ibfind (str)) < 0) {
    free (str);
    return SCM_BOOL_F;
  }
  free (str);
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibsre"
SCM
guile_ibsre (SCM ud, SCM enable)
{
  int ret, val;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (enable) || SCM_BOOLP (enable), enable, 
		   SCM_ARG2, FUNC_NAME, "exact or bool");

  if (SCM_BOOLP (enable))
    val = SCM_NFALSEP (enable);
  else
    val = SCM_NUM2INT (SCM_ARG2, enable);
  
  if ((ret = ibsre (SCM_NUM2INT (SCM_ARG1, ud), val)) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibsic"
SCM
guile_ibsic (SCM ud)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  
  if ((ret = ibsic (SCM_NUM2INT (SCM_ARG1, ud))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibclr"
SCM
guile_ibclr (SCM ud)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  
  if ((ret = ibclr (SCM_NUM2INT (SCM_ARG1, ud))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibonl"
SCM
guile_ibonl (SCM ud, SCM online)
{
  int ret, val;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (online) || SCM_BOOLP (online), online, 
		   SCM_ARG2, FUNC_NAME, "exact or bool");
  
  if (SCM_BOOLP (online))
    val = SCM_NFALSEP (online);
  else
    val = SCM_NUM2INT (SCM_ARG2, online);
  
  if ((ret = ibonl (SCM_NUM2INT (SCM_ARG1, ud), val)) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibwait"
SCM
guile_ibwait (SCM ud, SCM status_mask)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (status_mask), status_mask, 
		   SCM_ARG2, FUNC_NAME, "exact");

  if ((ret = ibwait (SCM_NUM2INT (SCM_ARG1, ud),
		     SCM_NUM2INT (SCM_ARG2, status_mask))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibrsp"
SCM
guile_ibrsp (SCM ud)
{
  int ret;
  char poll[16];

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  
  if ((ret = ibrsp (SCM_NUM2INT (SCM_ARG1, ud), poll)) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (poll[0]);
}
#undef FUNC_NAME

#define FUNC_NAME "ibtrg"
SCM
guile_ibtrg (SCM ud)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  
  if ((ret = ibtrg (SCM_NUM2INT (SCM_ARG1, ud))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "ibrsv"
SCM
guile_ibrsv (SCM ud, SCM status_byte)
{
  int ret;

  SCM_ASSERT_TYPE (SCM_EXACTP (ud), ud, 
		   SCM_ARG1, FUNC_NAME, "exact");
  SCM_ASSERT_TYPE (SCM_EXACTP (status_byte), status_byte, 
		   SCM_ARG2, FUNC_NAME, "exact");
  
  if ((ret = ibrsv (SCM_NUM2INT (SCM_ARG1, ud),
		    SCM_NUM2INT (SCM_ARG2, status_byte))) & ERR) {
    return SCM_BOOL_F;
  }
  return scm_int2num (ret);
}
#undef FUNC_NAME

#define FUNC_NAME "iberr"
SCM guile_iberr (void)
{
  return scm_int2num (iberr);
}
#undef FUNC_NAME

#define FUNC_NAME "ibcnt"
SCM guile_ibcnt (void)
{
  return scm_long2num (ibcntl);
}
#undef FUNC_NAME

void
gpib_bindings (void)
{
  scm_c_define_gsubr ("ibdev",  6, 0, 0, guile_ibdev);
  scm_c_define_gsubr ("ibwrt",  2, 0, 0, guile_ibwrt);
  scm_c_define_gsubr ("ibcmd",  2, 0, 0, guile_ibcmd);
  scm_c_define_gsubr ("ibrd",   2, 0, 0, guile_ibrd);
  scm_c_define_gsubr ("ibfind", 1, 0, 0, guile_ibfind);
  scm_c_define_gsubr ("ibsre",  2, 0, 0, guile_ibsre);
  scm_c_define_gsubr ("ibsic",  1, 0, 0, guile_ibsic);
  scm_c_define_gsubr ("ibclr",  1, 0, 0, guile_ibclr);
  scm_c_define_gsubr ("ibonl",  2, 0, 0, guile_ibonl);
  scm_c_define_gsubr ("ibwait", 2, 0, 0, guile_ibwait);
  scm_c_define_gsubr ("ibrsp",  1, 0, 0, guile_ibrsp);
  scm_c_define_gsubr ("ibtrg",  1, 0, 0, guile_ibtrg);
  scm_c_define_gsubr ("ibrsv",  1, 0, 0, guile_ibrsv);
  scm_c_define_gsubr ("iberr",  0, 0, 0, guile_iberr);
  scm_c_define_gsubr ("ibcnt",  0, 0, 0, guile_ibcnt);

  scm_c_define ("DCAS",  scm_int2num (DCAS));
  scm_c_define ("DTAS",  scm_int2num (DTAS));
  scm_c_define ("LACS",  scm_int2num (LACS));
  scm_c_define ("TACS",  scm_int2num (TACS));
  scm_c_define ("ATN",   scm_int2num (ATN));
  scm_c_define ("CIC",   scm_int2num (CIC));
  scm_c_define ("REM",   scm_int2num (REM));
  scm_c_define ("LOK",   scm_int2num (LOK));
  scm_c_define ("CMPL",  scm_int2num (CMPL));
  scm_c_define ("EVENT", scm_int2num (EVENT));
  scm_c_define ("SPOLL", scm_int2num (SPOLL));
  scm_c_define ("RQS",   scm_int2num (RQS));
  scm_c_define ("SRQI",  scm_int2num (SRQI));
  scm_c_define ("END",   scm_int2num (END));
  scm_c_define ("TIMO",  scm_int2num (TIMO));
  scm_c_define ("ERR",   scm_int2num (ERR));

  scm_c_define ("GTL", scm_int2num (GTL));
  scm_c_define ("SDC", scm_int2num (SDC));
  scm_c_define ("PPC", scm_int2num (PPC));
  scm_c_define ("GET", scm_int2num (GET));
  scm_c_define ("TCT", scm_int2num (TCT));
  scm_c_define ("LLO", scm_int2num (LLO));
  scm_c_define ("DCL", scm_int2num (DCL));
  scm_c_define ("PPU", scm_int2num (PPU));
  scm_c_define ("SPE", scm_int2num (SPE));
  scm_c_define ("SPD", scm_int2num (SPD));
  scm_c_define ("UNL", scm_int2num (UNL));
  scm_c_define ("UNT", scm_int2num (UNT));
  scm_c_define ("PPD", scm_int2num (PPD));

  scm_c_define ("EDVR", scm_int2num (EDVR));
  scm_c_define ("ECIC", scm_int2num (ECIC));
  scm_c_define ("ENOL", scm_int2num (ENOL));
  scm_c_define ("EADR", scm_int2num (EADR));
  scm_c_define ("EARG", scm_int2num (EARG));
  scm_c_define ("ESAC", scm_int2num (ESAC));
  scm_c_define ("EABO", scm_int2num (EABO));
  scm_c_define ("ENEB", scm_int2num (ENEB));
  scm_c_define ("EOIP", scm_int2num (EOIP));
  scm_c_define ("ECAP", scm_int2num (ECAP));
  scm_c_define ("EFSO", scm_int2num (EFSO));
  scm_c_define ("EBUS", scm_int2num (EBUS));
  scm_c_define ("ESTB", scm_int2num (ESTB));
  scm_c_define ("ESRQ", scm_int2num (ESRQ));
  scm_c_define ("ETAB", scm_int2num (ETAB));

  scm_c_define ("REOS", scm_int2num (REOS));
  scm_c_define ("XEOS", scm_int2num (XEOS));
  scm_c_define ("BIN",  scm_int2num (BIN));

  scm_c_define ("TNONE",  scm_int2num (TNONE));
  scm_c_define ("T10us",  scm_int2num (T10us));
  scm_c_define ("T30us",  scm_int2num (T30us));
  scm_c_define ("T100us", scm_int2num (T100us));
  scm_c_define ("T300us", scm_int2num (T300us));
  scm_c_define ("T1ms",   scm_int2num (T1ms));
  scm_c_define ("T3ms",   scm_int2num (T3ms));
  scm_c_define ("T10ms",  scm_int2num (T10ms));
  scm_c_define ("T30ms",  scm_int2num (T30ms));
  scm_c_define ("T100ms", scm_int2num (T100ms));
  scm_c_define ("T300ms", scm_int2num (T300ms));
  scm_c_define ("T1s",    scm_int2num (T1s));
  scm_c_define ("T3s",    scm_int2num (T3s));
  scm_c_define ("T10s",   scm_int2num (T10s));
  scm_c_define ("T30s",   scm_int2num (T30s));
  scm_c_define ("T100s",  scm_int2num (T100s));
  scm_c_define ("T300s",  scm_int2num (T300s));
  scm_c_define ("T1000s", scm_int2num (T1000s));
}

extern void
gpib_init ()
{
  scm_add_feature ("gpib");
  gpib_bindings ();
}
