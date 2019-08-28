#include"service.h"
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
#include"../socketHelper.h"
int main(int argc, char **argv){
    int listenfd;
    int connfd;
    int port;
    int client_len;
    struct sockaddr_in client_addr;

    /* check command line args */
    if (argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);
    listenfd = open_listenfd(port);
    while(1){
        client_len = sizeof(client_addr);
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
        doit(connfd);
        close(connfd);
    }
}

