#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define xor "1000001110000"
#define maxsize  1179564
int printtimes(double times[],int n) //print the times for the first part
{
    printf("time taken for first parts: \n");
    for(int k = 0; k < n; k++)
    {
        printf("%f  ",times[k]);
    }
    printf("\n");
    return 0;
}

int printtimes2(double times[],int n) //print the times for the second part
{
    printf("time taken for second parts: \n");
    for(int k = 0; k < n; k++)
    {
        printf("%f  ",times[k]);
    }
    printf("\n");
    return 0;
}

int avgtimes(double times1[],double times2[],int n,int m) //print the average time for all parts sent
{
    double sum = 0;
    for(int k = 0; k < n; k++)
    {
        sum+= times1[k];
    }
    for(int k = 0; k < m; k++)
    {
        sum+= times2[k];
    }
    sum = sum / (n + m);
    printf("avg time for all sends: %f",sum);
    return 0;
}

int write_file(int sock,double timeparta[20],int i) //receive the first part sent by the sender and write it into a file in order to test
{
   
    int n = 0;
    
    FILE *fp;
    char* filename = "writethis.txt";
    char buffer[maxsize];
    fp = fopen(filename,"w");
    if(fp == NULL)
    {
        printf("error in creating file");
        exit(1);
    }
     clock_t start, end;
     double cpu_time_used;
    while(n < maxsize / 2)
    {      
       start = clock();
        n += recv(sock,buffer,maxsize / 2,0);
        if(n == -1)
        {
           printf("error");
        }
        if(n == 0)
        {
            
            return 1;
        }
       end = clock();
       cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
       timeparta[i] = cpu_time_used;
       
        fprintf(fp, "%s", buffer);
    }   
    bzero(buffer,maxsize / 2);
    return 0;
}
int send_file(int sock,char* filecontent ,int len) //send function for authentication
{
    
    int sent = send(sock,filecontent,len,0);
    if(sent == 0)
        printf("authentication sent failed\n");
    else
        printf("authentication sent successfully\n");
    
    return sent;
}
int write_file2(int sock,double timepartb[20],int j) //receive the second part sent by the sender and write it into a file in order to test
{
    
    int n = 0;
    FILE *fp;
    char* filename = "writethis.txt";
    char buffer[maxsize];
    fp = fopen(filename,"w");
    if(fp == NULL)
    {
        printf("error in creating file");
        exit(1);
    }
     clock_t start, end;
     double cpu_time_used;   
    while(n < maxsize / 2)
    {      
       start = clock();
        n += recv(sock,&buffer[maxsize / 2],maxsize/2,0);
        if(n == -1)
        {
           printf("error");
        }
       end = clock();
       
       cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
       timepartb[j] = cpu_time_used;
        fprintf(fp, "%s", buffer);
    }
    bzero(&buffer[maxsize / 2] ,maxsize / 2);
    return 0;
}
int main() {
    char Receiver_massege[14] = xor; //holds the authentication message
   
    double timepartb[20]; //will hold the times that it took to send the second parts
    int j = 0; //the amount of times we sent the second part
    int i =0; //the amount of times we sent the first part
    double timeparta[20]; //will hold the times that it took to send the first parts
    //create receiver socket
    int receiver_socket = socket(AF_INET,SOCK_STREAM,0);

    //define the receiver address
    struct sockaddr_in server_address,client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to our IP and port
    bind(receiver_socket, (struct sockaddr*) &server_address,sizeof(server_address));
    listen(receiver_socket,2);
    
    //create another connection to send from receiver to sender
    socklen_t ca_size = sizeof(client_address);
    int sender_socket = accept(receiver_socket,(struct sockaddr *)&client_address,&ca_size);
    
    //receiving the parts of the file
    while(1){
     printf("waiting to receive part 1...\n");
     if(write_file(sender_socket,timeparta,i) != 1){ //receive first part
        
     }
     else{       
            printtimes(timeparta,i);
            printtimes2(timepartb,j);
            avgtimes(timeparta,timepartb,i,j);
            close(receiver_socket);
        break;
     }
     i++;
     printf("sending authentication...\n");
     int auth  = send_file(sender_socket,Receiver_massege,sizeof(Receiver_massege)); //send authentication
     char *Reno = "reno";//type of CC.
         socklen_t Reno_len = strlen(Reno);
        if (setsockopt(receiver_socket, IPPROTO_TCP,TCP_CONGESTION,Reno,Reno_len) != 0)//changing the cc from Cubic to Reno.
            {
                perror("error changing cc algorithm");
                exit(1);
            }
        else{
            printf("changed from cubic to reno!\n");
        }
     printf("receiving part 2...\n");
     write_file2(sender_socket,timepartb,j); //receive second part
     j++;
    }

    return 0;
}