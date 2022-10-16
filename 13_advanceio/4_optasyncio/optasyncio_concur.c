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

void sig_handler(int sig, siginfo_t *info, void *context)
{
    char buf[100] = {0};
    int ret = 0;

    if(POLL_IN == info->si_code)
    {
        ret = read(info->si_fd, buf, sizeof(buf));
        if(ret < 0)
        {
            perror("read error");
            exit(-1);
        }
        if(0 == info->si_fd)
        {
            printf("keyboard read : %s", buf);
            printf("keyboard read %d bytes data!\n", ret);
        }
        else
            printf("mouse read %d bytes data!\n", ret);
    }
}

int main(void)
{
    int fd = 0;
    int flags = 0;
    struct sigaction sig = {0};
    int ret = 0;

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
    fcntl(fd, F_SETSIG, SIGRTMIN);

    flags = fcntl(0, F_GETFL);
    flags |= O_NONBLOCK | O_ASYNC;
    fcntl(0, F_SETFL, flags);

    fcntl(0, F_SETOWN, getpid());
    fcntl(0, F_SETSIG, SIGRTMIN);

    sig.sa_sigaction = sig_handler;
    sig.sa_flags = SA_SIGINFO;
    sigemptyset(&sig.sa_mask);
    ret = sigaction(SIGRTMIN, &sig, NULL);
    if(ret < 0)
    {
        perror("sigaction error");
        exit(-1);
    }

    while(1);

    close(fd);
    return 0;
}