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

OBJS=	$(SRCS:.c=.o)

all: build
build: bridge

bridge: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o bridge

clean:
	$(RM) *~ *.o bridge

depend:
	@${MKDEP} ${CPPFLAGS} ${SRCS} > .depend
