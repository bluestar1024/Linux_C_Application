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

void sig_handler(int sig)
{
    char buf[100] = {0};
    int ret = 0;

    ret = read(fd, buf, sizeof(buf));
    if(ret > 0)
        printf("mouse read %d bytes data!\n", ret);
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

    signal(SIGIO, sig_handler);

    while(1);

    close(fd);
    return 0;
}