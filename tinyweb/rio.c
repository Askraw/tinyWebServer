#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<string.h>
#include"rio.h"


ssize_t rio_readn(int fd, void *usrbuf, size_t n){                 /* rio_Readn can return a short count(不足值) */
    size_t nleft = n;
    ssize_t  nread;
    char* bufp = usrbuf;
    while(nleft > 0){
        if((nread = read(fd, bufp, nleft)) < 0) {
            if(errno == EINTR)  /* Interrupted system call */
                nread = 0;      /* read() ag */
            else                /* errno by read() */
                return -1;
        } else if(nread == 0)
            break;              /*  EOF */
        nleft -= nread;
        bufp += nread;
    }
    return n-nleft;
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n){                /* rio_writen can only return n */
    size_t nleft = n;
    ssize_t  nwrite;
    char* bufp = usrbuf;

    while(nleft > 0){
        if((nwrite = write(fd, bufp, n)) <= 0){
            if(errno  == EINTR)
                nwrite = 0;
            else
                return -1;
        }
        nleft -= nwrite;
        bufp += nwrite;
    }
    return n;
}


void rio_readinitb(rio_t *rp, int fd){
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

// this function can be called many times , and everytime it will determine whether rio_buf has unread buf
// if rio has, it will read the unread buff in rio
// else it will read the rio_fd for more bytes
// 解释这个func应该就是 riot 与 一个fd相连  初始状态！：首先从fd至多读MAXLINE个byte到rio buf然后给cnt赋值读到的字节数
// 然后比较n和cnt 取较小的那个将其传给usrbuf 然后给rio bufptr移动 rio cnt更新
// 下一次调用rio read就有可能 cnt>0  这个时候其实说 rio上一次读到的buf有部分字节没读完 需要 直接进行下一个阶段
// 而其他函数会反复调用rio read 所以不会存在说如果哇这一次rio里有cnt没读完 接下来读不到这么多个字节里 会反复调用read

// 这里还能解释为什么能够减少时间 因为一次从 fd里 内核态read的说MAXLINE个字节 接下来会从riot的buf里读取字节直到MAXLINE
// 从而减少了反复调用read造成的内核态和用户态之间的切换 这也是就为什么要做rio的原因
static ssize_t rio_read(rio_t *rp, void *usrbuf, size_t n){
    int cnt;

    /* read unread buf to rio_buf */
    while (rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if(rp->rio_cnt < 0){
            if(errno != EINTR)
                return -1;
        }else if(rp->rio_cnt == 0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }
    /* copy rio_buf to usrbuf */
    cnt = n;
    if(rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n){             /* call rio_read once more up to EOF */  
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    while(nleft > 0){
        if((nread = rio_read(rp, bufp, nleft)) < 0){
            if(errno == EINTR)
                nread = 0;
            else 
                return -1;
        } else if(nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
     }
    return n-nleft;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen){       /*  at most read maxlen-1 char for the last char is '\0' */
    int n, cRead;
    char c;
    char *bufp = usrbuf;
    
    for(n=1; n<maxlen; n++){
        if((cRead = rio_read(rp, &c, 1)) == 1){
            *bufp++ = c;
            if(c == '\n')
                break;
        } else if(cRead == 0){                 /* EOF */
            if(n == 1)
                return 0;                      /* No Data */
            else 
                break;                         /* some data was read */
        } else 
            return -1;                         /*  Error */
    }
    *bufp = 0;                                 /* Why not '\0'? */
    return n;
}

