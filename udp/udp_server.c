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
#include <stdint.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 1000

void findfilesize(char file_name[20],char size[20]){
	FILE *f = fopen(file_name, "rb+");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(size,"%lu",fsize);
	printf("%s\n",size );
}

void filereadsend(char file_name[20],int sock,struct sockaddr_in remote,int TotalSize){
	int sentdata=0;
	int nbytes;
	char msg[MAXBUFSIZE];
	unsigned int remote_length;
	int count=0;
	remote_length = sizeof(remote);

	//int ret = strcmp(file_name,"foo4");
	//if (ret == 0){
		int Packetsize=1000;
		printf("Packetsize%d\n",Packetsize );
		FILE *file = fopen(file_name,"rb+");
		char buffer[Packetsize];
		int Packetleft =  TotalSize;
		while(fread(buffer,1,Packetsize,file)){
			printf("print %zu\n",sizeof(buffer));
			for(int i=0;i<5000000;i++);
			nbytes = sendto(sock,buffer,Packetsize,0,(struct sockaddr *)&remote, remote_length);
			printf("%s\n",buffer );
			printf("Total buffer %d\n",TotalSize );
			printf("The amount send%d\n",Packetsize );
			Packetleft = TotalSize - Packetsize;
			TotalSize=Packetleft;
			if(Packetleft<1000){
				Packetsize=Packetleft;
			}
			else{
				Packetsize=1000;
			}
			printf("The amount left %d\n",Packetsize );
			printf("Packet sent %d\n",count );	
			printf("The packets it sent%d\n",nbytes );
			count++;
			sentdata=sentdata+nbytes;
		}
		fclose(file);
	//}
}

int main (int argc, char * argv[] )
{


	int sock;                           //This will be our socket
	struct sockaddr_in sin,remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE]; 
	char file_name[20];  
	char size[20];
	
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
	bzero(buffer, sizeof(buffer));
	nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr *)&remote, &remote_length);
	int ret = strcmp(buffer,"get");
	if(ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
		if(nbytes>0){
			printf("The client sent %s \n",buffer);
			printf("The file name is %s\n",file_name );
		}
		else{
			printf("The client sent \n ");
		}
		findfilesize(file_name,size);
		nbytes = sendto(sock,size,strlen(size),0,(struct sockaddr *)&remote, remote_length);
		int TotalSize  = atoi(size);
		int NoOfPackets = TotalSize/MAXBUFSIZE;
		printf("No od packets to send %d\n",NoOfPackets );
		filereadsend(file_name,sock,remote,TotalSize);
		close(sock);
	}
}

