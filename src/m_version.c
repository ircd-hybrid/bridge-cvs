/************************************************************************
 *   Copyright (C) 2000,2001 Edward Brocklesby
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: m_version.c,v 1.2 2001/05/05 12:53:28 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"

int
m_version(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
  if (parc > 1 && strcasecmp(parv[1], ConfigFileEntry.myname) != 0)
	{
	  struct Client *them;

	  printf("remote version from %s for %s\n", parv[0], parv[1]);
	  /* remote version */
	  if ((them = find_client(parv[1])) == NULL)
		{
		  sendto_one(sptr, ":%s 402 %s %s :No such server", ConfigFileEntry.myname, parv[0], parv[1]);
		  return 0;
		}
	  sendto_one(them, ":%s VERSION %s", parv[0], parv[1]);
	  return 0;
	}

  printf("version for me from %s\n", parv[0]);
  sendto_one(sptr, ":%s 351 %s bridge/alpha(%s). %s :TS3/P8", ConfigFileEntry.myname, parv[0], SERIALNUM, ConfigFileEntry.myname);
  return 0;
}
