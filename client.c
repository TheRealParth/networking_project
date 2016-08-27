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


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{   int value;
    int routerNum = 0;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[256];
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
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("\n");
    //tell the server we're sending the router #
    write(sockfd,"ID",2);
    //send the router number
    write(sockfd, &routerNum, sizeof(routerNum));
    printf("R       :      I      :      L\n");
    printf("------------------------------\n");
    printf("0       :    Local    :      0\n");
    printf("1       :      0      :      1\n");
    printf("2       :      1      :      3\n");
    printf("3       :      2      :      7\n\n");
    
    int initValues[4][3] = {{0, -1, 0},{1,0,1},{2,1,3},{3,2,7}};
    
    //tell the server we're sending the initial values
   n = write(sockfd,"IV",2);
    if (n < 0)
        error("ERROR writing to socket");
    n = read(sockfd, &value, sizeof(value));
    if(n<0)
        error("ERROR reading from socket");
     while((value == 476233) || (value == 30313) ||  (value == 22089)) {
    
         for(int i = 0; i < 4; i++){
             for(int j = 0; j < 3; j++){
                 n = write(sockfd, &initValues[i][j], sizeof(initValues[i][j]));
            if (n < 0)
                     error("ERROR writing to socket");
                 n = read(sockfd, &value, sizeof(value));
            if(n<0)
                error("ERROR reading from socket");
            if(value != initValues[i][j]){
                j--;
            }
            
            }
            printf("\n");
        }
     }
//
//    clock_t start = clock(), diff;
//    diff = clock() - start;
//    
//    int msec = diff * 1000 / CLOCKS_PER_SEC;
//    printf("Time taken %d seconds %d milliseconds", msec/1000, msec%1000);
//    int  testVal= 12;
//    printf("Please enter the message: ");
//    bzero(buffer,256);
//    fgets(buffer,255,stdin);
    
    
//    write(sockfd, &testVal, sizeof(testVal));
//

    
   n = write(sockfd, "FN", 2);
//    n = write(sockfd,&initValues,sizeof(initValues));
    
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,256);
//    n = read(sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    
    close(sockfd);
    return 0;
}