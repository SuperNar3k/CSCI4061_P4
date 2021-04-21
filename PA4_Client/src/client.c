    #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include "../include/protocol.h"

FILE *logfp;

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

int main(int argc, char *argv[]) {

    // Process input arguments
    char *folderName = argv[1];
    int clients = atoi(argv[2]);
    char *serverIP = argv[3];
    char *serverPort = argv[4];

    // Create the log file
    createLogFile();

    // Spawn client processes
    // Create a TCP socket.
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);

    // Specify an address to connect to (we use the local host or 'loop-back' address).
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(serverPort);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect it.
    if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {

        char msgbuf[MAX_MSG_SIZE];
        char rspbuf[MAX_MSG_SIZE];
        printf("Enter a message to send: ");
        scanf("%s", msgbuf);

        write(sockfd, msgbuf, strlen(msgbuf));
        read(sockfd, rspbuf, MAX_MSG_SIZE);
        printf("%s\n", rspbuf);

        //close connection
        close(sockfd);

    } else {
        perror("Connection failed!");
    }
}

    // TO-DO: Wait for all client processes to terminate


    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;

}
