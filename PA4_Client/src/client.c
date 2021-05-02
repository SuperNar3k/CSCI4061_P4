#include "client.h"

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);
    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

void init_request(request_t* request, int requestCode, int clientID){
  request->requestCode = requestCode;
  request->clientID = clientID;
  request->flag = 0;
  for (int i = 0; i < 20; i++){
    request->data[i] = 0;
  }
}

/**
 * Read lines from files, and count words by word length.
 */
void process(FILE* fp, request_t* request){
  int wordLen;
  char word[chunkSize];

  //reads lines until end of file or error
  while (fscanf(fp, "%s", word) != EOF){
    wordLen = strlen(word);
    request->data[wordLen - 1]++ ;
    
  }
  fclose(fp);
}

void update(struct sockaddr_in* address, FILE* fp, int clientID){
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (connect(sockfd, (struct sockaddr *) address, sizeof(*address)) == 0) {
        fprintf(logfp, "[%d] open connection\n", clientID);
        request_t request;
        response_t response;
        init_request(&request, UPDATE_WSTAT, clientID);
        // write the file's data into the request.data
        process(fp, &request); 
        // write the request to the socket
        write(sockfd, &request, sizeof(request_t));
        // read the server's response
        read(sockfd, &response, sizeof(response_t));

        if(response.responseCode == RSP_NOK){ // client received RSP_NOK
            fprintf(logfp, "[%d] close connection (failed execution)\n", clientID);
        }
        else { // client received RSP_OK
            fprintf(logfp, "[%d] close connection (successful execution)\n", clientID);
        }
        //close connection
        close(sockfd);
    } else {
        perror("Connection failed!");
    }
}

void get_my_updates(struct sockaddr_in* address, int clientID){
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(connect(sockfd, (struct sockaddr *) address, sizeof(*address)) == 0){
        fprintf(logfp, "[%d] open connection\n", clientID);
        request_t request;
        response_t response;
        init_request(&request, GET_MY_UPDATES, clientID);
        // write the request to the socket
        write(sockfd, &request, sizeof(request_t));
        // read the server's response
        read(sockfd, &response, sizeof(response_t));

        fprintf(logfp, "[%d] GET_MY_UPDATES: %d %d\n", clientID, response.responseCode, response.data[0]);
        if(response.responseCode == RSP_NOK){ // client received RSP_NOK
            fprintf(logfp, "[%d] close connection (failed execution)\n", clientID);
        }
        else { // client received RSP_OK
            fprintf(logfp, "[%d] close connection (successful execution)\n", clientID);
        }
        //close connection
        close(sockfd);
    } else {
        perror("Connection failed!");
    }
}

void get_all_updates(struct sockaddr_in* address, int clientID){
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(connect(sockfd, (struct sockaddr *) address, sizeof(*address)) == 0){
        fprintf(logfp, "[%d] open connection\n", clientID);
        request_t request;
        response_t response;
        init_request(&request, GET_ALL_UPDATES, clientID);
        // write the request to the socket
        write(sockfd, &request, sizeof(request_t));
        // read the server's response
        read(sockfd, &response, sizeof(response_t));

        fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", clientID, response.responseCode, response.data[0]);
        if(response.responseCode == RSP_NOK){ // client received RSP_NOK
            fprintf(logfp, "[%d] close connection (failed execution)\n", clientID);
        }
        else { // client received RSP_OK
            fprintf(logfp, "[%d] close connection (successful execution)\n", clientID);
        }
        //close connection
        close(sockfd);
    } else {
        perror("Connection failed!");
    }
}

void get_wstat(struct sockaddr_in* address, int clientID){
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(connect(sockfd, (struct sockaddr *) address, sizeof(*address)) == 0){
        fprintf(logfp, "[%d] open connection\n", clientID);
        request_t request;
        response_t response;
        init_request(&request, GET_WSTAT, clientID);
        // write the request to the socket
        write(sockfd, &request, sizeof(request_t));
        // read the server's response
        read(sockfd, &response, sizeof(response_t));
        // setup the log message with the response data
        char wstatLog[MAX_LOG_MSG];
        sprintf(wstatLog, "[%d] GET_WSTAT: %d <", clientID, response.responseCode);
        for(int i = 0; i < MAX_NUM_CLIENTS; i++){ // appends data from the response to the log message
            char data[dataSize];
            sprintf(data, "%d", response.data[i]);
            if(i == 0){
                strcat(wstatLog, data);
            }
            else{
                strcat(wstatLog, " ");
                strcat(wstatLog, data);
            }
        }
        // print the GET_ALL_UPDATES log message
        fprintf(logfp, "%s>\n", wstatLog);
        if(response.responseCode == RSP_NOK){ // client received RSP_NOK
            fprintf(logfp, "[%d] close connection (failed execution)\n", clientID);
        }
        else { // client received RSP_OK
            fprintf(logfp, "[%d] close connection (successful execution)\n", clientID);
        }
        //close connection
        close(sockfd);
    } else {
        perror("Connection failed!");
    }
}

int client(int clientID, char* folderName, char* serverIP, int serverPort){
  // Specify an address to connect to 
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(serverPort);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");

  char fileName[maxFileNameLength];
  sprintf(fileName, "%s/%d.txt", folderName, clientID);

  FILE* fp;

  if((fp = fopen(fileName, "r")) != NULL){
    update(&address, fp, clientID);
    fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", clientID, 1);
  }
  else {
    fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", clientID, 0);
  }
  get_my_updates(&address, clientID);
  get_all_updates(&address, clientID);
  get_wstat(&address, clientID);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    // Process input arguments
    char *folderName = argv[1];
    int clients = atoi(argv[2]);
    char *serverIP = argv[3];
    char *serverPort = argv[4];
    int portNumber = atoi(serverPort);

    // Create the log file
    createLogFile();
    // Spawn client processes
    for (int i = 0; i < clients; i++){
        pid_t child = fork();
        if (child == 0){
            client(i + 1, folderName, serverIP, portNumber);
        }
        else if (client < 0) { // error creating child
            fprintf(stderr, "ERROR: failed to fork while spawning client processes\n");
			exit(EXIT_FAILURE);
		}
    }

    // TO-DO: Wait for all client processes to terminate
    for (int i = 0; i < clients; i++){
		pid_t terminated_pid = wait(NULL);
		if (terminated_pid == -1){ // error waiting for child
			fprintf(stderr, "ERROR: failed to wait for client proccess\n");
			exit(EXIT_FAILURE);
		}
	}

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;
}
