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

#define MOUSE       "/dev/input/event2"

int main(void)
{
    int fd = 0;
    int ret = 0;
    char buf[100];
    int flags = 0;
    fd_set rfd_set;

    fd = open(MOUSE, O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    while(1)
    {
        FD_ZERO(&rfd_set);
        FD_SET(0, &rfd_set);
        FD_SET(fd, &rfd_set);

        ret = select(fd+1, &rfd_set, NULL, NULL, NULL);
        if(ret < 0)
        {
            perror("select error");
            exit(-1);
        }
        else if(0 == ret)
        {
            puts("select timeout!");
            continue;
        }
        
        if(FD_ISSET(0, &rfd_set))
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

        if(FD_ISSET(fd, &rfd_set))
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

    close(fd);
    return 0; 
}