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
