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
#include"socketHelper.h"

int main(int argc, char **argv){
    int clientfd;
    int port;
    char *host;
    char buf[MAXLINE];
    rio_t rio;

    if(argc != 3){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    clientfd = open_clientfd(host, port);
    rio_readinitb(&rio, clientfd);

    while(fgets(buf, MAXLINE, stdin) != NULL){
        rio_writen(clientfd, buf, strlen(buf));
        printf("send ok\n");
        rio_readlineb(&rio, buf, MAXLINE);
        // printf("receive chars num: %d\n", recv);
        fputs(buf, stdout);
        
    }
    close(clientfd);
    exit(0);
}