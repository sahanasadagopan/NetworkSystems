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
#include <time.h>
#include <dirent.h>

#define PACKETSIZE 1000

void findfilesize(char file_name[20],char size[20]){
	FILE *f = fopen(file_name, "rb+");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(size,"%lu",fsize);
	printf("%s\n",size );
}

int filereadsend(char file_name[20],int sock,struct sockaddr_in remote,int TotalSize){
	int sentdata=0;
	int nbytes;
	char msg[PACKETSIZE];
	unsigned int remote_length;
	int count=0;
	remote_length = sizeof(remote);
	int Packetsize=1000;
	printf("Packetsize%d\n",Packetsize );
	FILE *file = fopen(file_name,"rb+");
	char buffer[Packetsize];
	int Packetleft =  TotalSize;
	char seq[20];
	char s[10]=",";
	char *token;
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
		nbytes = recvfrom(sock,seq,20,0,(struct sockaddr*)&remote, &remote_length);
		printf("Acknowledgment packet %s\n",seq );
		token = strtok(seq,",");
		printf("The ack only %s\n",token );
		int i=2;
		while(i<2){
			token = strtok(NULL,s);
			printf("The sequence number of the packet is:%s\n",token );
			i--;
		}
	}
	fclose(file);
	return count;
}

int filewrite(char file_name[20],int sock,struct sockaddr_in remote,int Totalsize){
	int addr_length = sizeof(struct sockaddr);
	int nbytes;
	char buffer[PACKETSIZE];
	FILE *file;
	file = fopen("foo1_sent","wb+");
	int recievedpack = 0;
	int bytesleft=Totalsize;
	int datasize =1000;
	int count=0;
	char seq[20];
	while(bytesleft!= 0){
		nbytes = recvfrom(sock,buffer,datasize,0,(struct sockaddr*)&remote, &addr_length);
		printf("%s\n",buffer );
		for(int i=0;i<5000000;i++);
		int data = fwrite(buffer,1,datasize,file);
		count++;
		sprintf(seq,"%d",count);
		char ack[20]="ack";
		strcat(ack,",");
		strcat(ack,seq);
		nbytes = sendto(sock,ack,20,0,(struct sockaddr *)&remote, addr_length);
		printf("%s\n",ack );
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

int main (int argc, char * argv[])
{
	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[PACKETSIZE];
	char *size;
	size = (char*) malloc(20*sizeof(char));
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
	}
	char command[20];
	char file_name[20];
	printf("Pick one of the commands\n");
	printf("get[file_name]\n");
	printf("put[file_name]\n");
	printf("delete[file_name]\n");
	printf("ls\n");
	printf("exit\n");
	printf("Type your command and file name ");
    scanf(" %s", command);
    int ret = strcmp(command,"get");
    if(ret ==0) {
    	scanf("%s",file_name);
    }
    ret = strcmp(command,"put");
    if(ret == 0){
    	scanf("%s",file_name);
    }
    ret = strcmp(command,"delete");
    if(ret == 0){
    	scanf("%s",file_name);
    }
    
	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	unsigned int remote_length = sizeof(remote);	
	nbytes = sendto(sock,command,strlen(command),0,(struct sockaddr *)&remote, sizeof(remote));
	ret = strcmp(command,"get");
	if (ret == 0){
		nbytes = sendto(sock,file_name,20,0,(struct sockaddr *)&remote, sizeof(remote));
		struct sockaddr_in from_addr;
		int addr_length = sizeof(struct sockaddr);
		bzero(buffer, sizeof(buffer));
		nbytes = recvfrom(sock,size,20,0,(struct sockaddr *)&remote, &addr_length);
		printf("size of the file to recieve is %s\n",size);
		int Totalsize = atoi(size);
		int NoOfPackets = Totalsize/PACKETSIZE;
		printf("No of packets %d\n",NoOfPackets );
		int Packetreceived = filewrite(file_name,sock,remote,Totalsize); 
		/*if(Packetreceived < NoOfPackets){
			filewrite(file_name,sock,remote,Totalsize);
		}
		else{
			nbytes = sendto(sock,"ack",strlen("ack"),0,(struct sockaddr *)&remote, sizeof(remote));
		}*/
		printf("Packet received %d\n",Packetreceived );
		printf("%d\n",nbytes );
		close(sock);		
	}
	ret = strcmp(command,"put");
	if (ret == 0){
		int addr_length = sizeof(struct sockaddr);
		nbytes = sendto(sock,file_name,strlen(file_name),0,(struct sockaddr *)&remote, sizeof(remote));
		findfilesize(file_name,size);
		nbytes = sendto(sock,size,strlen(size),0,(struct sockaddr *)&remote, addr_length);
		int Totalsize  = atoi(size);
		int NoOfPackets = Totalsize/PACKETSIZE;
		printf("Size to put%d\n",Totalsize);
		int packetsent=filereadsend(file_name,sock,remote,Totalsize);
		printf("The packets sent %d\n",packetsent);
		if (packetsent<NoOfPackets){
			filereadsend(file_name,sock,remote,Totalsize);
		}
		close(sock);
	}
	ret = strcmp(command , "ls");
	if(ret == 0){
		int addr_length = sizeof(struct sockaddr);
		nbytes = recvfrom(sock,size,100*sizeof(char),0,(struct sockaddr *)&remote, &addr_length);
		printf("%s\n",size );
		int NoOffiles =  atoi(size);
		while(NoOffiles != 0){
			nbytes = recvfrom(sock,buffer,100*sizeof(char),0,(struct sockaddr*)&remote, &addr_length);
			printf("%s\n", buffer);
			NoOffiles--;
		}
		close(sock);
	}
	ret = strcmp(command,"delete");
	if(ret == 0){
		char *closing;
		nbytes = sendto(sock,file_name,20,0,(struct sockaddr *)&remote, sizeof(remote));
		close(sock);
	}
}

