#include "client.h"

void init_request(request_t* request, int requestCode, int clientID){
  request->requestCode = requestCode;
  request->clientID = clientID;
  request->flag = 0;
  for (int i = 0; i < 20; i++){
    request->data[i] = 0;
  }
}

int main(int argc, char *argv[]){
  int clientID = atoi(argv[1]);
  char *folderName = argv[2];
  char *serverIP = argv[3];
  int serverPort = atoi(argv[4]);
  // Create a TCP socket.
  int sockfd = socket(AF_INET , SOCK_STREAM , 0);

  // Specify an address to connect to 
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(serverPort);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect it.
  if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
    request_t request;
    response_t response;
    init_request(&request, GET_WSTAT, clientID);
    write(sockfd, &request, sizeof(request_t));

    read(sockfd, &response, sizeof(response_t));
    //close connection
    close(sockfd);

  } else {
    perror("Connection failed!");
  }
  
  exit(EXIT_SUCCESS);
}