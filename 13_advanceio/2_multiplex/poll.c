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

int main(void)
{
    int fd = 0;
    int ret = 0;
    char buf[100];
    int flags = 0;
    struct pollfd fds[2];

    fd = open(MOUSE, O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = fd;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    while(1)
    {
        ret = poll(fds, 2, -1);
        if(ret < 0)
        {
            perror("poll error");
            exit(-1);
        }
        else if(0 == ret)
        {
            puts("poll timeout!");
            continue;
        }

        if(fds[0].revents & POLLIN)
        {
            memset(buf, 0, sizeof(buf));
            ret = read(0, buf, sizeof(buf));
            if(ret < 0)
            {
                perror("keyboard read error");
                exit(-1);
            }
            printf("keyboard read : %s", buf);
            printf("keyboard read %d bytes data!\n", ret);
        }

        if(fds[1].revents & POLLIN)
        {
            memset(buf, 0, sizeof(buf));
            ret = read(fd, buf, sizeof(buf));
            if(ret < 0)
            {
                perror("mouse read error");
                exit(-1);
            }
            printf("mouse read %d bytes data!\n", ret);
        }
    }
    return 0;
}