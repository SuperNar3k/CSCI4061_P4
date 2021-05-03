#ifndef SERVER_H
#define SERVER_H

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

int resultHistogram[WORD_LENGTH_RANGE];
int clientStatus[MAX_NUM_CLIENTS];
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

#endif //SERVER_H