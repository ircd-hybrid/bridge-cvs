/************************************************************************
 *   Copyright (C) 2000,2001 Edward Brocklesby
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: bridge.c,v 1.2 2001/02/11 08:00:19 ejb Exp $
 */

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
