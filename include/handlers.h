#ifndef __HANDLERS_H_INCLUDED
#define __HANDLERS_H_INCLUDED

int m_pass(struct Client *, struct Client *, int, char **);
int m_server(struct Client *, struct Client *, int, char **);
int m_protoctl(struct Client *, struct Client *, int, char **);
int m_ping(struct Client *, struct Client *, int, char **);
int m_nick(struct Client *, struct Client *, int, char **);

#endif
