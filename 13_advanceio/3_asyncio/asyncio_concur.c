#define _GNU_SOURCE

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

#define MOUSE       "/dev/input/event2" 
static int fd;

void io_handler(int sig)
{
    char buf[100] = {0};
    int ret = 0;

    ret = read(fd, buf, sizeof(buf));
    if(ret > 0)
        printf("mouse read %d bytes data!\n", ret);
}
void usr_handler(int sig)
{
    char buf[100] = {0};
    int ret = 0;

    ret = read(0, buf, sizeof(buf));
    if(ret > 0)
    {
        printf("keyboard read : %s", buf);
        printf("keyboard read %d bytes data!\n", ret);
    }
}

int main(void)
{
    int flags = 0;

    fd = open(MOUSE, O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    flags = fcntl(fd, F_GETFL);
    flags |= O_ASYNC;
    fcntl(fd, F_SETFL, flags);

    fcntl(fd, F_SETOWN, getpid());

    flags = fcntl(0, F_GETFL);
    flags |= O_NONBLOCK | O_ASYNC;
    fcntl(0, F_SETFL, flags);

    fcntl(0, F_SETOWN, getpid());
    fcntl(0, F_SETSIG, SIGUSR1);

    signal(SIGIO, io_handler);
    signal(SIGUSR1, usr_handler);

    while(1);

    close(fd);
    return 0;
}