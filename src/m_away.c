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
 * $Id: m_away.c,v 1.1 2001/05/07 21:31:58 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"

int
m_away(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  if (IsServer(sptr))
	{
	  sendto_serv_butone(NULL, ":%s WALLOPS :Received AWAY from server %s (via %s)",
						 ConfigFileEntry.myname, sptr->name, cptr->name);
	  return 0;
	}

  if (parc > 1)
	{
	  /* client setting themself away */
	  sptr->flags |= FLAGS_AWAY;
	  strcpy(sptr->user->away, parv[1]);
	  sendto_serv_butone(cptr, ":%s AWAY :%s", 
						 parv[0], parv[1]);
	  return 0;
	}
  else
	{
	  /* client returning from being away */
	  sptr->flags &= ~FLAGS_AWAY;
	  sendto_serv_butone(cptr, ":%s AWAY",
						 parv[0]);
	  return 0;
	}
}
