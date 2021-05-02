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
    response.requestCode = request.requestCode;

    int clientID = request.clientID;
    if(request.requestCode == UPDATE_WSTAT){
        response.responseCode = RSP_OK;
        pthread_mutex_lock(&lock);
        clientStatus[clientID - 1]++;
        for (int i = 0; i < WORD_LENGTH_RANGE; i++){ // writes data to the result histogram
            resultHistogram[i] += request.data[i];
        }
        pthread_mutex_unlock(&lock);
        printf("[%d] UPDATE_WSTAT\n", clientID);
    }
    else if(request.requestCode == GET_MY_UPDATES){
        response.responseCode = RSP_OK;
        pthread_mutex_lock(&lock);
        response.data[0] = clientStatus[clientID - 1]; // records the number of updates the client gave to the response data structure
        pthread_mutex_unlock(&lock);
        printf("[%d] GET_MY_UPDATES\n", clientID);
    }
    else if(request.requestCode == GET_ALL_UPDATES){
        response.responseCode = RSP_OK;
        pthread_mutex_lock(&lock);
        int totalRequests = 0;
        for(int i = 0; i < MAX_NUM_CLIENTS; i++){ // sums the number of updates from clients
            totalRequests += clientStatus[i];
        }
        response.data[0] = totalRequests;
        pthread_mutex_unlock(&lock);
        printf("[%d] GET_ALL_UPDATES\n", clientID);
    }
    else if(request.requestCode == GET_WSTAT){      
        response.responseCode = RSP_OK;
        pthread_mutex_lock(&lock);
        for (int i = 0; i < WORD_LENGTH_RANGE; i++){ // writes the current result histogram data to the response data struture
            response.data[i] = resultHistogram[i];
        }
        pthread_mutex_unlock(&lock);
        printf("[%d] GET_WSTAT\n", clientID);
    }
    else{
        response.responseCode = RSP_NOK;
    }

    write(info->clientFd, &response, sizeof(response_t));
    printf("close connection from %s:%d\n", info->clientIP, info->clientPort);
    close(info->clientFd);
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
