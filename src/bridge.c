/* a simple IRC bridging program */

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "setup.h"
#include "network.h"
#include "main.h"
#include "clients.h"
#include "config.h"
#include "serno.h"

int serv_fd;
time_t CurrentTime;

int
main(int argc, char **argv)
{
#if 0
	char linebuf[1000];
#endif

	printf("%% Bridge version %s [%s] startup: \n", BRIDGE_VERSION, SERIALNUM);
	
	memset(&local_cptr_list, 0, sizeof(local_cptr_list));
	memset(&serv_cptr_list, 0, sizeof(serv_cptr_list));
	memset(&cptr_list, 0, sizeof(cptr_list));
	
	printf("%% Loading configuration...\n");
	parse_config();
	
	printf("%% Initialising network...\n");

	if (bind_server())
		exit(EXIT_FAILURE);

	signal(SIGPIPE, SIG_IGN);
	
	printf("%% Startup complete.\n");
	for (;;) {
		CurrentTime = time(NULL);
		handle_pending();
		if (local_cptr_list.head)
			read_loop();
	}
	
	return 0;
}
