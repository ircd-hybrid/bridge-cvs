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
 * $Id: m_nick.c,v 1.2 2001/02/11 08:00:20 ejb Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"

int
m_nick(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	char *nick = NULL;
	long ts = 0;
	char *server = NULL;
	char *gecos = NULL;
	char *umode = NULL;
	char *username = NULL;
	char *hostname = NULL;
	int hops = 0;
	struct Client *newclient;
	dlink_node *node;

	switch (cptr->localClient->servertype) {
		case PROTOCOL_TS3:
			/* TS3 nick introduction .. */
			nick = parv[2];
			hops = atol(parv[3]);
			ts = atol(parv[4]);
			umode = parv[5];
			username = parv[6];
			hostname = parv[7];
			server = parv[8];
			gecos = parv[9];
			break;
		default:
			/* huh?? */
			printf("%% IRC:ERR:Received NICK command from unsupported server type %d\n", cptr->localClient->servertype);
			return 0;
	}
	
	nick = parv[0];
	
	printf("new nick: %s\n", nick);
	
	newclient = user_newslot(0);
	strncpy(newclient->name, nick, NAMELEN);
	strncpy(newclient->info, gecos, INFOLEN);
	newclient->hopcount = hops;
	newclient->type = TYPE_CLIENT;
	newclient->from = cptr;
	newclient->user = malloc(sizeof(struct User));
	strncpy(newclient->user->username, username, USERLEN);
	strncpy(newclient->user->hostname, hostname, HOSTLEN);
	newclient->user->umodes = 0;

	node = make_dlink_node();
	dlinkAdd(cptr, node, &cptr_list);
	
	return 0;
}
