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
 * $Id: protocol.c,v 1.7 2002/03/11 14:51:17 ejb Exp $
 */

#include "clients.h"
#include "config.h"
#include "send.h"
#include "channel.h"
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
		case PROTOCOL_28:
		  sendto_one(acptr, ":%s SERVER %s %d :%s", source, name, hopcount, info);
		  break;
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s SERVER %s %d 0 :%s", source, name, hopcount, info);
		  break;
		default:
		  printf("Unsupported protocol %d for send_out_server\n", acptr->localClient->servertype);
		  break;
		}
	}
}
  
void
send_out_nick(from, prefix, nick, hops, ts, umodes, user, host, server, name)
	 struct Client *from;
	 char *prefix, *nick, *user, *host, *server, *name;
	 int hops, umodes;
	 long ts;
{
  struct Client *acptr;
  dlink_node *node;

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  acptr = node->data;

	  if (acptr == from)
		continue;

	  switch(acptr->localClient->servertype)
		{
		case PROTOCOL_TS3:
		  sendto_one(acptr, "NICK %s %d %ld + %s %s %s :%s",
					 nick, hops, ts, user, host, server, name);
		  break;
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, "NICK %s %d %ld %s %s %s 0 :%s",
                     nick, hops, ts, user, host, server, name);
		  /* Unreal also needs to send the MODEs and SETHOST here.. */
		  break;
		case PROTOCOL_28:
		  sendto_one(acptr, "NICK %s :%d",
					 nick, hops);
		  sendto_one(acptr, ":%s USER %s %s %s :%s",
					 nick, user, host, server, name);
		  break;
		default:
		  printf("Unsupported protocol %d for send_out_nick\n", acptr->localClient->servertype);
		  break;
		}
	}
}
			
void
send_out_nickchange(from, oldnick, newnick, ts)
			 struct Client *from;
			 char *oldnick, *newnick;
			 long ts;
{
  struct Client *acptr;
  dlink_node *node;

  for (node = local_cptr_list.head; node; node = node->next) 
	{
	  acptr = node->data;

	  if (from == acptr)
		continue;

	  switch(acptr->localClient->servertype) 
		{
		case PROTOCOL_TS3:
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s NICK %s %ld", oldnick, newnick, ts);
		  break;
		default:
		  printf("Unsupported protocol %d for send_out_nickchange\n", acptr->localClient->servertype);
		  break;
		}
	}

}
	
void
send_out_join(cptr, user, channel)
	 struct Client *cptr;
	 char *user;
	 struct Channel *channel;
{
  dlink_node *node;
  struct Client *acptr;

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  acptr = node->data;

	  if (acptr == cptr)
		continue;

	  switch(acptr->localClient->servertype)
		{
		case PROTOCOL_TS3:
		  sendto_one(acptr, ":%s SJOIN %ld %s + :%s",
					 ConfigFileEntry.myname,
					 channel->ts, channel->name, user);
		  break;
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s JOIN %s",
					 user, channel->name);
		  break;
		}
	}
}	   

void
send_out_topic(cptr, chan, who, ts, newtopic)
	 struct Client *cptr, *who;
	 struct Channel *chan;
	 long ts;
	 char *newtopic;
{
  dlink_node *node;
  struct Client *acptr;

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  acptr = node->data;

	  if (acptr == cptr)
		continue;

	  switch (acptr->localClient->servertype)
		{
		case PROTOCOL_UNREAL:
		  sendto_one(acptr, ":%s TOPIC %s %s %ld :%s",
					 who->name, chan->name, who->name,
					 ts, newtopic);
		  break;
		case PROTOCOL_TS3:
		  sendto_one(acptr, ":%s TOPIC %s :%s",
					 who->name, chan->name, newtopic);
		  break;
		}
	}
}

void
send_out_umode(from, who, target, which, mode)
	 struct Client *from, *who, *target;
	 char which;
	 int mode;
{
  dlink_node *node;
  struct Client *acptr;

  for (node = local_cptr_list.head; node; node = node->next)
	{
	  acptr = node->data;

	  if (acptr == from)
		continue;

	  switch(mode)
		{
		case UMODE_WALLOPS:
		  sendto_one(acptr, ":%s MODE %s :%cw",
					 who->name, target->name, which);
		  break;
		case UMODE_SNOTICES:
		  sendto_one(acptr, ":%s MODE %s :%cs",
					 who->name, target->name, which);
		  break;
		case UMODE_INVISIBLE:
		  sendto_one(acptr, ":%s MODE %s :%ci",
					 who->name, target->name, which);
		  break;
		case UMODE_OPER:
		  sendto_one(acptr, ":%s MODE %s :%co",
					 who->name, target->name, which);
		  break;
		case UMODE_SERVADM:
		  switch(acptr->localClient->servertype)
			{
			case PROTOCOL_UNREAL:
			  sendto_one(acptr, ":%s MODE %s :%cA",
						 who->name, target->name, which);
			  break;
			case PROTOCOL_TS3:
			  sendto_one(acptr, ":%s MODE %s :%ca",
						 who->name, target->name, which);
			  break;
			}
		default:
		  break;
		}
	}
}
