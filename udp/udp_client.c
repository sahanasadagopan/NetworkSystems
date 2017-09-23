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

#define PACKETSIZE 1000

void filewrite(char file_name[20],int sock,struct sockaddr_in remote,int Totalsize){
	int addr_length = sizeof(struct sockaddr);
	int nbytes;
	char buffer[PACKETSIZE];
	//int ret = strcmp(file_name,"foo4");
	//if(ret == 0){
		FILE *file;
		file = fopen("foo4_sent","wb+");
		int recievedpack = 0;
		int bytesleft=Totalsize;
		int datasize =1000;
		while(bytesleft!= 0){
			nbytes = recvfrom(sock,buffer,datasize,0,(struct sockaddr*)&remote, &addr_length);
			printf("%s\n",buffer );
			int data = fwrite(buffer,1,datasize,file);

			recievedpack = recievedpack+datasize;
			printf("%d\n",recievedpack );
			int bytesleft=Totalsize-recievedpack;
			printf("%d\n",bytesleft );
			printf("%d\n",Totalsize );
			if(bytesleft<1000){
				datasize=bytesleft;	
			}
			else{
				datasize=1000;
			}
			
			if (bytesleft == 0){
				break;
				fclose(file);
			}
		}
		fclose(file);
	//}
}

int main (int argc, char * argv[])
{
	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[PACKETSIZE];
	char *size;
	size = (char*) malloc(100*sizeof(char));
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
	char command[20];
	char file_name[20];
	//int command=0;
	printf("Pick one of the commands\n");
	printf("get[file_name]\n");
	printf("put[file_name]\n");
	printf("delete[file_name]\n");
	printf("ls\n");
	printf("exit\n");
	printf("Type your command and file name ");
    scanf(" %s %s", command,file_name);
	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	unsigned int remote_length = sizeof(remote);	
	nbytes = sendto(sock,command,strlen(command),0,(struct sockaddr *)&remote, sizeof(remote));
	int ret = strcmp(command,"get");
	if (ret == 0){
		nbytes = sendto(sock,file_name,strlen(file_name),0,(struct sockaddr *)&remote, sizeof(remote));
		struct sockaddr_in from_addr;
		int addr_length = sizeof(struct sockaddr);
		bzero(buffer, sizeof(buffer));
		nbytes = recvfrom(sock,size,100*sizeof(char),0,(struct sockaddr *)&remote, &addr_length);
		printf("size of the file to recieve is %s\n",size);
		int Totalsize = atoi(size);
		int NoOfPackets = Totalsize/PACKETSIZE;
		printf("No of packets %d\n",NoOfPackets );
		//for(int i=0;i<NoOfPackets;i++){
		filewrite(file_name,sock,remote,Totalsize); 
		printf("%d\n",nbytes );
		if(nbytes>0){
			printf("The client didnt says \n");
		}
		else{
			printf("The client sent  \n ");
		}
		close(sock);		
	}

}

