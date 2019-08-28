#pragma once
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
#include"rio.h"
#define MAXLINE 8192
#define LISTENQ 1024
int open_clientfd(char *hostname, int port);
int open_listenfd(int port);
void echo(int connfd);

