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
 * $Id: clients.h,v 1.10 2002/03/11 14:53:14 ejb Exp $
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
#define USERLEN 10
#define HOSTLEN 64
#define TOPICLEN 120 

#define STATUS_UNREGISTERED 0
#define STATUS_DELETED 1
#define STATUS_REGISTERED 2

#define SENDQ_SIZE 10000000

#define RECVQ_SIZE 100000

#define TYPE_SERVER 1
#define TYPE_CLIENT 2

#define DoesCap(cptr, cap) ((cptr)->localClient->caps & (cap))

#define CAP_TS        0x00000001
#define CAP_HALFOPS   0x00000002
#define CAP_SJOIN     0x00000004
#define CAP_P28       0x00000010

#define IsRegistered(x) ((x)->localClient->status == STATUS_REGISTERED)
#define IsLocal(x) ((x)->localClient)
#define IsServer(x) (!((x)->user))

#define FLAGS_KILLED 0x000001
#define FLAGS_AWAY   0x000002

#define UMODE_INVISIBLE 0x00000001 /* ALL:           Invisible */
#define UMODE_OPER      0x00000002 /* ALL:           IRC Operator */
#define UMODE_SNOTICES  0x00000004 /* ALL:           Server notices */
#define UMODE_WALLOPS   0x00000008 /* ALL:           See wallops */
#define UMODE_SVSADM    0x00000010 /* UNREAL:        Services admin */
#define UMODE_SERVADM   0x00000020 /* HYBRID/UNREAL: Server admin */
#define UMODE_HELPOP    0x00000040 /* UNREAL:        Help operator */
#define UMODE_SERVICES  0x00000080 /* UNREAL:        Client is a service */
#define UMODE_NETADMIN  0x00000100 /* UNREAL:        Network Admin */
#define UMODE_TECHADMIN 0x00000200 /* UNREAL:        Technical Administrator */
#define UMODE_COADMIN   0x00000400 /* UNREAL:        Co-admin */
#define UMODE_REGNICK   0x00000800 /* UNREAL:        Registered nick */
#define UMODE_HIDE      0x00001000 /* UNREAL:        Hidden hostname */
#define UMODE_WHOIS     0x00002000 /* UNREAL:        See /whois requests */
#define UMODE_KIX       0x00004000 /* UNREAL:        "Only U: lines can kick you" */
#define UMODE_HIDING    0x00008000 /* UNREAL:        Hides join/part on channels */
#define UMODE_HIDEOPER  0x00010000 /* UNREAL:        Hide operator status */
#define UMODE_SETHOST   0x00020000 /* UNREAL:        Using a /vhost */
#define UMODE_SECURE    0x00040000 /* UNREAL:        SSL connection */

struct User 
{
  char username [USERLEN + 1];
  char hostname [HOSTLEN + 1];
  char server [NAMELEN + 1];
  long umodes;
  long ts;
  dlink_list channels;
  char away[TOPICLEN + 1];
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
  long flags;
  long umodes;

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
  FILE *file;
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
