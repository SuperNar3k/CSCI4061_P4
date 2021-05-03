#ifndef CLIENT_H
#define CLIENT_H

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

#endif //CLIENT_H