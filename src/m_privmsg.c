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
 * $Id: m_privmsg.c,v 1.2 2001/05/06 10:45:06 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"

/* :source PRIVMSG target :msg */

int
m_privmsg(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
  if (parc < 3)
	{
	  sendto_one(sptr, ":%s 461 %s PRIVMSG :Not enough parameters.",
				 ConfigFileEntry.myname, parv[0]);
	  return 0;
	}

  if (strchr("#+&!", parv[1][0]))
	{
	  /* channel message */
	  struct Channel *target;

	  if ((target = find_channel(parv[1])) == NULL)
		{
		  sendto_one(sptr, ":%s 401 %s %s :No such nick/channel.",
					 ConfigFileEntry.myname, parv[0], parv[1]);
		  return 0;
		}
	  printf("message from %s to %s; not sending to %s\n",
			 sptr->name, target->name, cptr->name);
	  sendto_serv_butone(cptr, ":%s PRIVMSG %s :%s",
						 sptr->name, target->name,
						 parv[2]);
	  return 0;
	}
  else
	{
	  /* message to person */
	  struct Client *target;

	  if ((target = find_client(parv[1])) == NULL)
		{
		  sendto_one(sptr, ":%s 401 %s %s :No such nick/channel.",
					 ConfigFileEntry.myname, parv[0], parv[1]);
		  return 0;
		}
	  
	  sendto_one(target, ":%s PRIVMSG %s :%s\n", 
				 parv[0], parv[1], parv[2]);
	}
  return 0;
}
