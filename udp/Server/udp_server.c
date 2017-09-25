/***********************************************************************************************
* Copyright (C) 2017 Sahana Sadagopan
*
**Redistribution,modification or use of this software in source or binary fors is permitted as long 
*as the files maintain this copyright.Sahana Sadagopan is not liable for any misuse of this material
*
*********************************************************************************************************/
/**
*@file udp_server.c
*@brief udp_server implementation
*
*This C file provides how to implement UDP Server
*@author Sahana Sadagopan
*@date September 2017
*@version 1.0
*
**/
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
*   Function - filewrite
*   Parameters - char filename, sock, sock structure, total buffer size 
*   Returns - int no of packets recieved
*   Purpose - recieves the data and write it into a file on the client side
**************************************************************************/

int filewrite(char file_name[20],int sock,struct sockaddr_in remote,int Totalsize){
	unsigned int addr_length = sizeof(struct sockaddr);
	char buffer[MAXBUFSIZE];
	char decryptbuff[MAXBUFSIZE];
	FILE *file;
	file = fopen("foo1_client","wb+");
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
		recievedpack = recievedpack+datasize;
		int bytesleft=Totalsize-recievedpack;
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
	return count;
}

/**************************************************************************
*   Function - filereadsend
*   Parameters - file name, socket, struct of sock, buffer size 
*   Returns - int packet sent
*   Purpose - It reads the file request by the client and sends to the server
***********************************************************************/

int filereadsend(char file_name[20],int sock,struct sockaddr_in remote,int TotalSize){
	int sentdata=0;
	int nbytes;
	char seq[20];
	unsigned int remote_length;
	int count=0;
	remote_length = sizeof(remote);
	int Packetsize=1000;
	FILE *file = fopen(file_name,"rb+");
	char buffer[Packetsize];
	int Packetleft =  TotalSize;
	char *token;
	char s[10]=",";
	char encryptedbuff[MAXBUFSIZE];
	while(fread(buffer,1,Packetsize,file)){
		encryptdata(buffer,Packetsize,encryptedbuff);
		nbytes = sendto(sock,encryptedbuff,Packetsize,0,(struct sockaddr *)&remote, remote_length);
		if(nbytes>0){
			printf("The server is sending \n");
		}
		else{
			printf("The server could not send \n ");
		}
		Packetleft = TotalSize - Packetsize;
		TotalSize=Packetleft;
		if(Packetleft<1000){
			Packetsize=Packetleft;
		}
		else{
			Packetsize=1000;
		}
		sentdata=sentdata+nbytes;
		count++;
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

int main (int argc, char * argv[] )
{


	int sock;                           //This will be our socket
	struct sockaddr_in sin,remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE]; 
	char file_name[20];  
	char size[20];
	char *check;
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
	/**
	* This comapares among the list of request that could be sent by the client.
	* if the client sends 'get' then it checks if the file is present
	* If the files isnt present it sends the error to the client and closes
	* the socket.
	* For reliability it checks if all the packets are recieved else it sends the 
	* packets again
	**/
	int ret = strcmp(buffer,"get");
	if(ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote, &remote_length);
		if(access(file_name,R_OK|W_OK|X_OK) != 0){
			printf("File doesnt exist you will get a segmentation fault\n");
			check = "File not found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
		else{
			printf("The file exist in this Folder\n");
			check = "file Found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
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
	/**
	* This compares the request from the client if it is equal to "put" 
	* then it checks if the filename was present in the server. if not throws a
	* error and closes he socket.
	* For reliability checks if all the packets are recieved else tell the client
	* to send them again.
	**/
	ret = strcmp(buffer,"put");
	if(ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
		check = (char *)malloc(20*(sizeof(char)));
		nbytes = recvfrom(sock,check,20,0,(struct sockaddr *)&remote, &remote_length);
		printf("%s\n",check);
		ret = strcmp(check,"File not found");
		if(ret == 0){
			printf("Please type a valid file name\n");
			close(sock);
		}
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
		if ((packet_recieved-1) < NoOfPackets ){
			filewrite(file_name,sock,remote,Totalsize);
		}
		else{
			char *acknowledgment=(char *)malloc(20*sizeof(char));
			nbytes = recvfrom(sock,acknowledgment,strlen(acknowledgment),0,(struct sockaddr *)&remote, &remote_length);
		}
		close(sock);
	}
	/**
	This gets all the file names in the folder from the server.
	**/
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
	/**
	* Delete the files if it exist with the server.
	* if the files are not present in the server folder it returns an
	* error to the client.
	**/
	ret = strcmp(buffer,"delete");
	if (ret == 0){
		nbytes = recvfrom(sock,file_name,20,0,(struct sockaddr *)&remote,&remote_length);
		printf("%s\n",file_name );
		if(access(file_name,R_OK|W_OK|X_OK) != 1){
			printf("The file exist in this Folder\n");
			check = "file Found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
		else{
			printf("File doesnt exist you will get a segmentation fault\n");
			check = "File not found";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
		if(remove(file_name) == 0){
			printf("Successfully deleted\n");
			check = "Deleted Successfully";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
		else{
			printf("Unable to delete or doesnt exsist\n");
			check = "Unable to delete";
			sendto(sock,check,20,0,(struct sockaddr *)&remote, remote_length);
		}
		close(sock);
	}
	/*
	*The server exit and closes the socket automatically expecting the client 
	* to close the socket. It checks if the return value of close(sock) is as 
	* expected else throws an error.
	*/
	ret = strcmp(buffer,"exit");
	if (ret == 0){
		int terminate=close(sock);
		if(terminate == 0){
			printf("server has exited %d\n",terminate );
		}
		else
			printf("cannot exit  %d\n",terminate);
	}

}

