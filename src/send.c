#include <sys/types.h>
#include <sys/socket.h>

#include <stdarg.h>
#include <stdio.h>

#include "clients.h"
#include "send.h"
#include "setup.h"

void
clear_sendq(struct Client *cptr)
{
	cptr->localClient->s_top = cptr->localClient->s_bottom = cptr->localClient->s_size = 0;
}


static int 
sqpush(struct Client *cptr, char v)
{
	if (cptr->localClient->s_size == SENDQ_SIZE)
		return -1;	/* fifo is full */

	/* else we push the value */
	cptr->localClient->sendq[cptr->localClient->s_bottom] = v;
	++cptr->localClient->s_bottom;
	cptr->localClient->s_size++;
	
	return 0;
	/* success */
}

static int
flush_sendq(struct Client *cptr)
{
	
	int sent;
	/* Amount we sent */

	sent = send(cptr->localClient->fd, cptr->localClient->sendq+cptr->localClient->s_top, cptr->localClient->s_size, 0);
	
	if(sent != -1) {
		cptr->localClient->s_size -= sent;
		cptr->localClient->s_top += sent;
	}

	/* Nothing left to send?  Start at the beginning! */
	if(cptr->localClient->s_size == 0)
		cptr->localClient->s_top = cptr->localClient->s_bottom = 0;
	
	if(sent == -1 || cptr->localClient->s_size) /* We failed to send everything */
		return -1;
	
	return 0;
}

void
sendto_serv_butone(struct Client *cptr, char *fmt, ...)
{
	va_list ap;
	char str[BUFSIZE];
	dlink_node *node, *next_ptr;
	struct Client *acptr;
	int len;
	
	va_start(ap, fmt);
	len = vsprintf(str, fmt, ap);
	va_end(ap);

	str[len++] = '\r';
	str[len++] = '\n';
	str[len++] = '\0';

	for (node = local_cptr_list.head; node; node = next_ptr) {
		acptr = node->data;
		if (acptr != cptr) {
			sendto_one(acptr, "%s", str);
		}
		
		next_ptr = node->next;
	}
}


void 
sendto_one(struct Client *cptr, char *fmt, ...)
{
	va_list ap;
	char str[BUFSIZE];
	char *c;
	int len;
	
	if (!IsLocal(cptr)) {
		printf("%% NET:ERR:Tried to send to non-local client %s!\n", cptr->name);
		return;
	}
	
	va_start(ap, fmt);
	len = vsprintf(str, fmt, ap);
	va_end(ap);

	str[len++] = '\r';
	str[len++] = '\n';
	str[len++] = '\0';
	
#ifdef DEBUG_SEND
	printf("%% NET:DBG:Sent: %s", str);
#endif

	for(c=str; *c; c++) 
		if (sqpush(cptr, *c) == -1) {
			/* SendQ exceeded, try to flush */
			if(flush_sendq(cptr) == -1) {
				/* failed, kill the client */
				exit_client(cptr, "SendQ Exceeded");
				return;
			}
			/* worked, continue sending data */
		}
	
	/* try to flush the sendq now if we can .. */
	if(cptr->localClient->s_size)
		flush_sendq(cptr);
}

