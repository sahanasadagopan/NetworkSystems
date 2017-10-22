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

#define CONNMAX 1000
#define BYTES 1024


int listenfd, clients[CONNMAX];
void error(char *);
void startServer(char *);
void respond(int);

//FILE *conf;
size_t result;
char *conffile,*array[20];
char *ROOT;
int main(int argc, char* argv[])
{
	int lsize,count=1;
	char confbuf[BYTES];
	struct sockaddr_in clientaddr;
	socklen_t addrlen;  
	FILE *conf = fopen("ws.conf","rb");
	fseek (conf,0,SEEK_END);
	lsize = ftell(conf);
	rewind(conf);
	result = fread(confbuf,1,lsize,conf);
	conffile=strtok(confbuf,"\n");
	array[0]=conffile;
	count=1;
	while( conffile != NULL ) {
      conffile = strtok(NULL, "\n");
      array[count]=conffile;
      count++;
    }
    if(strstr(array[1],"Listen")){ 
    	conffile = strtok(array[1]," ");
    	conffile = strtok(NULL,"\n");
    }
    char PORT[60];
	strcpy(PORT,conffile);
	if(strstr(array[3],"DocumentRoot")){
    	conffile = strtok(array[3]," ");
    	conffile = strtok(NULL,"\n");
    	ROOT = conffile;
    }
	fprintf(stdout,"Server started at port no. %s ",PORT);
	
	int slot=0;
	
	// Setting all elements to -1: signifies there is no client connected
	int i;
	for (i=0; i<CONNMAX; i++)
		clients[i]=-1;
	startServer(PORT);
	// ACCEPT connections
	while (1)
	{
		addrlen = sizeof(clientaddr);
		clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
		if ( fork()==0 )
		{
			respond(slot);
			exit(0);
		}
		while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
	}
	fclose(conf);
	return 0;
}

//start server
void startServer(char *port)
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
		if (listenfd == -1) continue;
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
	}
	if (p==NULL)
	{
		perror ("socket() or bind()");
		exit(1);
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
	char mesg[99999], *reqline[5], data_to_send[BYTES], path[99999],buf[8192];
	int rcvd, fd, bytes_read,lsize;
	int Packetsize=50000,nbytes;
	size_t readbuf;
    char buffer[Packetsize];
    
	memset( (void*)mesg, (int)'\0', 99999 );
	rcvd=recv(clients[n], mesg, 99999, 0);

	if (rcvd<0)    // receive error
		fprintf(stderr,("recv() error\n"));
	else    // message received
	{
		fprintf(stdout,"%s", mesg);
		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 ){
					reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
					FILE *filee = fopen("index.html","rb");
				    fseek(filee, 0, SEEK_END);
				    int fsize = ftell(filee);
				    fseek(filee, 0, SEEK_SET);
				    int count=0;
				    fclose(filee);
				    sprintf(buf,"HTTP/1.1 200 Document Follows\r\nContent-Type: text/html\r\nContent-Length: 3391\n\n");//34180
				   
				    write(clients[n], buf, strlen(buf));                  
				    FILE *file = fopen("index.html","rb");
				    while(fread(buffer,1,Packetsize,file))
				    {
				        nbytes = write(clients[n], buffer,3391);
				    }
				}
				else{
					strcpy(path, ROOT);
				strcpy(&path[strlen(ROOT)], reqline[1]);

				if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
				{
					FILE *fp= fopen(path,"rb");
					fseek(fp, 0, SEEK_END);
				    int fsize = ftell(fp);
				    fseek(fp, 0, SEEK_SET);
				    fclose(fp);
				    char content_type;
					char *parsing = reqline[1];
					reqline[3]=strtok(parsing,".");
					reqline[4]=strtok(NULL," \t\n");
					char *type = reqline[4];
					if(strcmp(type,"png")==0){
						if(strstr(array[13],".png")){
							conffile=strtok(array[13]," ");
							conffile=strtok(NULL,"\n");
							//printf("%s\n",conffile );
						}
						reqline[5] = conffile;
					}
					if(strcmp(type,"jpg")==0){
						if(strstr(array[11],".jpg")){
							conffile=strtok(array[11]," ");
							conffile=strtok(NULL,"\n");
						}
						reqline[5] = conffile;
					}
							
					if(strcmp(type,"html")==0){
						if(strstr(array[7],".html")){
							conffile=strtok(array[7]," ");
							conffile=strtok(NULL,"\n");
						}
						reqline[5] = conffile;
					}
							
					if(strcmp(type,"txt")==0){
						if(strstr(array[9],".txt")){
							conffile=strtok(array[9]," ");
							conffile=strtok(NULL,"\n");
							//printf("%s\n",conffile );
						}
						reqline[5] = conffile;
					}
							
					if(strcmp(type,"htm")==0){
						if(strstr(array[8],".htm")){
							conffile=strtok(array[8]," ");
							conffile=strtok(NULL,"\n");
							//printf("%s\n",conffile );
						}
						reqline[5] = conffile;
					}
							
					if(strcmp(type,"gif")==0){
						if(strstr(array[10],".gif")){
							conffile=strtok(array[10]," ");
							conffile=strtok(NULL,"\n");
							//printf("%s\n",conffile );
						}
						reqline[5] = conffile;
					}
					sprintf(buf,"HTTP/1.1 200 Document Follows\r\nContent-Type: %s\r\nContent-Length: %d\n\n",reqline[5],fsize);
					write(clients[n], buf, strlen(buf));
					write(clients[n],"Content-Length: %d \n\n",fsize);
					while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
						write (clients[n], data_to_send, bytes_read);
				}
				else    write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND

				}
			}
		}
	}

	//Closing SOCKET
	shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(clients[n]);
	clients[n]=-1;
}
