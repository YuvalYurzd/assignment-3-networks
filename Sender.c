#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "netinet/tcp.h"
#define xor "1000001110000"
#define maxsize  1179564

int send_file(int sock,char* filecontent ,int len) //send function
{
    
    int sent = send(sock,filecontent,len,0);
    if(sent == 0) //check if the server accepts requests
        printf("server doesnt accept requests\n");
    else if(sent < len) //check if the data was partly sent
        printf("file was only partly sent (%d/%d bytes).\n",sent,len);
    else{}
    return sent;
}

int main()
{

    
    //create a socket
    int sock = socket(AF_INET,SOCK_STREAM,0);
    
    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9001);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int connection_status = connect(sock,(struct sockaddr *) &server_address, sizeof(server_address));
    //check if there's an error in the connection
    if(connection_status == -1)
    {
        printf("there was an error in creating the connection \n");
    }
    //create the file
    FILE *fp;
    char *filename = "sendthis.txt";
    fp = fopen(filename,"r");
    if(fp == NULL)
    {
        printf("error in reading file");
        exit(1);
    }
    //read the file and calculate file length
    char filecontent[maxsize];
    fread(filecontent,sizeof(char),maxsize,fp);
    fclose(fp);

    char server_response[14]; //will hold the xor recieved from the receiver
    while(1){ //sending the file
    int choose; //an integer that will let the user decide if he wants to send the file again
    if(send_file(sock,filecontent,(maxsize/2)) == maxsize/2)
        printf("first part sent successfully\n");
    else
        printf("failed to send first part\n");
     int k = 0;   
     k = recv(sock,&server_response, sizeof(server_response),0);
    
     if(!strcmp(server_response,xor)){ //check if the xor equals to the authentication sent by receiver
        printf("authentication complete\n");
         char *reno = "reno";//type of CC.
         socklen_t reno_len = strlen(reno);
        if (setsockopt(sock, IPPROTO_TCP,TCP_CONGESTION,reno,reno_len) != 0)//changing the cc from Cubic to Reno.
            {
                perror("error changing cc algorithm");
                exit(1);
            }
        else{
            printf("changed from cubic to reno!\n");
        }
     }
    bzero(server_response,sizeof(server_response)); //clear buffer
    
    if(send_file(sock,(filecontent+(maxsize / 2)),(maxsize/2) + 1) == maxsize/2 + 1)
       printf("second part sent successfully\n");
    else
        printf("failed to send second part\n");
    printf("send file again?\n");
    scanf("%d",&choose);
    if(!choose){
        close(sock);
        return 0;
    }
     char *cubic = "cubic";//type of CC.
        socklen_t cubic_len = strlen(cubic);
        if (setsockopt(sock, IPPROTO_TCP,TCP_CONGESTION,cubic,cubic_len) != 0)//the change in CC from Reno to Cubic
        {
            perror("error changing cc algorithm");
            exit(1);
        }
        else{
            printf("changed from reno to cubic!\n");
        }
    }
    return 0;
}
