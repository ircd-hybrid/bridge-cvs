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
 * $Id: m_kill.c,v 1.2 2001/05/07 21:31:59 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"

/* :source KILL who :Path (reason) */

int
m_kill(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
  struct Client *victim;
  char *reason;
  char *path;

  path = parv[2];
  reason = strchr(parv[2], ' ');
  
  if (reason)
	*reason++ = '\0';
  else
	reason = "()";

  if ((victim = find_client(parv[1])) == NULL)
	{
	  /* we could get a KILL for a user we don't know about.. pass
		 it on still, but don't remove the client */
	  /* don't pass it on for now .. i'm not sure what the correct
		 behaviour is here.. we can't send back a KILL, since the client
		 won't exist there. for now, just flag it and return */
	  sendto_serv_butone(NULL, ":%s WALLOPS :Received KILL messages for non-existent user %s from %s via %s (Path: %s)",
						 ConfigFileEntry.myname, parv[1], sptr->name, cptr->name, path);
	  return 0;
	}


  victim->flags |= FLAGS_KILLED;

  /* no point putting the reason here really, noone will see it */
  exit_client(victim, cptr, reason);

  sendto_serv_butone(cptr, ":%s KILL %s :%s!%s %s",
					 sptr->name, victim->name,
					 path, ConfigFileEntry.myname,
					 reason);
  return 0;
}
