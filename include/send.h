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
 * $Id: send.h,v 1.3 2001/05/07 21:31:56 ejb Exp $
 */

#ifndef __SEND_H_INCLUDED
#define __SEND_H_INCLUDED

void clear_sendq(struct Client *);
void sendto_one(struct Client *, char *, ...);
void sendto_serv_butone(struct Client *, char *, ...);
void sendto_cap_serv_butone(struct Client *, int, char *, ...);

#endif
