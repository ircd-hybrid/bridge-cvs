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
 * $Id: m_nick.c,v 1.8 2001/05/16 02:13:40 ejb Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
m_nick(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
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
	int s28 = 0; /* if we are doing a 2.8-style nick */
#if 0
	struct Client *acptr;
#endif

	if (parc == 2) 
	  {
		/* nick change */
		nick = parv[1];
		if (parc == 3) /* TS sends timestamp here */
		  ts = atol(parv[2]);

		/* note: we have to send out the nick change *before* we actually
		   change their nick, or parv[0] gets clobbered and we lose the
		   original nick */

#if 0
		if (acptr == find_client(nick))
		  {
			if (strcmp(sptr->user->hostname, acptr->user->hostname) == 0 &&
				strcmp(sptr->user->username, acptr->user->username) == 0)
			  {
			/* hm! nick already exists. so we process nick collide */
			if (acptr->user->ts < ts) 
			  {
				/* new nick has higher TS (newer nick).
				   so, what we do here depends. */
#endif
		send_out_nickchange(cptr, parv[0], nick, ts);
		strncpy(sptr->name, nick, sizeof(sptr->name) - 1);
		sptr->user->ts = ts;
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
		gecos = parv[7];
		break;

	  case PROTOCOL_28:
		/* we handle 2.8-style NICKs differently.
		   because 2.8 sends NICK first, then USER after,
		   we don't have all the information we need here to establish a nick. */
		nick = parv[1];
		hops = atoi(parv[2]);
		ts = time(NULL); /* XXX is this right? */
		umode = "+";
		username = "";
		hostname = "";
		server = "";
		gecos = "";
		s28 = 1;
		break;
	  default:
		/* huh?? */
		printf("%% IRC:ERR:Received NICK command from unsupported server type %d\n", cptr->localClient->servertype);
		return 0;
	}

	newclient = user_newslot(0);
	strncpy(newclient->name, nick, NAMELEN);
	newclient->hopcount = hops;
	newclient->type = TYPE_CLIENT;
	newclient->local = cptr;
	newclient->user = malloc(sizeof(struct User));

	if (!s28)
	  {
		/* do we want a NICK with a from of NULL?
		   could cause problems. leave it for now */
		newclient->from = find_client(server);

		strncpy(newclient->info, gecos, INFOLEN);
	
		strncpy(newclient->user->server, server, NAMELEN - 1);
		strncpy(newclient->user->username, username, USERLEN - 1);
		strncpy(newclient->user->hostname, hostname, HOSTLEN - 1);
	  }

	newclient->user->umodes = 0;
	newclient->user->ts = ts;
	node = make_dlink_node();
	dlinkAdd(newclient, node, &cptr_list);
	node = make_dlink_node();
	dlinkAdd(newclient, node, &client_cptr_list);
	/* can't do this yet with 2.8! */
	if (!s28)
	  send_out_nick(cptr, parv[0], nick, hops, ts, 0, username, hostname, server, gecos);
	return 0;
}
