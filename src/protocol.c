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
 * $Id: protocol.c,v 1.1 2001/05/05 12:53:28 ejb Exp $
 */

#include "clients.h"
#include "config.h"
#include "send.h"
#include "protocol.h"

void
send_out_server(sptr, source, name, hopcount, info)
	 struct Client *sptr;
	 char *source, *name, *info;
	 int hopcount;
{
  struct Client *acptr;
  dlink_node *node;

  for (node = local_cptr_list.head; node; node = node->next) 
	{
	  acptr = node->data;
	  
	  if (acptr == sptr)
		continue;

	  switch(acptr->localClient->servertype) 
		{
		case PROTOCOL_TS3:
		  sendto_one(acptr, ":%s SERVER %s %d :%s", source, name, hopcount, info);
		  break;
		case PROTOCOL_P8:
		  sendto_one(acptr, ":%s SERVER %s %d 0 :%s", source, name, hopcount, info);
		  break;
		default:
		  printf("Unsupported protocol %d for send_out_server\n", acptr->localClient->servertype);
		  break;
		}
	}
}
  
void
send_out_nickchange(oldnick, newnick, ts)
	 char *oldnick, *newnick;
	 long ts;
{
  struct Client *acptr;
  dlink_node *node;

  for (node = local_cptr_list.head; node; node = node->next) 
	{
	  acptr = node->data;

	  switch(acptr->localClient->servertype) 
		{
		case PROTOCOL_TS3:
		case PROTOCOL_P8:
		  sendto_one(acptr, ":%s NICK %s %ld", oldnick, newnick, ts);
		  break;
		default:
		  printf("Unsupported protocol %d for send_out_nickchange\n", acptr->localClient->servertype);
		  break;
		}
	}

}
	
