#ifndef _PROTOCOL_H_INCLUDED
#define _PROTOCOL_H_INCLUDED

void send_out_nickchange(struct Client *, char *, char *, long);
void send_out_newclient(struct Client *, char *, long);
void send_out_server(struct Client *, char *, char *, int, char *);
void send_out_nick(struct Client *, char *, char *, int, long, int, char *, char *, char *, char *);
void send_out_join(struct Client *, char *, char *);

#endif
