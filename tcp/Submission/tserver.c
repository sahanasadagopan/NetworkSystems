/***********************************************************************************************
* Copyright (C) 2017 Sahana Sadagopan
*
**Redistribution,modification or use of this software in source or binary fors is permitted as long 
*as the files maintain this copyright.Sahana Sadagopan is not liable for any misuse of this material
*
*********************************************************************************************************/
/**
*@file tserve.c.c
*@brief tcp server implementation
*
*This C file provides how to implement TCP Server
*@author Sahana Sadagopan
*@date September 2017
*@version 1.0
*
**/
#include "tserver.h"

size_t result;
char *conffile,*array[20],*ROOT;
int listenfd, reciever[MAX];
/**************************************************************************
*   Function - main
*   Parameters - command line inputs just run ./tserver
*   Returns - int
*   Purpose - This opens the conf file handles the parameters for Port and PATH
**************************************************************************/
int main(int argc, char* argv[])
{
	int lsize,count=1,i,slot=0;;
	char confbuf[BYTES], PORT[60];;
	struct sockaddr_in addr;
	socklen_t addrlen;  

	FILE *conf = fopen("ws.conf","rb");
	fseek (conf,0,SEEK_END);
	lsize = ftell(conf);
	rewind(conf);
	result = fread(confbuf,1,lsize,conf);
	conffile=strtok(confbuf,"\n");
	array[0]=conffile;
	while( conffile != NULL ) {
      conffile = strtok(NULL, "\n");
      array[count]=conffile;
      count++;
    }
    if(strstr(array[1],"Listen")){ 
    	conffile = strtok(array[1]," ");
    	conffile = strtok(NULL,"\n");
    }
    
	strcpy(PORT,conffile);
	if(strstr(array[3],"DocumentRoot")){
    	conffile = strtok(array[3]," ");
    	conffile = strtok(NULL,"\n");
    	ROOT = conffile;
    }
	/* Checking for the client connection*/
	for (i=0; i<MAX; i++)
		reciever[i]=-1;
	Server(PORT,slot);
	
	while (1)
	{
		addrlen = sizeof(addr);
		reciever[slot] = accept (listenfd, (struct sockaddr *) &addr, &addrlen);
		if ( fork()==0 )
		{
			file_handler(slot);
			exit(0);
		}
		while (reciever[slot]!=-1) slot = (slot+1)%MAX;
	}
	fclose(conf);
	return 0;
}

/**************************************************************************
*   Function - Server
*   Parameters - PORT and number of clients
*   Returns - void
*   Purpose - THis sents the Connection of the server woith the clients
**************************************************************************/
void Server(char *port,int n)
{
	struct addrinfo info, *res, *child;
	memset (&info, 0, sizeof(info));
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_STREAM;
	info.ai_flags = AI_PASSIVE;
	if (getaddrinfo( NULL, port, &info, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}
	for (child = res; child!=NULL; child=child->ai_next)
	{
		listenfd = socket (AF_INET, SOCK_STREAM, 0);
		if (bind(listenfd, child->ai_addr, child->ai_addrlen) == 0)
		    break;
	}
	freeaddrinfo(res);
	if ( listen (listenfd, 1000000) != 0 )
	{
		perror("listen() error");
		exit(1);
	}
}


/**************************************************************************
*   Function - file_handler
*   Parameters - clients number
*   Returns - void
*   Purpose - This returns the file content when a GET and POST request are made
**************************************************************************/

void file_handler(int n)
{
	char mesg[100000], *request[5],*putreq[5],*postarray[200],put[100],putsub[10000],data_to_send[BYTES], path[10000],buf[10000];
	int rcvd, fd, bytes_read,i=1;
	int Packetsize=50000,len,length =0;;
    char buffer[Packetsize];
    
    /**Recieving the GET or the POST request*/
	rcvd=recv(reciever[n], mesg, 100000, 0);
	strcpy(put,mesg);
	strcpy(putsub,put);
	printf("copied message %s\n",put );
	/*Checking for error*/
	if (rcvd<0)   
		fprintf(stderr,("recv() error\n"));
	/*Checks and breaks the request to process*/
	else    
	{
		fprintf(stdout,"%s\n", mesg);
		request[0] = strtok (mesg, " \t\n");
		fprintf(stdout, "request %s\n", request[0]);
		/*Checks if the request is GET or POST*/
		if ( strncmp(request[0], "GET\0", 4)==0 || strncmp(request[0],"POST\0",5)==0)
		{
			request[1] = strtok (NULL, " \t");
			request[2] = strtok (NULL, " \t\n");
			putreq[0] = strtok (put, " \n");
			/*If POST calculates the POSTDATA size*/
			printf("/*****************************%s*******************/\n",putreq[0] );
			if(strncmp(putreq[0],"POST\0",5)==0){
				printf("put 3 \n" );
				putreq[1]=strtok(putsub,"\n");
				postarray[0]=putreq[1];
				printf("post aray%s\n",postarray[0] );
				while(putreq[1] != NULL){
					putreq[1]=strtok(NULL,"\n");
					postarray[i]=putreq[1];
					i++;
					printf("put 1 %s\n",postarray[14] );
				}
				for(int k =13;k<17;k++){
					length = length + strlen(postarray[k]);
					printf("content length only%d\n",length );
				}
			}
			/* Error Handling if request is bad*/
			if ( strncmp( request[2], "HTTP/1.0", 8)!=0 && strncmp( request[2], "HTTP/1.1", 8)!=0 )
			{
				write(reciever[n], "HTTP/1.0 400 Bad Request\n", 25);
				write(reciever[n],"<html><body> Bad Request Reason: Invalid Method:<<request method>></body></html>",strlen("<html><body> Bad Request Reason: Invalid Method:<<request method>></body></html>"));
			}
			else
			{
				/*Condition for default handling without any request given to the server*/
				if ( strncmp(request[1], "/\0", 2)==0 ){
					conffile = strtok(array[5]," ");
					conffile = strtok(NULL,"\n");
					request[1] = "/index.html";       
					FILE *filee = fopen(conffile,"rb");
				    fseek(filee, 0, SEEK_END);
				    int fsize = ftell(filee);
				    fseek(filee, 0, SEEK_SET);
				    sprintf(buf,"HTTP/1.1 200 Document Follows\r\nContent-Type: text/html\r\nContent-Length: %d\n\n",fsize);//34180
				   
				    write(reciever[n], buf, strlen(buf));                  
				    while(fread(buffer,1,Packetsize,filee))
				    {
				        write(reciever[n], buffer,3391);
				    }
				    fclose(filee);
				}
				else{
					/*If specific file mentioned finds the path and open the file*/
					strcpy(path, ROOT);
					strcpy(&path[strlen(ROOT)], request[1]);

					if ( (fd=open(path, O_RDONLY))!=-1 )    
					{
						/* Handles error if the file isnt found*/
						if(fd == -1){
							write(reciever[n],"HTTP/1.1 404 Not Found\n\n<other-headers>\n\n<html><body>404 Not Found Reason URL doesnt exsist:<<requested url>></body></html>",strlen("HTTP/1.1 404 Not Found\n\n<other-headers>\n\n<html><body>404 Not Found Reason URL doesnt exsist:<<requested url>></body></html>"));
							shutdown (reciever[n], SHUT_RDWR);         
							close(reciever[n]);
							reciever[n]=-1;
						}
						/*Finds the SIze of the file*/
						FILE *fp= fopen(path,"rb");
						fseek(fp, 0, SEEK_END);
					    int fsize = ftell(fp);
					    fseek(fp, 0, SEEK_SET);
					    fclose(fp);
					    printf("%d\n",fsize );
						char *parsing = request[1];
						request[3]=strtok(parsing,".");
						request[4]=strtok(NULL," \t\n");
						char *type = request[4];
						if(strcmp(type,"png")==0){
							if(strstr(array[13],".png")){
								conffile=strtok(array[13]," ");
								conffile=strtok(NULL,"\n");
								//printf("%s\n",conffile );
							}
							request[5] = conffile;
						}
						if(strcmp(type,"jpg")==0){
							if(strstr(array[11],".jpg")){
								conffile=strtok(array[11]," ");
								conffile=strtok(NULL,"\n");
							}
							request[5] = conffile;
						}
								
						if(strcmp(type,"html")==0){
							if(strstr(array[7],".html")){
								conffile=strtok(array[7]," ");
								conffile=strtok(NULL,"\n");
							}
							request[5] = conffile;
						}
								
						if(strcmp(type,"txt")==0){
							if(strstr(array[9],".txt")){
								conffile=strtok(array[9]," ");
								conffile=strtok(NULL,"\n");
								//printf("%s\n",conffile );
							}
							request[5] = conffile;
						}
								
						if(strcmp(type,"htm")==0){
							if(strstr(array[8],".htm")){
								conffile=strtok(array[8]," ");
								conffile=strtok(NULL,"\n");
								//printf("%s\n",conffile );
							}
							request[5] = conffile;
						}
								
						if(strcmp(type,"gif")==0){
							if(strstr(array[10],".gif")){
								conffile=strtok(array[10]," ");
								conffile=strtok(NULL,"\n");
								//printf("%s\n",conffile );
							}
							request[5] = conffile;
						}
						/*Sends the Get response and the file to the client*/
						sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\n\n",request[5],fsize);
						write(reciever[n], buf, strlen(buf));
						printf("comparing and post req%s\n",putreq[0] );
						int j=13;
						/*Sends the client response to the client*/
						if(strncmp(putreq[0],"POST\0",5)==0){
							write(reciever[n],"<html><body><pre><h1>",strlen("<html><body><pre><h1>"));
							while(j<(i-1)){
								sprintf(buf,"%s",postarray[j]);
								write(reciever[n],buf,strlen(buf));
								printf("%zu\n",strlen(buf));
								printf("set header part1%s\n",postarray[j]);
								j++;
							}
							write(reciever[n],"</h1></pre></body></html>\n\n",strlen("</h1></pre></body></html>\n\n"));
						}
						while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
							write (reciever[n], data_to_send, bytes_read);
					}
					else{
						/*If only a plain POST request is handled*/
						if(strncmp(putreq[0],"POST\0",5)==0){
							len = 46+length;
							sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\n\n",len);
							write(reciever[n], buf, strlen(buf));
							printf("no of times go into loop %d\n",i );
							int j=13;
							write(reciever[n],"<html><body><pre><h1>",strlen("<html><body><pre><h1>"));
							while(j<(i-1)){
								sprintf(buf,"%s",postarray[j]);
								write(reciever[n],buf,strlen(buf));
								printf("%zu\n",strlen(buf));
								printf("set header part1%s\n",postarray[j]);
								j++;
							}
							write(reciever[n],"</h1></pre></body></html>\n\n",strlen("</h1></pre></body></html>\n\n"));
						}else{
							/*None of the methods above are use to impplement error handling*/
							write(reciever[n], "HTTP/1.1 501 Not Implemented\n\n", 30); 
							write(reciever[n],"<other-headers>\n\n<html><body>501 Not Implemented<<error type>>:<<requested data>></body></html>\n\n",strlen("<other-headers>\n\n<html><body>501 Not Implemented<<error type>>:<<requested data>></body></html>\n\n"));
						}
					}    
				}
			}
		}
	}
	/*Stop sending or recieving any data and return 0 for pending request*/
	int closing = shutdown (reciever[n], SHUT_RDWR);  
	if(closing <0){
		fprintf(stderr, "shutdown() error:%d\n",closing);
	}
	close(reciever[n]);
	/*completing that request and setting to -1*/
	reciever[n]=-1;
}
