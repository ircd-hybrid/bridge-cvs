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
 * $Id: m_pass.c,v 1.2 2001/02/11 08:00:20 ejb Exp $
 */


#include <stdio.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"

int
m_pass(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	if (parc == 3) { /* three argument to PASS, so it's a possible TS server */
		if (strcmp(parv[2], "TS")) /* does TS */
			cptr->localClient->caps |= CAP_TS;
		else if (strcmp(parv[2], "EFNEXT")) /* EFNeXT - Does TS too */
			cptr->localClient->caps |= CAP_TS | CAP_EFNEXT;
	}
	
	strncpy(cptr->localClient->pass, parv[1], sizeof(cptr->localClient->pass));
	return 0;
}
