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
 * $Id: m_nick.c,v 1.3 2001/05/04 23:11:24 ejb Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"

/*
** TS3:
** when its a server introducing a new nick
**      parv[0] = sender prefix
**      parv[1] = nickname
**      parv[2] = hop count
**      parv[3] = TS
**      parv[4] = umode
**      parv[5] = username
**      parv[6] = hostname
**      parv[7] = server
**      parv[8] = ircname
*/

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
			nick = parv[1];
			hops = atoi(parv[2]);
			ts = atol(parv[3]);
			umode = parv[4];
			username = parv[5];
			hostname = parv[6];
			server = parv[7];
			gecos = parv[8];
			break;
		default:
			/* huh?? */
			printf("%% IRC:ERR:Received NICK command from unsupported server type %d\n", cptr->localClient->servertype);
			return 0;
	}

	/*	
	printf("new nick: %s\n", nick);
	*/

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
	dlinkAdd(newclient, node, &cptr_list);
	
	return 0;
}
