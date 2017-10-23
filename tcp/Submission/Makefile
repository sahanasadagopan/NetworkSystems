#@author Sahana Sadagopan
# credits of original Sam Siewert
INCLUDE_DIRS = 
LIB_DIRS = 

CDEFS= 
CFLAGS= -O0 -Wall -Werror -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

PRODUCT=tserver

HFILES= tserver.h
CFILES= tserver.c 

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

tserver:	tserver.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ tserver.o $(LIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
