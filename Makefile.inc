CC=	cc
RM=	rm -f
CFLAGS=	-O2 -Wall -Werror -g
CPPFLAGS=-I../include
MKDEP=	$(CC) -MM
LDFLAGS=

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
