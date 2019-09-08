#include"sbuf.h"
#include"../mutex.h"
#define NTHREADS 4
#define SBUFSIZE 16

void echo_cnt(int connfd);
void *thread(void *vargp);

sbuf_t sbuf;

// while insert connfd to sbuf_t
// each thread get connfd from sbuf_t
int main(int argc, char **argv){
    int i, listenfd, connfd, port;
    socklen_t client_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    pthread_t tid;
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    sbuf_init (&sbuf, SBUFSIZE);
    listenfd = open_listenfd(port);

    for(int i=0; i<NTHREADS; i++){
        pthread_create(&tid, NULL, thread, NULL);
    }
    while(1){
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);
        sbuf_insert(&sbuf, connfd);
    }
}

void *thread(void *vargp){
    pthread_detach(pthread_self());
    while(1){
        int connfd = sbuf_remove(&sbuf);
        echo_cnt(connfd);
        close(connfd);
    }
}