/*
    C socket server example
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <stdlib.h>
 


int check_identity(int client_sock){
	char client_message[20];
	recv(client_sock , client_message , 20 , 0);	
	printf("client username: %s\n",client_message );
	recv(client_sock , client_message , 20 , 0);	
	printf("client password%s\n",client_message );
	return 0;

}


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size,packet_size,count=0;
    char size[10];
    struct sockaddr_in server , client;
    char client_message[22133];
    //FILE *fp;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( atoi(argv[1]) );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
        
    //Listen
    while(1){
    	FILE *file;
	    listen(socket_desc , 3);
	     
	    //Accept and incoming connection
	    puts("Waiting for incoming connections...");
	    c = sizeof(struct sockaddr_in);
	     
	    //accept connection from an incoming client
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    if (client_sock < 0)
	    {
	        perror("accept failed");
	        return 1;
	    }
	    puts("Connection accepted");
	    /*******************Check Usernaem and Password*********************/
	    int ret = check_identity(client_sock);
	    read_size=recv(client_sock,client_message,20,0);
	    printf("command %s\n",client_message );
	    /******************PUT command**************************************/
	    if(strcmp(client_message,"PUT")==0){
		    //file = fopen("recieved1","w+");
		    while(count<2){
		    	if(count==0){
		    		char filename[20],path[20];
		    		strcpy(path,"/home/sahana/NetworkSystems/DFS/Server1/");
		    		read_size = recv(client_sock,filename,20,0);
		    		//printf("filename 1 %s\n", filename);
		    		strcat(path,filename);
		    		printf("filename %s\n",path );
		    		file = fopen(path,"w+");
		    		if(file==0){
		    			printf("INVALID FILE %s\n",path);
		    		}
		    		if(read_size = recv(client_sock , size ,10, 0)<0){
				    	printf("recv error\n");
				    	exit(0);
			    	}
				    printf("size recieved%s\n",size );
				    count++;
				    printf("count %d\n",count );
				    packet_size=atoi(size);
				    read_size = recv(client_sock , client_message , packet_size , 0);
				   	printf("The actual data recieved in server 1 %d\n",read_size);
				    if(read_size=fwrite(client_message,packet_size,1,file)==0){
				    	printf("error write\n");
				    }
				    fclose(file);
			    }
		    	if(count == 1){
		    		char filename[20],path[20];
		    		strcpy(path,"/home/sahana/NetworkSystems/DFS/Server1/");
		    		read_size = recv(client_sock,filename,20,0);
		    		strcat(path,filename);
		    		printf("filename 2 %s\n",filename );
		    		file=fopen(path,"w+");
		    		if(read_size = recv(client_sock , size ,10, 0)<0){
				    	printf("recv error\n");
				    	exit(0);
				    }
				    printf("size recieved%s\n",size );
				    count++;
				    printf("count %d\n",count );
				    packet_size=atoi(size);
				    read_size = recv(client_sock , client_message , packet_size , 0);
				   	printf("The actual data recieved in server 1 %d\n",read_size);
				    fwrite(client_message,packet_size,1,file);
				    fclose(file);
		    	}
			    
			}
			
		}
	    

	    if(read_size == 0)
	    {
	        puts("Client disconnected");
	        fflush(stdout);
	    }
	    else if(read_size == -1)
	    {
	        perror("recv failed");
	    }
    }
    return 0;
}
