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
 * $Id: clients.h,v 1.4 2001/05/05 15:44:59 ejb Exp $
 */

#ifndef __CLIENTS_H_INCLUDED
#define __CLIENTS_H_INCLUDED

#include <sys/types.h>

#include <netinet/in.h>

#include <stdio.h>

#include "tools.h"

#define BUFSIZE 512

#define NAMELEN 64
#define IPLEN 64
#define PASSLEN 32
#define INFOLEN 128

#define STATUS_UNREGISTERED 0
#define STATUS_DELETED 1
#define STATUS_REGISTERED 2

#define SENDQ_SIZE 10000000

#define RECVQ_SIZE 100000

#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#define DoesCap(cptr, cap) ((cptr)->localClient->caps & (cap))

#define CAP_TS        00000001
#define CAP_EFNEXT    00000002
#define CAP_SJOIN     00000004

#define IsRegistered(x) ((x)->localClient->status == STATUS_REGISTERED)
#define IsLocal(x) ((x)->localClient)

#define USERLEN 10
#define HOSTLEN 64

struct User 
{
  char username [USERLEN + 1];
  char hostname [HOSTLEN + 1];
  char server [NAMELEN + 1];
  int umodes;
  long ts;
};

struct Client 
{
  struct Client *next;
  struct Client *prev;
  char name[NAMELEN + 1]; /* name of client (irc.server.com) */
  char info[INFOLEN + 1];
  struct Client *from;
  struct Client *local;
  int hopcount;
  int type; /* TYPE_SERVER or TYPE_CLIENT */
  
  struct LocalClient *localClient; /* always null for clients */
  struct User *user;
};

struct LocalClient 
{
	char pass[PASSLEN + 1]; /* password they sent - this is unused after registration */
	char exitreason[BUFSIZE];
	char host[IPLEN + 1];
	struct in_addr ip;  /* IP */
	int fd;
	int status;
	int servertype;
	unsigned long caps;

	char bufsave[BUFSIZE];
	int bufsavelen;
	
	char            sendq[SENDQ_SIZE];
	int             s_top;
	int             s_bottom;
	int             s_size;
};

typedef struct {
  int servers;
  int clients;
} count_t;

extern count_t Count;

extern dlink_list serv_cptr_list;
extern dlink_list local_cptr_list;
extern dlink_list cptr_list;
extern dlink_list client_cptr_list;

void new_client(struct sockaddr_in, int);
struct Client *user_newslot(int);
int read_loop(void);
void exit_client(struct Client*, struct Client *, char *);
struct Client *find_client(char *);
void send_myinfo(struct Client *);
void send_netburst(struct Client *);
struct Client *find_server(char *);

#endif
