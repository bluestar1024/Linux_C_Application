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

int main(void)
{
    int fd = 0;
    int ret = 0;
    char buf[100];

    fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        perror("read error");
        close(fd);
        exit(-1);
    }

    close(fd);

    printf("read %d bytes data!\n", ret);
    return 0;
}