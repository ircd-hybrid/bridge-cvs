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
 * $Id: clients.c,v 1.10 2002/03/11 14:53:16 ejb Exp $
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
#include <time.h>

#include "clients.h"
#include "commands.h"
#include "send.h"
#include "config.h"
#include "main.h"
#include "channel.h"

static void send_chan_list(struct Client *, struct Channel *, dlink_list *, char *, int);

dlink_list local_cptr_list;
dlink_list serv_cptr_list;
dlink_list cptr_list;
dlink_list client_cptr_list;

void
new_client(addr, fd)
	 struct sockaddr_in addr;
	 int fd;
{
	struct Client *cptr;
	
	cptr = user_newslot(1);
	cptr->localClient = malloc(sizeof(struct LocalClient));
	memset(cptr->localClient, 0, sizeof(struct LocalClient));
	
	cptr->localClient->fd = fd;
	cptr->localClient->file = fdopen(fd, "r+");
	
	memcpy(&cptr->localClient->ip, &addr.sin_addr, sizeof(cptr->localClient->ip));
	strcpy(cptr->localClient->host, inet_ntoa(cptr->localClient->ip));
	printf("%% USR:INF:Client connecting: %s\n", cptr->localClient->host);
}

struct Client *
user_newslot(local)
	 int local;
{
	struct Client *cptr;
	dlink_node *node;
	
	cptr = malloc(sizeof(struct Client));

	memset(cptr, 0, sizeof(struct Client));
	
	if (local) 
	  {
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

	for(node = local_cptr_list.head; node; node = next_ptr) 
	  {
		cptr = node->data;
		
		if (!cptr->localClient) 
		  {
			printf("%% PANIC: found non-local client on local_cptr_list!");
			exit(1);
		  }
		
		if(cptr->localClient->status != STATUS_DELETED)
		  FD_SET(cptr->localClient->fd, &readfs);
		if(cptr->localClient->fd > last)
		  last = cptr->localClient->fd;
		next_ptr = node->next;
	  }
	
	switch(select(last + 1, &readfs, NULL, NULL, &tv)) 
	  {
	  case -1: /* An error in select- Bad Thing. */
		fprintf(stderr, "select failed: %s\n", strerror(errno));
		return -1;
	  case 0: /* Nobody is saying anything.  Why listen? -- this is normal. */
		return 0;
	  default:
		break;
	  }
                
	for(node = local_cptr_list.head; node; node = node->next) 
	  {
		int fd;
		cptr = node->data;

		fd = cptr->localClient->fd;

		/* go thru list of clients and
		 * handle their commands if they have 
		 * something to do.
		 */
		if(FD_ISSET(fd, &readfs) != 0) 
		  {
			handle_data(cptr);
			FD_CLR(fd, &readfs);
			node = local_cptr_list.head;
		  }
		if (node == NULL)
		  break;
	  }
	
	/* If no users or none deleted, don't worry and return */
	if(!local_cptr_list.head) 
	  return -1;
	                
	/* Remove users who have left from the linked list */ 
	cptr = local_cptr_list.head->data;
	
	for(;;) 
	  {
		if(!cptr)                     /* the end of the list */
		  break;
		
		if(cptr->localClient->status == STATUS_DELETED) 
		  {
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
exit_client(cptr, from, reason)
	 struct Client *cptr, *from;
	 char *reason;
{
	dlink_node *dm;
	
	if (cptr->user == NULL)
	  {
		/* exit server; send SQUIT */
		if (IsLocal(cptr))
		  {
			/* local client.. */
			dm = dlinkFind(&local_cptr_list, cptr);
			dlinkDelete(dm, &local_cptr_list);
			free(dm);
		  }
		
		if ((dm = dlinkFind(&serv_cptr_list, cptr)) != NULL)
		  {
			dlinkDelete(dm, &serv_cptr_list);
			free(dm);
		  }

		if ((dm = dlinkFind(&cptr_list, cptr)) != NULL)
		  {
			dlinkDelete(dm, &cptr_list);
			free(dm);
		  }

		/* send an SQUIT for it */
		sendto_serv_butone(from, "SQUIT %s", cptr->name);
	  }
	else
	  {
		/* it's a client; remote it from the list and send a QUIT */
		/* -- but not if it's killed */
		if (!(cptr->flags & FLAGS_KILLED))
			sendto_serv_butone(from, ":%s QUIT :%s", cptr->name, reason);

		if ((dm = dlinkFind(&cptr_list, cptr)) != NULL)
		  {
			dlinkDelete(dm, &cptr_list);
			free(dm);
		  }

		if ((dm = dlinkFind(&client_cptr_list, cptr)) != NULL)
		  {
			dlinkDelete(dm, &client_cptr_list);
			free(dm);
		  }
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
find_client(name)
	 char *name;
{
	struct Client *cptr;
	dlink_node *node;
	
	for (node = cptr_list.head; node; node = node->next) 
	  {
		cptr = node->data;
		
		if (strcasecmp(cptr->name, name) == 0)
		  return cptr;
	  }
	
	return find_server(name);
}

struct Client *
find_server(name)
	 char *name;
{
	struct Client *cptr;
	dlink_node *node;
	
	for (node = serv_cptr_list.head; node; node = node->next) 
	  {
		cptr = node->data;
	
		if (strcasecmp(cptr->name, name) == 0)
			return cptr;
	  }
	
	return NULL;
}

void
send_myinfo(cptr)
	 struct Client *cptr;
{
	struct nConf *conf;
	
	conf = find_nconf(cptr->name);
	
	switch (cptr->localClient->servertype) 
	  {
	  case PROTOCOL_UNREAL:
		sendto_one(cptr, "PASS :%s", conf->pass);
		sendto_one(cptr, "PROTOCTL SJOIN SJOIN2 SJ3");
		sendto_one(cptr, "SERVER %s %d :%s", ConfigFileEntry.myname, 1, ConfigFileEntry.myinfo);
		break;
	  case PROTOCOL_TS3:
		sendto_one(cptr, "PASS %s :TS", conf->pass);
		sendto_one(cptr, "CAPAB HOPS");
		sendto_one(cptr, "SERVER %s %d :%s", ConfigFileEntry.myname, 1, ConfigFileEntry.myinfo);
		sendto_one(cptr, "SVINFO 3 3 0 :%d", CurrentTime);
		break;
	  case PROTOCOL_28:
		sendto_one(cptr, "PASS %s", conf->pass);
		sendto_one(cptr, "SERVER %s %d :%s", ConfigFileEntry.myname, 1, ConfigFileEntry.myinfo);
		break;
	  default:
		printf("%% SRV:ERR:send_myinfo called for unknown server type!\n");
		break;
	  }
}

/* have to say who we're sending to because of different servers
   using different modes for different things */
char *
user_mode_to_string(to, cptr)
	 struct Client *cptr, *to;
{
  static char modes[64]; /* hi sts :> */
  char *p = modes;

  if (cptr->umodes & UMODE_OPER)
	*p++ = 'o';
  if (cptr->umodes & UMODE_INVISIBLE)
	*p++ = 'i';
  if (cptr->umodes & UMODE_SERVADM)
	switch(to->localClient->servertype)
	  {
	  case PROTOCOL_TS3:
		*p++ = 'a';
		break;
	  case PROTOCOL_UNREAL:
		*p++ = 'A';
		break;
	  }
  if (cptr->umodes & UMODE_SVSADM)
	switch(to->localClient->servertype)
	  {
	  case PROTOCOL_TS3:
		/* no way to express this using TS3 */
		break;
	  case PROTOCOL_UNREAL:
		*p++ = 'a';
		break;
	  }

  return modes;
}
  
void
send_netburst(cptr)
	 struct Client *cptr;
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
				  case PROTOCOL_UNREAL:
					/* XXX send '0', we don't support server numerics right now */
					sendto_one(cptr, ":%s SERVER %s %d 0 :%s", 
							   acptr->from ? acptr->from->name : ConfigFileEntry.myname, 
							   acptr->name, acptr->hopcount, acptr->info);
					break;
				  case PROTOCOL_TS3:
				  case PROTOCOL_28:
					sendto_one(cptr, ":%s SERVER %s %d :%s", 
							   acptr->from ? acptr->from->name : ConfigFileEntry.myname, 
							   acptr->name, acptr->hopcount, acptr->info);
					break;
				  default:
					printf("unknown server type %d while bursting\n", cptr->localClient->servertype);
					break;
				  }
				doneservs++;
			  }
		  }
	  }

	/* now we send users */
	for (node = client_cptr_list.head; node; node = node->next)
	  {
		acptr = node->data;

		switch (cptr->localClient->servertype)
		  {
		  case PROTOCOL_UNREAL:
			sendto_one(cptr, "NICK %s %d %ld %s %s %s 0 :%s",
                       acptr->name, acptr->hopcount + 1, acptr->user->ts, acptr->user->username,
					   acptr->user->hostname, acptr->from->name, acptr->info);
			sendto_one(cptr, ":%s MODE %s :+%s",
					   acptr->name, acptr->name, user_mode_to_string(cptr, acptr));
			break;
		  case PROTOCOL_TS3:
			sendto_one(cptr, "NICK %s %d %ld +%s %s %s %s :%s",
					   acptr->name, acptr->hopcount + 1, acptr->user->ts, user_mode_to_string(cptr, acptr),
					   acptr->user->username, acptr->user->hostname, acptr->from->name, acptr->info);
			break;
		  case PROTOCOL_28:
			/* stupid. */
			sendto_one(cptr, "NICK %s :%d",
					   acptr->name, acptr->hopcount + 1);
			sendto_one(cptr, ":%s USER %s %s %s :%s",
					   acptr->name, acptr->user->username, acptr->user->hostname,
					   acptr->from->name, acptr->info);
			sendto_one(cptr, ":%s MODE %s :+%s",
					   acptr->name, acptr->name, user_mode_to_string(cptr, acptr));
			break;
		  default:
			printf("Unsupported protocol for NICK in send_netburst\n");
			break;
		  }

		/* send AWAY too.. i don't think sending to all servers will cause a problem */
		if (acptr->flags & FLAGS_AWAY)
		  sendto_one(cptr, ":%s AWAY :%s",
					 acptr->name, acptr->user->away);
	  }

	/* now we send channels */
	{
	  dlink_node *node;
	  struct Channel *chan;

	  /* loop through each channel */
	  for (node = channels.head; node; node = node->next)
		{
		  chan = node->data;
		  send_chan_list(cptr, chan, &chan->ops, "@", MODE_OP);
		  send_chan_list(cptr, chan, &chan->halfops, "%", MODE_HALFOP);
		  send_chan_list(cptr, chan, &chan->voices, "+", MODE_VOICE);
		  send_chan_list(cptr, chan, &chan->peons, "", MODE_PEON);
		}
	}

	/* Unreal needs this */
	sendto_one(cptr, "NETINFO 0 %ld 2302 0 0 0 0 :Hybridnet", time(NULL));
}

void
send_chan_list_28(cptr, chan, list, type)
	 struct Client *cptr;
	 struct Channel *chan;
	 dlink_list *list;
	 int type;
{
  dlink_node *node;
  struct Client *acptr;
  char mode = '\0';

  for (node = list->head; node; node = node->next)
	{
	  acptr = node->data;

	  sendto_one(cptr, ":%s JOIN %s",
				 acptr->name, chan->name);
	  
	  switch(type)
		{
		case MODE_OP:
		  mode = 'o';
		  break;
		case MODE_VOICE:
		  mode = 'v';
		  break;
		case MODE_HALFOP:
		  mode = 'h';
		  break;
		}

	  if (mode != '\0')
		sendto_one(cptr, ":%s MODE %s +%c %s",
				   ConfigFileEntry.myname, chan->name,
				   mode, acptr->name);
	}
}

void
send_chan_list(cptr, chan, list, symbol, type)
	 struct Client *cptr;
	 struct Channel *chan;
	 dlink_list *list;
	 char *symbol;
	 int type;
{
  dlink_node *node;
  char nickbuf[BUFSIZE];
  char sjbuf[BUFSIZE];
  char modebuf[BUFSIZE];
  char *mb = modebuf;
  char *n = nickbuf;
  struct Client *client;

  switch(cptr->localClient->servertype)
	{
	case PROTOCOL_28:
	  /* special processing for this, it doesn't use SJOIN. */
	  send_chan_list_28(cptr, chan, list, type);
	  return;
	default:
	  break;
	}

  *mb++ = '+';

  if (chan->mode & MODE_INVITE)
	*mb++ = 'i';
  if (chan->mode & MODE_N)
	*mb++ = 'n';
  if (chan->mode & MODE_PRIVATE)
	*mb++ = 'p';
  if (chan->mode & MODE_SECRET)
	*mb++ = 's';
  if (chan->mode & MODE_TOPIC)
	*mb++ = 't';
  if (chan->mode & MODE_M)
	*mb++ = 'm';
  if (chan->mode & MODE_KEY)
	*mb++ = 'k';
  if (chan->mode & MODE_LIMIT)
	*mb++ = 'l';
	  
  *mb++ = ' ';
  
  /* key / limit ? add that too */
  if (chan->mode & MODE_KEY)
	mb += sprintf(mb, "%s ", chan->key);
  if (chan->mode & MODE_LIMIT)
	mb += sprintf(mb, "%ld ", chan->limit); 
  
  *mb = '\0';
  
  /* ok, we can use the modebuf for every SJOIN we
	 send. now, we have to build each SJOIN from
	 clients in the channel, being careful not to overflow
	 the buffer. */
  for (node = list->head; node; node = node->next)
	{
	  client = node->data;

	  if (20 + strlen(chan->name) + strlen(modebuf) + strlen(nickbuf) + 2 >= BUFSIZE)
		{
		  /* restart */
		  sprintf(sjbuf, "SJOIN %ld %s %s :%s", chan->ts, chan->name, modebuf, nickbuf);
		  sendto_one(cptr, "%s", sjbuf);
		  n = nickbuf;
		}

	  n += sprintf(n, " %s%s", symbol, client->name);
	}
  if (n != nickbuf)
	{
	  sprintf(sjbuf, "SJOIN %ld %s %s :%s", chan->ts, chan->name, modebuf, nickbuf);
	  sendto_one(cptr, "%s", sjbuf);
	}
}
					 
	  
				
