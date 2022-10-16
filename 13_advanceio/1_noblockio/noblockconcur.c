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

#define MOUSE       "/dev/input/event2"

int main(void)
{
    int fd = 0;
    int ret = 0;
    char buf[100];
    int flags = 0;

    fd = open(MOUSE, O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    flags = fcntl(0, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(0, F_SETFL, flags);

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = read(fd, buf, sizeof(buf));
        if(ret > 0)
            printf("mouse read %d bytes data!\n", ret);
        
        memset(buf, 0, sizeof(buf));
        ret = read(0, buf, sizeof(buf));
        if(ret > 0)
        {
            printf("keyboard read : %s", buf);
            printf("keyboard read %d bytes data!\n", ret);
        }
    }

    close(fd);
    return 0;
}