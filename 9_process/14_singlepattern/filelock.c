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

#define PID_FILE        "sfile.pid"

int main(void)
{
    char str[20] = {0};
    int fd = 0;
    int ret = 0;

    fd = open(PID_FILE, O_RDWR | O_CREAT, 0666);
    if(-1 == fd)
    {
        perror("program already exists");
        exit(-1);
    }

    ret = flock(fd, LOCK_EX | LOCK_NB);
    if(ret < 0)
    {
        perror("flock error");
        exit(-1);
    }

    puts("program runing!");

    ftruncate(fd, 0);
    sprintf(str, "%d\n", getpid());
    write(fd, str, strlen(str));

    sleep(10);
    return 0;
}