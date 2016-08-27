//
//  client.c
//  project_cs356
//
//  Created by parth patel on 8/26/16.
//
//

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>


typedef enum {false,true} bool;

bool done = false;
bool receiving = false;
int yes = 2;
int no = 1;
int value;
int routerNum = 0;
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
int initValues[4][3] = {{0, -1, 0},{1,0,1},{2,1,3},{3,2,7}};
int router1[3][3];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void printValues(){
    printf("--------------------------------------\n");
    printf("                Router 0              \n");
    printf("--------------------------------------\n");
    printf("     R      :      I      :     L     \n");
    printf("--------------------------------------\n");
    int i = 0;
    while(i < 4){
        if (initValues[i][1] == -1)
            printf("     %i      :    Local    :      %i    \n", initValues[i][0], initValues[i][2]);
        else
            printf("     %i      :      %i      :      %i    \n", initValues[i][0], initValues[i][1], initValues[i][2] );
        i++;
    }
    printf("--------------------------------------\n\n");
}

void printValues1(){
    printf("--------------------------------------\n");
    printf("                Router 1              \n");
    printf("--------------------------------------\n");
    printf("     R      :      I      :     L     \n");
    printf("--------------------------------------\n");
    int i = 0;
    while(i < 3){
        if (router1[i][1] == -1)
            printf("     %i      :    Local    :      %i    \n", router1[i][0], router1[i][2]);
        else
            printf("     %i      :      %i      :      %i    \n", router1[i][0], router1[i][1], router1[i][2] );
        
        i++;
    }
    printf("     3      :     N/A     :     N/A    \n" );
    
    printf("--------------------------------------\n\n");
}
void enterLoop( void* buffer, unsigned long newsockfd){
    bool takeRouterNumber = false;
    int i = 0;
    int j = 0;
    while(!done){
        bzero((int *) &value, sizeof(value));
        bzero(buffer,256);
        
        n = read(newsockfd, &value, sizeof(value));
        if (n < 0) error("ERROR reading value");
        
            if(i == 3) {j++; i=0; }
            
            //=====IF NOTICE IS FOR ROUTER ID
            if (value == 17481) {
                //=====RETURN RECEIPT OF ROUTER ID NOTICE
                n = write(newsockfd, &yes, sizeof(yes));
                if(n < 0)
                    error("ERROR writing back to client");
                //=====READ ROUTER ID
                n = read(newsockfd, &value, sizeof(value));
                if(n < 0)
                    error("ERROR reading value");
                //=====CONFIRM ROUTER ID
                n = write(newsockfd, &value, sizeof(value));
                if(n < 0)
                    error("ERROR writing back to client");
                
                printf("------ Router %d has connected ------- \n\n", value );
                //NOTICE FOR INITIAL VALUES
            }else if((value == 476233) || (value == 30313) ||  (value == 22089)) {
                receiving = true;
                printf("Receiving the intial router values...\n\n");
                //SENDING RECEIPT OF INITIAL VALUES NOTICE
                n = write(newsockfd, &yes, sizeof(yes));
                if(n < 0)
                    error("ERROR writing back to client");
                continue;
                //IF FINISH NOTICE RECEIVED
            } else if (value == 20038){
                receiving = false;
                //SEND RECEIPT OF FINISH
                n = write(newsockfd, &yes, sizeof(yes));
                if(n < 0)
                    error("ERROR writing back to client");
                //PRINT THE CURRENT VALUES
                printValues1();
                
                
                done = true;
                continue;
            } else if ((value < -2) || (value > 10)) {
                
                continue;
            } else {
                if(receiving){
                    
                    router1[j][i] = value;
                    
                    n = write(newsockfd, &value, sizeof(value));
                    if(n < 0)
                        error("ERROR writing back to client");
                    i++;
                } else {
                    i = 0;
                }
                continue;
            }
            
            
        
    }
}



int main(int argc, char *argv[])
{
    
    //=======PRE LAUNCH JARGON
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    printf("\nThis is router 0. \n\n");
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");
    
    //=======PRE LAUNCH JARGON ENDS
    
    printValues();
    
    //======SEND ROUTER ID NOTICE
    n = write(sockfd,"ID",2);
    if (n < 0)
        error("ERROR writing to socket");
    
    //======READ RECEIPT OF ID NOTICE
    n = read(sockfd, &value, sizeof(value));
    if (n < 0)
        error("ERROR reading from socket");
    
    //======SEND ROUTER ID
    if(value == 2){
        n = write(sockfd, &routerNum, sizeof(routerNum));
        if (n < 0)
            error("ERROR writing to socket");
    } else {
        error("RECEIPT OF ID NOTICE SHOULD RETURN 2");
    }
    //======CONFIRM DATA RECEIVED
    n = read(sockfd, &value, sizeof(value));
    if (n < 0)
        error("ERROR reading from socket");
    if (value != routerNum){
        error("NON MATCHING ROUTER ID, value DOES NOT EQUAL routerNum");
    }
    
    //======SEND NOTICE FOR INITIAL VALUES
    n = write(sockfd,"IV",2);
    if (n < 0)
        error("ERROR writing to socket");
    //======READ RECEIPT OF INTIAL VALUES NOTICE
    n = read(sockfd, &value, sizeof(value));
    if(n<0)
        error("ERROR reading from socket");
    
    //======SEND INIT VALUES
    if(value == 2){
         for(int i = 0; i < 4; i++){
             for(int j = 0; j < 3; j++){
                 
                 //WRITE VALUE
                 n = write(sockfd, &initValues[i][j], sizeof(initValues[i][j]));
                 if (n < 0)
                     error("ERROR writing to socket");
                 //CONFIRM VALUE
                 n = read(sockfd, &value, sizeof(value));
                 if(n<0)
                     error("ERROR reading from socket");
                 
            //if return value is not the same, repeat iteration.
            if(value != initValues[i][j]){
                j--;
            }
            }
            
        }
    } else {
        error("RETURN VALUE INVALID SHOULD = 2 ");
    }
    
    //======SEND NOTICE OF FINISH
    n = write(sockfd, "FN", 2);
    if (n < 0)
        error("ERROR writing to socket");
    //======READ RECEIPT OF FINISH
    n = read(sockfd, &value, sizeof(value));
    if (n < 0)
        error("ERROR reading from socket");
    if(value != 2){
        error("RETURN VALUE INVALID SHOULD = 2 ");
    }
    
    
    //SOCKET LISTENER
    enterLoop(buffer, sockfd);
    
    
    bzero(buffer,256);
    close(sockfd);
    return 0;
}