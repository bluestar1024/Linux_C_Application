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

int main(int argc, char **argv)
{
    int fd = -1;
    struct flock lock = {0};
    int ret = 0 ;

    if(2 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    fd = open(argv[1], O_RDONLY | O_CREAT | O_TRUNC, 0664);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ftruncate(fd, 1024);

    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 200;
    lock.l_len = 100;
    ret = fcntl(fd, F_SETLK, &lock);
    if(ret < 0)
    {
        perror("fcntl error");
        exit(-1);
    }
    puts("file lock success!");

    while(1)
        sleep(1);
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    puts("file unlock success!");

    close(fd);
    return 0;
}