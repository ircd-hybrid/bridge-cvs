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
 * $Id: m_trace.c,v 1.1 2001/05/06 10:45:07 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "version.h"

int
m_trace(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
  struct Client *acptr;
  char *next;

  acptr = find_server(parv[1]);
  if (acptr == NULL)
	return 0;

  if (acptr->from == NULL)
	next = acptr->name;
  else
	next = acptr->from->name;

  sendto_one(sptr, ":%s 200 %s Link %s %s :%s",
			 ConfigFileEntry.myname, parv[0], VERSION, parv[1], next);
  sendto_one(acptr, ":%s TRACE %s", parv[0], parv[1]);
  return 0;
}
