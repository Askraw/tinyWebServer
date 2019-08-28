#include"../rio.h"
#include"../socketHelper.h"
#include<unistd.h>
#include<sys/types.h>
#include<sys/select.h>
#include<stdio.h>
#include <sys/time.h>

int select(int n, fd_set *fdset, NULL, NULL, NULL);
FD_ZERO(fd_set *fdset);
FD_CLR(int fd, fd_set *fdset);
FD_SET(int fd, fd_set *fdset);
FD_ISSET(int fd, fd_set *fdset);
// int select(int n, fd_set *fdset, NULL, NULL, NULL);

// FD_ZERO(fd_set *fdset);
// FD_CLR(int fd, fd_set *fdset);
// FD_SET(int fd, fd_set *fdset);
// FD_ISSET(int fd, fd_set *fdset);
