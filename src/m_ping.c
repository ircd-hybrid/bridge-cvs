#include "clients.h"
#include "handlers.h"
#include "send.h"

int
m_ping(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	sendto_one(cptr, "PONG :%s", parv[1]);
	return 0;
}
