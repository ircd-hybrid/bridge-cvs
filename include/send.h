#ifndef __SEND_H_INCLUDED
#define __SEND_H_INCLUDED

void clear_sendq(struct Client *);
void sendto_one(struct Client *cptr, char *fmt, ...);
void sendto_serv_butone(struct Client *cptr, char *fmt, ...);

#endif
