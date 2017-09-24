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
#include <dirent.h>

#define MAXBUFSIZE 1000

void findfilesize(char file_name[20],char size[20]){
	FILE *f = fopen(file_name, "rb+");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(size,"%lu",fsize);
	printf("%s\n",size );
}

int filewrite(char file_name[20],int sock,struct sockaddr_in remote,int Totalsize){
	int addr_length = sizeof(struct sockaddr);
	int nbytes;
	char buffer[MAXBUFSIZE];
	FILE *file;
	file = fopen("foo1_client","wb+");
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
	return recievedpack;
}

int filereadsend(char file_name[20],int sock,struct sockaddr_in remote,int TotalSize){
	int sentdata=0;
	int nbytes;
	char msg[MAXBUFSIZE];
	unsigned int remote_length;
	int count=0;
	remote_length = sizeof(remote);
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
	return count;
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
		printf("No of packets to send %d\n",NoOfPackets );
		int Packetsent=filereadsend(file_name,sock,remote,TotalSize);
		printf("The number of packet sent %d \n",Packetsent);
 		if((Packetsent-1)<NoOfPackets){
 			filereadsend(file_name,sock,remote,TotalSize);
 		}
 		else{
 			nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr *)&remote, &remote_length);
 		}
		close(sock);
	}
	ret = strcmp(buffer,"put");
	if(ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
		if(nbytes>0){
			printf("The client sent %s \n",buffer);
			printf("The file name is %s\n",file_name );
		}
		else{
			printf("The client sent \n ");
		}
		nbytes = recvfrom(sock,size,100*sizeof(char),0,(struct sockaddr *)&remote, &remote_length);
		printf("size of the file to recieve is %s\n",size);
		int Totalsize = atoi(size);
		int NoOfPackets = Totalsize/MAXBUFSIZE;
		printf("No of packets %d\n",NoOfPackets );
		int packet_recieved=filewrite(file_name,sock,remote,Totalsize);
		printf("THe packets got %d\n",packet_recieved );
		if (packet_recieved < NoOfPackets ){
			filewrite(file_name,sock,remote,Totalsize);
		}
		else{
			char *acknowledgment;
			nbytes = recvfrom(sock,acknowledgment,strlen(acknowledgment),0,(struct sockaddr *)&remote, &remote_length);
		}
		close(sock);
	}
	ret = strcmp(buffer , "ls");
	if (ret == 0){
		int count=0;
		DIR *directory,*folder;
		struct dirent *dir,*fir;
		char *filelist;
		char size[20];
		directory = opendir(".");
		if (!directory) { puts("opendir failed"); exit(1); }
		while ((dir = readdir (directory)) != NULL) {
    		count++;
  		}
  		sprintf(size,"%d",count);
  		nbytes = sendto(sock,size,20,0,(struct sockaddr *)&remote, remote_length);
 		closedir(directory);
 		folder = opendir(".");
		if (!folder) { puts("opendir failed"); exit(1); }
		while ((fir = readdir (folder)) != NULL) {
    		printf ("%s\n", fir->d_name);
    		filelist = fir->d_name;
    		nbytes = sendto(sock,filelist,20,0,(struct sockaddr *)&remote, remote_length);
    		for (int i=0;i<10;i++);
    		count++;
  		}
  		closedir(folder);
  		close(sock);
	}
	ret = strcmp(buffer,"delete");
	if (ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
		int deleteditem=0;
		printf("%s\n",file_name );
		//deleteditem = remove(file_name);
		//printf("deleted %d\n",deleteditem );
		if(remove(file_name) == 0)
			printf("Successfully deleted\n");
		else
			printf("Unable to delete or doesnt exsist\n");
		close(sock);
	}
	ret = strcmp(buffer,"exit");
	if (ret == 0){
		char *closing;
		int terminate=close(sock);
		if(terminate == 0){
			printf("server has exited %d\n",terminate );
		}
		else
			printf("cannot exit  %d\n",terminate);
	}

}

