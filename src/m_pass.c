#include <stdio.h>
#include <string.h>

#include "clients.h"
#include "handlers.h"

int
m_pass(struct Client *cptr, struct Client *sptr, int parc, char **parv)
{
	if (parc == 3) { /* three argument to PASS, so it's a possible TS server */
		if (strcmp(parv[2], "TS")) /* does TS */
			cptr->localClient->caps |= CAP_TS;
		else if (strcmp(parv[2], "EFNEXT")) /* EFNeXT - Does TS too */
			cptr->localClient->caps |= CAP_TS | CAP_EFNEXT;
	}
	
	strncpy(cptr->localClient->pass, parv[1], sizeof(cptr->localClient->pass));
	return 0;
}
