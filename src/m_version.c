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
 * $Id: m_version.c,v 1.1 2001/05/04 23:11:25 ejb Exp $
 */

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"

int
m_version(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	sendto_one(sptr, ":%s 351 %s bridge/alpha. %s :TS3/P8", ConfigFileEntry.myname, parv[0], ConfigFileEntry.myname);
	return 0;
}
