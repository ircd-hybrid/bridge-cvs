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
 * $Id: m_topic.c,v 1.2 2001/05/16 02:13:40 ejb Exp $
 */

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "version.h"
#include "channel.h"
#include "protocol.h"

int
m_topic(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  struct Channel *chan;
  char *who = NULL, *channel = NULL, *newtopic = NULL;
  long ts = 0;
  struct Client *acptr;

  if (parc < 3)
	return 0;

  switch(cptr->localClient->servertype)
	{
	  /* :server TOPIC #chan nick ts :topic */
	case PROTOCOL_UNREAL:
	  who = parv[0];
	  channel = parv[1];
	  ts = atol(parv[3]);
	  newtopic = parv[4];
	  break;
	case PROTOCOL_TS3:
	  who = parv[0];
	  channel = parv[1];
	  ts = (long)time(NULL);
	  newtopic = parv[2];
	  break;
	default:
	  break;
	}

  if ((chan = find_channel(channel)) == NULL)
	{
	  /* no such channel */
	  sendto_serv_butone(NULL, ":%s WALLOPS :Received TOPIC for non-existant channel %s from %s via %s",
						 ConfigFileEntry.myname, channel, who, cptr->name);
	  return 0;
	}

  if ((acptr = find_client(who)) == NULL)
	{
	  sendto_serv_butone(NULL, ":%s WALLOPS :Received TOPIC from non-existant client %s via %s",
						 ConfigFileEntry.myname, who, cptr->name);
	  return 0;
	}

  strncpy(chan->topic, newtopic, TOPICLEN);
  send_out_topic(cptr, chan, acptr, ts, newtopic);

  return 0;
}
