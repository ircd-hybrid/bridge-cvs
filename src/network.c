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
 * $Id: network.c,v 1.2 2001/02/11 08:00:21 ejb Exp $
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#include "network.h"
#include "setup.h"
#include "main.h"
#include "clients.h"
#include "config.h"

int 
bind_server(void) 
{
	char listenhost[64];
	struct sockaddr_in addr;
	int opt = 1;

	if (!ConfigFileEntry.port) {
		fprintf(stderr, "%% NET:ERR:No port has been defined.\n");
		exit(EXIT_FAILURE);
	}
	
	addr.sin_port = htons(ConfigFileEntry.port);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	if ((serv_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "%% NET:ERR:socket(): %s\n", strerror(errno));
		return -1;
	}

	setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

	if (bind(serv_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		fprintf(stderr, "%% NET:ERR:bind(): %s\n", strerror(errno));
		return -1;
	}
	
	strcpy(listenhost, inet_ntoa(addr.sin_addr));

	if (listen(serv_fd, 5) == -1) {
		fprintf(stderr, "%% NET:ERR:Can't listen on server port! Eiting..\n");
		return -1;
	}

	printf("%% NET:INF:Listening on %s:%d\n", listenhost, ntohs(addr.sin_port));
	
	return 0;
}

int
handle_pending(void)
{
	int client_fd;
	fd_set readfs;
	
	struct sockaddr_in sock;
	struct timeval tv;

	socklen_t socklen = sizeof(sock);
	
	FD_ZERO(&readfs);
	FD_SET(serv_fd, &readfs);

	tv.tv_sec = 0;
	tv.tv_usec = 100;
	
	if (!select(serv_fd + 1, &readfs, NULL, NULL, &tv))
		return 0;
	
	client_fd = accept(serv_fd, (struct sockaddr *)&sock, &socklen);
	
	if(client_fd == -1) {
		fprintf(stderr, "%% NET:ERR:accept failed: %s\n", strerror(errno));
		return -1;
	}
	
	/* try to set the socket to non-blocking */
	if(fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
		fprintf(stderr, "%% NET:ERR:Could not set socket non-blocking: %s\n", strerror(errno));
		close(client_fd);
		return -1;
	}
	
	new_client(sock, client_fd);
	return 0;
	
}

