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
 * $Id: m_squit.c,v 1.2 2001/05/07 16:36:53 ejb Exp $
 */

#include <string.h>

#include "clients.h"
#include "handlers.h"
#include "send.h"
#include "config.h"
#include "serno.h"

int
m_squit(cptr, sptr, parc, parv)
	 struct Client *cptr, *sptr;
	 int parc;
	 char **parv;
{
  struct Client *quit;

  if ((quit = find_client(parv[1])) == NULL)
	{
	  /* no server! */
	  printf("%% IRC:ERR:SQUIT for non-existant server %s\n", parv[1]);
	  return 0;
	}

  printf("squit for server %s (reason: %s)\n",
		 quit->name, parv[2]);

  if (parc > 1)
	exit_client(quit, cptr, parv[2]);
  else
	exit_client(quit, cptr, sptr->name);
  
  return 0;
}
