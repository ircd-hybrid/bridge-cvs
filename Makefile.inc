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
# $Id: Makefile.inc,v 1.4 2001/05/16 02:13:34 ejb Exp $

CC=	cc
RM=	rm -f
#CFLAGS=	-O2 -Wall -Werror 
CFLAGS=	-Wall -Werror -g
CPPFLAGS=-I../include
MKDEP=	$(CC) -MM
LDFLAGS=

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
