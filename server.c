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
int n;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void enterLoop( void* buffer,unsigned long newsockfd){
    bool takeRouterNumber = false;
    int clientRouter;
    while(!done){
        int router;
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) {printf("Disconnected"); done = true; continue;};
        
        if(sizeof(buffer) > 0) {
            if(takeRouterNumber) clientRouter = atoi(buffer);
            if(router == 0) {
                printf("\nAM I SUPPOSED TO BE HERE?! \n");
                takeRouterNumber = true;
            }
            router = strcmp(buffer, "router");
            
            printf("%s", buffer);
        };
        
        n = write(newsockfd,"I got your message",18);
        
    }
}


int main(int argc, char *argv[])
{
    
    
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    
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
            error("ERROR on binding");
        
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        

        done = false;
        enterLoop(buffer, newsockfd);

    }
   
    
    return 0;
}