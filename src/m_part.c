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
 * $Id: m_part.c,v 1.1 2001/05/07 16:36:52 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"
#include "channel.h"

int
m_part(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  /* char *user = parv[0]; */
  char *channel = parv[1];
  struct Channel *chan;
  char *reason = NULL;

  if ((chan = find_channel(channel)) == NULL)
	return 0; /* shouldn't ever happen */

  remove_user_from_channel(sptr, chan);

  if (parc > 2)
	reason = parv[2];

  if (reason)
	sendto_serv_butone(cptr, ":%s PART %s :%s", parv[0], parv[1], reason);
  else
	sendto_serv_butone(cptr, ":%s PART %s", parv[0], parv[1]);

  return 0;
}
