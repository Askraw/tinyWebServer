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
#include"socketHelper.h"

int main(int argc, char **argv){
    int listenfd;
    int connfd;
    int port;
    int clientlen;
    struct sockaddr_in client_addr;
    struct hostent *hp;
    char *haddrp;
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }else{
        fprintf(stdout,"usage: %s <port>\n", argv[0]);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while(1){
        clientlen = sizeof(client_addr);
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &clientlen);
        hp = gethostbyaddr((const char*)&client_addr.sin_addr.s_addr, sizeof(client_addr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(client_addr.sin_addr);
        printf("server connected to %s (%s)\n", hp->h_name, haddrp);
        
        echo(connfd);
        close(connfd);
        printf("connect close\n");
    }
    exit(0);
}