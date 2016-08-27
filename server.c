//
//  server.h
//  project_cs356
//
//  Created by parth patel on 8/26/16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"


typedef enum {false,true} bool;
bool done = false;
bool receiving = false;
int n;
int value;
int routerNum = 1;
char buffer[256];
int yes = 2;
int no = 1;
int initValues[4][3] = {{0, 2, 1},{1,-1,0},{2,1,1},{3}};
int sockfd, newsockfd, portno;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}



void printValues(){
    printf("--------------------------------------\n");
    printf("                Router 1              \n");
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

}

void enterLoop( void* buffer, unsigned long newsockfd){
    bool takeRouterNumber = false;
    int i = 0;

    while(!done){
        bzero((int *) &value, sizeof(value));
        bzero(buffer,256);
        
        n = read(newsockfd, &value, sizeof(value));
        if (n < 0) error("ERROR reading value");

   
        if(sizeof(buffer) > 0) {
            if(i == 3) {printf("\n"); i = 0; }
            
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
                printf("Receiving the intial router values...\n");
                //SENDING RECEIPT OF INITIAL VALUES NOTICE
                n = write(newsockfd, &yes, sizeof(yes));
                if(n < 0)
                    error("ERROR writing back to client");
                continue;
            } else if (value == 20038){
                receiving = false;
                printf("Complete...\n");
                //STORE ROUTER DATA SOMEWHERE.
                //PRINT THE CURRENT VALUES
                printValues();
                done = true;
                continue;
            }else if ((value < -2) || (value > 10)){
                continue;
            }else {
                if(receiving){
                    printf(" %d ",value);
                    n = write(newsockfd, &value, sizeof(value));
                    if(n < 0)
                        error("ERROR writing back to client");
                        i++;
                } else {
                    i = 0;
                }
                continue;
            }
            
            
        };
    }
}


int main(int argc, char *argv[])
{
    

    socklen_t clilen;
    
    
    struct sockaddr_in serv_addr, cli_addr;
    
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    printValues();
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    printf("\n");
    if (sockfd < 0)
        error("ERROR opening socket");
    //resets all the values in the buffer to 0 takes two args, pointer and size of buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //converts string input port number to integer
    portno = atoi(argv[1]);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0) error("ERROR on accept");
    
    

    
    
    
    enterLoop(buffer, newsockfd);


    while(done){
        close(newsockfd);
        close(sockfd);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
        printf("\n");
        if (sockfd < 0)
            error("ERROR opening socket");
        //resets all the values in the buffer to 0 takes two args, pointer and size of buffer
        bzero((char *) &serv_addr, sizeof(serv_addr));
        //converts string input port number to integer
        portno = atoi(argv[1]);
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0)
            continue;
        
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0) error("ERROR on accept");
    
        done = false;
        enterLoop(buffer, newsockfd);
        close(newsockfd);
        close(sockfd);
        bzero((int *) &value, sizeof(value));
    }
   
    
    return 0;
}