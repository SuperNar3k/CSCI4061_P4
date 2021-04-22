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

int resultHistogram[20];
pthread_mutex_t lock;

typedef struct request{
  int requestCode;
  int clientID;
  int data[20];
  int flag;
}request_t;

typedef struct response{
  int requestCode;
  int responseCode;
  int data[20];
}response_t;