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
 * $Id: channels.c,v 1.3 2001/05/16 02:13:38 ejb Exp $
 */

#include <string.h>
#include <stdlib.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"

dlink_list channels;

struct Channel *
find_channel(name)
	 char *name;
{
  dlink_node *node;
  struct Channel *chan;

  for (node = channels.head; node; node = node->next)
	{
	  chan = node->data;
	  if (strcasecmp(chan->name, name) == 0)
		return chan;
	}
  return NULL;
}

struct Channel *
new_channel(name)
	 char *name;
{
  struct Channel *nc;
  dlink_node *node;

  nc = malloc(sizeof(struct Channel));
  memset(nc, 0, sizeof(struct Channel));
  strncpy(nc->name, name, CHANLEN - 1);

  node = make_dlink_node();
  dlinkAdd(nc, node, &channels);
  return nc;
}

void
add_user_to_channel(user, channel, type)
	 struct Client *user;
	 struct Channel *channel;
	 int type;
{
  dlink_node *node;
  dlink_list *list = NULL;

  node = make_dlink_node();
  switch(type)
	{
	case T_PEON:
	  list = &channel->peons;
	  break;
	case T_OP:
	  list = &channel->ops;
	  break;
	case T_VOICE:
	  list = &channel->voices;
	  break;
	case T_HOP:
	  list = &channel->halfops;
	  break;
	}

  dlinkAdd(user, node, list);
  node = make_dlink_node();
  dlinkAdd(channel, node, &user->user->channels);
}

void
remove_user_from_channel(user, channel)
	 struct Client *user;
	 struct Channel *channel;
{
  dlink_node *node;
  dlink_list *list;

  if ((node = dlinkFind(&channel->ops, user)) == NULL)
	if ((node = dlinkFind(&channel->halfops, user)) == NULL)
	  if ((node = dlinkFind(&channel->voices, user)) == NULL)
		if ((node = dlinkFind(&channel->peons, user)) == NULL)
		  return;
		else
		  list = &channel->peons;
	  else
		list = &channel->voices;
	else
	  list = &channel->halfops;
  else
	list = &channel->ops;
		  
  dlinkDelete(node, list);

  /* well, it should never be NULL, but we can't be too careful */
  if ((node = dlinkFind(&user->user->channels, channel)))
	dlinkDelete(node, &user->user->channels);
}

void
remove_from_all_channels(user)
	 struct Client *user;
{
  dlink_node *node;
  struct Channel *chan;

  for (node = user->user->channels.head; node; node = node->next)
	{
	  chan = node->data;

	  remove_user_from_channel(user, chan);
	}
}
