#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"

/* argument for this command depend on the server linking to us */
/* P8 sends:
** SERVER name hopcount :Description
** Actually, Unreal seems to send some extra data at the start of
** the description, but we'll ignore that for now
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

	if (!IsRegistered(cptr)) /* local server introduction */
	{
		if (find_client(parv[1])) {
			printf("%% IRC:ERR:Rejected server %s - already exists\n", parv[1]);
			exit_client(cptr, "Server already exists");
			return 0;
		}
		
		if (!(conf = find_nconf(parv[1]))) {
			exit_client(cptr, "No N Line");
			return 0;
		}
		
		cptr->hopcount = atoi(parv[2]);
		strncpy(cptr->name, parv[1], NAMELEN);
		strncpy(cptr->info, parv[3], INFOLEN);
		cptr->localClient->status = STATUS_REGISTERED;
		cptr->localClient->servertype = conf->protocol;
		cptr->type = TYPE_SERVER;
		
		node = make_dlink_node();
		dlinkAdd(cptr, node, &serv_cptr_list);
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
		
		newclient = user_newslot(0);
		strncpy(newclient->name, parv[1], NAMELEN);
		strncpy(newclient->info, parv[3], INFOLEN);
		newclient->hopcount = atoi(parv[2]);
		newclient->type = TYPE_SERVER;
		newclient->from = cptr;
		node = make_dlink_node();
		dlinkAdd(newclient, node, &serv_cptr_list);
		dlinkAdd(newclient, node, &cptr_list);
		sendto_serv_butone(cptr, ":%s SERVER %s %d :%s", sptr->name, newclient->name, newclient->hopcount + 1, newclient->info);
	}

	return 0;
}