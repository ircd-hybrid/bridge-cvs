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

#define CLIENT_EXITED -2

struct cmd cmdtab[] = {
	{"pass", MFLG_UNREG, m_pass},
	{"server", MFLG_UNREG, m_server},
	{"protoctl", MFLG_UNREG, m_protoctl},
	{"ping", MFLG_UNREG, m_ping},
	{"nick", 0, m_nick},
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
	char buffer[1024];
	int i;
	char line[BUFSIZE];
	char *s;
	char *b;
	int bytes;
	
	i = recv(cptr->localClient->fd, buffer, BUFSIZE, 0);
	if (i == -1) {
		printf("%% NET:ERR:Read error from %s[%s]: %s\n", IsRegistered(cptr) ? cptr->name : "unknown", cptr->localClient->host, strerror(errno));
		cptr->localClient->fd = -1;
		exit_client(cptr, "Read error");
		return -1;
	}
	b = buffer;
	bytes = 0;
	
	for (;;) {
		int lbytes = 0;
		
		s = line;
		
		if (cptr->localClient->bufsavelen) {
			strncpy(line, cptr->localClient->bufsave, cptr->localClient->bufsavelen);
			s += cptr->localClient->bufsavelen;
			lbytes = cptr->localClient->bufsavelen;
			cptr->localClient->bufsavelen = 0;
		}
		
		while (bytes <= i && *b && *b != '\r') {
			bytes++;
			lbytes++;
			*s++ = *b++;
			if (bytes == i) { /* we reached the end of the buffer, and didn't see \r\n yet */
				/* so, save what we did get of that line into the client.. */
				strncpy(cptr->localClient->bufsave, line, lbytes);
				cptr->localClient->bufsavelen = lbytes;
				return 0;
			}
		}
		
		*s = *b = '\0';
		s += 2; b += 2;
		bytes += 2;
		
#if 0
		printf("Read line: %s\n", line);
#endif
		parse(cptr, line, s - 3);
		if (bytes >= i)
			break;
	}
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
	return 0;
}

int parse(struct Client *cptr, char *pbuffer, char *bufend)
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

      if( (s = strchr(ch, ' ')))
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
          
          if (!from) {
			  printf("%% IRC:ERR:Client %s is unknown!\n", sender);
			  /* unknown prefix, client should be removed */
			  return -1;
		  }
		  
		  if (from->from && from->from != cptr) /* from->from == NULL for local server */
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

  if( strlen(ch) >= 3 && *(ch + 3) == ' '  && /* ok, lets see if its a possible numeric.. */
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

  end = bufend - 1;
  
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
