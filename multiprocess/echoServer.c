#include"../rio.h"
#include"../socketHelper.h"
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
void echo(int connfd);
void sigchld_handler(int sig){
    while(waitpid(-1, 0, WNOHANG) > 0)
        ;
    return ;
}

int main(int argc, char **argv){
    int listenfd;
    int connfd;
    int port;
    socklen_t client_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    signal(SIGCHLD, sigchld_handler);
    listenfd = open_listenfd(port);
    while(1){
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
        if(fork() == 0){
            close(listenfd);
            echo(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

}