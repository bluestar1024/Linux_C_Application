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
    struct flock wrlock = {0};
    struct flock rdlock = {0};
    int ret = 0;
    char buf[100] = {0};
    int num = atoi(argv[2]);

    if(3 != argc)
    {
        perror("usage error");
        exit(-1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0664);
    if(fd < 0)
    {
        perror("open error");
        exit(-1);
    }

    ftruncate(fd, 1024);

    wrlock.l_type = F_WRLCK;
    wrlock.l_whence = SEEK_SET;
    wrlock.l_start = 100;
    wrlock.l_len = 100;
    fcntl(fd, F_SETLKW, &wrlock);
    puts("write file lock success!");

    rdlock.l_type = F_RDLCK;
    rdlock.l_whence = SEEK_SET;
    rdlock.l_start = 400;
    rdlock.l_len = 100;
    fcntl(fd, F_SETLKW, &rdlock);
    puts("read file lock success!");
    
    sleep(5);

    ret = lseek(fd, 100, SEEK_SET);
    if(ret < 0)
    {
        perror("lseek error");
        exit(-1);
    }
    sprintf(buf, "hello world %d_%d_%s_%s", num, num, argv[2], argv[2]);
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
    {
        perror("write error");
        exit(-1);
    }
    puts("data write to file success!");

    ret = lseek(fd, 400, SEEK_SET);
    if(ret < 0)
    {
        perror("lseek error");
        exit(-1);
    }
    ret = read(fd, buf, sizeof(buf));
    if(ret < 0)
    {
        perror("read error");
        exit(-1);
    }
    printf("read buf : %s!\n", buf);

    wrlock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &wrlock);
    puts("write file unlock success!");

    rdlock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &rdlock);
    puts("read file unlock success!");

    close(fd);
    return 0;
}