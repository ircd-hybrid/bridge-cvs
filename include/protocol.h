#ifndef _PROTOCOL_H_INCLUDED
#define _PROTOCOL_H_INCLUDED

void send_out_nickchange(char *, char *, long);
void send_out_newclient(struct Client *, char *, long);
void send_out_server(struct Client *, char *, char *, int, char *);

#endif
