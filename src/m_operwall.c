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
 * $Id: m_operwall.c,v 1.1 2001/05/07 16:36:52 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"

int
m_operwall(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  char *source = sptr->name;
  char *text = parv[1];
  dlink_node *node;
  struct Client *acptr;

  /* this is a little easier than globops.. */

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  acptr = node->data;

	  if (acptr == cptr)
		continue;

	  switch(acptr->localClient->servertype)
		{
		case PROTOCOL_TS3:
		  sendto_one(acptr, ":%s OPERWALL :%s",
					 source, text);
		  break;
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s GLOBOPS :%s",
					 source, text);
		  break;
		}
	}
  return 0;
}
