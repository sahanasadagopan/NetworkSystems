/*

*
*
*
*/

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


int listenfd, reciever[MAX];
void error(char *);
void Server(char *,int );
void respond(int);

//FILE *conf;
size_t result;
char *conffile,*array[20];
char *ROOT;
int main(int argc, char* argv[])
{
	int lsize,count=1,i,slot=0;;
	char confbuf[BYTES], PORT[60];;
	struct sockaddr_in clientaddr;
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
	// Setting all elements to -1: signifies there is no client connected
	for (i=0; i<MAX; i++)
		reciever[i]=-1;
	Server(PORT,slot);
	// ACCEPT connections
	while (1)
	{
		addrlen = sizeof(clientaddr);
		reciever[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
		if ( fork()==0 )
		{
			respond(slot);
			exit(0);
		}
		while (reciever[slot]!=-1) slot = (slot+1)%MAX;
	}
	fclose(conf);
	return 0;
}

//start server
void Server(char *port,int n)
{
	struct addrinfo hints, *res, *p;

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo( NULL, port, &hints, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}
	// socket and bind

	for (p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (AF_INET, SOCK_STREAM, 0);
		//if (listenfd == -1) continue;
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
	}
	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen (listenfd, 1000000) != 0 )
	{
		perror("listen() error");
		exit(1);
	}
}



//client connection
void respond(int n)
{
	char mesg[100000], *request[5],*putreq[5],*postarray[200],put[100],putsub[10000],data_to_send[BYTES], path[10000],buf[10000];
	int rcvd, fd, bytes_read,i=1;
	int Packetsize=50000,len,length =0;;
    char buffer[Packetsize];
    
	memset( (void*)mesg, (int)'\0', 100000 );
	rcvd=recv(reciever[n], mesg, 100000, 0);
	strcpy(put,mesg);
	strcpy(putsub,put);
	len = strlen(putsub);
	printf("size of header %d\n",len );
	printf("copied message %s\n",put );
	if (rcvd<0)    // receive error
		fprintf(stderr,("recv() error\n"));
	else    // message received
	{
		fprintf(stdout,"%s\n", mesg);
		request[0] = strtok (mesg, " \t\n");
		//strcpy(putreq[0],request[0]);
		//printf("put 1 is the whole%s\n",putreq[0] );
		fprintf(stdout, "request %s\n", request[0]);
		if ( strncmp(request[0], "GET\0", 4)==0 || strncmp(request[0],"POST\0",5)==0)
		{
			request[1] = strtok (NULL, " \t");
			request[2] = strtok (NULL, " \t\n");
			putreq[0] = strtok (put, " \n");

			printf("*****************************%s\n",putreq[0] );
			if(strncmp(putreq[0],"POST\0",5)==0){
				printf("put 3 \n" );
				putreq[1]=strtok(putsub,"\n");
				postarray[0]=putreq[1];
				//putreq[2]=strlen(putreq[0]);
				printf("post aray%s\n",postarray[0] );
				while(putreq[1] != NULL){
					putreq[1]=strtok(NULL,"\n");
					postarray[i]=putreq[1];
					i++;
					printf("put 1 %s\n",postarray[14] );
					//printf("size of postman %s\n",putreq[2] );
				}
				//int k= 13;
				
				for(int k =13;k<17;k++){
					length = length + strlen(postarray[k]);
					printf("content length only%d\n",length );
				}
				//printf("size of postman %s\n",putreq[2] );

			}
			if ( strncmp( request[2], "HTTP/1.0", 8)!=0 && strncmp( request[2], "HTTP/1.1", 8)!=0 )
			{
				write(reciever[n], "HTTP/1.0 400 Bad Request\n", 25);
				write(reciever[n],"<html><body> Bad Request Reason: Invalid Method:<<request method>></body></html>",strlen("<html><body> Bad Request Reason: Invalid Method:<<request method>></body></html>"));
			}
			else
			{
				if ( strncmp(request[1], "/\0", 2)==0 ){
					//if(strstr(array[5],"DirectoryIndex")){
					conffile = strtok(array[5]," ");
					conffile = strtok(NULL,"\n");
					//}
					request[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
					FILE *filee = fopen(conffile,"rb");
				    fseek(filee, 0, SEEK_END);
				    int fsize = ftell(filee);
				    fseek(filee, 0, SEEK_SET);
				    sprintf(buf,"HTTP/1.1 200 Document Follows\r\nContent-Type: text/html\r\nContent-Length: %d\n\n",fsize);//34180
				   
				    write(reciever[n], buf, strlen(buf));                  
				    //FILE *file = fopen("index.html","rb");
				    while(fread(buffer,1,Packetsize,filee))
				    {
				        write(reciever[n], buffer,3391);
				    }
				    fclose(filee);
				}
				else{
					strcpy(path, ROOT);
					strcpy(&path[strlen(ROOT)], request[1]);

					if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
					{
						if(fd == -1){
							write(reciever[n],"HTTP/1.1 404 Not Found\n\n<other-headers>\n\n<html><body>404 Not Found Reason URL doesnt exsist:<<requested url>></body></html>",strlen("HTTP/1.1 404 Not Found\n\n<other-headers>\n\n<html><body>404 Not Found Reason URL doesnt exsist:<<requested url>></body></html>"));
							shutdown (reciever[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
							close(reciever[n]);
							reciever[n]=-1;
						}
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
						sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\n\n",request[5],fsize);
						write(reciever[n], buf, strlen(buf));
						printf("comparing and post req%s\n",putreq[0] );
						int j=13;
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
						//write(reciever[n],"Content-Length: %d \n\n",fsize);
						while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
							write (reciever[n], data_to_send, bytes_read);
					}
					else{
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
							write(reciever[n], "HTTP/1.1 501 Not Implemented\n\n", 30); //FILE NOT FOUND
							write(reciever[n],"<other-headers>\n\n<html><body>501 Not Implemented<<error type>>:<<requested data>></body></html>\n\n",strlen("<other-headers>\n\n<html><body>501 Not Implemented<<error type>>:<<requested data>></body></html>\n\n"));
						}
					}    
				}
			}
		}
	}

	//Closing SOCKET
	shutdown (reciever[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(reciever[n]);
	reciever[n]=-1;
}
