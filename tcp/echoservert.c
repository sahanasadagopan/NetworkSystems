/* 
 * echoservert.c - A concurrent echo server using threads
 */

//#include "nethelp.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>      /* for fgets */
#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

void echo(int connfd);
void *thread(void *vargp);


int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */

int readline(int fd, char * buf, int maxlen)
{
  int nc, n = 0;
  for(n=0; n < maxlen-1; n++)
    {
      nc = read(fd, &buf[n], 1);
      if( nc <= 0) return nc;
      if(buf[n] == '\n') break;
    }
  buf[n+1] = 0;
  return n+1;
}


int main(int argc, char **argv) {
    int listenfd;
    int  *connfdp, port,clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 
    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while (1) {
	connfdp = malloc(sizeof(int));
	*connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
	pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* thread routine */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}

/*
 * echo - read and echo text lines until client closes connection
 */
void echo(int connfd) 
{
    size_t n; 
    char buf[MAXLINE]; 
    int Packetsize=50000;
    char buffer[Packetsize];
    char originalbuf[MAXLINE];
    char *recieve[3];
    int nbytes;
    while((n = recv(connfd, buf, MAXLINE,0)) != 0) {
        if(n<0){
            fprintf(stderr, "recv() error \n");
        }
        if (n == 0){
            fprintf(stderr, "Client disconnected unexpectedly\n");
        }
        else{
            printf("%s\n",buf);
            strncpy(originalbuf,buf,200);
            recieve[0] = strtok(buf," \t\n");
            //recieve[1]=strtok(recieve[0]," ");
            printf("recv 0 %s\n",recieve[0]);
            if(strncmp(recieve[0],"GET\0",strlen("GET\0"))==0){
                fprintf(stdout, "Request got\n");
                recieve[1] = strtok(NULL," \t");
                recieve[2] = strtok(NULL," \t\n");
                if ( strncmp( recieve[2], "HTTP/1.0", 8)!=0 && strncmp( recieve[2], "HTTP/1.1", 8)!=0 )
                {
                    write(connfd, "HTTP/1.0 400 Bad Request\n", 25);
                }
                else{
                    if ( strncmp(recieve[1], "/\0", 2)==0 )
                        recieve[1] = "apple_ex.png"; 
                        printf("gt here\n");
                }
                //printf("get is %s\n",recieve[1]);

            }
            else{
                write(connfd, "HTTP/1.1 400 Bad Request\n", strlen("HTTP/1.1 400 Bad Request\n"));
                fprintf(stderr, "Invalid request\n");
            }
            printf("original buffer : %s\n",originalbuf );
            

        }
    	printf("server received %d bytes\n", n);
        char buf[1024];
        // sending file in chunks /
        FILE *filee = fopen(recieve[1],"rb+");
        fseek(filee, 0, SEEK_END);
        int fsize = ftell(filee);
        fseek(filee, 0, SEEK_SET);
        int count=0;
        fclose(filee);
        //sprintf(size,"%d",fsize);
        printf("%d\n",fsize );
        int size =100;
        sprintf(buf,"HTTP/1.1 400 Document Follows\r\nContent-Type: image/png\r\nContent-Length: 34180\n\n");
        //strcat(version," 400 Bad Request\n");   
        //write(connfd, "HTTP/1.1 400 Bad Request\n", strlen("HTTP/1.1 400 Bad Request\n"));    
        //write(connfd, "HTTP/1.1 400 Bad Request\n", strlen("HTTP/1.1 400 Bad Request"));
        
        //itoa (strlen("<html><body>400 Bad Request Reason: Invalid Method :<<request method>></body></html>"), status, 10 );
        //strcat(length,status);
        
       // write(connfd, "Content-Type: html\n", 20); 
        //write(connfd,"Content length: 84\n\n",19);
        write(connfd, buf, strlen(buf));
        //write(connfd,"<html><body>400 Bad Request Reason: Invalid Method :<<request method>></body></html>",strlen("<html><body>400 Bad Request Reason: Invalid Method :<<request method>></body></html>"));
        
        FILE *file = fopen(recieve[1],"rb");
                
           
        
       while(fread(buffer,1,Packetsize,file))
        {
            //printf("%s\n",buf );
            count++;
            printf("count %d\n",count );
            //fflush(stdout);
            nbytes = write(connfd, buffer,fsize);
            if(nbytes>0){   
                printf("The server is sending %s\n",buffer);
            }
            else{
                printf("The server could not send %s\n ",buffer);
            }  
        }
    //test.html
    }
}
