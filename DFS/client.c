/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include "client.h"

//global//
char *parts[100];
char *broken;
int create_sock(int n,char* port){
    printf("value of port connecting%s\n",port );
    int sock[4];
    struct sockaddr_in server;
    
     
    sock[n] = socket(AF_INET , SOCK_STREAM , 0);
    if (sock[n] == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(port) );
   
    //Connect to remote server
    if (connect(sock[n] , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        printf("%s\n",port );
        return 0;
    }
    puts("Connected\n");
    printf("port that was connected %s\n",port);
    return sock[n];

}

int findfilesize(char file_name[20]){
    int lsize=0;
   FILE *filename=fopen(file_name,"r");
   fseek (filename,0,SEEK_END);
   lsize = ftell(filename);
   rewind(filename);
   printf("full size%d\n",lsize );
   return lsize;
}

check_t getcommand(char filename[20],char command[20]){
    printf("Please enter the command\n");
    scanf("%s",command);
    int ret=strcmp(command,"PUT");
    if(ret == 0){
        scanf("%s",filename);
        printf("%s\n",filename );
    }
    ret = strcmp(command,"GET");
    if(ret == 0){
        scanf("%s",filename);
        printf("%s\n",filename );
    }
    return SUCCESS;
}

check_t check_identity(int sock){
    char conf_file[200],username[20],password[20];
    int size,i=0;
    FILE* conf=fopen("dfc.conf","r");
    size=findfilesize("dfc.conf");
    fread(conf_file,size,1,conf);
    broken=strtok(conf_file,"\n");
    parts[0]=broken;
    while(broken != NULL){
        broken=strtok(NULL,"\n");
        printf("%s\n",parts[i] );
        i++; 
        parts[i]=broken;

    }
    parts[4]=strtok(parts[4]," ");
    printf("%s\n",parts[4] );
    while(parts[4]!=NULL){
        parts[4]=strtok(NULL," \n");  
        parts[i]=parts[4]; 
        i++;
    }
    parts[5]=strtok(parts[5]," ");
    while(parts[5]!=NULL){
        parts[5]=strtok(NULL," \n");
        parts[i]=parts[5];
        i++;
    }
    printf("%s\n",parts[7] );
    printf("%s\n",parts[9] );
    fclose(conf); 
    if(send(sock,parts[7],20,0)<0){
        perror("Error in sending username\n");
    }
    if(send(sock,parts[9],20,0)<0){
        perror("Error in sending username\n");
    }
    return SUCCESS;
}

check_t send_command(int sock,char command[20]){
    printf("command is%s.\n",command );
    if(send(sock,command,20,0)<0){
        perror("Error in sending command\n");
    }
    return SUCCESS;
}

check_t send_filename(int sock, char filename[20],char number[2]){
    char serverfilename[20];
    strcpy(serverfilename,filename);
    strcat(serverfilename,number);
    printf("%s\n",serverfilename);
    if(send(sock,serverfilename,20,0)<0){
        printf("Error in sending the name\n");
    }
    return SUCCESS;
}

int main(int argc , char *argv[])
{
    int sock[4];
    char message[100000] , server_reply[2000],command[20],filename[20],size[10];
    FILE *fp; 
    int size_divide,count=0,fullsize,packet_size,lsize=0,sizesent;

    char* port1=argv[1];
    char* port2=argv[2];
    char* port3= argv[3];
    char* port4= argv[4];
    sock[0]=create_sock(0,port1);    
    sock[1]=create_sock(1,port2);
    sock[2]=create_sock(2,port3);    
    sock[3]=create_sock(3,port4);
    
    int ret=check_identity(sock[0]);
    ret=check_identity(sock[1]);
    ret=check_identity(sock[2]);
    ret=check_identity(sock[3]);

    //printf("%d\n",ret );
    printf("got in main\n");
    getcommand(filename,command);

    send_command(sock[0],command);
    send_command(sock[1],command);
    send_command(sock[2],command);
    send_command(sock[3],command);
    
    if(strcmp(command,"PUT")==0){
        fp = fopen(filename,"r");
        if(fp==NULL){
        	perror("file open error\n");
        	exit(0);
        }
        lsize=findfilesize(filename);
        size_divide=lsize/4;
        printf("%d\n",size_divide );
        packet_size=size_divide;
        
        while(fullsize<lsize){
        	printf("I sent %d\n",count );
        	if(count>2){
            	size_divide=lsize-3*(size_divide);
                packet_size=size_divide;
                printf("%d\n",size_divide);
            }
        	fread(message,packet_size,1,fp);
        //Send some data
            sizesent=size_divide;
            sprintf(size,"%d",sizesent);

            if(count==0||count ==1){
                //Send filename
                if(count == 0){
                    send_filename(sock[0],filename,".1");
                }
                if(count == 1){
                    send_filename(sock[0],filename,".2");
                }
                if(send(sock[0] ,size ,10 , 0)<0){
                    printf("Error in send in 0 nd 1\n");
                    exit(0);
                }
                printf("size sent %s\n",size );   
                if( send(sock[0] , message ,size_divide , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }
            }

            if(count==3||count==0){
                if(count == 3){
                    send_filename(sock[3],filename,".4");
                }
                if(count == 0){
                    send_filename(sock[3],filename,".1");
                }
               if(send(sock[3] ,size ,10 , 0)<0){
                printf("Error in send in 3 and 0\n");
                exit(0);
                }
                printf("size sent %s\n",size );
                
                
                if( send(sock[3] , message ,size_divide , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                } 
            }

            if(count==1||count==2){
                if(count == 1){
                    send_filename(sock[1],filename,".2");
                }
                if(count == 2){
                    send_filename(sock[1],filename,".3");
                }
                if(send(sock[1] ,size ,10 , 0)<0){
                printf("Error in send in 1 nd 2\n");
                exit(0);
                }
                printf("size sent %s\n",size );
                
                
                if( send(sock[1] , message ,size_divide , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }    
            }
            if(count==2||count==3){
                if(count == 2){
                    send_filename(sock[2],filename,".3");
                }
                if(count == 3){
                    send_filename(sock[2],filename,".4");
                }
               if(send(sock[2] ,size ,10 , 0)<0){
                printf("Error in send in 3 nd 2\n");
                exit(0);
                }
                printf("size sent %s\n",size );
                
                
                if( send(sock[2] , message ,size_divide , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                } 
            }
            
            //fprintf(stdout, "%s\n",message );
            fullsize+=packet_size;
            printf("I sent %d\n",fullsize );
            count +=1;
        }
    }
    if(strcmp(command,"GET")==0){
           printf("Put command\n"); 
    }
    if(strcmp(command,"LIST")==0){
        printf("List command\n");


    }
     
    close(sock[0]);
    //close(sock[1]);

    return 0;
}