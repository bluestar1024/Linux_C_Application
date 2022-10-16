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
    int srcfd, dstfd;
    void *srcaddr, *dstaddr;
    struct stat info;

    if(3 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    srcfd = open(argv[1], O_RDONLY);
    if(srcfd < 0)
    {
        perror("open src file error");
        exit(-1);
    }

    dstfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0664);
    if(dstfd < 0)
    {
        perror("open dest file error");
        exit(-1);
    }

    fstat(srcfd, &info);
    ftruncate(dstfd, info.st_size);

    srcaddr = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, srcfd, 0);
    if(srcaddr == MAP_FAILED)
    {
        perror("map src file error");
        exit(-1);
    }

    dstaddr = mmap(NULL, info.st_size, PROT_WRITE, MAP_SHARED, dstfd, 0);
    if(dstaddr == MAP_FAILED)
    {
        perror("map dest file error");
        exit(-1);
    }

    memcpy(dstaddr, srcaddr, info.st_size);

    munmap(srcaddr, info.st_size);
    munmap(dstaddr, info.st_size);
    close(srcfd);
    close(dstfd);
    return 0;
}