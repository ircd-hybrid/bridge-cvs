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
 * $Id: clients.c,v 1.3 2001/05/05 12:53:26 ejb Exp $
 */


#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "clients.h"
#include "commands.h"
#include "send.h"
#include "config.h"
#include "main.h"

dlink_list local_cptr_list;
dlink_list serv_cptr_list;
dlink_list cptr_list;

void
new_client(struct sockaddr_in addr, int fd)
{
	struct Client *cptr;
	
	cptr = user_newslot(1);
	cptr->localClient = malloc(sizeof(struct LocalClient));
	memset(cptr->localClient, 0, sizeof(struct LocalClient));
	
	cptr->localClient->fd = fd;
	
	memcpy(&cptr->localClient->ip, &addr.sin_addr, sizeof(cptr->localClient->ip));
	strcpy(cptr->localClient->host, inet_ntoa(cptr->localClient->ip));
	printf("%% USR:INF:Client connecting: %s\n", cptr->localClient->host);
}

struct Client *
user_newslot(int local)
{
	struct Client *cptr;
	dlink_node *node;
	
	cptr = malloc(sizeof(struct Client));

	memset(cptr, 0, sizeof(struct Client));
	
	if (local) {
		node = make_dlink_node();
		dlinkAdd(cptr, node, &local_cptr_list);
	}
	
	return cptr;
}

int 
read_loop(void)
{
	struct Client *cptr;
	struct timeval tv;
	fd_set readfs;
	int last = 0;
	dlink_node *node, *next_ptr;
	
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
        
	FD_ZERO(&readfs);

	for(node = local_cptr_list.head; node; node = next_ptr) {
		cptr = node->data;
		
		if (!cptr->localClient) {
			printf("%% PANIC: found non-local client on local_cptr_list!");
			exit(1);
		}

		if(cptr->localClient->status != STATUS_DELETED)
			FD_SET(cptr->localClient->fd, &readfs);
		if(cptr->localClient->fd > last)
			last = cptr->localClient->fd;
		next_ptr = node->next;
	}
	
	switch(select(last + 1, &readfs, NULL, NULL, &tv)) {
        case -1: /* An error in select- Bad Thing. */
			fprintf(stderr, "select failed: %s\n", strerror(errno));
			return -1;
        case 0: /* Nobody is saying anything.  Why listen? -- this is normal. */
			return 0;
        default:
			break;
	}
                
	cptr = local_cptr_list.head->data;

	for(;;) {
		/* go thru list of clients and
		 * handle their commands if they have 
		 * something to do.
		 */
		if(FD_ISSET(cptr->localClient->fd, &readfs) != 0) {
			handle_data(cptr);
		}
		
		/* make sure we still have some clients .. */
		if(cptr && cptr->next)
			cptr = cptr->next;
		else
			break;
	}
	                
	/* If no users or none deleted, don't worry and return */
	if(!local_cptr_list.head) 
		return -1;
	                
	/* Remove users who have left from the linked list */ 
	cptr = local_cptr_list.head->data;
	
	for(;;) {
		if(!cptr)                     /* the end of the list */
			break;
		
		if(cptr->localClient->status == STATUS_DELETED) {
			/* deleted client - remove */
			exit_client(cptr, NULL, "Connection Closed");
			cptr = local_cptr_list.head->data;
			/* reset cptr to top of list */
		}
		else                          /* move on to the next client */
			cptr = cptr->next;
	}
	
	return 0;
}

void 
exit_client(struct Client *cptr, struct Client *from, char *reason)
{
	dlink_node *dm;
	
	if (cptr->type == TYPE_SERVER)
	  {
		/* exit server; send SQUIT */
		if (IsLocal(cptr))
		  {
			/* local client.. */
			dm = dlinkFind(&local_cptr_list, cptr);
			dlinkDelete(dm, &local_cptr_list);
			free(dm);
		  }
		
		dm = dlinkFind(&serv_cptr_list, cptr);
		dlinkDelete(dm, &local_cptr_list);
		free(dm);
		dm = dlinkFind(&cptr_list, cptr);
		dlinkDelete(dm, &cptr_list);
		free(dm);

		/* send an SQUIT for it */
		sendto_serv_butone(cptr, ":%s SQUIT", cptr->name);
	  }
	else
	  {
		/* it's a client; remote it from the list and send a QUIT */
		sendto_serv_butone(from, ":%s QUIT :%s", cptr->name, reason);
		dm = dlinkFind(&cptr_list, cptr);
		dlinkDelete(dm, &cptr_list);
		free(dm);
	  }
	
	if (IsLocal(cptr))
	  {
		if (cptr->localClient->fd > 0 )
		  sendto_one(cptr, "ERROR :Closing Link %s[%s]: %s", IsRegistered(cptr) ? cptr->name : "unknown", reason);
		/* only show exit message for registered clients */
		if (IsRegistered(cptr)) 
		  printf("%% USR:INF:Connection to %s[%s] closed\n", cptr->name, cptr->localClient->host);
	  }
	free(cptr);
}

struct Client *
find_client(char *name)
{
	struct Client *cptr;
	dlink_node *node;
	
	for (node = cptr_list.head; node; node = node->next) 
	{
		cptr = node->data;
		
		if (!strcmp(cptr->name, name))
			return cptr;
	}
	
	return NULL;
}

struct Client *
find_server(char *name)
{
	struct Client *cptr;
	dlink_node *node;
	
	for (node = serv_cptr_list.head; node; node = node->next) {
		cptr = node->data;
	
		if (!strcmp(cptr->name, name))
			return cptr;
	}
	
	return NULL;
}

void
send_myinfo(struct Client *cptr)
{
	struct nConf *conf;
	
	conf = find_nconf(cptr->name);
	
	switch (cptr->localClient->servertype) {
		case PROTOCOL_P8:
			sendto_one(cptr, "PASS :%s", conf->pass);
			sendto_one(cptr, "SERVER %s %d :%s", ConfigFileEntry.myname, 1, ConfigFileEntry.myinfo);
			sendto_one(cptr, "PROTOCTL SJOIN");
			break;
		case PROTOCOL_TS3:
			sendto_one(cptr, "PASS %s :TS", conf->pass);
			sendto_one(cptr, "SERVER %s %d :%s", ConfigFileEntry.myname, 1, ConfigFileEntry.myinfo);
			sendto_one(cptr, "SVINFO 3 3 0 :%d", CurrentTime);
			break;
		default:
			printf("%% SRV:ERR:send_myinfo called for unknown server type!\n");
			break;
	}
}

void
send_netburst(struct Client *cptr)
{
	struct Client *acptr;
	dlink_node *node;
	int hops = 0;
	int doneservs = 0;
	int did_this_round = 1;

	/* send servers first */

	/* send everything for <x> hops.. */
	for (hops = 1; did_this_round && doneservs < Count.servers; hops++)
	  {
		did_this_round = 0;
		printf("sending for hops = %d\n", hops);
		for (node = serv_cptr_list.head; node; node = node->next) 
		  {
			acptr = node->data;
			
			if (acptr == cptr || acptr->from == cptr)
			  continue;
			
			if (acptr->hopcount == hops)
			  {
				did_this_round++;
				switch (cptr->localClient->servertype) 
				  {
				  case PROTOCOL_P8:
					/* XXX send '0', we don't support server numerics right now */
					sendto_one(cptr, ":%s SERVER %s %d 0 :%s", acptr->from ? acptr->from->name : ConfigFileEntry.myname, acptr->name, acptr->hopcount, acptr->info);
					break;
				  case PROTOCOL_TS3:
					sendto_one(cptr, ":%s SERVER %s %d :%s", acptr->from ? acptr->from->name : ConfigFileEntry.myname, acptr->name, acptr->hopcount, acptr->info);
					break;
				  default:
					printf("unknown server type %d while bursting\n", cptr->localClient->servertype);
					break;
				  }
				doneservs++;
			  }
		  }
	  }
}

				
