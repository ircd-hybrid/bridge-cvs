#   Copyright (C) 2000,2001 Edward Brocklesby
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 1, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# $Id: Makefile,v 1.4 2001/05/05 12:53:26 ejb Exp $

include ../Makefile.inc

SRCS=	bridge.c \
	network.c \
	clients.c \
	send.c \
	commands.c \
	string.c \
	tools.c \
	config.c \
	m_pass.c \
	m_server.c \
	m_protoctl.c \
	m_ping.c \
	m_nick.c \
	m_version.c \
	protocol.c \
	m_quit.c \

OBJS=	$(SRCS:.c=.o)

all: build
build: bridge

bridge: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o bridge

clean:
	$(RM) *~ *.o bridge

depend:
	@${MKDEP} ${CPPFLAGS} ${SRCS} > .depend
