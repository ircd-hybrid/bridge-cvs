#ifndef _PROTOCOL_H_INCLUDED
#define _PROTOCOL_H_INCLUDED

#include "channel.h"

void send_out_nickchange(struct Client *, char *, char *, long);
void send_out_newclient(struct Client *, char *, long);
void send_out_server(struct Client *, char *, char *, int, char *);
void send_out_nick(struct Client *, char *, char *, int, long, int, char *, char *, char *, char *);
void send_out_join(struct Client *, char *, struct Channel *);
void send_out_topic(struct Client *, struct Channel *, struct Client *, long, char *);
void send_out_umode(struct Client *, struct Client *, struct Client *, char, int);

#endif
