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
 * $Id: m_sjoin.c,v 1.3 2001/05/16 02:13:40 ejb Exp $
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"

/* SJOIN format..
** TS3:
**   :prefix SJOIN TS channel modes [key / limit] :nick nick...
** Unreal:
**   SJOIN TS channel [modes [key / limit]] :nick nick...
*/

int
m_sjoin(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  char *chan, *mode = NULL, *p = NULL, *nick;
  long ts;
  int nickparv = 0; /* which parv the nicks are in.. */
  struct Channel *channel;
  struct Client *acptr;
  char nb[BUFSIZE];
  char *nickbuf = nb;
  
  /* these two are always the same */
  ts = atol(parv[1]);
  chan = parv[2];

  if ((channel = find_channel(chan)) == NULL)
	channel = new_channel(chan);
  
  /* XXX */
  channel->ts = ts;
  /* the rest are protocol dependent.. */
  switch(cptr->localClient->servertype)
	{
	case PROTOCOL_UNREAL:
	  /* start with it here.. */
	  nickparv = 3;
	  /* did we get an optional mode? */
	  if (parc > 4) {
		mode = parv[3];
		/* if so, move the nickparv along one too */
		nickparv++;
	  }
	  break;
	case PROTOCOL_TS3:
	  /* TS3 always sends a mode */
	  nickparv = 4;
	  mode = parv[3];
	  break;
	}

  if (mode)
	p = mode;

  if (p)
	while (*p)
	  {
		switch (*p)
		  {
		  case 'i':
			channel->mode |= MODE_INVITE;
			break;
		  case 'n':
			channel->mode |= MODE_N;
			break;
		  case 'p':
			channel->mode |= MODE_PRIVATE;
			break;
		  case 't':
			channel->mode |= MODE_TOPIC;
		  break;
		  case 'm':
			channel->mode |= MODE_M;
			break;
		  case 's':
			channel->mode |= MODE_SECRET;
			break;
			/* these two cause the nicks to move along one.. */
		  case 'k':
			channel->mode |= MODE_KEY;
			strncpy(channel->key, parv[nickparv], KEYLEN - 1);
			nickparv++;
			break;
		  case 'l':
			channel->mode |= MODE_LIMIT;
			channel->limit = atol(parv[nickparv]);
			nickparv++;
			break;
		  case 'f': 
			nickparv++;
		  default:
			/* unknown mode.. ignore */
		  }
		p++;
	  }
  
  /* now, extract the nicks so we can add them to the channel.. */
  strncpy(nickbuf, parv[nickparv], BUFSIZE - 1);
  nickbuf[BUFSIZE-1] = '\0';
 
  while (*nickbuf == ' ')
	nickbuf++;

  for (nick = strtok(nickbuf, " ");
	   nick;
	   nick = strtok(NULL, " "))
	{
	  /* split nickbuf into each nick, and add it */
	  int type = T_PEON;
	  char mode = '\0';

	  switch (nick[0]) /* op/voice/halfop.. */
		{
		case '@':
		  type = T_OP;
		  break;
		case '%':
		  type = T_HOP;
		  break;
		case '+':
		  type = T_VOICE;
		  break;
		}

	  if (type != T_PEON)
		nick++;

	  if ((acptr = find_client(nick)) == NULL)
		sendto_one(cptr, ":%s KILL %s :%s (%s(?) <- %s)",
				   ConfigFileEntry.myname, nick, 
				   ConfigFileEntry.myname, nick, parv[0]);
	  else
		add_user_to_channel(acptr, channel, type);

	  /* we now have to send to 2.8-style servers here.
		 this is really not a nice way to do things; the
		 2.8 protocol sucks. */
	  sendto_cap_serv_butone(NULL, CAP_P28, ":%s JOIN %s",
							 acptr->name, channel->name);
	  switch(type)
		{
		case T_OP:
		  mode = 'o';
		  break;
		case T_VOICE:
		  mode = 'v';
		default:
		  break;
		}
	  if (mode != '\0')
		sendto_cap_serv_butone(NULL, CAP_P28, ":%s MODE %s +%c %s",
							   ConfigFileEntry.myname, channel->name,
							   mode, acptr->name);
	}

  /* finally, send everything to other servers.
	 we do this here rather than in protocol.c because of the
	 strange format of SJOINs :\ */
  {
	dlink_node *node;
	struct Client *aptr;
	char sjbuf[BUFSIZE];
	char *t_sjbuf = sjbuf;
	
	t_sjbuf += sprintf(t_sjbuf, "SJOIN %ld %s +", ts, chan);
	
	if (channel->mode & MODE_INVITE)
	  *t_sjbuf++ = 'i';
	if (channel->mode & MODE_N)
	  *t_sjbuf++ = 'n';
	if (channel->mode & MODE_PRIVATE)
	  *t_sjbuf++ = 'p';
	if (channel->mode & MODE_SECRET)
	  *t_sjbuf++ = 's';
	if (channel->mode & MODE_TOPIC)
	  *t_sjbuf++ = 't';
	if (channel->mode & MODE_M)
	  *t_sjbuf++ = 'm';
	if (channel->mode & MODE_KEY)
	  *t_sjbuf++ = 'k';
	if (channel->mode & MODE_LIMIT)
	  *t_sjbuf++ = 'l';
	
	*t_sjbuf++ = ' ';

	/* key / limit ? add that too */
	if (channel->mode & MODE_KEY)
	  t_sjbuf += sprintf(t_sjbuf, "%s ", channel->key);
	if (channel->mode & MODE_LIMIT)
	  t_sjbuf += sprintf(t_sjbuf, "%ld ", channel->limit); 
	
	*t_sjbuf++ = ':';

	strcpy(t_sjbuf, parv[nickparv]);
	
	for (node = local_cptr_list.head; node; node = node->next)
	  {
		aptr = node->data;
		
		if (aptr == cptr)
		  continue;

		switch (aptr->localClient->servertype)
		  {
		  case PROTOCOL_UNREAL:
			sendto_one(aptr, "%s", sjbuf);
			break;
		  case PROTOCOL_TS3:
			sendto_one(aptr, ":%s %s", sptr, sjbuf);
			break;
		  }
	  }
  }
  return 0;
}
