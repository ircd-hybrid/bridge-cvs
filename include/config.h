#ifndef __CONFIG_H_INCLUDED
#define __CONFIG_H_INCLUDED

#include "clients.h"

#define PROTOCOL_TS3 0
#define PROTOCOL_P8 1
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
