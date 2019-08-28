#include"../rio.h"
#include"../socketHelper.h"
#include<pthread.h>

void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv){
    int listenfd;
    int *connfdp;
    int port;
    socklen_t client_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    pthread_t tid;

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    listenfd = open_listenfd(port);
    while(1){
        connfdp = malloc(sizeof(int));
        *connfdp = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
        pthread_create(&tid, NULL, thread, connfdp);
    }
}

void *thread(void *vargp){
    int connfd = *((int*)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}