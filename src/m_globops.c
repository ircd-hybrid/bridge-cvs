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
 * $Id: m_globops.c,v 1.1 2001/05/07 16:36:51 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"

int
m_globops(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  char *source = sptr->name;
  char *text = parv[1];
  dlink_node *node;
  struct Client *acptr;

  /* we have to be quite special here.
	 rules for sending globops:

	 from SERVER to TS3: send as WALLOPS
	 from CLIENT to TS3: send as OPERWALL
	 from anything to Unreal: send as GLOBOPS
	 from anything to anything: send as WALLOPS
  */

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  char *type;

	  acptr = node->data;

	  if (acptr == cptr)
		continue;

	  switch(acptr->localClient->servertype)
		{
		case PROTOCOL_TS3:
		  if (sptr->user == NULL)
			type = "WALLOPS"; 
		  else
			type = "OPERWALL";

		  sendto_one(acptr, ":%s %s :%s",
					 source, type, text);
		  break;
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s GLOBOPS :%s",
					 source, text);
		  break;
		}
	}
  return 0;
}
