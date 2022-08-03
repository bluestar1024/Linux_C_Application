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

int main(void)
{
    signal(SIGHUP,SIG_IGN);

    printf("process ID : %d!\n", getpid());
    printf("process group ID : %d!\n", getpgrp());
    printf("process SID : %d!\n", getsid(0));
    printf("parent process ID : %d!\n", getppid());

    while(1)
    {
        sleep(3);
        puts("runing...");
    }
    return 0;
}