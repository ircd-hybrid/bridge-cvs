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
 * $Id: m_nick.c,v 1.6 2001/05/06 18:05:52 ejb Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"
#include "protocol.h"

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
** or for a nick change:
**      parv[0] = sender prefix
**      parv[1] = new nick
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

	if (parc < 4) 
	  {
		/* nick change */
		nick = parv[1];
		if (parc == 3) /* TS sends timestamp here */
		  ts = atol(parv[2]);

		if ((newclient = find_client(parv[0])) == NULL)
		  {
			/* no old nick? */
			sendto_serv_butone(NULL, ":%s WALLOPS :NICK from unknown user %s(?)", ConfigFileEntry.myname, parv[0]);
			return 0;
		  }

		/* note: we have to send out the nick change *before* we actually
		   change their nick, or parv[0] gets clobbered and we lose the
		   original nick */

		send_out_nickchange(cptr, parv[0], nick, ts);
		strncpy(newclient->name, nick, sizeof(newclient->name) - 1);
		newclient->user->ts = ts;
		return 0;
	  }

	/* else its a new nick being introduced */
	switch (cptr->localClient->servertype) 
	  {
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
	  case PROTOCOL_UNREAL:
		/* Unreal nick */
		nick = parv[1];
		hops = atoi(parv[2]);
		ts = atol(parv[3]);
		umode = "+";
		username = parv[4];
		hostname = parv[5];
		server = parv[6];
		gecos = parv[8];
		break;
	  default:
		/* huh?? */
		printf("%% IRC:ERR:Received NICK command from unsupported server type %d\n", cptr->localClient->servertype);
		return 0;
	}

	newclient = user_newslot(0);
	strncpy(newclient->name, nick, NAMELEN);
	strncpy(newclient->info, gecos, INFOLEN);
	newclient->hopcount = hops;
	newclient->type = TYPE_CLIENT;
	newclient->from = find_client(server);
	newclient->local = cptr;
	newclient->user = malloc(sizeof(struct User));
	strncpy(newclient->user->server, server, NAMELEN - 1);
	strncpy(newclient->user->username, username, USERLEN - 1);
	strncpy(newclient->user->hostname, hostname, HOSTLEN - 1);
	newclient->user->umodes = 0;
	newclient->user->ts = ts;
	node = make_dlink_node();
	dlinkAdd(newclient, node, &cptr_list);
	node = make_dlink_node();
	dlinkAdd(newclient, node, &client_cptr_list);
	send_out_nick(cptr, parv[0], nick, hops, ts, 0, username, hostname, server, gecos);
	return 0;
}
