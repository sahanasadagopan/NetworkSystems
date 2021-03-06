/***********************************************************************************************
* Copyright (C) 2017 Sahana Sadagopan
*
**Redistribution,modification or use of this software in source or binary fors is permitted as long 
*as the files maintain this copyright.Sahana Sadagopan is not liable for any misuse of this material
*
*********************************************************************************************************/
/**
*@file udp_client.c
*@brief udp_client implementation
*
*This C file provides how to implement UDP client
*@author Sahana Sadagopan
*@date September 2017
*@version 1.0
*
**/
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

/**************************************************************************
*   Function - findfilesize
*   Parameters - file_name an size 
*   Returns - void
*   Purpose - It calculates the size of the file given the name of the file
**************************************************************************/
void findfilesize(char file_name[20],char size[20]){
	FILE *f = fopen(file_name, "rb+");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	sprintf(size,"%lu",fsize);
	printf("%s\n",size );
}

/**************************************************************************
*   Function - decryptdata
*   Parameters - buffer pointer, datasize, decryptbuffer pointer
*   Returns - void
*   Purpose - The decryption of the encoded buffer
**************************************************************************/

void decryptdata(char *buffer, int datasize, char *decryptbuff){
	char encryptedkey[14] = {'A','B','C','D','E','F','G','H','I','1','2','3','4','5'};
	int j=0;
	for(int i=0;i<datasize;i++){
		decryptbuff[i] = buffer[i] - encryptedkey[j] ;
		j++;
		if(j==13){
			j=0;
		}
	}
}

/**************************************************************************
*   Function - encrypdata
*   Parameters - input buffer pointer, packet size , encrypted buffer 
*   Returns - void
*   Purpose - The functions encrypts the data through ceaser cipher technique
**************************************************************************/

void encryptdata(char *buffer,int Packetsize,char *encryptedbuff){
	char encryptedkey[14] = {'A','B','C','D','E','F','G','H','I','1','2','3','4','5'};
	int j=0;
	for(int i=0;i<Packetsize;i++){
		encryptedbuff[i] = buffer[i] + encryptedkey[j];
		j++;
		if(j==13){
			j=0;
		}
	}
}

/**************************************************************************
*   Function - filereadsend
*   Parameters - file name, socket, struct of sock, buffer size 
*   Returns - int packet sent
*   Purpose - It reads the file request by the client and sends to the server
**************************************************************************/

int filereadsend(char file_name[20],int sock,struct sockaddr_in remote,int TotalSize){
	int sentdata=0;
	int nbytes;
	char encryptedbuff[PACKETSIZE];
	unsigned int remote_length;
	int count=0;
	remote_length = sizeof(remote);
	int Packetsize=1000;
	FILE *file = fopen(file_name,"rb+");
	char buffer[Packetsize];
	int Packetleft =  TotalSize;
	char seq[20];
	char s[10]=",";
	char *token;
	while(fread(buffer,1,Packetsize,file)){
		encryptdata(buffer,Packetsize,encryptedbuff);
		nbytes = sendto(sock,encryptedbuff,Packetsize,0,(struct sockaddr *)&remote, remote_length);
		Packetleft = TotalSize - Packetsize;
		TotalSize=Packetleft;
		if(Packetleft<1000){
			Packetsize=Packetleft;
		}
		else{
			Packetsize=1000;
		}
		count++;
		sentdata=sentdata+nbytes;
		nbytes = recvfrom(sock,seq,20,0,(struct sockaddr*)&remote, &remote_length);
		token = strtok(seq,",");
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

/**************************************************************************
*   Function - filewrite
*   Parameters - char filename, sock, sock structure, total buffer size 
*   Returns - int no of packets recieved
*   Purpose - recieves the data and write it into a file on the client side
**************************************************************************/

int filewrite(char file_name[20],int sock,struct sockaddr_in remote,int Totalsize){
	unsigned int addr_length = sizeof(struct sockaddr);
	char buffer[PACKETSIZE];
	char decryptbuff[PACKETSIZE];
	FILE *file;
	file = fopen("foo1_sent","wb+");
	int recievedpack = 0;
	int bytesleft=Totalsize;
	int datasize =1000;
	int count=0;
	char seq[20];
	while(bytesleft!= 0){
		int nbytes = recvfrom(sock,buffer,datasize,0,(struct sockaddr*)&remote, &addr_length);
		if(nbytes>0){
			printf("The client sent \n");
		}
		else{
			printf("The client did not sent \n ");
		}
		decryptdata(buffer,datasize,decryptbuff);
		fwrite(decryptbuff,1,datasize,file);
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
	char *check;
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
	nbytes = sendto(sock,command,strlen(command),0,(struct sockaddr *)&remote, sizeof(remote));
	/**
	* This compares the command by the client 
	* incase the command is get the function send the filename from the server
	* it returns the check from the server if the file exist.
	* If the file doesnt exist close the socket. Checks if the nbytes are recieved 
	* properly. If the packets aren't recieved properly tells the server to send
	* again
	**/
	ret = strcmp(command,"get");
	if (ret == 0){
		nbytes = sendto(sock,file_name,20,0,(struct sockaddr *)&remote, sizeof(remote));
		unsigned int addr_length = sizeof(struct sockaddr);
		bzero(buffer, sizeof(buffer));
		check = (char *)malloc(20*(sizeof(char)));
		nbytes = recvfrom(sock,check,20,0,(struct sockaddr *)&remote, &addr_length);
		printf("%s\n",check);
		ret = strcmp(check,"File not found");
		if(ret == 0){
			printf("Please type a valid file name\n");
			close(sock);
		}
		nbytes = recvfrom(sock,size,20,0,(struct sockaddr *)&remote, &addr_length);
		printf("size of the file to recieve is %s\n",size);
		int Totalsize = atoi(size);
		int NoOfPackets = Totalsize/PACKETSIZE;
		printf("No of packets %d\n",NoOfPackets );
		int Packetreceived = filewrite(file_name,sock,remote,Totalsize); 
		if(Packetreceived < NoOfPackets){
			filewrite(file_name,sock,remote,Totalsize);
		}
		else{
			nbytes = sendto(sock,"ack",strlen("ack"),0,(struct sockaddr *)&remote, sizeof(remote));
		}
		printf("Packet received %d\n",Packetreceived );
		printf("%d\n",nbytes );
		close(sock);		
	}

	/**
	* This compares the command by the client request.
	* Incase the request by the client is 'put' the following is executed.
	* It sends a file to the server. If the file doesnt exist in the folder
	* it throws an error and closes the socket.
	* It checks the case if the nbytes are sent properly.
	* The reliability is checked if all the packets are not lost then send again
	**/


	ret = strcmp(command,"put");
	if (ret == 0){
		unsigned int addr_length = sizeof(struct sockaddr);
		nbytes = sendto(sock,file_name,strlen(file_name),0,(struct sockaddr *)&remote, sizeof(remote));
		if(access(file_name,R_OK|W_OK|X_OK) != 0){
			printf("File doesnt exist you will get a segmentation fault\n");
			check = "File not found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, addr_length);
		}
		else{
			printf("The file exist in this Folder\n");
			check = "file Found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, addr_length);
		}
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
		else{
			nbytes = sendto(sock,"ack",strlen("ack"),0,(struct sockaddr *)&remote, sizeof(remote));
		}
		close(sock);
	}
	/**
	This gets all the file names in the folder from the server.
	**/
	ret = strcmp(command , "ls");
	if(ret == 0){
		unsigned int addr_length = sizeof(struct sockaddr);
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
	/**
	* Delete the files if it exist with the server.
	* if the files are not present in the server folder it returns an
	* error to the client.
	**/
	ret = strcmp(command,"delete");
	if(ret == 0){
		unsigned int addr_length = sizeof(struct sockaddr);
		nbytes = sendto(sock,file_name,20,0,(struct sockaddr *)&remote, sizeof(remote));
		check = (char *)malloc(20*(sizeof(char)));
		nbytes = recvfrom(sock,check,20,0,(struct sockaddr *)&remote, &addr_length);
		printf("%s\n",check);
		ret = strcmp(check,"File not found");
		if(ret == 0){
			printf("Please type a valid file name\n");
			close(sock);
		}
		nbytes = recvfrom(sock,check,20,0,(struct sockaddr *)&remote, &addr_length);
		ret = strcmp(check,"Deleted Successfully");
		if (ret == 0){
			printf("Deleted the requested file\n");
		}
		else{
			printf("Could not delete the file \n");
		}
		close(sock);
	}
	/*
	*The client exit and closes the socket automatically expecting the server
	* to close the socket
	*/
	ret = strcmp(command,"exit");
	if (ret == 0){
		int terminate=close(sock);
		if(terminate == 0){
			printf("server has exited %d\n",terminate );
		}
		else
			printf("cannot exit  %d\n",terminate);

	}
}

