#ifndef __TSERVER
#define __TSERVER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

#define MAX 1000
#define BYTES 1024



void Server(char *,int );
void file_handler(int);


#endif
