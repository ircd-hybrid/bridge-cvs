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
 * $Id: handlers.h,v 1.7 2001/05/07 16:36:47 ejb Exp $
 */

#ifndef __HANDLERS_H_INCLUDED
#define __HANDLERS_H_INCLUDED

int m_pass(struct Client *, struct Client *, int, char **);
int m_server(struct Client *, struct Client *, int, char **);
int m_protoctl(struct Client *, struct Client *, int, char **);
int m_ping(struct Client *, struct Client *, int, char **);
int m_nick(struct Client *, struct Client *, int, char **);
int m_version(struct Client *, struct Client *, int, char **);
int m_quit(struct Client *, struct Client *, int, char **);
int m_privmsg(struct Client *, struct Client *, int, char **);
int m_trace(struct Client *, struct Client *, int, char **);
int m_sjoin(struct Client *, struct Client *, int, char **);
int m_squit(struct Client *, struct Client *, int, char **);
int m_part(struct Client *, struct Client *, int, char **);
int m_globops(struct Client *, struct Client *, int, char **);
int m_wallops(struct Client *, struct Client *, int, char **);
int m_operwall(struct Client *, struct Client *, int, char **);
int m_join(struct Client *, struct Client *, int, char **);
int m_kill(struct Client *, struct Client *, int, char **);

#endif
