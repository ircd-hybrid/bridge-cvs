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
 * $Id: config.h,v 1.3 2001/05/05 15:45:00 ejb Exp $
 */

#ifndef __CONFIG_H_INCLUDED
#define __CONFIG_H_INCLUDED

#include "clients.h"

#define PROTOCOL_TS3 0
#define PROTOCOL_UNREAL 1
#define PROTOCOL_28 2

struct ConfigFileEntryT 
{
	char myname[NAMELEN];
	char myinfo[INFOLEN];
	int port;
};

extern struct ConfigFileEntryT ConfigFileEntry;

struct nConf 
{
	char server[NAMELEN];
	char pass[PASSLEN];
	int protocol;
};

void parse_config(void);
struct nConf *find_nconf(char *);

#endif
