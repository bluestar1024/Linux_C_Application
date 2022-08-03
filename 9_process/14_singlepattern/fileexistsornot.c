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

#define PID_FILE        "sfile.pid"

void delete_file(void)
{
    remove(PID_FILE);
}

int main(void)
{
    int fd = open(PID_FILE, O_RDWR | O_CREAT | O_EXCL, 0666);
    if(-1 == fd)
    {
        perror("program already exists");
        exit(-1);
    }

    atexit(delete_file);

    puts("program start!");
    sleep(10);
    puts("program end!");

    close(fd);
    return 0;
}