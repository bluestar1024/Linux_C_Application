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
    char buf[100] = "hello world";

    if(2 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ret = flock(fd, LOCK_EX | LOCK_NB);
    if(ret)
        perror("flock error");
    else
        puts("file lock success!");

    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
    {
        perror("write error");
        exit(-1);
    }
    printf("write buf : %s!\n", buf);

    ret = lseek(fd, 0, SEEK_SET);
    if(ret < 0)
    {
        perror("lseek error");
        exit(-1);
    }

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        perror("read error");
        exit(-1);
    }
    printf("read buf : %s!\n", buf);

    ret = flock(fd, LOCK_UN);
    if(ret)
    {
        perror("flock error");
        exit(-1);
    }
    puts("file unlock success!");

    close(fd);
    return 0;
}