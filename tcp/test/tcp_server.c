#include <sys/socket.h>
#include <netinet/in.h>

#ifndef ERESTART
#define ERESTART EINTR
#endif


void main(int argc, char *argv[]){
	struct sockaddr_in myaddr;
	int fd;
	char hostname[128]; /* host name, for debugging */
	struct sockaddr_in client_addr;  /* client's address */
	gethostname(hostname, 128);

/* Creating the socket for tcp/ip connection*/
	if((fd = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("cannot create socket");
		return 0;
	}
	printf("Created Socket %d\n",fd );
	memset((char *)&myaddr,0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s__addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(80);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0){
		perror("bind failed");
		return 0;
	}

	alen = sizeof(myaddr);
	if (getsockname(fd, (struct sockaddr *)&myaddr, &alen) < 0) {
		perror("getsockname failed");
		return 0;
	}

	printf("bind complete. Port number = %d\n", ntohs(myaddr.sin_port));
	if(listen(fd,10)<0){
		perror("listen failed");
		exit(1);
	}
	printf("Server started on %s \n",hostname );
	alen = sizeof(client_addr);

	for(;;){
		while ((rqst = accept(svc,(struct sockaddr *)&client_addr,&alen))<0){

			if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
				perror("accept failed");
				exit(1);
			}
		}

		printf("received a connection from: %s port %d\n",
			//inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        	//shutdown(rqst, 2);    /* close the connection */

		}

}

