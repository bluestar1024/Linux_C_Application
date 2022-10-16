#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <pthread.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int fd = -1;
    int ret = 0;
    struct stat sbuf;
    pid_t pid;
    struct flock lock = {0};

    if(2 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0664);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ret = write(fd, "hello world!", 12);
    if(ret != 12)
    {
        perror("write error");
        exit(-1);
    }

    ret = fstat(fd, &sbuf);
    if(ret < 0)
    {
        perror("fstat error");
        exit(-1);
    }
    ret = fchmod(fd, (sbuf.st_mode &~ S_IXGRP) | S_ISGID);
    if(ret < 0)
    {
        perror("fchmod error");
        exit(-1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork error");
        exit(-1);
    }
    else if(0 == pid)
    {
        int flags = 0;
        char buf[100] = {0};

        sleep(1);

        flags = fcntl(fd, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);

        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        ret = fcntl(fd, F_SETLK, &lock);
        if(ret < 0)
            perror("child process read lock error");
        else
            puts("child process read lock success!");
        
        ret = lseek(fd, 0, SEEK_SET);
        if(ret < 0)
        {
            perror("lseek error");
            _exit(-1);
        }

        ret = read(fd, buf, sizeof(buf));
        if(ret < 0)
            perror("read error");
        else
            printf("buf : %s!\n", buf);
    }
    else
    {
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        ret = fcntl(fd, F_SETLK, &lock);
        if(ret < 0)
        {
            perror("father process write lock error");
            exit(-1);
        }
        puts("father process write lock success!");
        puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        ret = wait(NULL);
        if(ret < 0)
        {
            perror("wait error");
            exit(-1);
        }

        lock.l_type = F_UNLCK;
        ret = fcntl(fd, F_SETLK, &lock);
        if(ret < 0)
        {
            perror("father process unlock error");
            exit(-1);
        }
    }

    close(fd);
    return 0;
}