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

static int fd = -1;

void sig_handler(int sig)
{
    int ret = 0;

    ret = flock(fd, LOCK_UN);
    if(ret)
    {
        perror("flock error");
        exit(-1);
    }
    puts("file unlock success!");
    
    close(fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    if(2 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY | O_CREAT);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ret = flock(fd, LOCK_EX | LOCK_NB);
    if(ret)
    {
        perror("flock error");
        exit(-1);
    }
    puts("file lock success!");

    signal(SIGINT, sig_handler);

    while(1)
        sleep(1);
    return 0;
}