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
 * $Id: m_user.c,v 1.1 2001/05/16 02:13:41 ejb Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "clients.h"
#include "handlers.h"
#include "config.h"
#include "send.h"
#include "protocol.h"

/*
** second part of an ircd2.8.21 nick introduction.
** parv[0] = nick being introduced
** parv[1] = username
** parv[2] = hostname
** parv[3] = servername
** parv[4] = infoline
*/

int
m_user(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
	struct Client *newclient;

	if ((newclient = find_client(parv[0])) == NULL)
	  {
		sendto_serv_butone(NULL, ":%s WALLOPS :Received USER before NICK from %s for %s", 
						   cptr->name, parv[0]);
		return 0;
	  }

	newclient->from = find_client(parv[3]);

	strncpy(newclient->info, parv[4], INFOLEN);
	strncpy(newclient->user->server, parv[3], NAMELEN - 1);
	strncpy(newclient->user->username, parv[1], USERLEN - 1);
	strncpy(newclient->user->hostname, parv[2], HOSTLEN - 1);

	send_out_nick(cptr, cptr->name, parv[0], newclient->hopcount + 1, 
				  newclient->user->ts, 0, parv[1], parv[2], parv[3], parv[4]);
	return 0;
}
