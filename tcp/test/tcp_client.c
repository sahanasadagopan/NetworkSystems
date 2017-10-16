#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>	/* needed for os x*/
#include <string.h>	/* for strlen */
#include <netdb.h>  

void main(int argc, char *argv[]){
	struct sockaddr_in myserver;
	struct sock_in servaddr;

	if((fd = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("cannot create socket");
		return 0;
	}

	memset((char *)&myaddr,0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(80);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr))<0){
		perror("bind failed");
		return 0;
	}

	alen = sizeof(myaddr);
	if(getsockname(fd,(struct sockaddr *)&myaddr,&alen)<0){

		perror("sockname failed");
		return 0;
	}
	printf("local port number = %d", ntohs(myaddr.sin_port));

	memset((char *)&servaddr,0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	hp = gethostbyname(host);

	if(!hp){
		fprintf(stderr,"no address obtained %s",host);
		return 0;
	}

}
