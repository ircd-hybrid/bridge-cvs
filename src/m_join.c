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
 * $Id: m_join.c,v 1.4 2001/05/16 02:13:39 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"
#include "protocol.h"

int
m_join(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
  /* char *user = parv[0]; */
  char *channels = parv[1];
  struct Channel *chan;
  char *channel;

  /* well, guess what..
	 irc 2.8 sends out :user JOIN #channel1,#channel2
	 for netjoins. *sigh* */

  for (channel = strtok(channels, ",");
	   channel;
	   channel = strtok(NULL, ","))
	{
	  if ((chan = find_channel(channel)) == NULL)
		{
		  chan = new_channel(channel);
		}
	  
	  add_user_to_channel(sptr, chan, T_PEON);
	  
	  /* at this point we have to convert the join into
		 an SJOIN .. */
	  
	  send_out_join(cptr, sptr->name, chan);
	}
  return 0;
}
