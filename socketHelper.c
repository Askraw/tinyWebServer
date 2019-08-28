
#include"rio.h"
#include"socketHelper.h"
int open_clientfd(char *hostname, int port){
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in server_addr;

    if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    
    if((hp = gethostbyname(hostname)) == NULL)
        return -2;
    
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0], (char *)&server_addr.sin_addr.s_addr, hp->h_length);
    server_addr.sin_port = htons((unsigned int)port);

    if(connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return -1;
    return clientfd;

}
int open_listenfd(int port){
    int listenfd;
    int optval = 1;
    struct sockaddr_in server_addr;
    
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) < 0)
        return -1;

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned int)port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd,  (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return -1;
    if(listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
}

void echo(int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio, connfd);

    printf("connfd start echo!\n");
    while((n = rio_readlineb(&rio, buf, MAXLINE)) != 0){
        printf("server received %d bytes\n", n);
        printf("message is: %s", buf);
        // char buf2[100] = "test";
        //  rio_writen(connfd, buf2, 100);
        rio_writen(connfd, buf, n);
    }
    
}