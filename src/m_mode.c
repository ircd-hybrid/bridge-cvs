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
 * $Id: m_mode.c,v 1.1 2001/05/07 21:31:59 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"
#include "protocol.h"

#define MODE_ADD 1
#define MODE_DEL 2

#define WHICH(what) ((what) == MODE_ADD ? '+' : '-')

int
m_mode(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  /* MODE is always going to be the hardest thing
	 to pass between servers. */

  if (strchr("#+&!", parv[1][0]))
	{
	  /* channel mode.  for now, do nothing. */
	  /* *sigh*.. i wish we could. */
	  struct Channel *channel;
	  char *p = parv[2];
	  int curparv = 3; /* where the next mode argument can be found */
	  int what = MODE_ADD;
	  char mbuf[BUFSIZE], *mbuf_p = mbuf;
	  char pbuf[BUFSIZE], *pbuf_p = pbuf;
	  char mbuf_hop[BUFSIZE], *mbuf_hop_p = mbuf_hop;
	  char pbuf_hop[BUFSIZE], *pbuf_hop_p = pbuf_hop;

	  struct Client *acptr;

	  *pbuf_p = *mbuf_p = *mbuf_hop_p = *pbuf_hop_p = '\0';

	  if ((channel = find_channel(parv[1])) == NULL)
		{
		  sendto_one(sptr, ":%s 403 %s %s :No such channel.",
					 ConfigFileEntry.myname, sptr->name, parv[1]);
		  return 0;
		}

	  while (*p)
		{
		  switch (*p)
			{
			case '+':
			  what = MODE_ADD;
			  break;

			case '-':
			  what = MODE_DEL;
			  break;

			case 't':
			  if (what == MODE_ADD)
				channel->mode |= MODE_TOPIC;
			  else
				channel->mode &= ~MODE_TOPIC;
			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 't';
			  break;

			case 'i':
			  if (what == MODE_ADD)
				channel->mode |= MODE_INVITE;
			  else
				channel->mode &= ~MODE_INVITE;
			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 'i';
			  break;

			case 'p':
			  if (what == MODE_ADD)
				channel->mode |= MODE_PRIVATE;
			  else
				channel->mode &= ~MODE_PRIVATE;
			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 'p';
			  break;

			case 's':
			  if (what == MODE_ADD)
				channel->mode |= MODE_SECRET;
			  else
				channel->mode &= ~MODE_SECRET;
			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 's';
			  break;

			case 'm':
			  if (what == MODE_ADD)
				channel->mode |= MODE_M;
			  else
				channel->mode &= ~MODE_M;
			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 'm';
			  break;

			case 'o':
			  curparv++;
			  if ((acptr = find_client(parv[curparv - 1])) == NULL)
				continue;
			  
			  remove_user_from_channel(acptr, channel);
			  if (what == MODE_ADD)
				add_user_to_channel(acptr, channel, T_OP);
			  else
				add_user_to_channel(acptr, channel, T_PEON);

			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 'o';
			  strcpy(pbuf_p, acptr->name);
			  pbuf_p += strlen(acptr->name);
			  *pbuf_p++ = ' ';
			  break;

			case 'h':
			  curparv++;
			  if ((acptr = find_client(parv[curparv - 1])) == NULL)
				continue;

			  remove_user_from_channel(acptr, channel);
			  if (what == MODE_ADD)
				add_user_to_channel(acptr, channel, T_HOP);
			  else
				add_user_to_channel(acptr, channel, T_PEON);

			  *mbuf_hop_p++ = WHICH(what);
			  *mbuf_hop_p++ = 'h';
			  strcpy(pbuf_hop_p, acptr->name);
			  pbuf_hop_p += strlen(acptr->name);
			  *pbuf_hop_p++ = ' ';
			  break;

			case 'v':
			  curparv++;
			  if ((acptr = find_client(parv[curparv - 1])) == NULL)
				continue;

			  remove_user_from_channel(acptr, channel);
			  if (what == MODE_ADD)
				add_user_to_channel(acptr, channel, T_VOICE);
			  else
				add_user_to_channel(acptr, channel, T_PEON);

			  *mbuf_p++ = WHICH(what);
			  *mbuf_p++ = 'v';
			  strcpy(pbuf_p, acptr->name);
			  pbuf_p += strlen(acptr->name);
			  *pbuf_p++ = ' ';
			  break;

			default:
			  break;
			}
		  p++;
		}

	  *mbuf_p = *pbuf_p = *mbuf_hop_p = *pbuf_hop_p = '\0';

	  if (mbuf[0])
		sendto_serv_butone(cptr, ":%s MODE %s %s %s",
						   parv[0], parv[1], mbuf, pbuf);
	  if (mbuf_hop[0])
		sendto_cap_serv_butone(cptr, CAP_HALFOPS, ":%s MODE %s %s %s",
						   parv[0], parv[1], mbuf_hop, pbuf_hop);
	  return 0;
	}
  else
	{
	  /* user mode. */
	  struct Client *target;
	  char *p = parv[2];
	  int what = MODE_ADD;

	  if ((target = find_client(parv[1])) == NULL)
		{
		  /* setting a mode for a non-existant client */
		  sendto_serv_butone(NULL, ":%s WALLOPS :Received MODE for non-existent client %s from %s via %s",
							 ConfigFileEntry.myname, parv[1], parv[0], cptr->name);
		  return 0;
		}

	  if (IsServer(target))
		{
		  sendto_serv_butone(NULL, ":%s WALLOPS :Received MODE for server %s from %s via %s",
							 ConfigFileEntry.myname, parv[1], parv[0], cptr->name);
		  return 0;
		}

	  while (*p)
		{
		  /* go through each character.. */
		  switch (*p)
			{
			case '+':
			  what = MODE_ADD;
			  break;

			case '-':
			  what = MODE_DEL;
			  break;

			case 'i': /* invisible; generic */
			  if (what == MODE_ADD)
				target->umodes |= UMODE_INVISIBLE;
			  else
				target->umodes &= ~UMODE_INVISIBLE;
			  send_out_umode(cptr, sptr, sptr, WHICH(what), *p);
			  break;

			case 'o': /* IRC operator; generic */
			  if (what == MODE_ADD)
				target->umodes |= UMODE_OPER;
			  else
				target->umodes &= ~UMODE_OPER;
			  printf("user %s set mode %c%c\n", sptr->name, WHICH(what), *p);
			  send_out_umode(cptr, sptr, sptr, WHICH(what), *p);
			  break;

			default:
			  /* lots more to do... ignore for now */
			  break;
			}
		  p++;
		}
	}
  return 0;
}
