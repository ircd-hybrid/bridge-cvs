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


