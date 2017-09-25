#@author Sahana Sadagopan
# Credits to Sam Siewert
INCLUDE_DIRS = 
LIB_DIRS = 

CDEFS= 
CFLAGS= -Wall -Werror -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

PRODUCT=udp_client

HFILES= 
CFILES= udp_client.c 

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

udp_client:	udp_client.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ udp_client.o $(LIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
