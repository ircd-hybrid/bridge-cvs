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
 * $Id: commands.c,v 1.8 2001/05/07 16:36:50 ejb Exp $
 */


#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include "commands.h"
#include "str.h"
#include "send.h"
#include "main.h"
#include "tools.h"
#include "handlers.h"
#include "setup.h"
#include "config.h"

#define CLIENT_EXITED -2

struct cmd cmdtab[] = {
	{"pass", MFLG_UNREG, m_pass},
	{"server", MFLG_UNREG, m_server},
	{"protoctl", MFLG_UNREG, m_protoctl},
	{"ping", MFLG_UNREG, m_ping},
	{"nick", 0, m_nick},
	{"version", 0, m_version},
	{"quit", MFLG_UNREG, m_quit},
	{"privmsg", 0, m_privmsg},
	{"trace", 0, m_trace},
	{"sjoin", 0, m_sjoin},
	{"squit", 0, m_squit},
	{"part", 0, m_part},
	{"globops", 0, m_globops},
	{"operwall", 0, m_operwall},
	{"wallops", 0, m_wallops},
	{"join", 0, m_join},
	{"kill", 0, m_kill},
	{NULL, 0, NULL},
};

static int handle_command(struct cmd *, struct Client *, struct Client *, int, char **);

struct cmd *
find_command(char *name)
{
	int i;
	
	for (i = 0; cmdtab[i].command; i++) {
		if (!strcasecmp(cmdtab[i].command, name))
			return &cmdtab[i];
	}
	return NULL;
}

int
handle_data(struct Client *cptr)
{
  char line[BUFSIZE];
  char *b, *next, *start;
  int lbytes, tot;
  int i;
  
  b = line;
  lbytes = 0;

  if (cptr->localClient->bufsavelen > 0)
	{
		strncpy(line, cptr->localClient->bufsave,
		    cptr->localClient->bufsavelen);
		b += cptr->localClient->bufsavelen;
		lbytes = cptr->localClient->bufsavelen;
		cptr->localClient->bufsavelen = 0;
	}

  tot = lbytes;
  i = recv(cptr->localClient->fd, b, BUFSIZE - lbytes - 1, 0);
  if (i == -1) 
	{
	  printf("%% NET:ERR:Read error from %s[%s]: %s\n", IsRegistered(cptr) ? cptr->name : "unknown", cptr->localClient->host, strerror(errno));
	  cptr->localClient->fd = -1;
	  exit_client(cptr, NULL, "Read error");
	  return -1;
	}
  else if (i == 0)
 	 {
		/* got disconnected */
		cptr->localClient->fd = -1;
	  	exit_client(cptr, NULL, "Client disconnected");
		return -1;
	}
  
  tot += i;
  *(line + tot) = '\0';
  next = b = line;
	  
again:
  b = next;
  start = b;
  lbytes = 0;
  while (*b != '\0' && *b != '\r' && *b != '\n') {
	  lbytes++;
	  b++;
  }

  if (*b == '\0')
	{
	  /* so, save what we did get of that line into the client.. */
		if (lbytes >= 512) {
			printf("ERR: line too long\n");
			return -1;
		}
		strncpy(cptr->localClient->bufsave, start, lbytes);
		cptr->localClient->bufsavelen = lbytes;
		return 0;
	}
  next = b + 1;
  if (*b == '\r')
	  next++;
  *b = '\0';
  printf("Read line: %s\n", start);
  parse(cptr, start);

  if (*next != '\0')
  	goto again;

  return 0;
}		
	
char* strtoken(char** save, char* str, char* fs)
{
	char* pos = *save;
	char* tmp;
	
	if (str)
		pos = str;
	
	while (pos && *pos && strchr(fs, *pos) != NULL)
		++pos;
	
	if (!pos || !*pos)
		return (pos = *save = NULL);
	
	tmp = pos;
	
	while (*pos && strchr(fs, *pos) == NULL)
		++pos;
	
	if (*pos)
		*pos++ = '\0';
	else
		pos = NULL;
	
	*save = pos;
	return tmp;
}


static void
string_to_array(char *string, char *end, int *parc, char *parv[MAX_ARG])
{
  char *ap;
  char *p = NULL;
  
  for(ap = strtoken(&p,string," "); ap; ap = strtoken(&p, NULL, " "))
	  if(*ap != '\0')
	  {
		  parv[(*parc)] = ap;
		  
		  if (ap[0] == ':') {
			  char *tendp = ap;
			  
			  while (*tendp++);
			  
			  if ( tendp < end ) /* more tokens to follow */
				  ap [ strlen (ap) ] = ' ';
			  
			  if (ap[0] == ':')
				  ap++;
			  
			  parv[(*parc)++] = ap;
			  break;
		  }
		  
		  if(*parc < MAX_ARG)
			  ++(*parc);
		  else
			  break;
	  }
  parv[(*parc)] = NULL;
}

static int
handle_command(struct cmd *mptr, struct Client *cptr, struct Client *from, int i, char *hpara[MAX_ARG])
{
  int (*f)(struct Client *, struct Client *, int, char **) = NULL;
	
  if (!IsRegistered(cptr))
    {
      /* if its from a possible server connection
       * ignore it.. more than likely its a header thats sneaked through
       */
		
      if(!(mptr->flags & MFLG_UNREG))
		  return -1;
    }
	
  f = mptr->f;
  	
  return (*f)(cptr, from, i, hpara);
}

int
do_numeric(char *numeric, struct Client *cptr, struct Client *from, int i, char **para)
{
  struct Client *target;
  char buffer[BUFSIZE * 2];
  int j;

  if ((target = find_client(para[1])) == NULL)
	{
	  sendto_serv_butone(NULL, ":%s WALLOPS :Numeric from %s to non-existant client %s", 
						 ConfigFileEntry.myname, para[0], para[1]);
	  return 0;
	}

  if (numeric[0] == '0')
	numeric[0] = '1';

  buffer[0] = '\0';

  /* :a <123> b a b :c */
  for (j = 1; j < i - 1; j++)
	{
	  strcat(buffer, " ");
	  strcat(buffer, para[j]);
	}
  
  strcat(buffer, " :");
  strcat(buffer, para[i - 1]);

  sendto_one(target, ":%s %s%s", para[0], numeric, buffer);
  return 0;
}

int parse(struct Client *cptr, char *pbuffer)
{
  struct Client*  from = cptr;
  char*           ch;
  char*           s;
  char*           end;
  int             i;
  char*           numeric = 0;
  struct cmd* mptr;
  int status;
  char *para[MAX_ARG];
  char *sender;
  
  for (ch = pbuffer; *ch == ' '; ch++)   /* skip spaces */
    /* null statement */ ;

  para[0] = from->name;

  if (*ch == ':')
    {
      ch++;

      /*
      ** Copy the prefix to 'sender' assuming it terminates
      ** with SPACE (or NULL, which is an error, though).
      */

      sender = ch;

      if ((s = strchr(ch, ' ')))
		{
		  *s = '\0';
		  s++;
		  ch = s;
		}
	  
      i = 0;
	  
      if (*sender)
		{
          from = find_client(sender);
		  
          para[0] = from->name;
          
          if (!from) 
			{
			  printf("%% IRC:ERR:Client %s is unknown!\n", sender);
			  sendto_serv_butone(NULL, ":%s WALLOPS :Unknown prefix %s from %s\n", ConfigFileEntry.myname,
								 sender, cptr->name);
			  /* unknown prefix, client should be removed */
			  if (strchr(sender, '.'))
				sendto_one(cptr, ":%s SQUIT %s :%s(?) <- %s",
						   ConfigFileEntry.myname, sender,
						   sender, cptr->name);
			  else
				sendto_one(cptr, ":%s KILL %s :%s (%s(?) <- %s)",
						   ConfigFileEntry.myname, sender,
						   ConfigFileEntry.myname, sender, cptr->name);
			  return -1;
			}
		  
		  if (from->from && from->local != cptr) /* from->from == NULL for local server */
            {
				/* wrong direction */
				printf("%% IRC:ERR:Wrong direction from %s (was %s, should be %s)\n", from->name, sender, cptr->name);
				return -1;
			}
		}
      while (*ch == ' ')
		  ch++;
    }
  
  if (*ch == '\0')
	{
	  return -1;
	}

  if (strlen(ch) >= 3 && *(ch + 3) == ' '  && /* ok, lets see if its a possible numeric.. */
      isdigit(*ch) && isdigit(*(ch + 1)) && isdigit(*(ch + 2)) )
    {
      mptr = NULL;
      numeric = ch;
      s = ch + 3;       /* I know this is ' ' from above if */
      *s++ = '\0';      /* blow away the ' ', and point s to next part */
    }
  else
    { 
      if( (s = strchr(ch, ' ')) )
        *s++ = '\0';

      mptr = find_command(ch);

      if (!mptr)
          return -1;
    }

  end = s;
  while (*end != '\0')
	 end++;
  end--;

  i = 1;
  
  if (s)
    string_to_array(s, end, &i, para);
   
  if (mptr == NULL)
    return do_numeric(numeric, cptr, from, i, para);
  else
  {
    status = handle_command(mptr, cptr, from, i, para);
    if (cptr->localClient->fd < 0)
      return CLIENT_EXITED;
    else
      return status;
  }
}

