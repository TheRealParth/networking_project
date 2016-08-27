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
int value;
int routerNum = 0;
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
int initValues[4][3] = {{0, -1, 0},{1,0,1},{2,1,3},{3,2,7}};

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void printVal(){
    printf("R       :      I      :      L\n");
    printf("------------------------------\n");
    printf("0       :    Local    :      0\n");
    printf("1       :      0      :      1\n");
    printf("2       :      1      :      3\n");
    printf("3       :      2      :      7\n\n");
}
void give() {
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
    }
    //======CONFIRM DATA RECEIVED
    n = read(sockfd, &value, sizeof(value));
    if (n < 0)
        error("ERROR reading from socket");
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
    printf("\n");
    //=======PRE LAUNCH JARGON ENDS
    
    
    
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
        printf("WTF IS HAPPENING %i", value);
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
    } else {
        error("RETURN VALUE INVALID SHOULD = 2 ");
    }
    
    //======SEND NOTICE OF FINISH
    n = write(sockfd, "FN", 2);
    if (n < 0)
        error("ERROR writing to socket");
    
    
    bzero(buffer,256);
//    n = read(sockfd,buffer,255);
//    if (n < 0)
//        error("ERROR reading from socket");
    
//    while(!done){
//        bzero(buffer,256);
//        n = read(newsockfd, &value, sizeof(value));
//        if (n <= 0) {
//            printf("Disconnected");
//            done = true;
//            continue;
//        };
//        
//        if(sizeof(buffer) > 0) {
//            if(i == 3) {printf("\n"); i = 0; }
//            if (value == 17481) {
//                read(newsockfd, &value, sizeof(value));
//                printf("------ Router %d has connected ------- \n\n", value );
//            }else if((value == 476233) || (value == 30313) ||  (value == 22089)) {
//                receiving = true;
//                printf("Receiving the intial router values...\n");
//                n = write(newsockfd, &value, sizeof(value));
//                if(n < 0)
//                    error("ERROR writing back to client");
//                continue;
//            } else if (value == 20038){
//                receiving = false;
//                printf("Complete...\n");
//                continue;
//            }else if ((value < -2) || (value > 10)){
//                continue;
//            }else {
//                if(receiving){
//                    printf(" %d ",value);
//                    n = write(newsockfd, &value, sizeof(value));
//                    if(n < 0)
//                        error("ERROR writing back to client");
//                    i++;
//                } else {
//                    i = 0;
//                }
//                continue;
//            }
//        };
//    }

    
    close(sockfd);
    return 0;
}