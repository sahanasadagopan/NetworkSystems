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
#include <dirent.h>
#define MAXBUFSIZE 100000

/* You will have to modify the program below */

int main (int argc, char * argv[])
{
	char array[]="hooray";


	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];

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
    scanf(" %s %s", command,file_name);
	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	unsigned int remote_length = sizeof(remote);	
	nbytes = sendto(sock,command,strlen(command),0,(struct sockaddr *)&remote, sizeof(remote));
	nbytes = sendto(sock,file_name,strlen(file_name),0,(struct sockaddr *)&remote, sizeof(remote));
	struct sockaddr_in from_addr;
	int addr_length = sizeof(struct sockaddr);
	bzero(buffer, sizeof(buffer));
	int ret;
	ret = strcmp(command,"get");
	if (ret ==0){
		nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr *)&remote, &addr_length);  
		printf("%d\n",nbytes );

	if(nbytes>0){
		printf("The client didnt says \n");
	}
	else{
		printf("The client sent %s \n ",buffer);
		}
	FILE *file;
	printf("%ld\n", sizeof(buffer) );
	file=fopen("foo1_trans2","wb");
	int data= fwrite(buffer,1, sizeof(buffer),file);
	fclose (file);

	close(sock);
	}
	ret=strcmp(command,"put");
	printf("%d\n",ret );
	if(ret == 0){
		FILE *file;
		char *msg;
		size_t result;
		long lSize;
		file = fopen ( file_name , "rb" );
  		if (file==NULL) {fputs ("File error",stderr); exit (1);}
  		fseek (file , 0 , SEEK_END);
  		lSize = ftell (file);
  		rewind (file);
  		msg = (char*) malloc (sizeof(char)*lSize);
  		if (msg == NULL) {fputs ("Memory error",stderr); exit (2);}
 		result = fread (msg,1,lSize,file);
  		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
  		printf("%ld\n",lSize );
		nbytes = sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&remote, sizeof(remote));
		fclose (file);
		free (msg);
		close(sock);
	}
	/*ret = strcmp(command,"ls");
	if (ret == 0){	
		size_t i = 0, j;
  		size_t size = 1;
	 	char msg[100];
	 	int count=0;
	 	char **names,**tmp;
	    DIR *directory;
  		struct dirent *dir;

  		names = malloc(size * sizeof *names); //Start with 1

  		directory = opendir(".");
  		if (!directory) { puts("opendir failed"); exit(1); }

 		while ((dir = readdir(directory)) != NULL) {
     		names[i]=strdup(dir->d_name);
     		if(!names[i]) { puts("strdup failed."); exit(1); }
     			i++;
     		if (i>=size) { // Double the number of pointers
        		tmp = realloc(names, size*2*sizeof *names );
        		if(!tmp) { puts("realloc failed."); exit(1); }
        		else { names = tmp; size*=2;  }
     		}
  		}
  		closedir(directory); 
  		printf("%s\n",names[0] );
  		printf("%zu\n",strlen(*names) );
  		for(int j=0;j<i;j++){
	    	nbytes = sendto(sock,names[j], 100,0,(struct sockaddr *)&remote, sizeof(remote));  
	    	printf("%s\n",names[j] );
	    }
	    
	    close(sock);
		}*/
}


