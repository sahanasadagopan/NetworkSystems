#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 100000

int main (int argc, char * argv[] )
{


	int sock;                           //This will be our socket
	struct sockaddr_in sin,remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE]; 
	char file_name[20];            //a buffer to store our received message
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("unable to create socket");
	}
    
    else{
		printf("created socket: descriptor=%d\n", sock);
	}


	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}
	else{
		printf("binding\n");
	}

	remote_length = sizeof(remote);
	//unsigned int sin_length = sizeof(sender);
	//waits for an incoming message
	bzero(buffer, sizeof(buffer));
	nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr *)&remote, &remote_length);
	nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
	//printf("%d\n",nbytes);
	//printf("just waiting here to recieve\n");
	if(nbytes>0){
		printf("The client sent %s \n",buffer);
		printf("The file name is %s\n",file_name );
	}
	else{
		printf("The client sent \n ");
	}
	FILE *file = fopen(file_name,"r");
	char msg[MAXBUFSIZE];
	//fseek(file,0,SEEK_SET);
	//while(msg != EOF){
	//while(1){
	int data=fread(msg, 1,MAXBUFSIZE,file);
	printf("%s",msg);
	nbytes = sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&remote, remote_length);
	printf("%d\n",nbytes );
	//}

	
	/*if(file == NULL){
		printf("THe file doenst exist\n");
	}
	nbytes = sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&remote, remote_length);
	bzero(msg, 200);
	strcpy(msg, "end");
    sendto(sock, msg, sizeof(msg), 0, (struct sockaddr*)&remote, remote_length);*/

	//printf("%d\n",msg);
	close(sock);
}

