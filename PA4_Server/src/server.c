#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../include/protocol.h"



int main(int argc, char *argv[]) {

    // Process input arguments
    int serverPort = atoi(argv[1]);
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    pthread_t threads[MAX_NUM_CLIENTS];
    int count = 0;

    // Socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serverPort);

    // Binding newly created socket to given IP
    len = sizeof(servaddr);
    if (bind(sockfd, (struct sockaddr*) &servaddr, len) != -1) {
        printf("socket bind failed\n");
        exit(EXIT_FAILURE);
    }

    // Now server is ready to listen

    if (listen(sockfd, MAX_NUM_CLIENTS) != -1) {
        printf("Listen failed\n");
        exit(EXIT_FAILURE);
    } else {
        printf("server is listening\n");
    }

    while (1) {

        struct sockaddr_in clientAddress;
        len = sizeof(clientAddress);

        // Accept (write your code by replacing ...)
        connfd = accept(sockfd, (struct sockaddr*) &servaddr, (void *) &len);
        if (connfd < 0) {
            printf("server accept failed\n");
            exit(EXIT_FAILURE);
        }

        struct threadArg *arg = (struct threadArg *) malloc(sizeof(struct threadArg));
        arg->clientFd = connfd;
        arg->clientIP = inet_ntoa(clientAddress.sin_addr);
        arg->clientPort = clientAddress.sin_port;

        // Create detachable threads
        int s;
        s = pthread_create(&threads[count], NULL, threadFunction, (void *) arg);
        if (s != 0) {
            fprintf(stderr, "pthread_create failed.\n");
            exit(EXIT_FAILURE); 
        }
        s = pthread_detach(threads[count]);
        if (s != 0) {
            fprintf(stderr, "pthread_detach failed.\n");
            exit(EXIT_FAILURE);
        }
        count++;
        if (count == MAX_CONCURRENT_CLIENTS) count = 0;
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
} 
