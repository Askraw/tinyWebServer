#pragma once
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#define RIO_BUFSIZE 8192

typedef struct{
    int rio_fd;                             // file descriptor for the Internal buffer
    int rio_cnt;                            // Unread bytes in internal buf
    char *rio_bufptr;                       // Next unread byte in internal buf
    char rio_buf[RIO_BUFSIZE];              // Internal buffer
}rio_t;

ssize_t rio_readn(int fd, void *usrbuf, size_t n);        /* read up tp n byte from fd's current positon to usrbuf */
ssize_t rio_writen(int fd, void *usrbuf, size_t n);       /* write up tp n bytes from usrbuf to fd */

/* above funcs without buf  below funcs with buf */
/* RIO function with buffer. The purpose of the buffer is to reduce the overhead caused by multiple calls to system-level IO functions and into kernel mode. */

void rio_readinitb(rio_t *rp, int fd);                    /* initialize the rio_t struct */
static ssize_t rio_read(rio_t *rp, void *usrbuf, size_t n);  /* buffered Unix read function */

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);    /* read from file(rio_t) to usrbuf for up to n bytes */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);    /* read aline from rp to usrbuf and read up to maxlen-1 for lastchar is '\0' */

