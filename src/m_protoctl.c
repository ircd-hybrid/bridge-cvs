#include "clients.h"
#include "handlers.h"

/* bahamut-style protoctl command */

int
m_protoctl(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	int i = 0;
	
	while (i < parc) {
		if (!strcmp(parv[i], "SJOIN"))
			cptr->localClient->caps |= CAP_SJOIN;
		i++;
	}
	
	return 0;
}
