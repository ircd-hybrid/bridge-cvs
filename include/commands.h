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
 * $Id: commands.h,v 1.2 2001/02/11 08:00:17 ejb Exp $
 */

#ifndef __COMANDS_H_INCLUDED
#define __COMANDS_H_INCLUDED

#include "clients.h"

#define MAX_ARG 64

#define MFLG_UNREG 1

struct cmd 
{
	char *command;	/* what the command is */
	int flags;
	int (*f)(struct Client *, struct Client *, int, char **);	/* Function we run */
};

int handle_data(struct Client *);
int parse(struct Client *, char *, char *);


#endif


