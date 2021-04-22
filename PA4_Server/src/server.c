#include "../include/server.h"

struct clientInfo {
    int clientFd;
    char *clientIP;
    int clientPort;
};

void* requestThread(void* args){
    struct clientInfo* info = (struct clientInfo*) args;
    request_t request;
    response_t response;
    read(info->clientFd, &request, sizeof(request_t));

    if(request.flag == GET_WSTAT){      
        response.requestCode = request.requestCode;
        response.responseCode = 1;
        pthread_mutex_lock(&lock);
        for (int i = 0; i < WORD_LENGTH_RANGE; i++){
            response.data[i] = resultHistogram[i];
        }
        pthread_mutex_unlock(&lock);
        printf("[%d] GET_WSTAT\n", request.clientID);
    }

    write(info->clientFd, &response, sizeof(response_t));
    return NULL;
}

int main(int argc, char *argv[]) {

    // Process input arguments
    int serverPort = atoi(argv[1]);
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    pthread_t threads[MAX_NUM_CLIENTS];
    int count = 0;

    // Initialize resultHistogram lock
    pthread_mutex_init(&lock, NULL);

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
    if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
        printf("socket bind failed\n");
        exit(EXIT_FAILURE);
    }

    // Now server is ready to listen
    if (listen(sockfd, MAX_NUM_CLIENTS) == -1) {
        printf("Listen failed\n");
        exit(EXIT_FAILURE);
    } else {
        printf("server is listening\n");
    }

    while (1) {

        struct sockaddr_in clientAddress;
        len = sizeof(clientAddress);

        // Accept (write your code by replacing ...)
        connfd = accept(sockfd, (struct sockaddr*) &clientAddress, (void *) &len);
        if (connfd < 0) {
            printf("server accept failed\n");
            exit(EXIT_FAILURE);
        }

        struct clientInfo *arg = (struct clientInfo *) malloc(sizeof(struct clientInfo));
        arg->clientFd = connfd;
        arg->clientIP = inet_ntoa(clientAddress.sin_addr);
        arg->clientPort = clientAddress.sin_port;

        printf("open connection from %s:%d\n", arg->clientIP, arg->clientPort);

        // Create detachable threads
        int s;
        s = pthread_create(&threads[count], NULL, requestThread, (void *) arg);
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
