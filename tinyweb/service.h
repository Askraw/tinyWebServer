#pragma once
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

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parser_uri(char *uri, char *file_name, char *cgi_args);
void serve_static(int fd, char* file_name, int file_size);
void get_file_type(char* file_name, char* file_type);
void serve_dynamic(int fd, char *file_name, char *cgi_args);
void client_error(int fd, char* cause, char* errnum, char* short_msg, char* long_meg);