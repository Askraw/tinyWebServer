#pragma once
#include<sys/socket.h>
#include<sys/types.h>
#include"rio.h"
#define MAXLINE 8192
#define LISTENQ 1024
int open_clientfd(char *hostname, int port);
int open_listenfd(int port);
void echo(int connfd);

