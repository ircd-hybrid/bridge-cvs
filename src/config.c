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
 * $Id: config.c,v 1.5 2001/05/16 02:13:39 ejb Exp $
 */


#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "clients.h"
#include "tools.h"
#include "str.h"
#include "setup.h"

dlink_list nConf_list;

struct ConfigFileEntryT ConfigFileEntry;

void
new_nconf(server, password, protocol)
	 char *server, *password, *protocol;
{
  struct nConf *newconf;
  dlink_node *node = make_dlink_node();
  
  newconf = malloc(sizeof(struct nConf));
  strcpy(newconf->server, server);
  strcpy(newconf->pass, password);

  if (!strcasecmp(protocol, "TS3"))
	newconf->protocol = PROTOCOL_TS3;
  else if (!strcasecmp(protocol, "Unreal"))
	newconf->protocol = PROTOCOL_UNREAL;
  else if (!strcmp(protocol, "2.8"))
	newconf->protocol = PROTOCOL_28;
  else {
	fprintf(stderr, "%% CNF:ERR:Protocol '%s' is unknown\n", protocol);
	exit(EXIT_FAILURE);
  }
  
  printf("%% CNF:INF:Server %s with password %s uses protocol %s\n", server, password, protocol);
  dlinkAdd(newconf, node, &nConf_list);
}

struct nConf *
find_nconf(name)
	 char *name;
{
  struct nConf *conf;
  dlink_node *node;
  
  for (node = nConf_list.head; node; node = node->next)
	{
	  conf = node->data;
	  if (!strcmp(conf->server, name))
		return conf;
	}
  return NULL;
}

void
new_mconf(name, port, info)
	 char *name, *port, *info;
{
  int p = atoi(port);

  strcpy(ConfigFileEntry.myname, name);
  strcpy(ConfigFileEntry.myinfo, info);
  
  ConfigFileEntry.port = p;
  
  printf("%% CNF:INF:Bridge server %s [%s] listens on port %d\n", ConfigFileEntry.myname, ConfigFileEntry.myinfo, ConfigFileEntry.port);
}


void
parse_config(void)
{
  FILE *conf;
  char line[100];
  int parc;
  char **parv;
  char **oldparv;
  int lineno = 0;
  
  parv = malloc(sizeof(char *) * MAX_ARG);
  oldparv = parv;
  
  if((conf = fopen(CPATH, "r")) == NULL) 
	{
	  fprintf(stderr, "%% CNF:ERR:%s:Cannot open config file: %s\n", CPATH, strerror(errno));
	  exit(EXIT_FAILURE);
	}

  memset(&ConfigFileEntry, 0, sizeof(ConfigFileEntry));
	
  while(fgets(line, sizeof(line), conf) != NULL) 
	{
	  line[strlen(line)-1] = '\0';
	  
	  lineno++;
	  
	  if (line[0] == '#' || line[0] == '\0')
		continue;
	  
	  create_raw_vector(&parc, parv, line, ":");
	  parc--;
	  parv++;
	  
	  switch(line[0]) 
		{
		case 'N':
		  if (parc != 3) 
			{
			  fprintf(stderr, "%% CNF:ERR:%s:%d:N requires 3 fields\n", CPATH, lineno);
			  continue;
			}
		  
		  new_nconf(parv[0], parv[1], parv[2]);
		  break;
		  
		case 'M':
		  if (parc != 3) 
			{
			  fprintf(stderr, "%% CNF:ERR:%s:%d:M requires 3 fields\n", CPATH, lineno);
			  continue;
			}
		  
		  new_mconf(parv[0], parv[1], parv[2]);
		  break;
#if 0
		case 'O':
		  if (parc != 1)
			{
			  fprintf(stderr, "%% CNF:ERR:%s%d:O requires only 1 field\n", CPATH, lineno);
			  continue;
			}
		  parse_option(parv[1]);
		  break;
#endif
		default:
		  fprintf(stderr, "%% CNF:ERR:%s:%d:Unknown conf section %s\n", CPATH, lineno, parv[0]);
		  continue;
		}
	}
  free(oldparv);
}

	
