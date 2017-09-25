#@author Sahana Sadagopan
# Credits to Sam Siewert
INCLUDE_DIRS = 
LIB_DIRS = 

CDEFS= 
CFLAGS= -Wall -Werror -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

PRODUCT=udp_server

HFILES= 
CFILES= udp_server.c 

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

udp_server:	udp_server.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ udp_server.o $(LIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
