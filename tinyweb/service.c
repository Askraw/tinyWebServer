#include "service.h"
#include<sys/mman.h>

extern char ** environ;
void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parser_uri(char *uri, char *file_name, char *cgi_args);
void serve_static(int fd, char* file_name, int file_size);
void get_file_type(char* file_name, char* file_type);
void serve_dynamic(int fd, char *file_name, char *cgi_args);
void client_error(int fd, char* cause, char* errnum, char* short_msg, char* long_meg);

void doit(int fd){
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE];
    char method[MAXLINE];
    char uri[MAXLINE];
    char version[MAXLINE];
    char file_name[MAXLINE];
    char cgi_args[MAXLINE];
    rio_t rio;

    /* Read request form line and headers */
    rio_readinitb(&rio, fd);
    rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    if(strcasecmp(method, "GET")){
        client_error(fd, method, "501", "NOT Implemented", "Tiny does not implement this method");
        return ;
    }
    read_requesthdrs(&rio);

    /* Parse URI from GET request */
    // printf("getURL: %s\n", uri);
    is_static = parser_uri(uri, file_name, cgi_args);
    // printf("getFileName: %s\n", file_name);

    if(stat(file_name, &sbuf) < 0){
        client_error(fd, file_name, "404", "Not found", "Tiny couldn't find this file");
        return ;
    }

    if(is_static){
        if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
            // printf("test1: %d\n", S_ISREG(sbuf.st_mode));
            // printf("test2: %d\n", S_IRUSR & sbuf.st_mode);
            client_error(fd, file_name, "403", "Forbidden", "Tiny couldn't read this file");
            return ;
        }
        serve_static(fd, file_name, sbuf.st_size);
    }else{
        if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
            client_error(fd, file_name, "403", "Forbidden", "Tiny couldn't run the CGI program");
            return ;
        }
        serve_dynamic(fd, file_name, cgi_args);
    }

}

void client_error(int fd, char *cause, char *errnum, char *short_msg, char *long_msg){
    char buf[MAXLINE];
    char body[MAXLINE];

    /* build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, short_msg);
    sprintf(body, "%s<p>%s: %s\r\n", body, long_msg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, short_msg);
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp){
    char buf[MAXLINE];

    rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")){     /* str1==str2 return 0  str1<str2 return negative ... */
        rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return ;
}

int parser_uri(char *uri, char *file_name, char *cgi_args){
    char *ptr;

    // printf("uri: %s\n", uri);
    // printf("isSubstr?: %d\n", strstr(uri, "cgi-bin"));
    // char *str1 = strstr(uri, "cgi-bin");
    // printf("matchuri: %s\n", str1);
    
    if(!strstr(uri, "cgi-bin")){   /* strstr(str1, str2) is str2 substr of str1 */  /* static content */
        strcpy(cgi_args, "");
        strcpy(file_name, ".");
        strcat(file_name, uri);
        if(uri[strlen(uri)-1] == '/'){
            strcat(file_name, "home.html");
            // printf("return: %d\n", 1);
        }
        return 1;
    }else{     /*  dynamic content */
        ptr = index(uri, '?');
        if(ptr){
            strcpy(cgi_args, ptr+1);
            *ptr = '\0';
        }
        else 
            strcpy(cgi_args, "");
        strcpy(file_name, ".");
        strcat(file_name, uri);
        // printf("return: %d\n", 0);
        return 0;
    }
}

void get_file_type(char *file_name, char *file_type){
    if(strstr(file_name, ".html"))
        strcpy(file_type, "text/html");
    else if(strstr(file_name, ".gif"))
        strcpy(file_type, "image/gif");
    else if(strstr(file_name, ".jpg"))
        strcpy(file_type, "image/jpeg");
    else
        strcpy(file_type, "text/plain");
    
}
void serve_static(int fd, char *file_name, int file_size){
    int srcfd;
    char *srcp;
    char file_type[MAXLINE];
    char buf[MAXLINE];

    /* Send response headers to client */
    get_file_type(file_name, file_type);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, file_size);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, file_type);
    rio_writen(fd, buf, strlen(buf));

    /* Send response body to client */
    srcfd = open(file_name, O_RDONLY, 0);
    srcp = mmap(0,  file_size, PROT_READ, MAP_PRIVATE, srcfd,  0);
    close(srcfd);
    rio_writen(fd, srcp, file_size);
    munmap(srcp, file_size);
}

void serve_dynamic(int fd, char *file_name, char *cgi_args){
    char buf[MAXLINE];
    char *empty_list = {NULL};

    /* return first part of HTTP response */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    rio_writen(fd, buf, strlen(buf));

    if(fork() == 0){  /* child */
        /* Real server would set all CGI raws here */
        setenv("QUERY_STRING", cgi_args, 1);
        dup2(fd, STDOUT_FILENO);
        execve(file_name, empty_list, environ);
    }
    wait(NULL);
}