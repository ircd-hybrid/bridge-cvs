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
 * $Id: m_server.c,v 1.3 2001/05/05 12:53:27 ejb Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"
#include "protocol.h"

/* argument for this command depend on the server linking to us */
/* P8 sends:
** SERVER name hopcount :Description
** Actually, Unreal seems to send some extra data at the start of
** the description, but we'll ignore that for now
** -- it seems this data is a server numeric; for now we ignore it.
** i'm not sure it's possible to do anything useful with it, when we
** have non-numeric servers on the network.. we'll see.
*/

int
m_server(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	struct nConf *conf;
	dlink_node *node;
	
	if (parc < 4) {
		printf("%% IRC:ERR:Not enough parameters for SERVER from %s\n", sptr->name);
		return 0;
	}
	
	if (find_client(parv[1])) {
	  printf("%% IRC:ERR:Rejected server %s - already exists\n", parv[1]);
	  exit_client(cptr, NULL, "Server already exists");
	  return 0;
	}
	
	if (!IsRegistered(cptr)) /* local server introduction */
	{
		if (!(conf = find_nconf(parv[1]))) {
			exit_client(cptr, NULL, "No N Line");
			return 0;
		}

		Count.servers++;
		cptr->hopcount = atoi(parv[2]);
		strncpy(cptr->name, parv[1], NAMELEN);
		strncpy(cptr->info, parv[3], INFOLEN);
		cptr->localClient->status = STATUS_REGISTERED;
		cptr->localClient->servertype = conf->protocol;
		cptr->type = TYPE_SERVER;
		
		node = make_dlink_node();
		printf("adding %s to server list\n", cptr->name);
		dlinkAdd(cptr, node, &serv_cptr_list);
		node = make_dlink_node();
		dlinkAdd(cptr, node, &cptr_list);
		send_myinfo(cptr);
		printf("%% SRV:INF:Link with server %s established\n", cptr->name);
		send_netburst(cptr);
		sendto_serv_butone(cptr, "SERVER %s %d :%s", cptr->name, cptr->hopcount + 1, cptr->info);
	}
	else
	{
		/* remote server introduction */
		struct Client *newclient;
		
		Count.servers++;
		newclient = user_newslot(0);
		strncpy(newclient->name, parv[1], NAMELEN);
		strncpy(newclient->info, parv[3], INFOLEN);
		newclient->hopcount = atoi(parv[2]);
		newclient->type = TYPE_SERVER;
		newclient->from = sptr;
		newclient->local = cptr;
		node = make_dlink_node();
		printf("adding %s to server list\n", newclient->name);
		dlinkAdd(newclient, node, &serv_cptr_list);
		node = make_dlink_node();
		dlinkAdd(newclient, node, &cptr_list);
		send_out_server(cptr, sptr->name, newclient->name, newclient->hopcount + 1, newclient->info);
	}

	return 0;
}
