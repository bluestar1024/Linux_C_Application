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
    struct flock lock;
    char buf[100] = {0};
    int num = atoi(argv[2]);
    int ret = 0;

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

    if(2 == num)
    {
        ret = read(fd, buf, sizeof(buf));
        if(ret < 0)
        {
            perror("read error");
            exit(-1);
        }
        printf("read buf : %s!\n", buf);

        ret = lseek(fd, 0, SEEK_SET);
        if(ret < 0)
        {
            perror("lseek error");
            exit(-1);
        }
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd, F_SETLKW, &lock);
    puts("file lock success!");

    sleep(5);

    sprintf(buf, "hello world %d_%d_%s_%s", num, num, argv[2], argv[2]);
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
    {
        perror("open error");
        exit(-1);
    }
    puts("data write to file success!");

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    puts("file unlock success!");

    close(fd);
    return 0;
}