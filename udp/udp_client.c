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
#include <errno.h>

#define MAXBUFSIZE 100

/* You will have to modify the program below */

int main (int argc, char * argv[])
{
	char array[]="hooray";


	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];

	struct sockaddr_in remote;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}



	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
    if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("binding\n");
		
	}
	
	else{
		printf("create socket \n");
		//printf("%d\n",sock );
	}
	//char *command;
	printf("Pick one of the commands\n");
	printf("get[file_name]\n");
	printf("put[file_name]\n");
	printf("delete[file_name]\n");
	printf("ls\n");
	printf("exit\n");
	printf("Type the command and filename");
	//scanf("The user's IP address  and file%c \n", command);

	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	char *command = "apple";
	unsigned int remote_length = sizeof(remote);	
	nbytes = sendto(sock,command,strlen(command),0,(struct sockaddr *)&remote, sizeof(remote));
	// Blocks till bytes are received
	struct sockaddr_in from_addr;
	int addr_length = sizeof(struct sockaddr);
	bzero(buffer, sizeof(buffer));

	nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr *)&remote, &addr_length);  
	printf("%d\n",nbytes );
	if(nbytes>0){
		printf("The client didnt says \n");
	}
	else{
		printf("The client sent %s \n ",buffer);
	}
	printf("Server says %s\n", buffer);

	close(sock);

}

